
#ifndef _MEDIA_FS_FAT32_FAT_H_
#define _MEDIA_FS_FAT32_FAT_H_

#include "BootSector.h"
#include "DirectoryCluster.h"
#include "ClusterChain.h"
#include "FileInfo.h"
#include "Name.h"
#include "LongNamePart.h"
#include "FsInfo.h"
#include "DirIterator.h"

#include "../IoFlags.h"
#include "../PathIterator.h"
#include "../Path.h"
#include "../Mbr.h"
#include "../Error.h"

#include "../../../Utils/map_static.h"

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

template<class DataProvider, class DateTimeProvider, int openedFilesMax = 30>
class Engine
{
protected:

	//
	// map для хранения информации об открытых файлах
	//
	static const unsigned long OpenedFilesMax = openedFilesMax;
	typedef Rblib::map_static<unsigned long, FileInfo, OpenedFilesMax> OpenedFilesMap;
	typedef void (*WdtClearType)(void);

public:

	typedef unsigned char ErrorType; // тип ошибки
	typedef DirIterator<DataProvider, DateTimeProvider, OpenedFilesMax> DirIterator; // тип итератора для обхода содержимого каталога
	friend DirIterator;

public:

	Engine(DataProvider& provider)
	: _provider(provider)
	, _cluster(provider)
	, _clusterSize(0)
	, _rootCluster(0)
	, _clusterCount(0)
	, _lastOperationError(Error::None)
	, _wdtClear(0)
	{
	}

	//
	// инициализация
	//

	// Проверяет наличие MBR на диске
	// если MBR присутствует, начинает работать с 1-м рабочим разделом
	// работу с MBR/разделами надо куда-то вынести (в другой класс, не FAT32)
	bool Init()
	{
		ClearError();

		// считываем 0-й сектор диска
		// здесь должен лежать MBR или BootSector FAT
		if (!_provider.ReadSector(0, (unsigned char*)&_mbrTmp))
			return false;
	  
		// _mbrTmp и _bootSectorTmp лежат в одном union, так что можем проверить сразу обе структуры
		// если MBR, то берём сектор первого активного раздела и продолжаем инициализацию с этого сектора
		if (!_bootSectorTmp.Valid())
		{
			if (_mbrTmp.Valid()) 
			{
				for (unsigned char p = 0; p < Mbr::MaxPartitionsNumber; p++)
				{
					if (_mbrTmp.GetPartition(p).Valid() && _mbrTmp.GetPartition(p).Fat32())
					{
						return Init(_mbrTmp.GetPartition(p).Offset() * _sectorSize);
					}
				}

				return false; // нет подходящих разделов
			}

			return false; // в 0-м секторе отсутствует и MBR, и boot sector
		}

		// MBR отсутствует, в 0-м секторе лежит boot sector, и он уже считан
		// продолжаем инициализацию
		return InitFromBootSector(0);
	}

	// инициализация
	// задаём смещение до начального сектор раздела
	bool Init(unsigned long offset)
	{
		ClearError();

		if (!_provider.ReadSector(offset / _sectorSize, (unsigned char*)&_bootSectorTmp))
			return false;

		return InitFromBootSector(offset);
	}

	// задать функцию сброса сторожевого таймера
	void SetWdtClear(WdtClearType wdtClear)
	{
		_wdtClear = wdtClear;
		_clusterChain.SetWdtClear(_wdtClear);
	}

	//
	// ошибка последней операции
	//

	inline ErrorType LastOperationError()
	{
		return _lastOperationError;
	}

	inline bool IoFault()
	{
		return _lastOperationError == Error::IOFault;
	}

	//
	// размер диска
	//

	// размер диска в байтах
	inline unsigned long long DiskSize()
	{
		return (unsigned long long) _clusterCount * _clusterSize;
	}

	// оставшееся место в байтах
	inline unsigned long long FreeSpace()
	{
		// перед вызовом FreeSpace надо вызывать FreeSpaceValid для проверки!
		// если поле FreeCount в FSInfo действительно, возвращаем актуальное значение из ClusterChain
		if (FreeSpaceValid())
			return (unsigned long long) _clusterChain.FreeClusters() * _clusterSize; // переводим в байты
		return DiskSize(); // иначем говорим, что всё свободно
	}
	
	// оставшееся место актуально?
	inline bool FreeSpaceValid()
	{
		return _fsInfo.FreeCount() <= _clusterCount;
	}

	//
	// открыть файл
	//

	inline unsigned long OpenFileInOut(char* path)
	{
		return OpenFile(path, IOFlags::In | IOFlags::Out);
	}

	unsigned long OpenFile(char* path, IOFlags::T flags = IOFlags::In)
	{
		ClearError();

		if (flags == IOFlags::In) // при открытии файла только на чтение возвращаем ошибку, если файл не существует (новый не создаём)
		{
			if (!FindEntry(path, true))
				return 0;
		}
		else
		{
			if (!FindOrCreateEntry(path, true)) 
				return 0;
		}

		const DirEntry& entry = _cluster.CurrentRecord();

		if (entry.Directory())
			return 0;

		if ((flags & IOFlags::Out) && !(flags & IOFlags::In) && !(flags & IOFlags::App))
		{
			if (!ClearCurrentFile())
				return 0;
		}

		if (!RegisterFile(entry.Cluster(), _cluster.Number(), _cluster.CurrentRecordNum(), entry.Size(), flags))
			return 0;

		return entry.Cluster();
	}

