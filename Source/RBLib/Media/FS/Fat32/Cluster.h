
#ifndef _MEDIA_FS_FAT32_CLUSTER_H_
#define _MEDIA_FS_FAT32_CLUSTER_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

template<class DataProvider>
class Cluster
{
public:

	//
	// инициализация
	//

	Cluster(DataProvider& dataProvider)
	: _dataProvider(dataProvider)
	, _firstDataSector(0)
	, _sectorsPerCluster(0)
	, _clusterNumber(0)
	, _bufferModified(false)
	{
		memset(_sectorBuffer, 0xAB, _sectorSize);
	}

	// входные параметры в секторах DataProvider-а
	Cluster(DataProvider& dataProvider,
	unsigned long firstDataSector,
	unsigned char sectorsPerCluster)
	: _dataProvider(dataProvider)
	, _firstDataSector(firstDataSector)
	, _sectorsPerCluster(sectorsPerCluster)
	, _bufferValid(false)
	, _clusterNumber(0)
	{
	}

	// входные параметры в секторах DataProvider-а
	inline void Init(unsigned long firstDataSector, unsigned short sectorsPerCluster)
	{
		_firstDataSector = firstDataSector;
		_sectorsPerCluster = sectorsPerCluster;
		_bufferValid = false;
		_bufferModified = false;
	}

	bool Set(unsigned long cluster)
	{
		if (cluster < 2)
			return false;

		if (_clusterNumber == cluster) // если кластер не поменялся
			return true; // ничего не делаем

		if (!Flush())
			return false;

		_clusterNumber = cluster;
		_bufferValid = false;
		_bufferModified = false;
		_startSector = StartSectorForClaster(cluster);

		return true;
	}

	inline bool Valid()
	{
		return _bufferValid;
	}

	inline unsigned long Number()
	{
		return _clusterNumber;
	}

	signed short Read(unsigned char* buffer, unsigned short len, unsigned short offset)
	{
		unsigned char startSector = OffsetSector(offset);
		unsigned short offsetInSector = offset % _sectorSize;
		
		unsigned short remainingLen = len; 

		for(unsigned char s = startSector; s < _sectorsPerCluster && remainingLen > 0; s++)
		{
			if (!PrepareBuffer(s))
				return -1;

			unsigned short copyLen = ((remainingLen + offsetInSector) > _sectorSize) ?
										(_sectorSize - offsetInSector) : remainingLen; 

			memcpy(buffer + len - remainingLen, _sectorBuffer + offsetInSector, copyLen);

			remainingLen -= copyLen;
			offsetInSector = 0;
		}

		return len - remainingLen;
	}

	inline signed short Read(unsigned char* buffer, unsigned short len)
	{
		return Read(buffer, len, 0);
	}

	signed short Write(unsigned char* buffer, unsigned short len, unsigned short offset)
	{
		unsigned char startSector = OffsetSector(offset);
		unsigned short offsetInSector = offset % _sectorSize;

		unsigned short remainingLen = len; 

		for(unsigned char s = startSector; s < _sectorsPerCluster && remainingLen > 0; s++)
		{
			unsigned short copyLen = ((remainingLen + offsetInSector) > _sectorSize) ?
				(_sectorSize - offsetInSector) : remainingLen; 

			if (!PrepareBuffer(s, copyLen < _sectorSize)) // если переписываем весь сектор, то он не будет считываться
				return -1;

			memcpy(_sectorBuffer + offsetInSector, buffer + len - remainingLen, copyLen);

			remainingLen -= copyLen;
			offsetInSector = 0;

			_bufferModified = true;
		}

		if (!Flush())
			return -1;

		return len - remainingLen;
	}

	inline signed short Write(unsigned char* buffer, unsigned short len)
	{
		return Write(buffer, len, 0);
	}

	// очистка текущего кластера
	inline bool Clear()
	{
		return ClearFromSector(_startSector);
	}

	// очистка заданного кластера
	inline bool Clear(unsigned long cluster)
	{
		return ClearFromSector(StartSectorForClaster(cluster));
	}

	inline void SetModified()
	{
		_bufferModified = true;
	}

	// сохраняет содержимое сектора из буфера на диск, если буфер модифицировался
	bool Flush()
	{
		if (_bufferModified)
			return Save();
		return true;
	}

	// сохраняет сожержимое сектора из буфера на диск
	bool Save()
	{
		if (!_bufferValid)
			return false;

		if (!_dataProvider.WriteSector(_startSector + _sectorInBuffer, _sectorBuffer))
		{
			_bufferValid = false;
			_bufferModified = false;
			return false;
		}

		_bufferModified = false;

		return true;
	}

	bool SaveToSector(unsigned char sector)
	{
		_bufferValid = false;
		_bufferModified = false;

		if (!_dataProvider.WriteSector(_startSector + sector, _sectorBuffer))
			return false;

		_sectorInBuffer = sector;
		_bufferValid = true;

		return true;
	}

protected:

	inline unsigned long StartSectorForClaster(unsigned long cluster)
	{
		return (cluster - 2) * _sectorsPerCluster + _firstDataSector;
	}

	// sector - индекс сектора относительно начала кластера
	inline bool PrepareBuffer(unsigned char sector)
	{
		return PrepareBuffer(sector, true);
	}

	// sector - индекс сектора относительно начала кластера
	bool PrepareBuffer(unsigned char sector, bool read)
	{
		if (sector >= _sectorsPerCluster)
			return false;

		if (_sectorInBuffer == sector && _bufferValid)
			return true;

		if (_bufferModified)
			if (!Save())
				return false;

		_bufferModified = false;

		if (read)
		{
			_bufferValid = false;
			if (!_dataProvider.ReadSector(_startSector + sector, _sectorBuffer))
				return false;
		}

		_sectorInBuffer = sector;
		_bufferValid = true;

		return true;
	}

	// очистка кластера, начинающегося с заданного сектора
	bool ClearFromSector(unsigned long startSector)
	{
		unsigned char* emptySector = EmptySector();		

		_bufferModified = false;
		_bufferValid = false;

		for (int s = 0; s < _sectorsPerCluster; s++)
		{
			if (!_dataProvider.WriteSector(startSector + s, emptySector))
				return false;
		}

		return true;
	}

	// возвращает указатель на пустой сектор
	static unsigned char* EmptySector()
	{
		static const unsigned long nullBuffer[128] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
													, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
													, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
													, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
													, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
													, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
													, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
													, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		return (unsigned char*) nullBuffer;
	}

	inline unsigned char OffsetSector(unsigned short offset)
	{
		return offset / _sectorSize;
	}

	inline unsigned char* Buffer()
	{
		return _sectorBuffer;
	}

protected:

	static const unsigned short _sectorSize = DataProvider::SectorSize; // сектор DataProvider-а
	unsigned short _sectorsPerCluster;

private:

	DataProvider& _dataProvider;
	unsigned long _firstDataSector;
	unsigned long _startSector;
	unsigned long _clusterNumber;

	unsigned char _sectorBuffer[_sectorSize];

	unsigned long _sectorInBuffer;
	bool _bufferValid;
	bool _bufferModified;

};

}
}
}
}

#endif // _MEDIA_FS_FAT32_CLUSTER_H_

