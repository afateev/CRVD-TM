
#ifndef _MEDIA_FS_FAT32_DIRECTORY_CLUSTER_H_
#define _MEDIA_FS_FAT32_DIRECTORY_CLUSTER_H_

#include "Cluster.h"
#include "DirEntry.h"

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

template<class DataProvider>
class DirectoryCluster : public Cluster<DataProvider>
{

  typedef Cluster<DataProvider> Base;
  
public:

	//
	// �������������
	//

	DirectoryCluster(DataProvider& dataProvider) : Base(dataProvider), _recordsBuffer((DirEntry*)Base::Buffer())
	{
	}

	// ������� ��������� � �������� DataProvider-�
	DirectoryCluster(DataProvider& dataProvider,
						unsigned long firstDataSector,
						unsigned char sectorsPerCluster)
	: Base(dataProvider, firstDataSector, sectorsPerCluster)
	, _recordsBuffer((DirEntry*)Base::Buffer())
	, _recordsPerCluster(sectorsPerCluster * _recordsPerSector)
	{
	}

	// ������� ��������� � �������� DataProvider-�
	inline void Init(unsigned long firstDataSector, unsigned short sectorsPerCluster)
	{
		Base::Init(firstDataSector, sectorsPerCluster);
		_recordsPerCluster = sectorsPerCluster * _recordsPerSector;
	}

	inline bool Set(unsigned long cluster)
	{
		return Base::Set(cluster);
	}

	inline bool Set(unsigned long cluster, unsigned short record)
	{
		if (!Base::Set(cluster))
			return false;
		
		_currentRecord = record;

		return true;
	}

	inline bool SetToLastRecord(unsigned long cluster)
	{
		return Set(cluster, _recordsPerCluster - 1);
	}

	//
	// ������ � ����������
	//

	inline unsigned short CurrentRecordNum()
	{
		return _currentRecord;
	}

	inline DirEntry& CurrentRecord()
	{
		return Record(_currentRecord);
	}

	DirEntry& Record(unsigned short r)
	{
		static DirEntry _emptyEntry;

		if (!PrepareRecord(r))
			return _emptyEntry;

		if (r >= _recordsPerCluster)
			return _emptyEntry;

		return _recordsBuffer[RecordInSector(r)];
	}

	//
	// �����
	//

	// ����� � �������� ������ �� �����
	bool FindEntry(char* name, unsigned char len, unsigned char& checkedLen)
	{
		for(unsigned char s = 0; s < Base::_sectorsPerCluster; s++)
		{
			if (!Base::PrepareBuffer(s))
				return false;

			for(unsigned char r = 0; r < _recordsPerSector; r++)
			{
				if (_recordsBuffer[r].LastInDir())
					return false;

				checkedLen = CheckRecord(r, name, len, checkedLen);
				if (checkedLen != len)
					continue;

				//
				// ������� �������� ������ ��� ������������������ �������
				// ���������� ���������� � ��������� ������
				//

				// ���������� ��������������� ������� �������
				if (_recordsBuffer[r].Long())
					_currentRecord = s * _recordsPerSector + r + 1; // ��� ����� ������, ��������� �� �������
				else
				{
					_currentRecord = s * _recordsPerSector + r; // ������� ������
					return true; // �������� ������ ������� �������� (����� �������� ���) ��� ��� ������ ������ � ��������
				}

				// ��������������� �������� ������ � ��������� ��������?
				if (_currentRecord >= _recordsPerCluster)
					return false;

				// ���� ��������� ������ � �������, �� ���� ��������� ������ � ���������
				if (r == _recordsPerSector - 1)
					PrepareRecord(_currentRecord);

				// ������ ���� ���������� ��������� ������, ����� ������ � ������� � ���������� �����
				if (!CheckValidShort(_currentRecord))
				{
					checkedLen  = 0;
					continue;
				}

				return true;
			}
		}

		return false;
	}
	
	// ����� � �������� ������ �� �����
	bool FindEntry(char* name, unsigned char len)
	{
		unsigned char checkedLen = 0;
		return FindEntry(name, len, checkedLen);
	}

	// ���������� ������� �������, ��������������� ���������
	inline unsigned char LongCount()
	{
		return _currentLongCount;
	}

	// �������, � ������� �������� ������������������ ������� �������, ��������������� ���������
	// ���������, ���� ��������� ������ ������������� ���� �� ���� ������� (LongCount ������� �� 0)
	inline unsigned long EntryStartCluster()
	{
		return _currentStartCluster;
	}

	// ����� ����� �� ��������� ���������� ������ �������
	// cnt - ������� ����� �������
	unsigned short FindEmptyBlock(unsigned short cnt)
	{
		unsigned short foundCnt = 0;

		for(unsigned char s = 0; s < Base::_sectorsPerCluster; s++)
		{
			if (!Base::PrepareBuffer(s))
				return 0;

			for(unsigned char r = 0; r < _recordsPerSector; r++)
			{
				if (_recordsBuffer[r].Free()) // ��������� ������ (����� �������� � �.�.)
				{
					if (foundCnt == 0) // ���� �� ����� �� ���� ��������� �������
					{
						_currentRecord = s * _recordsPerSector + r;
					}

					if (++foundCnt >= cnt)
						return foundCnt;

				}
				else if (_recordsBuffer[r].LastInDir()) // ������� � ���� ������ ��� ������
				{
					if (foundCnt == 0) // ���� �� ����� �� ���� ��������� �������
					{
						_currentRecord = s * _recordsPerSector + r;
					}

					return foundCnt + _recordsPerSector * (Base::_sectorsPerCluster - s) - r; // �� ���������� � �������� �����
				}
				else
				{
					foundCnt = 0;
				}
			}
		}

		return foundCnt;
	}