	inline bool CloseFile(unsigned long cluster)
	{
		ClearError();

		// сохраняем изменения, если есть
		if (!Flush(cluster))
		{
			SetError(Error::IOFault);
		}

		// если была ошибка, то точно вернём false
		return UnregisterFile(cluster) && _lastOperationError == 0;
	}

	//
	// задать абсолютную позицию в файле, начиная от начала
	//

	bool SeekFile(unsigned long cluster, unsigned long pos)
	{
		ClearError();

		FileInfo* info = GetFileInfo(cluster);

		if (info == 0)
			return false;

		if (pos > info->Size())
			return false;

		info->SetPos(pos);

		return true;
	}

	//
	// создать файл заданного размера (файл будет содержать мусор)
	//

	bool CreateFile(char* path, unsigned long len)
	{
		ClearError();

		if (!CreateEntry(path, true))
			return false;

		DirEntry& entry = _cluster.CurrentRecord();

		// если получилась директория или непустой файл, возвращаем ошибку
		if (entry.Directory() || entry.Size())
			return false;

		// если однго кластера достаточно
		if (len <= _clusterSize)
			return true;

		unsigned long currentLen = len;
		unsigned long cluster = entry.Cluster();

		for(; currentLen > _clusterSize; currentLen -= _clusterSize)
		{
			cluster = AddClusterToChain(cluster);
			if (cluster == 0)
				break;
		}

		// если не смогли выделить кластер, пишем размер, который смогли выделить
		entry.SetSize(cluster ? len : (len - currentLen));

		//устанавливаем дату создания
		SetEntryCreateDate(entry);

		if (!_cluster.Save())
			return false;

		return true;
	}

	unsigned long CreateFileAndOpen(char* path, unsigned long len)
	{
		ClearError();

		if (!CreateFile(path, len))
			return 0;

		DirEntry& entry = _cluster.CurrentRecord();

		if (!RegisterFile(entry.Cluster(), _cluster.Number(), _cluster.CurrentRecordNum(), entry.Size(), IOFlags::Out))
			return 0;

		return entry.Cluster();
	}

	// изменить размер файла открытого файла
	// возвращает новый размер файла или 0 (в случае ошибки)
	// метод не следует использовать для обнуления размера файла
	unsigned long ChangeFileSize(unsigned long cluster, unsigned long size)
	{
		ClearError();

		FileInfo* info = GetFileInfo(cluster);

		// файл должен быть открыт на запись или дозапись
		if (info == 0 || !info->CheckOneFlag(IOFlags::Out | IOFlags::App))
			return false;

		if (info->Size() == size)
			return true;

		unsigned long currentSize = size;

		// идём по цепочке кластеров и добавляем новые, если надо
		for(; currentSize > _clusterSize; currentSize -= _clusterSize)
		{
			if (ClusterEoc(cluster))
				cluster = AddClusterToChain(cluster); 
			else if (!LastOperationError()) // если не было ошибки в ClusterEoc
				cluster = NextCluster(cluster);
			else
				cluster = 0; // была ошибка

			if (cluster == 0)
				return 0; // ошибка
		}

		// освобождаем ненужные кластера (если новый размер меньше старого)
		if (cluster && !ClusterEoc(cluster) && !SetClusterChainEocAndSave(cluster))
			return 0; // ошибка

		// далее сохраняем новый размер

		if (!SetCluster(info->RecordCluster(), info->Record()))
			return 0;

		// если не смогли выделить кластер, пишем размер, который смогли выделить
		_cluster.CurrentRecord().SetSize(cluster ? size : (size - currentSize));

		// устанавливаем дату модификации
		SetEntryWriteDate(_cluster.CurrentRecord());

		if (!_cluster.Save())
			return 0;

		// обновляем хранимую об открытом файле информацию
		info->SetSize(_cluster.CurrentRecord().Size());
		info->SetPos(0);

		return _cluster.CurrentRecord().Size();
	}

	//
	// считать данные из файла
	//

	unsigned long ReadFile(unsigned long cluster, unsigned char* buffer, unsigned long len)
	{
		ClearError();

		FileInfo* info = GetFileInfo(cluster);

		if (info == 0 || len == 0)
			return 0;

		if (len > info->MaxReadLen()) // читать можем только до конца файла
			len = info->MaxReadLen();

		cluster = GetClusterForPos(cluster, info->LastPos(), info->LastCluster(), info->Pos()); // кластер, начиная с которого будем читать
		if (cluster == 0)
			return 0;

		info->SaveStartPos(cluster); // сохраняем позицию кластера в истории

		unsigned short offset = GetOffsetForPos(info->Pos()); // смещение в кластере, с которого будем читать
		unsigned long remainingLen = len; // сколько осталось прочитать

		while(cluster && remainingLen)
		{
			info->SavePostion(cluster); // сохраняем пару позиция-кластер

			if (!SetCluster(cluster))
				return 0;

			signed short readRes = _cluster.Read(buffer + len - remainingLen, (remainingLen > _clusterSize) ? _clusterSize : (unsigned short) remainingLen, offset);

			if (readRes < 0)
			{
				SetError(Error::IOFault);
				return 0;
			}

			info->IncPos(readRes); // обновляем текущую позицию

			remainingLen -= readRes;
			offset = 0; // обнуляем смещение (следующие кластера точно нужно читать с начала)
			cluster = NextCluster(cluster);
		}

		return len - remainingLen; // возвращаем количество прочитанных байтов
	}

