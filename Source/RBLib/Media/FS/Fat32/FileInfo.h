
#ifndef _MEDIA_FS_FAT32_FILE_INFO_H_
#define _MEDIA_FS_FAT32_FILE_INFO_H_

#include "PositionHistory.h"
#include "../IoFlags.h"

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

//
// Информация об открытом файле
//
// надо использовать имя класса FileInfo (без подчёркивания), которое определено внизу
template<bool _notUsed>
class _FileInfo
{
public:

	_FileInfo()
	: _fixedSize(0)
	, _size(0)
	, _pos(0)
	, _record(0)
	, _recCluster(0)
	, _flags(0)
	, _lastCluster(0)
	, _lastPos(0)
	{
	}

	_FileInfo(unsigned long recCluster, unsigned short record, unsigned long size, IOFlags::T flags)
	: _fixedSize(size)
	, _size(size)
	, _pos((flags & IOFlags::App) ? size : 0)
	, _record(record)
	, _recCluster(recCluster)
	, _flags(flags)
	, _lastCluster(0)
	, _lastPos(0)
	{
	}

	//
	// задать размер кластера
	//
	static inline void SetClustrerSize(unsigned short clusterSize)
	{
		_clusterSize = clusterSize;
	}

	//
	// Чтение
	//

	inline unsigned long Size()
	{
		return _size;
	}

	inline bool SizeFixed()
	{
		return _size == _fixedSize;
	}

	inline unsigned long Pos()
	{
		return _pos;
	}

	inline unsigned long RecordCluster()
	{
		return _recCluster;
	}

	inline unsigned short Record()
	{
		return _record;
	}

	inline IOFlags::T Flags()
	{
		return _flags;
	}

	inline bool CheckFlags(IOFlags::T flags)
	{
		return (_flags & flags) == flags;
	}

	inline bool CheckOneFlag(IOFlags::T flags)
	{
		return (_flags & flags) ? true : false;
	}

	inline unsigned long LastPos()
	{
		return _lastPos;
	}

	inline unsigned long LastCluster()
	{
		return _lastCluster;
	}

	inline unsigned long MaxReadLen()
	{
		return _size - _pos;
	}

	//
	// Изменение
	//

	inline void FixSize()
	{
		_fixedSize = _size;
	}

	inline void RollbackSize()
	{
		SetSize(_fixedSize);
	}

	inline void SetSize(unsigned long sz)
	{
		_size = sz; // новый размер
		_history.RemoveAllFrom(_size); // удаляем историю за пределами нового размера

		if (_pos > _size) // корректируем позицию
			_pos = _size;

		// корректируем сохранённое
		if (_lastPos >= _size)
		{
			_lastPos = 0;
			UpdateLastPos(_size);
		}
	}

	inline void SetPos(unsigned long pos)
	{
		_pos = pos;
		// обновляем последнюю запомненную пару позиция/кластер,
		// подбирая наиболее подходящие значения из истории обращений
		UpdateLastPos(pos);
	}

	inline void IncPos(unsigned long inc)
	{
		unsigned long posOld = _pos;

		_pos += inc;

		 // если переполнение (превысили макс. значение) или больше размера
		if (_pos < posOld || _pos > _size)
			_pos = _size;
	}

	// увеличивает позицию и подгоняет размер под неё, если он меньше
	inline void IncPosAndSize(unsigned long inc)
	{
		unsigned long posOld = _pos;

		_pos += inc;

		if (_pos < posOld) // если переполнение (превысили макс. значение)
			_pos = _size = -1UL; // ставим макс. значение
		else if (_size < _pos)
			_size = _pos;
	}

	// запоминает последний кластер, к которому было обращение,
	// и соответствующую ему позицию
	inline void SavePostion(unsigned long cluster)
	{
		unsigned long p = _pos - _pos % _clusterSize; // определяем позицию для начала кластера
		if (p == 0)
			return;
		_lastPos = p;
		_lastCluster = cluster;
	}

	// сохраняет пару кластер-позиция в истории обращений к файлу
	inline void SaveStartPos(unsigned long cluster)
	{
		unsigned long p = _pos - _pos % _clusterSize; // определяем позицию для начала кластера
		if (p == 0)
			return;
		_history.Add(p, cluster);
	}

private:
	
	// обновляет последнюю запомненную пару позиция/кластер,
	// подбирая наиболее подходящие значения из истории обращений
	void UpdateLastPos(unsigned long pos)
	{
		unsigned long p;
		unsigned long c;
		if (!_history.Find(pos, p, c))
			return;
		if (_lastPos <= pos && _lastPos >= p)
			return;
		_lastPos = p;
		_lastCluster = c;
	}

private:

	PositionHistory<4> _history;

	unsigned long _lastPos;	// позиция, для которой известен кластер (0 - поле не актуально)
	unsigned long _lastCluster; // кластер для _lastPos (0 - поле не актуально)

	unsigned long _fixedSize; // сохранённый размер файла
	unsigned long _size; // текущий размер файла
	unsigned long _pos; // текущая позиция
	unsigned short _record; // индекс записи о файле в кластере _recCluster
	unsigned long _recCluster; // кластер (каталога), содержащий запись о файле
	IOFlags::T _flags; // флаги, с которыми был открыт файл

	static unsigned short _clusterSize; // размер кластера, нужен для работы с историей позиция
};

template<bool _notUsed>
unsigned short _FileInfo<_notUsed>::_clusterSize = 4096; // размер кластера по умолчанию

// имя класса для пользователя
typedef _FileInfo<false> FileInfo;

}
}
}
}

#endif // _MEDIA_FS_FAT32_FILE_INFO_H_

