
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

	// ������� ��������� � ������� DataProvider-�
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

	// ������� ��������� � �������� DataProvider-�
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

	// ������� ��������� � �������� DataProvider-�
	// freeCount - ���������� ��������� ���������
	void Init(unsigned long startSectorAbsolute, unsigned long size, unsigned char fatsCount, unsigned long freeCount)
	{
		_freeCount = freeCount;
		Init(startSectorAbsolute, size, fatsCount);
	}

	inline void SetProvider(DataProvider* provider)
	{
		_dataProvider = provider;
	}

	// ������ ������� ������ ����������� �������
	void SetWdtClear(WdtClearType wdtClear)
	{
		_wdtClear = wdtClear;
	}

	// ���������� ��������� �������� ���������� ��������� ���������
	// ����������� ���������� �� ������ ������ (����� � Init)
	inline void SetFreeClustersCount(unsigned long freeCount)
	{
		_freeCount = freeCount;
	}

	// ������������� ����� ��������, � �������� ������ ������ ��������� �����
	inline void SetNextFree(unsigned long nextFreeCluster)
	{
		_firstFreeCluster = nextFreeCluster;
	}

	//
	// ������ ��������� ��������
	//
	inline bool LastOperationFailed()
	{
		return _bufferValid == false;
	}

	//
	// ��������� ���������� �� �������
	//

	// ���������� ��������� ���������
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

	// ���������� ��������� ������� � �������
	inline signed long NextCluster(unsigned cluster)
	{
		if (!PrepareSectorBuffer(cluster))
			return -1;

		unsigned long nextCluster = _sectorBuffer[ClusterRecordInSector(cluster)];

		return (nextCluster < _lastInChain) ? nextCluster : 0;
	}

	// ���������� ������ ��������� �������
	inline unsigned long FindFreeCluster()
	{
		return FindNextFreeCluster(_firstFreeCluster);
	}

	// ������� ��������� ��������� �������
	inline unsigned long FindNextFreeCluster(unsigned long cluster)
	{
		return FindNextFreeCluster(ClusterSector(cluster), ClusterRecordInSector(cluster));
	}

	//
	// ��������� �������
	//

	// ������������� ����� �������� ���������� �������� ��� ��������� ��������
	// �� ���� ����� �� ���������
	inline bool SetNextCluster(unsigned long parent, unsigned long child)
	{
		return UpdateClusterRecord(parent, child);
	}

	// ������������� ����� �������� ���������� �������� ��� ��������� ��������
	// ����� ��������� �� ����
	inline bool SetNextClusterAndSave(unsigned long parent, unsigned long child)
	{
		return UpdateClusterRecordAndSave(parent, child);
	}

	// �������� ������� ��� ���������
	// �� ���� ����� �� ���������
	inline bool FreeCluster(unsigned long cluster)
	{
		return UpdateClusterRecord(cluster, _freeCluster);
	}

	// �������� ������� ��� ���������
	// ����� ��������� �� ����
	inline bool FreeClusterAndSave(unsigned long cluster)
	{
		return FreeCluster(cluster);
	}

	// �������� ������� ��� ��������� � �������
	// �� ���� ����� �� ���������
	inline bool SetEoc(unsigned long cluster)
	{
		return UpdateClusterRecord(cluster, _lastInChain);
	}

	// �������� ������� ��� ��������� � �������
	// ����� ��������� �� ����
	inline bool SetEocAndSave(unsigned long cluster)
	{
		return UpdateClusterRecordAndSave(cluster, _lastInChain);
	}

	// ��������� ������ ������ ������� � �������
	inline bool SetChainEocAndSave(unsigned long cluster)
	{
		return FreeChainAndSave(cluster, false);
	}

	// ����������� ��� ������� ���������
	inline bool FreeChain(unsigned long cluster)
	{
		return FreeChain(cluster, true);
	}

	// ����������� ��� ������� ��������� � �� ���������
	inline bool FreeChainAndSave(unsigned long cluster)
	{
		return FreeChainAndSave(cluster, true);
	}

	// ��������� ���������� ������� �� ������ �� ����, ���� ����� ���������������
	inline bool Flush()
	{
		if (_bufferModified)
			return SaveBuffer();
		return true;
	}