	//
	// записать данные в файл
	//

	unsigned long WriteFile(unsigned long cluster, unsigned char* buffer, unsigned long len)
	{
		ClearError();

		FileInfo* info = GetFileInfo(cluster);
		if (info == 0 || len == 0)
			return 0;

		cluster = GetOrCreateClusterForPos(cluster, info->LastPos(), info->LastCluster(), info->Pos());  // кластер, начиная с которого будем писать
		if (cluster == 0)
			return 0;

		info->SaveStartPos(cluster); // сохраняем позицию кластера в истории

		unsigned short offset = GetOffsetForPos(info->Pos()); // смещение в кластере, с которого будем писать
		unsigned long remainingLen = len; // сколько ещё осталось записать

		while(cluster)
		{
			info->SavePostion(cluster); // сохраняем пару позиция-кластер

			if (!SetCluster(cluster))
				return 0;

			signed short writeRes = _cluster.Write(buffer + len - remainingLen, (remainingLen > _clusterSize) ? _clusterSize : (unsigned short)remainingLen, offset);
			if (writeRes < 0)
			{
				SetError(Error::IOFault);
				return 0;
			}

			info->IncPosAndSize(writeRes); // обновляем текущую позицию и размер, если требуется
			offset = 0; // обнуляем смещение (следующие кластера точно нужно писать с начала)
			remainingLen -= writeRes;

			if (remainingLen == 0) // если всё записали, выходим
				break;

			cluster = GetOrCreateNextCluster(cluster);
		}

		return len - remainingLen; // возвращаем количество записанных байтов
	}

	//
	// создать каталог
	//

	bool CreateDirectory(char* path)
	{
		ClearError();

		unsigned long parentCluster = CreateNewDirEntry(path);

		if (parentCluster == 0)
			return false;

		DirEntry& entry = _cluster.CurrentRecord();
		entry.SetDirectoryAttr();
		SetEntryCreateDate(entry); // ставим дату создания
		if (!_cluster.Save())
			return false;

		unsigned long newDirCluster = entry.Cluster();

		if (!SetCluster(newDirCluster))
			return false;

		if (!_cluster.Clear()) //?
			return false;

		// добавляем каталоги "." и ".."
		_cluster.Record(0).SetAsCurrentDirEntry(newDirCluster);
		_cluster.Record(1).SetAsParentDirEntry((parentCluster == _rootCluster) ? 0 : parentCluster);
		// ставим правильную дату создания
		SetEntryCreateDate(_cluster.Record(0));
		SetEntryCreateDate(_cluster.Record(1));

		return _cluster.SaveToSector(0);
	}

	//
	// Узнать размер ОТКРЫТОГО файла
	//

	unsigned long FileSize(unsigned long cluster)
	{
		ClearError();

		FileInfo* info = GetFileInfo(cluster);

		if (info == 0)
			return -1;

		return info->Size();
	}

	//
	// Сбрасывает несохранённые изменения на носитель
	//

	inline bool Flush(unsigned long cluster)
	{
		ClearError();

		if (!FlushClusterChain()) // FAT
		{
			RollbackFileSize(cluster);
			return false;			
		}

		if (!FlushFileInfo(cluster)) // размер файла
		{
			return false;
		}

		if (!_cluster.Flush()) // могли остаться несохранённые данные в _cluster
			return false;

		return FlushFsInfo(); // количество свободных кластеров
	}

	//
	// Сбрасывает несохранённые изменения общих данных
	//
	inline bool Flush()
	{
		ClearError();
		if (!_cluster.Flush()) // могли остаться несохранённые данные в _cluster
			return false;
		if (!FlushClusterChain()) // FAT
			return false;
		return FlushFsInfo(); // количество свободных кластеров
	}

	//
	// возвращает итератор для перебора содержимого каталога
	//
	DirIterator OpenDir(char* path)
	{
		if (Path::IsRoot(path)) // если нужен корневой каталог
			return DirIterator(*this, _rootCluster);

		if (!FindEntry(path, false)) // нашли запись или нет?
			return DirIterator(*this); // возвращаем невалидный итератор

		if (_cluster.CurrentRecord().File()) // заданный путь соответствует файлу?
			return DirIterator(*this); // возвращаем невалидный итератор

		return DirIterator(*this, _cluster.CurrentRecord().Cluster());
	}