	inline unsigned short RecordsPerCluster()
	{
		return _recordsPerCluster;
	}

protected:

	// sector - ������ ������� ������������ ������ ��������
	inline bool PrepareRecord(unsigned short record)
	{
		return Base::PrepareBuffer(RecordSector(record));
	}

	inline unsigned char RecordSector(unsigned short record)
	{
		return record / _recordsPerSector;
	}

	inline unsigned char RecordInSector(unsigned short record)
	{
		return record % _recordsPerSector;
	}

	inline unsigned char CheckRecord(unsigned short r, char* name, unsigned char len, unsigned char checkedLen)
	{
		if (_recordsBuffer[r].Empty())
			return 0;

		if (_recordsBuffer[r].Long())
			return CheckRecordLong(r, name, len, checkedLen);

		return CheckRecordShort(r, name, len, checkedLen);
	}

	unsigned char CheckRecordLong(unsigned short r, char* name, unsigned char len, unsigned char checkedLen)
	{
		if (!CheckLongSequence(r, checkedLen)) // �������� ��������� �� ������������������ ������� �������
			return 0;

		if (_recordsBuffer[r].Last()) // ���� ������ ������ � ������, �� ����� �������� ���������� � ����� �������
			checkedLen = 0;

		unsigned char longNamePartLen = _recordsBuffer[r].Name(_longNameBuffer);
		if (longNamePartLen > len)
			return 0;

		if (!CheckStringEnding(name, len - checkedLen, _longNameBuffer, longNamePartLen))
			return 0;

		unsigned char checkedLenNew = checkedLen + longNamePartLen;

		if (checkedLenNew == len && !_recordsBuffer[r].First())
			return 0;

		return checkedLenNew;
	}

	unsigned char CheckRecordShort(unsigned short r, char* name, unsigned char len, unsigned char checkedLen)
	{
		// ���� ����������� ����� ����� ������� �����, �� ������� ������ ��� �������
		// � ������ ����� ��������������� �������� ������ (��������� ����������� �����)
		if (checkedLen == len)
			return (_recordsBuffer[r].CheckSumShort() == _currentCheckSum) ? checkedLen : 0;

		// ��� ���������� �������� ��� � ������ � �������
		unsigned char shortNameLen = _recordsBuffer[r].Name(_shortNameBuffer);

		if (shortNameLen != len)
			return 0;

		// ���������� ���������� ������� �������, ����� ���� �������,
		// ��� � �������� ������ ��� �� ����� �������
		_currentLongCount = 0;

		// ���� ����� �������, �� �� ����� �������� ������ ��������
		return strncmp(name, _shortNameBuffer, len) ? 0 : len;
	}

	bool CheckLongSequence(unsigned short r, unsigned char checkedLen)
	{
		if (_recordsBuffer[r].Last())
		{
			_currentLongOrder = _recordsBuffer[r].Order();
			_currentLongCount = _recordsBuffer[r].Order();
			_currentCheckSum = _recordsBuffer[r].CheckSumLong();
			// ���������� �������, � ������� ���������� ������������������ ������� �������
			_currentStartCluster = Base::Number();
			return true;
		}

		// ���� �� ������ ������ � ������, �� ��������� ������ ������ ���� checkedLen != 0, ����� ����������
		// ���� ������ �� ���������, �� ���������� ������
		// ���� ����������� ����� �� ���������, ���������� ������
		if (checkedLen == 0
			|| --_currentLongOrder != _recordsBuffer[r].Order()
			|| _currentCheckSum != _recordsBuffer[r].CheckSumLong())
		{
			return false;
		}

		return true;
	}

	// ���������� �������� ������, ����������� ������ �������
	inline bool CheckValidShort(unsigned short r)
	{
		unsigned char recordInSector = RecordInSector(r);

		if (_recordsBuffer[recordInSector].Empty()
			|| _recordsBuffer[recordInSector].Long()
			|| _recordsBuffer[recordInSector].CheckSumShort() != _currentCheckSum)
		{
			return false;
		}

		return true;
	}

	//
	// ��������� �����
	//

	static bool CheckStringEnding(char* str, unsigned char strLen, unsigned short* ending, unsigned char endingLen)
	{
		unsigned char i = 0;
		for(; i < strLen && i < endingLen; i++)
		{
			if ((str[strLen - i - 1] > 127) || (str[strLen - i - 1] != ending[endingLen - i - 1]))
				return false;
		}

		return true;
	}

private:

	static const unsigned char _recordsPerSector = Base::_sectorSize / sizeof(DirEntry);
	DirEntry* _recordsBuffer;
	unsigned short _recordsPerCluster;

	unsigned short _longNameBuffer[256];
	char _shortNameBuffer[15];

	unsigned short _currentRecord; // ������ ������� �������� ������ � ������� ��������
	unsigned char _currentLongCount; // ���������� ������� �������, ��������������� � ������� �������� �������
	unsigned char _currentLongOrder;
	unsigned char _currentCheckSum;
	unsigned long _currentStartCluster; // �������, � ������� ��������� ������ ������� ������
};

}
}
}
}

#endif // _MEDIA_FS_FAT32_DIRECTORY_CLUSTER_H_

