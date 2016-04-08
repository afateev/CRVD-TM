
#ifndef _MEDIA_FS_FAT32_CLASTER_CHAIN_H_
#define _MEDIA_FS_FAT32_CLASTER_CHAIN_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

template<class DataProvider>
class ClusterChain
{
protected:
	typedef void (*WdtClearType)(void);
public:

	ClusterChain()
	: _dataProvider(0)
	, _startSector(0)
	, _size(0)
	, _bufferValid(false)
	, _bufferModified(false)
	, _firstFreeCluster(0)
	, _fatsCount(0)
	, _freeCount(0)
	, _wdtClear(0)
	{
	}


	ClusterChain(DataProvider* provider)
	: _dataProvider(provider)
	, _startSector(0)
	, _size(0)
	, _bufferValid(false)
	, _bufferModified(false)
	, _firstFreeCluster(0)
	, _fatsCount(0)
	, _freeCount(0)
	, _wdtClear(0)
	{
	}

	// входные параметры в скторах DataProvider-а
	ClusterChain(DataProvider* provider, unsigned long startSectorAbsolute, unsigned long size, unsigned long fatsCount)
	: _dataProvider(provider)
	, _startSector(startSectorAbsolute)
	, _size(size)
	, _bufferValid(false)
	, _bufferModified(false)
	, _firstFreeCluster(0)
	, _fatsCount(fatsCount)
	, _freeCount(0)
	, _wdtClear(0)
	{
	}

	// входные параметры в секторах DataProvider-а
	void Init(unsigned long startSectorAbsolute, unsigned long size, unsigned char fatsCount)
	{
		if (_startSector != startSectorAbsolute || _size != size)
		{
			_bufferValid = false;
			_bufferModified = false;
		}

		_startSector = startSectorAbsolute;
		_size = size;
		_firstFreeCluster = 0;
		_fatsCount = fatsCount;
	}

	// входные параметры в секторах DataProvider-а
	// freeCount - количество свободных кластеров
	void Init(unsigned long startSectorAbsolute, unsigned long size, unsigned char fatsCount, unsigned long freeCount)
	{
		_freeCount = freeCount;
		Init(startSectorAbsolute, size, fatsCount);
	}

	inline void SetProvider(DataProvider* provider)
	{
		_dataProvider = provider;
	}

	// задать функцию сброса сторожевого таймера
	void SetWdtClear(WdtClearType wdtClear)
	{
		_wdtClear = wdtClear;
	}

	// установить начальное значение количества свободных кластеров
	// обязательно установить до первой записи (можно в Init)
	inline void SetFreeClustersCount(unsigned long freeCount)
	{
		_freeCount = freeCount;
	}

	// устанавливает номер кластера, с которого нуджно искать свободное место
	inline void SetNextFree(unsigned long nextFreeCluster)
	{
		_firstFreeCluster = nextFreeCluster;
	}

	//
	// статус последней операции
	//
	inline bool LastOperationFailed()
	{
		return _bufferValid == false;
	}

	//
	// получение информации из таблицы
	//

	// количетсво свободных кластеров
	inline unsigned long FreeClusters()
	{
		return _freeCount;
	}

	inline unsigned long NextFree()
	{
		return _firstFreeCluster;
	}

	inline bool ClusterEoc(unsigned cluster)
	{
		if (!PrepareSectorBuffer(cluster))
			return false;

		return _sectorBuffer[ClusterRecordInSector(cluster)] >= _lastInChain;
	}

	// возвращает следующий кластер в цепочке
	inline signed long NextCluster(unsigned cluster)
	{
		if (!PrepareSectorBuffer(cluster))
			return -1;

		unsigned long nextCluster = _sectorBuffer[ClusterRecordInSector(cluster)];

		return (nextCluster < _lastInChain) ? nextCluster : 0;
	}

	// возвращает первый свободный кластер
	inline unsigned long FindFreeCluster()
	{
		return FindNextFreeCluster(_firstFreeCluster);
	}

	// находит следующий свободный кластер
	inline unsigned long FindNextFreeCluster(unsigned long cluster)
	{
		return FindNextFreeCluster(ClusterSector(cluster), ClusterRecordInSector(cluster));
	}

	//
	// изменение таблицы
	//