	//
	// удaлить файл
	//
	bool DeleteFile(char* path)
	{
		ClearError();

		if (!FindEntry(path, true)) // ищем запись о файле
			return false;

		if (!FreeChain(_cluster.CurrentRecord().Cluster())) // освобождаем цепочку кластеров файла
			return false;

		UnregisterFile(_cluster.CurrentRecord().Cluster()); // закрываем файл, если открыт

		// далее освобождаем все записи о файле (короткую и длинные, если есть)

		unsigned long emptyCount = _cluster.LongCount() + 1; // длинные и одна короткая

		// освобождаем длинные записи (могут быть в 2-х кластерах, на их границе)
		do
		{
			for (signed short r = _cluster.CurrentRecordNum(); r >=0 && emptyCount; r--, emptyCount--)
			{
				DirEntry& entry = _cluster.Record(r);
				if (!entry.Valid())
				{
					SetError(Error::IOFault);
					return false;
				}
				_cluster.Record(r).SetEmpty();
				_cluster.SetModified();
			}
			// если не всё очистили, переключаемся на другой кластер
			if (emptyCount && !SetClusterToLastRecord(_cluster.EntryStartCluster()))
				return false;
		} while(emptyCount);

		return true;
	}

protected:

	//
	// инициализация
	//

	// инициализация, в _bootSectorTmp уже загружен загрузочный сектор
	bool InitFromBootSector(unsigned long offset)
	{
		_rootCluster = _bootSectorTmp.RootCluster();
		_clusterSize = _bootSectorTmp.ClusterSize();
		_clusterCount = _bootSectorTmp.ClusterCount();

		FileInfo::SetClustrerSize(_clusterSize);

		if (!_bootSectorTmp.Valid())
			return false;

		if (!InitClusterChain(offset))
			return false;

		_cluster.Init((offset + _bootSectorTmp.FirstDataSectorOffset()) / _sectorSize
							, _clusterSize / _sectorSize);

		// считываем структуру (нам нужно количество свобоных кластеров),
		// переписываем неподдерживаемые в актуальном состоянии поля FSInfo неизвестными значениями
		return ResetFsInfo(offset);
	}

	// переписывает FSInfo неизвестными значениями
	bool ResetFsInfo(unsigned long offset)
	{
		_fsInfoSector = offset / _sectorSize + _bootSectorTmp.FsInfoSector(); // запоминаем сектор структуры

		// считываем FSInfo
		// !!! тут _bootSectorTmp становится неактуальным (т.к. всё в 1-м union-е) !!!
		if (!_provider.ReadSector(_fsInfoSector, (unsigned char*)&_fsInfo)) // считываем FSInfo
			return false;

		if (!_fsInfo.Valid())
			_fsInfo.Reset(); // если кривая, то приводим в нормальный вид

		// если количество свободных кластеров недействительно и диск пустой после форматирования,
		// устанавливаем новое значение
		if (_fsInfo.FreeCount() > _clusterCount && DiskEmptyAfterFormat())
			_fsInfo.SetFreeCount(_clusterCount - 1); // занят только один кластер (под корневой каталог)

		if (LastOperationError()) // если была ошибка ввода/вывода в DiskEmptyAfterFormat
			return false;

		// сохраняем изменения
		if (!_provider.WriteSector(_fsInfoSector, (unsigned char*)&_fsInfo))
			return false;

		// если знаения полей допустимы, то передаём их в ClusterChain
		if (_fsInfo.FreeCount() <= _clusterCount)
			_clusterChain.SetFreeClustersCount(_fsInfo.FreeCount());
		if (_fsInfo.NextFree() <= _clusterCount)
			_clusterChain.SetNextFree(_fsInfo.NextFree());

		return true;
	}

	bool InitClusterChain(unsigned long offset)
	{
		_clusterChain.SetProvider(&_provider);
		_clusterChain.Init((offset + _bootSectorTmp.FatOffset(0)) / _sectorSize
							, _bootSectorTmp.FatSizeInBytes() / _sectorSize
							, _bootSectorTmp.FatsCount());

		return true;
	}

	//
	// поиск записи
	//

	// поиск записи, соответствующей заданному пути
	bool FindEntry(char* path, bool file)
	{
		PathIterator iter(path);
		unsigned long cluster = _rootCluster;

		for(; !iter.End(); iter.Next())
		{
			if (!FindEntryInDir(cluster, iter.Name(), (unsigned char) iter.NameLength()))
				return false;

			cluster = _cluster.CurrentRecord().Cluster();
		}

		return file && iter.File() && _cluster.CurrentRecord().File()
				|| !file && _cluster.CurrentRecord().Directory();
	}

	// поиск или создание записи, соответствующей заданному пути
	bool FindOrCreateEntry(char* path, bool file)
	{
		PathIterator iter(path);
		unsigned long cluster = _rootCluster;

		for(; !iter.End(); iter.Next())
		{
			if (!FindEntryInDir(cluster, iter.Name(), (unsigned char) iter.NameLength()))
			{
				if (LastOperationError()) // была ошибка
					return false;

				// информация о последнем имени не меняется, если в iter.Next() не найден следующий элемент
				iter.Next();
				if (!iter.End() || (file && !iter.File()))
					return false;
				// iter.Name() не меняется, если в iter.Next() не найден следующий элемент
				return CreateEntry(cluster, iter.Name(), (unsigned char)iter.NameLength());
			}

			cluster = _cluster.CurrentRecord().Cluster();
		}

		return file && iter.File() && _cluster.CurrentRecord().File() // надо было файл, задали путь к файлу, и на диске файл
				|| !file && _cluster.CurrentRecord().Directory(); // надо было папку, и на диске папка
	}

