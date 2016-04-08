
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
// ���������� �� �������� �����
//
// ���� ������������ ��� ������ FileInfo (��� �������������), ������� ���������� �����
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
	// ������ ������ ��������
	//
	static inline void SetClustrerSize(unsigned short clusterSize)
	{
		_clusterSize = clusterSize;
	}

	//
	// ������
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
	// ���������
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
		_size = sz; // ����� ������
		_history.RemoveAllFrom(_size); // ������� ������� �� ��������� ������ �������

		if (_pos > _size) // ������������ �������
			_pos = _size;

		// ������������ ����������
		if (_lastPos >= _size)
		{
			_lastPos = 0;
			UpdateLastPos(_size);
		}
	}

	inline void SetPos(unsigned long pos)
	{
		_pos = pos;
		// ��������� ��������� ����������� ���� �������/�������,
		// �������� �������� ���������� �������� �� ������� ���������
		UpdateLastPos(pos);
	}

	inline void IncPos(unsigned long inc)
	{
		unsigned long posOld = _pos;

		_pos += inc;

		 // ���� ������������ (��������� ����. ��������) ��� ������ �������
		if (_pos < posOld || _pos > _size)
			_pos = _size;
	}

	// ����������� ������� � ��������� ������ ��� ��, ���� �� ������
	inline void IncPosAndSize(unsigned long inc)
	{
		unsigned long posOld = _pos;

		_pos += inc;

		if (_pos < posOld) // ���� ������������ (��������� ����. ��������)
			_pos = _size = -1UL; // ������ ����. ��������
		else if (_size < _pos)
			_size = _pos;
	}

	// ���������� ��������� �������, � �������� ���� ���������,
	// � ��������������� ��� �������
	inline void SavePostion(unsigned long cluster)
	{
		unsigned long p = _pos - _pos % _clusterSize; // ���������� ������� ��� ������ ��������
		if (p == 0)
			return;
		_lastPos = p;
		_lastCluster = cluster;
	}

	// ��������� ���� �������-������� � ������� ��������� � �����
	inline void SaveStartPos(unsigned long cluster)
	{
		unsigned long p = _pos - _pos % _clusterSize; // ���������� ������� ��� ������ ��������
		if (p == 0)
			return;
		_history.Add(p, cluster);
	}

private:
	
	// ��������� ��������� ����������� ���� �������/�������,
	// �������� �������� ���������� �������� �� ������� ���������
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

	unsigned long _lastPos;	// �������, ��� ������� �������� ������� (0 - ���� �� ���������)
	unsigned long _lastCluster; // ������� ��� _lastPos (0 - ���� �� ���������)

	unsigned long _fixedSize; // ���������� ������ �����
	unsigned long _size; // ������� ������ �����
	unsigned long _pos; // ������� �������
	unsigned short _record; // ������ ������ � ����� � �������� _recCluster
	unsigned long _recCluster; // ������� (��������), ���������� ������ � �����
	IOFlags::T _flags; // �����, � �������� ��� ������ ����

	static unsigned short _clusterSize; // ������ ��������, ����� ��� ������ � �������� �������
};

template<bool _notUsed>
unsigned short _FileInfo<_notUsed>::_clusterSize = 4096; // ������ �������� �� ���������

// ��� ������ ��� ������������
typedef _FileInfo<false> FileInfo;

}
}
}
}

#endif // _MEDIA_FS_FAT32_FILE_INFO_H_