	// устанавливает новое значение следующего кластера для заданного кластера
	// на диск сразу не сохраняет
	inline bool SetNextCluster(unsigned long parent, unsigned long child)
	{
		return UpdateClusterRecord(parent, child);
	}

	// устанавливает новое значение следующего кластера для заданного кластера
	// сразу сохраняет на диск
	inline bool SetNextClusterAndSave(unsigned long parent, unsigned long child)
	{
		return UpdateClusterRecordAndSave(parent, child);
	}

	// помечает кластер как свободный
	// на диск сразу не сохраняет
	inline bool FreeCluster(unsigned long cluster)
	{
		return UpdateClusterRecord(cluster, _freeCluster);
	}

	// помечает кластер как свободный
	// сразу сохраняет на диск
	inline bool FreeClusterAndSave(unsigned long cluster)
	{
		return FreeCluster(cluster);
	}

	// помечает кластер как последний в цепочке
	// на диск сразу не сохраняет
	inline bool SetEoc(unsigned long cluster)
	{
		return UpdateClusterRecord(cluster, _lastInChain);
	}

	// помечает кластер как последний в цепочке
	// сразу сохраняет на диск
	inline bool SetEocAndSave(unsigned long cluster)
	{
		return UpdateClusterRecordAndSave(cluster, _lastInChain);
	}

	// оставляет только первый кластер в цепочке
	inline bool SetChainEocAndSave(unsigned long cluster)
	{
		return FreeChainAndSave(cluster, false);
	}

	// освобождает всю цепочку кластеров
	inline bool FreeChain(unsigned long cluster)
	{
		return FreeChain(cluster, true);
	}

	// освобождает всю цепочку кластеров и всё сохраняет
	inline bool FreeChainAndSave(unsigned long cluster)
	{
		return FreeChainAndSave(cluster, true);
	}

	// сохраняет содержимое сектора из буфера на диск, если буфер модифицировался
	inline bool Flush()
	{
		if (_bufferModified)
			return SaveBuffer();
		return true;
	}

protected:

	//
	// вычисление индексов
	//

	// возвращает индекс сектора для данного кластера (относительно начала FAT, не абсолютный)
	inline unsigned long ClusterSector(unsigned long cluster)
	{
		return cluster / _recordsInSector;
	}

	// индекс записи заданного кластера в секторе
	inline unsigned long ClusterRecordInSector(unsigned long cluster)
	{
		return cluster % _recordsInSector;
	}

	//
	// чтение и запись секторов
	//

	// сохраняет сожержимое сектора из буфера на диск
	bool SaveBuffer()
	{
		if (!_bufferValid)
			return false;

		if (_dataProvider == 0)
			return false;

		for(unsigned char i = 0; i < _fatsCount; i++)
		{
			if (!_dataProvider->WriteSector(_startSector + i * _size + _sectorInBuffer, (unsigned char*) _sectorBuffer))
			{
				_bufferModified = false;
				_bufferValid = false;
				return false;
			}
		}

		_bufferModified = false;

		return true;
	}

	// считывает указанный сектор FAT в буфер
	bool UpdateBuffer(unsigned long sector)
	{
		if (_bufferModified)
			if (!SaveBuffer())
				return false;

		if (_dataProvider == 0)
			return false;

		_bufferModified = false;
		_bufferValid = false;

		if (!_dataProvider->ReadSector(_startSector + sector, (unsigned char*) _sectorBuffer))
			return false;

		_sectorInBuffer = sector;
		_bufferValid = true;

		return true;
	}

	// Считывает в буфер сектор с записью о заданном кластере (если в буфере уже не лежит этот кластер)
	bool PrepareSectorBuffer(unsigned long cluster)
	{
		unsigned long sector = ClusterSector(cluster);
		if (_sectorInBuffer == sector && _bufferValid)
			return true;

		return UpdateBuffer(sector);
	}

	// Считывает в буфер заданный сектор (если в буфере уже не лежит этот кластер)
	bool PrepareBuffer(unsigned long sector)
	{
		if (_sectorInBuffer == sector && _bufferValid)
			return true;

		return UpdateBuffer(sector);
	}

	//
	// получение информации из таблицы
	//