	// создание записи, соответствующей заданному пути
	// если запись существует, вернём ошибку
	bool CreateEntry(char* path, bool file)
	{
		PathIterator iter(path);
		unsigned long cluster = _rootCluster;

		for(; !iter.End(); iter.Next())
		{
			if (!FindEntryInDir(cluster, iter.Name(), (unsigned char) iter.NameLength()))
			{
				if (LastOperationError()) // была ошибка
					return false;

				// информация о последнем имени не меняется, если в iter.Next() не найден следующий элемент
				iter.Next();
				if (!iter.End() || (file && !iter.File()))
					return false;
				// iter.Name() не меняется, если в iter.Next() не найден следующий элемент
				return CreateEntry(cluster, iter.Name(), (unsigned char)iter.NameLength());
			}

			cluster = _cluster.CurrentRecord().Cluster();
		}

		return false;
	}

	// создаёт запись для новой директории, возвращает первый кластер директории родителя или 0
	unsigned long CreateNewDirEntry(char* path)
	{
		PathIterator iter(path);
		unsigned long cluster = _rootCluster;

		for(PathIterator iter(path); !iter.End(); iter.Next())
		{
			if (!FindEntryInDir(cluster, iter.Name(), (unsigned char) iter.NameLength()))
			{
				if (LastOperationError()) // была ошибка
					return 0;

				// информация о последнем имени не меняется, если в iter.Next() не найден следующий элемент
				if (iter.Next(), !iter.End())
					return 0;

				// iter.Name() не меняется, если в iter.Next() не найден следующий элемент
				if (!CreateEntry(cluster, iter.Name(), (unsigned char)iter.NameLength()))
					return 0;

				return cluster;
			}

			cluster = _cluster.CurrentRecord().Cluster();
		}

		SetError(Error::AlreadyExist);

		return 0;
	}

	// поиск записи в каталоге по имени
	bool FindEntryInDir(unsigned long cluster, char* name, unsigned char len)
	{
		unsigned char checkedLen = 0;

		while(cluster)
		{
			WdtClear();

			if (!SetCluster(cluster))
				return false;

			if (_cluster.FindEntry(name, len, checkedLen))
				return true; // результат поиска в _cluster.CurrentRecord()

			if (!_cluster.Valid()) // если тут кластер невалидный, значит была ошибка чтения
			{
				SetError(Error::IOFault);
				return false;
			}

			cluster = NextCluster(cluster);
		}

		return false;
	}

	// поиск места для нужного количества записей (если не найдёт в середине, то в конеце каталога)
	bool FindEmptyBlock(unsigned long cluster, unsigned short cnt)
	{
		unsigned short foundCnt = 0;

		unsigned long startCluster = 0;
		unsigned short startRecord = 0;
		unsigned short foundSummary = 0;

		unsigned long lastDirCluster = 0;

		while(cluster)
		{
			if (!SetCluster(cluster))
				return false;

			foundCnt = _cluster.FindEmptyBlock(cnt);

			// если не нашли совсем ничего
			if (foundCnt == 0)
			{
				if (!_cluster.Valid()) // если тут кластер невалидный, значит была ошибка чтения
					return false;
				foundSummary = 0;
			}
			else // нашли
			{
				// если ещё не находили или сейчас нашли не в начале сектора (т.е. начало новой послоедовательности)
				if (!foundSummary || _cluster.CurrentRecordNum() != 0)
				{
					startCluster = cluster;
					startRecord = _cluster.CurrentRecordNum();
					foundSummary = foundCnt;
				}
				else
				{
					foundSummary += foundCnt;
				}

				// найден нужный кусок
				if (foundSummary >= cnt)
				{
					if (!SetCluster(startCluster, startRecord))
						return false;
					return true; // результат поиска в _cluster.CurrentRecord()
				}
			}

			lastDirCluster = cluster;
			cluster = NextCluster(cluster);

			if (LastOperationError()) // была ошибка IO в NextCluster
				return false;
		}

		if (foundCnt)
		{
			if (!SetCluster(startCluster, startRecord))
				return false;
			return AddNextCluster(lastDirCluster, true); // добавляем новый кластер
		}
		else
			return AddNextClusterAndSet(lastDirCluster, true); // добавляем новый кластер, текущая запись в нём 0
	}

	//
	// добавление нового кластера в цепочку
	//
	
	// добавить новый кластер
	unsigned long AddClusterToChain(unsigned long cluster)
	{
		unsigned long freeCluster = FindFreeCluster();

		if (freeCluster == 0)
			return 0;

		if (!SetNextCluster(cluster, freeCluster)) // сразу не сохраняем
			return 0;

		if (!SetClusterEoc(freeCluster)) // тут тоже теперь не сохраняем
			return 0;

		return freeCluster;
	}

