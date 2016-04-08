
#ifndef _MEDIA_FS_FAT32_BOOT_SECTOR_H_
#define _MEDIA_FS_FAT32_BOOT_SECTOR_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

#pragma pack(push, 1)

class BootSector
{
public:

	inline bool Valid()
	{
		if (_bootSignature == _bootSigValue
			&& _signature == _sectorSigValue
			&& (_jmpBoot[0] == _jmpBootValue1 || _jmpBoot[0] == _jmpBootValue2))
		{
			return true;
		}

		return false;
	}
	
	// первый сектор, где могул лежать данные (за FAT)
	inline unsigned long FirstDataSector()
	{
		return _reservedSectorsCount + _numberOfFats * _fatSize32Bit;
	}

	// смещение в байтах до первого сектора с данными относительно начала раздела
	unsigned long FirstDataSectorOffset()
	{
		return FirstDataSector() * _bytesPerSector;
	}

	inline unsigned long RootCluster()
	{
		return _rootCluster;
	}

	// возвращает сектор в котором лежит корневой каталог
	inline unsigned long RootDirSector()
	{
		return (_rootCluster - 2) + FirstDataSector();
	}

	// первый сектор таблицы FAT с указанным номером
	inline unsigned long FatSector(unsigned char fatNum = 0)
	{
		return _reservedSectorsCount + fatNum * _fatSize32Bit;
	}

	// Сектро со структурой FsInfo
	inline unsigned short FsInfoSector()
	{
		return _fsInfoSector;
	}

	inline unsigned short SectorSize()
	{
		return _bytesPerSector;
	}

	inline unsigned short ClusterSize()
	{
		return _sectorsPerCluster * _bytesPerSector;
	}

	// количество "полезных" кластеров на диске
	inline unsigned long ClusterCount()
	{
		return (_totalSectorsCount32Bit - _reservedSectorsCount - _numberOfFats * _fatSize32Bit) / _sectorsPerCluster;
	}

	inline unsigned char SectorsPerCluster()
	{
		return _sectorsPerCluster;
	}

	// смещение в байтах до нужной таблицы относительно начала раздела
	inline unsigned long FatOffset(unsigned char fatIndex = 0)
	{
		return _bytesPerSector * (_reservedSectorsCount + fatIndex * _fatSize32Bit);
	}

	inline unsigned long FatSizeInBytes()
	{
		return _bytesPerSector * _fatSize32Bit;
	}

	inline unsigned char FatsCount()
	{
		return _numberOfFats;
	}

private:

	static const unsigned char _bootSigValue = 0x29;
	static const unsigned short _sectorSigValue = 0xAA55;
	static const unsigned short _jmpBootValue1 = 0xEB;
	static const unsigned short _jmpBootValue2 = 0xE9;

	unsigned char _jmpBoot[3];
	unsigned char _oemName[8];
	unsigned short _bytesPerSector;
	unsigned char _sectorsPerCluster;
	unsigned short _reservedSectorsCount;
	unsigned char _numberOfFats;
	unsigned short _rootEntryCount;
	unsigned short _totalSectorsCount16Bit;
	unsigned char _mediaType;
	unsigned short _fatSize16Bit;
	unsigned short _sectorsPerTrack;
	unsigned short _numberOfHeads;
	unsigned long _hiddenSectors;
	unsigned long _totalSectorsCount32Bit;
	unsigned long _fatSize32Bit;
	unsigned short _extFlags;
	unsigned short _version;
	unsigned long _rootCluster;
	unsigned short _fsInfoSector;
	unsigned short _backupBootSector;
	unsigned char _reserved[12];
	unsigned char _driveNumber;
	unsigned char _reserved1;
	unsigned char _bootSignature;
	unsigned long _volumeId;
	unsigned char _volumeLabel[11];
	unsigned char _fileSystemType[8];
	unsigned char _bootData[420];
	unsigned short _signature;
};

#pragma pack (pop)

}
}
}
}

#endif // _MEDIA_FS_FAT32_BOOT_SECTOR_H_