	// находит следующий свободный кластер, начиная с указанной записи в указанном секторе
	unsigned long FindNextFreeCluster(unsigned long sector, unsigned long record)
	{
		// перебираем сектора FAT
		for (unsigned long s = sector; s < _size; s++)
		{
			WdtClear(); // сброс сторожевого таймера, т.к. процедура может занять много времени

			if (!PrepareBuffer(s))
				return 0;

			for(unsigned long r = record; r < _recordsInSector; r++)
			{
				if (_sectorBuffer[r] == _freeCluster)
					return (_firstFreeCluster = s * _recordsInSector + r);
			}
			
			record = 0;
		}

		return (_freeCount = _firstFreeCluster = 0); // не нашли
	}

	//
	// изменение таблицы
	//

	// модифицирует запись для заданного кластера
	// на диск сразу не сохраняет
	bool UpdateClusterRecord(unsigned long cluster, unsigned long value)
	{
		if (!PrepareSectorBuffer(cluster))
			return false;

		unsigned long valuePrev = _sectorBuffer[ClusterRecordInSector(cluster)];
		_sectorBuffer[ClusterRecordInSector(cluster)] = value;

		if (valuePrev != value)
			_bufferModified = true;

		// если кластер освободился, то обновляем позицию первого свободного кластера, если надо
		if (value == _freeCluster && cluster < _firstFreeCluster)
			_firstFreeCluster = cluster;

		UpdateFreeSpace(valuePrev, value);

		return true;
	}

	// модифицирует запись для заданного кластера
	// сразу сохраняет на диск
	inline bool UpdateClusterRecordAndSave(unsigned long cluster, unsigned long value)
	{
		if (!UpdateClusterRecord(cluster, value))
			return false;
		if (!Flush())
			return false;
		return true;
	}

	//
	// обновляет количество свободных кластеров
	// в зависимости от старого и нового состояния кластера
	//
	void UpdateFreeSpace(unsigned long oldValue, unsigned long newValue)
	{
		if (oldValue == _freeCluster) // если кластер был пустым
		{
			if (newValue != _freeCluster && _freeCount) // и стал не пустым
				_freeCount--; // то свободных кластеров стало на один меньше
		}
		else // если кластер не был пустым
		{
			if (newValue == _freeCluster) // и стал пустым
				_freeCount++; // то свободных кластеров стало на один больше
		}
	}

	// освобождает цепочку кластеров
	// если total = true, освобождает всю цепочку
	// если total = false, первый кластер остаётся занятым (получает значение EOC)
	bool FreeChain(unsigned long cluster, bool total)
	{
		signed long nextFreeCluster = NextCluster(cluster); // запоминаем следующий за кластер
		if (nextFreeCluster < 0)
			return false; // ошибка IO
		// устанавливаем нужное значение в первый кластер цепочки
		UpdateClusterRecord(cluster, total ? _freeCluster : _lastInChain); // тот же кластер, поэтому ошибку не проверяем

		// освобождаем остальные кластера цепочки
		cluster = nextFreeCluster;
		while(cluster)
		{
			if ((nextFreeCluster = NextCluster(cluster)) < 0)
				return false; // ошибка IO
			FreeCluster(cluster); // тот же кластер, поэтому ошибку не проверяем
			cluster = nextFreeCluster;
		}

		return true;
	}

	bool FreeChainAndSave(unsigned long cluster, bool total)
	{
		if (!FreeChain(cluster, total))
			return false;
		return Flush();
	}

	// сброс сторожевого таймера
	inline void WdtClear()
	{
		if (_wdtClear)
			_wdtClear();
	}

private:

	DataProvider* _dataProvider;
	unsigned long _startSector; // номер начального сектора "в терминах" DataProvider
	unsigned long _size; // размер таблицы в секторах "в терминах" DataProvider
	unsigned char _fatsCount; // количество таблиц (обычно 2)

	// буфер для считывания сектора
	static const unsigned long _sectorSize = 512; // размер сектора "в терминах" DataProvider
	static const unsigned long _recordsInSector = _sectorSize / 4;
	unsigned long _sectorBuffer[_recordsInSector];
	unsigned long _sectorInBuffer;
	bool _bufferValid;
	bool _bufferModified;

	unsigned long _firstFreeCluster;
	unsigned long _freeCount;

	WdtClearType _wdtClear;

	// значения в таблице
	static const unsigned long _lastInChain = 0x0FFFFFF8;
	static const unsigned long _freeCluster = 0;
};

}
}
}
}

#endif // _MEDIA_FS_FAT32_CLASTER_CHAIN_H_