	// добавить кластер
	// почистить его, если надо
	// установить на него _cluster, если надо
	bool AddNextCluster(unsigned long cluster, bool clear, bool set)
	{
		unsigned long newCluster = AddClusterToChain(cluster);

		if (newCluster == 0)
			return false;

		if (set)
		{
			if (!SetCluster(newCluster, 0))
				return false;
		}

		if (clear)
		{
			if (!_cluster.Clear(newCluster))
				return false;
		}

		return true;
	}

	// добавить кластер
	inline bool AddNextCluster(unsigned long cluster, bool clear)
	{
		return AddNextCluster(cluster, clear, false);
	}

	// добавить кластер, не чистить
	inline bool AddNextCluster(unsigned long cluster)
	{
		return AddNextCluster(cluster, false, false);
	}

	// добавить кластер и установить на него _cluster
	inline bool AddNextClusterAndSet(unsigned long cluster, bool clear)
	{
		return AddNextCluster(cluster, clear, true);
	}

	// добавить кластер и установить на него _cluster, не чистить
	inline bool AddNextClusterAndSet(unsigned long cluster)
	{
		return AddNextCluster(cluster, false, true);
	}

	// возвращает имеющийся или свежедобавленный следующий кластер
	inline unsigned long GetOrCreateNextCluster(unsigned long cluster)
	{
		unsigned long clusterTmp = NextCluster(cluster);

		if (LastOperationError()) // была ошибка IO в NextCluster
			return 0;

		if (clusterTmp)
			return clusterTmp;

		return AddClusterToChain(cluster);
	}

	//
	// модификация записи о файле
	//
	bool SetNewFileSize(FileInfo* info)
	{
		if (info->SizeFixed())
			return true;

		if (!SetCluster(info->RecordCluster(), info->Record()))
			return false;

		// берём запись о файле
		DirEntry& fileEntry = _cluster.CurrentRecord();
		if (fileEntry.Valid()) // если считалась нормально
		{
			fileEntry.SetSize(info->Size()); // ставим новый размер
			SetEntryWriteDate(fileEntry); // ставим дату модификации
			if (_cluster.Save())
			{
				info->FixSize(); // запоминаем последний удачно сохранённый размер файла
				return true;
			}
		}

		info->RollbackSize(); // меняем размер на последний удачно сохранённый

		return false;
	}

	// очистить файл, которому соответствует текущая запись в _cluster
	bool ClearCurrentFile()
	{
		_cluster.CurrentRecord().SetSize(0);
		SetEntryWriteDate(_cluster.CurrentRecord()); // ставим дату модификации

		if (!_cluster.Save())
			return false;

		return SetClusterChainEocAndSave(_cluster.CurrentRecord().Cluster());
	}

	//
	// срздание записи
	//

	bool CreateEntry(unsigned long cluster, char* name, unsigned char len)
	{
		Name::Type type = Name::Check(name, len);

		if (type == Name::NotValid)
			return false;

		// короткое имя

		// корооткая запись

		DirEntry shortEntry;
		shortEntry.Clear();

		// устанавливаем дату и время создания записи
		SetEntryCreateDate(shortEntry);

		if (type == Name::Short)
		{
			shortEntry.SetShortName(name, len);
		}
		else
		{
			char shortName[13];
			unsigned char shortNameLen = Name::ToShort(name, len, shortName);
			shortEntry.SetShortName(shortName, shortNameLen);
		}

		unsigned char checkSum = shortEntry.CheckSumShort();

		unsigned char longEntriesCnt = (type == Name::Short) ? 0 : LongNamePart::Count(len);

		if (!FindEmptyBlock(cluster, longEntriesCnt + 1)) // результат в _cluster
			return false;

		cluster = _cluster.Number();

		bool setLast = true;

		LongNamePart part(name, len);

		while(cluster)
		{
			unsigned short r = 0;

			for(r = _cluster.CurrentRecordNum(); r < _cluster.RecordsPerCluster() && longEntriesCnt > 0; r++, longEntriesCnt--)
			{
				DirEntry& record = _cluster.Record(r);
				record.Clear();

				record.SetLongNameAttr();
				record.SetOrder(longEntriesCnt);
				record.SetCheckSum(checkSum);
				record.SetLongName(part.GetUnicode(), part.Length());
				if (setLast)
				{
					record.SetLast();
					setLast = false;
				}

				part.Next();
				_cluster.SetModified();
			}

			if (!_cluster.Flush())
				return false;

			// переключаемся на первую запись следующего кластера
			if (r == _cluster.RecordsPerCluster())
			{
				cluster = NextCluster(cluster);
				if (LastOperationError()) // была ошибка IO в NextCluster
					return false;
				if (!SetCluster(cluster, 0)) // установка на первую запись следующего кластера
					return false;
			}
			else
			{
				if (!SetCluster(cluster, r)) // установка на следующую запись в этом кластере
					return false;
			}

			// если заполнили все длинные записи
			if (longEntriesCnt == 0)
				break;
		}

		unsigned long newCluster = FindFreeCluster();
		if (newCluster == 0)
			return false;

		if (!SetClusterEoc(newCluster))
			return false;

		shortEntry.SetCluster(newCluster);
		_cluster.CurrentRecord() = shortEntry;
		_cluster.SetModified();

		return _cluster.Flush();
	}

