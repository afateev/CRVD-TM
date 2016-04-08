
#ifndef _MEDIA_FS_FAT32_DIR_ITERATOR_H_
#define _MEDIA_FS_FAT32_DIR_ITERATOR_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

template<class DataProvider, class DateTimeProvider, int openedFilesMax>
class Engine;

template<class DataProvider, class DateTimeProvider, int openedFilesMax>
class DirIterator
{
protected:

	typedef Engine<DataProvider, DateTimeProvider, openedFilesMax> EngineType;

public:

	struct DirEntryInfo
	{
		DirEntryInfo()
		: _directory(false)
		, _size(0)
		, _name(0)
		, _nameLen(0)
		{
		}

		DirEntryInfo(char* name, unsigned char nameLen, unsigned long size, bool dir)
		: _directory(dir)
		, _size(size)
		, _name(name)
		, _nameLen(nameLen)
		{
		}

		inline void Set(char* name, unsigned char nameLen, unsigned long size, bool dir)
		{
			_directory = dir;
			_size = size;
			_name = name;
			_nameLen = nameLen;
		}
		
		inline bool Dir() const
		{
			return _directory;
		}

		inline unsigned long Size() const
		{
			return _size;
		}

		inline const char* Name() const
		{
			return _name;
		}

		inline unsigned char NameLen() const
		{
			return _nameLen;
		}

	protected:

		bool _directory;
		unsigned long _size;
		char* _name;
		unsigned char _nameLen;
	};

public:

	DirIterator(EngineType& engine)
	: _engine(engine)
	, _firstCluster(0)
	{
	}

	DirIterator(EngineType& engine, unsigned long cluster)
	: _engine(engine)
	, _firstCluster(cluster)
	, _cluster(cluster)
	, _record(0)
	{
		ResetName();
	}

	DirIterator(const DirIterator& iter)
	: _engine(iter._engine)
	, _firstCluster(iter._firstCluster)
	, _cluster(iter._cluster)
	, _record(iter._record)
	{
		ResetName();
	}
	
	DirIterator& operator=(const DirIterator& iter)
	{
		_engine = iter._engine;
		_firstCluster = iter._firstCluster;
		_cluster = iter._cluster;
		_record = iter._record;

		return *this;
	}

	inline bool Valid()
	{
		return _firstCluster;
	}

	inline void Reset()
	{
		_cluster = _firstCluster;
		_record = 0;
	}

	char* NextItem(DirEntryInfo* dirEntryInfo = 0)
	{
		while(_cluster)
		{
			if (!_engine._cluster.Set(_cluster))
				return 0;

			while(_record < _engine._cluster.RecordsPerCluster())
			{
				DirEntry& entry = _engine._cluster.Record(_record++);

				if (entry.Empty() || !entry.Valid()) // конец обхода, достигнут конец (возможно, из-за ошибки чтения карты)!
				{
					_cluster = 0;
					return 0;
				}

				unsigned char nameLen;
				if (nameLen = ProcessEntry(entry)) // если результат не нулевой, то получили очередное имя
				{
					if (dirEntryInfo)
						dirEntryInfo->Set(_nameBuffer + _bufferStart, nameLen, entry.Size(), entry.Directory());
					return _nameBuffer + _bufferStart;
				}
			}

			// переходим на первую запись в следующем кластере
			_record = 0;
			_cluster = _engine.NextCluster(_cluster);
		}

		_cluster = 0;
		return 0; // конец обхода, достигнут конец!
	}

protected:

	unsigned char ProcessEntry(DirEntry& entry)
	{
		if (entry.Short()) // если короткая запись
			return ProcessShort(entry);

		ProcessLong(entry);
		
		return 0;
	}

	unsigned char ProcessShort(DirEntry& entry)
	{
		if (_cs != entry.CheckSumShort() || _order) // если не совпало CRC или кол-во длинных записей
		{
			_bufferStart = 0;
			return entry.Name(_nameBuffer); // берём короткое имя, возвращаем его длину
		}

		return _nameBufferLength - _bufferStart - 1; // возвращаем длину строки, накопленной в буфера
	}

	void ProcessLong(DirEntry& entry)
	{
		if (entry.Last()) // если это первая длинная запись в наборе (содержит последний кусок имени, поэтому last)
		{
			_cs = entry.CheckSumLong(); // запоминаем CRC
			_order = entry.Order(); // запоминаем кол-во записей
			ResetName(); // сбрасываем буфер имени
		}
		else if (_order == 0xff || _cs != entry.CheckSumLong() || _order != entry.Order())
		{
			_order = 0xff;
			return;
		}

		_order--;
		_bufferStart -= entry.NameReverse(_nameBuffer, _bufferStart);
	}

	inline void ResetName()
	{
		_bufferStart = _nameBufferLength - 1;
		_nameBuffer[_nameBufferLength - 1] = 0;
	}

protected:

	EngineType& _engine; // движок FAT
	unsigned long _firstCluster; // первый кластер каталога, который обходим

	static const unsigned char _nameBufferLength = 255;
	char _nameBuffer[_nameBufferLength];
	unsigned char _bufferStart;

	unsigned long _cluster;
	unsigned short _record;
	unsigned long _cs;
	unsigned char _order;
};

}
}
}
}

#endif // _MEDIA_FS_FAT32_DIR_ITERATOR_H_