protected:

	//
	// ���������� ��������
	//

	// ���������� ������ ������� ��� ������� �������� (������������ ������ FAT, �� ����������)
	inline unsigned long ClusterSector(unsigned long cluster)
	{
		return cluster / _recordsInSector;
	}

	// ������ ������ ��������� �������� � �������
	inline unsigned long ClusterRecordInSector(unsigned long cluster)
	{
		return cluster % _recordsInSector;
	}

	//
	// ������ � ������ ��������
	//

	// ��������� ���������� ������� �� ������ �� ����
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

	// ��������� ��������� ������ FAT � �����
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

	// ��������� � ����� ������ � ������� � �������� �������� (���� � ������ ��� �� ����� ���� �������)
	bool PrepareSectorBuffer(unsigned long cluster)
	{
		unsigned long sector = ClusterSector(cluster);
		if (_sectorInBuffer == sector && _bufferValid)
			return true;

		return UpdateBuffer(sector);
	}

	// ��������� � ����� �������� ������ (���� � ������ ��� �� ����� ���� �������)
	bool PrepareBuffer(unsigned long sector)
	{
		if (_sectorInBuffer == sector && _bufferValid)
			return true;

		return UpdateBuffer(sector);
	}

	//
	// ��������� ���������� �� �������
	//

	// ������� ��������� ��������� �������, ������� � ��������� ������ � ��������� �������
	unsigned long FindNextFreeCluster(unsigned long sector, unsigned long record)
	{
		// ���������� ������� FAT
		for (unsigned long s = sector; s < _size; s++)
		{
			WdtClear(); // ����� ����������� �������, �.�. ��������� ����� ������ ����� �������

			if (!PrepareBuffer(s))
				return 0;

			for(unsigned long r = record; r < _recordsInSector; r++)
			{
				if (_sectorBuffer[r] == _freeCluster)
					return (_firstFreeCluster = s * _recordsInSector + r);
			}
			
			record = 0;
		}

		return (_freeCount = _firstFreeCluster = 0); // �� �����
	}

	//
	// ��������� �������
	//

	// ������������ ������ ��� ��������� ��������
	// �� ���� ����� �� ���������
	bool UpdateClusterRecord(unsigned long cluster, unsigned long value)
	{
		if (!PrepareSectorBuffer(cluster))
			return false;

		unsigned long valuePrev = _sectorBuffer[ClusterRecordInSector(cluster)];
		_sectorBuffer[ClusterRecordInSector(cluster)] = value;

		if (valuePrev != value)
			_bufferModified = true;

		// ���� ������� �����������, �� ��������� ������� ������� ���������� ��������, ���� ����
		if (value == _freeCluster && cluster < _firstFreeCluster)
			_firstFreeCluster = cluster;

		UpdateFreeSpace(valuePrev, value);

		return true;
	}

	// ������������ ������ ��� ��������� ��������
	// ����� ��������� �� ����
	inline bool UpdateClusterRecordAndSave(unsigned long cluster, unsigned long value)
	{
		if (!UpdateClusterRecord(cluster, value))
			return false;
		if (!Flush())
			return false;
		return true;
	}

	//
	// ��������� ���������� ��������� ���������
	// � ����������� �� ������� � ������ ��������� ��������
	//
	void UpdateFreeSpace(unsigned long oldValue, unsigned long newValue)
	{
		if (oldValue == _freeCluster) // ���� ������� ��� ������
		{
			if (newValue != _freeCluster && _freeCount) // � ���� �� ������
				_freeCount--; // �� ��������� ��������� ����� �� ���� ������
		}
		else // ���� ������� �� ��� ������
		{
			if (newValue == _freeCluster) // � ���� ������
				_freeCount++; // �� ��������� ��������� ����� �� ���� ������
		}
	}

	// ����������� ������� ���������
	// ���� total = true, ����������� ��� �������
	// ���� total = false, ������ ������� ������� ������� (�������� �������� EOC)
	bool FreeChain(unsigned long cluster, bool total)
	{
		signed long nextFreeCluster = NextCluster(cluster); // ���������� ��������� �� �������
		if (nextFreeCluster < 0)
			return false; // ������ IO
		// ������������� ������ �������� � ������ ������� �������
		UpdateClusterRecord(cluster, total ? _freeCluster : _lastInChain); // ��� �� �������, ������� ������ �� ���������

		// ����������� ��������� �������� �������
		cluster = nextFreeCluster;
		while(cluster)
		{
			if ((nextFreeCluster = NextCluster(cluster)) < 0)
				return false; // ������ IO
			FreeCluster(cluster); // ��� �� �������, ������� ������ �� ���������
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

	// ����� ����������� �������
	inline void WdtClear()
	{
		if (_wdtClear)
			_wdtClear();
	}

private:

	DataProvider* _dataProvider;
	unsigned long _startSector; // ����� ���������� ������� "� ��������" DataProvider
	unsigned long _size; // ������ ������� � �������� "� ��������" DataProvider
	unsigned char _fatsCount; // ���������� ������ (������ 2)

	// ����� ��� ���������� �������
	static const unsigned long _sectorSize = 512; // ������ ������� "� ��������" DataProvider
	static const unsigned long _recordsInSector = _sectorSize / 4;
	unsigned long _sectorBuffer[_recordsInSector];
	unsigned long _sectorInBuffer;
	bool _bufferValid;
	bool _bufferModified;

	unsigned long _firstFreeCluster;
	unsigned long _freeCount;

	WdtClearType _wdtClear;

	// �������� � �������
	static const unsigned long _lastInChain = 0x0FFFFFF8;
	static const unsigned long _freeCluster = 0;
};

}
}
}
}

#endif // _MEDIA_FS_FAT32_CLASTER_CHAIN_H_