	//
	// работа с файлами
	//

	// зарегать открытый файл
	// cluster - первый кластер файла
	// recCluster - кластер с записью о файле
	// rec - запись о файле в recCluster
	// size - размер файла
	bool RegisterFile(unsigned long cluster, unsigned long recCluster, unsigned short rec, unsigned long size, IOFlags::T flags)
	{
		typename OpenedFilesMap::iterator iter = _openedFilesMap.find(cluster);
		
		if (iter != _openedFilesMap.end())
			return false;

		return _openedFilesMap.insert(OpenedFilesMap::value_type(cluster, FileInfo(recCluster, rec, size, flags))).second;
	}

	bool UnregisterFile(unsigned long cluster)
	{
		typename OpenedFilesMap::iterator iter = _openedFilesMap.find(cluster);

		if (iter == _openedFilesMap.end())
			return false;

		_openedFilesMap.erase(iter);

		return true;
	}

	FileInfo* GetFileInfo(unsigned long cluster)
	{
		typename OpenedFilesMap::iterator iter = _openedFilesMap.find(cluster);

		if (iter == _openedFilesMap.end())
			return 0;

		return &iter->second;
	}

	void RollbackFileSize(unsigned long cluster)
	{
		FileInfo* info = GetFileInfo(cluster);
		if (!info)
			return;
		info->RollbackSize();
		if (info->LastPos() >= 65536)
			info = 0;
	}

	//
	// параллельная модификация FAT таблиц
	//

	// помечает кластер как последний в цепочке без сохранения на носитель
	inline bool SetClusterEoc(unsigned long cluster)
	{
		if (!_clusterChain.SetEoc(cluster))
			return false;

		return true;
	}

	inline bool SetClusterEocAndSave(unsigned long cluster)
	{
		if (!_clusterChain.SetEocAndSave(cluster))
			return false;

		return true;
	}

	inline bool SetClusterChainEocAndSave(unsigned long cluster)
	{
		if (!_clusterChain.SetChainEocAndSave(cluster))
			return false;

		return true;
	}

	inline bool SetNextCluster(unsigned long cluster, unsigned long nextCluster)
	{
		if (!_clusterChain.SetNextCluster(cluster, nextCluster))
			return false;

		return true;
	}

	inline bool FreeChain(unsigned long cluster)
	{
		if (!_clusterChain.FreeChain(cluster))
		{
			SetError(Error::IOFault);
			return false;
		}
		return true;
	}

	//
	// сброс несохранённых изменений на диск
	//

	// сохраняет изменения в FAT таблицах, если они есть
	inline bool FlushClusterChain()
	{
		if (!_clusterChain.Flush())
		{
			SetError(Error::IOFault);
			return false;
		}

		return true;
	}

	// сохраняет изменённую информацию о файле
	// фактически сохраняет только новый размер файла
	inline bool FlushFileInfo(unsigned long cluster)
	{
		FileInfo* info = GetFileInfo(cluster);

		if (info == 0)
			return false;

		if (!SetNewFileSize(info))
		{
			SetError(Error::IOFault);
			return false;
		}

		return true;
	}

	inline bool FlushFsInfo()
	{
		bool modified = false;
		// если поле было FreeCount неактуально при инициализации, то и менять его не надо,
		// т.к. мы не знаем сколько места на самом деле свободно;
		// если значение не поменялось, то тоже менять и сохранять не надо
		if (_fsInfo.FreeCount() <= _clusterCount && _clusterChain.FreeClusters() != _fsInfo.FreeCount())
		{
			_fsInfo.SetFreeCount(_clusterChain.FreeClusters()); // обновляем количество свободных кластеров
			modified = true;
		}

		// значение NextFree у нас правильное в любом случае,
		// если не изменилось, не сохраняем
		if (_fsInfo.NextFree() != _clusterChain.NextFree())
		{
			_fsInfo.SetNextFree(_clusterChain.NextFree());
			modified = true;
		}

		// если нет изменений,
		if (modified == false)
			return true; // не надо ничего сохранять, выходим

		// сохраняем изменения
		if (!_provider.WriteSector(_fsInfoSector, (unsigned char*)&_fsInfo))
		{
			SetError(Error::IOFault);
			return false;
		}

		return true;
	}

	//
	// Информация из FAT таблицы
	//

	inline bool ClusterEoc(unsigned long cluster)
	{
		if (_clusterChain.ClusterEoc(cluster))
			return true;
		if (_clusterChain.LastOperationFailed())
			SetError(Error::IOFault);
		return false;
	}

	inline unsigned long NextCluster(unsigned long cluster)
	{
		signed long res = _clusterChain.NextCluster(cluster);
		
		if (res < 0)
		{
			SetError(Error::IOFault);
			return 0;
		}

		return res;
	}

	inline unsigned long FindFreeCluster()
	{
		return _clusterChain.FindFreeCluster();
	}

	//
	// найти (или создать) кластер в цепочке, соответствующий позиции
	//

	inline unsigned long GetClusterForPos(unsigned long cluster, unsigned long lastPos, unsigned long lastCluster, unsigned long newPos)
	{
		return GetClusterForPos(cluster, lastPos, lastCluster, newPos, false);
	}

	inline unsigned long GetOrCreateClusterForPos(unsigned long cluster, unsigned long lastPos, unsigned long lastCluster, unsigned long newPos)
	{
		return GetClusterForPos(cluster, lastPos, lastCluster, newPos, true);
	}

	unsigned long GetClusterForPos(unsigned long cluster
									, unsigned long lastPos
									, unsigned long lastCluster
									, unsigned long newPos
									, bool AddClusterIfNeed)
	{
		if (lastPos && newPos >= lastPos && lastCluster > 0)
		{
			cluster = lastCluster;
			newPos -= lastPos - lastPos % _clusterSize;
		}

		unsigned long clusterPrev = 0;

		while(newPos >= _clusterSize && cluster)
		{
			clusterPrev = cluster;
			cluster = NextCluster(cluster);

			if (LastOperationError()) // была ошибка IO в NextCluster
				return 0;

			newPos -= _clusterSize;
		}

		if (cluster)
			return cluster;

		return (AddClusterIfNeed && newPos < _clusterSize) ? AddClusterToChain(clusterPrev) : 0;
	}

	// смещение в кластере для позиции
	inline unsigned short GetOffsetForPos(unsigned long pos)
	{
		return (unsigned short)(pos % _clusterSize);
	}

	// Модификация полей даты и времени для записи

	void SetEntryCreateDate(DirEntry& entry)
	{
		typename DateTimeProvider::DateType date = DateTimeProvider::GetDate();
		entry.SetCreateDateWise(date.Year(), date.Month(), date.DayOfMonth());

		typename DateTimeProvider::TimeType time = DateTimeProvider::GetTime();
		entry.SetCreateTimeWise(time.Hours(), time.Minutes(), time.Seconds());
	}

	void SetEntryWriteDate(DirEntry& entry)
	{
		typename DateTimeProvider::DateType date = DateTimeProvider::GetDate();
		entry.SetWriteDateWise(date.Year(), date.Month(), date.DayOfMonth());

		typename DateTimeProvider::TimeType time = DateTimeProvider::GetTime();
		entry.SetWriteTime(time.Hours(), time.Minutes(), time.Seconds());
	}

	void SetEntryAccessDate(DirEntry& entry)
	{
		typename DateTimeProvider::DateType date = DateTimeProvider::GetDate();
		entry.SetLastAccessDate(date.Year(), date.Month(), date.DayOfMonth());
	}

	//
	// пустой диск после форматирования?
	// это так, если первая запись корневого каталого является пустой и после неё нет занятых записей
	// и в цепочке кластеров каталога только один кластер
	//
	bool DiskEmptyAfterFormat()
	{
		ClearError();

		if (!SetCluster(_rootCluster))
			return false;
		DirEntry& rec = _cluster.Record(0);

		if (!rec.Valid()) // была ошибка ввода/вывода
		{
			SetError(Error::IOFault);
			return false;
		}

		if (!rec.LastInDir())
			return false;

		return ClusterEoc(_rootCluster) && !LastOperationError();
	}

	//
	// устанавка объекта _cluster на нужный кластер и запись с контролем ошибок
	//
	inline bool SetCluster(unsigned long cluster, unsigned short record = 0)
	{
		if (!_cluster.Set(cluster, record))
		{
			SetError(Error::IOFault);
			return false;
		}
		return true;
	}

	inline bool SetClusterToLastRecord(unsigned long cluster)
	{
		if (!_cluster.SetToLastRecord(cluster))
		{
			SetError(Error::IOFault);
			return false;
		}
		return true;
	}

	//
	// ошибки
	//

	inline void ClearError()
	{
		_lastOperationError = Error::None;
	}

	inline void SetError(ErrorType err)
	{
		_lastOperationError = err;
	}

	//
	// сброс сторожевого таймера
	//
	inline void WdtClear()
	{
		if (_wdtClear)
			_wdtClear();
	}

private:

	OpenedFilesMap _openedFilesMap;

	// в основном эти поля актуальны только при инициализации
	// использовать можно только _fsInfo
	union
	{
		Mbr _mbrTmp;
		BootSector _bootSectorTmp;
		FsInfo _fsInfo;
	};

	DataProvider& _provider;

	DirectoryCluster<DataProvider> _cluster;

	unsigned short _clusterSize;
	unsigned long _rootCluster;
	unsigned long _clusterCount;

	unsigned long _fsInfoSector;

	ClusterChain<DataProvider> _clusterChain; // сама таблица (FAT)

	static const unsigned short _sectorSize = 512;

	ErrorType _lastOperationError;

	WdtClearType _wdtClear; // указатель на функцию сброса сторожевого таймера
};

}
}
}
}

#endif // _MEDIA_FS_FAT32_FAT_H_

