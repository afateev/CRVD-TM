
#ifndef _MEDIA_FS_FAT32_POSITION_HISTORY_H_
#define _MEDIA_FS_FAT32_POSITION_HISTORY_H_

template<unsigned long _maxSize = 4>
class PositionHistory
{
private:

	struct PosInfo 
	{
		unsigned long Pos;
		unsigned long Cluster;
		PosInfo(){}
		PosInfo(unsigned long pos, unsigned long cluster) : Pos(pos), Cluster(cluster) {}
	};

public:

	PositionHistory() : _size(0)
	{
	}

	// �������� �������
	void Add(unsigned long pos, unsigned long cluster)
	{
		for (unsigned long i = 0; i < _size; i++)
		{
			// ���� ������� ��� ���� � ������, ������ ��������� � ������
			if (_info[i].Pos == pos)
			{
				// �� ������ ������ ���������� �������� � ���������, ���� ������,
				// �� ���� �� ������ ������ ����
				if (_info[i].Cluster != cluster)
					_info[i].Cluster = cluster;
				Move2Top(i); // ��������� ������ ������
				return;
			}
		}
		MoveDown(_size); // �������� ������ �� 1 ����
		_info[0] = PosInfo(pos, cluster); // ����� ������ �������� � ������ ������
		_size += (_size < _maxSize) ? 1 : 0; // ����������� ������, ���� ����
	}

	// ������� ��� ���������� �������, ������� � pos
	void RemoveAllFrom(unsigned long pos)
	{
		for(unsigned long i = 0; i < _size;)
		{
			if (_info[i].Pos >= pos)
				MoveUp(i); // ������� �� �� 1 �����
			else
				i++;
		}
	}

	// ����� �������
	bool Find(unsigned long pos, unsigned long& savedPos, unsigned long& savedCluster)
	{
		unsigned long p = 0;
		unsigned long index = 0;

		// ��������� ����� ������� ������� �� ����������
		for(unsigned long i = 0; i < _size; i++)
		{
			if (_info[i].Pos <= pos && _info[i].Pos > p)
			{
				p = _info[i].Pos;
				index = i;
			}
		}

		// ���� �� ����� ������ �����������
		if (p == 0)
			return false;

		// ���������� ��������
		savedPos = _info[index].Pos;
		savedCluster = _info[index].Cluster;

		Move2Top(index); // ��������� ������ ������

		return true;
	}

private:

	// ������� ���������� ������� ������
	inline void Move2Top(unsigned long index)
	{
		PosInfo i = _info[index];
		MoveDown(index);
		_info[0] = i;
	}

	// ������� �������� �� 1 ������� (� �������) ���� �� 0 �� index
	inline void MoveDown(unsigned long index)
	{
		if (index == 0)
			return;
		if (index >= _maxSize)
			index = _maxSize - 1;
		for(unsigned long i = index; i > 0; i--)
			_info[i] = _info[i - 1];
	}

	// ������� �������� �� 1 ������� (� �������) �����, ������� � index,
	// ���������� ��� �������� ������
	inline void MoveUp(unsigned long index)
	{
		if (index >= _size)
			return;
		for(unsigned long i = index; i < _size - 1; i++)
			_info[i] = _info[i + 1];
		_size--;
	}

private:

	PosInfo _info[_maxSize];
	unsigned long _size;

};

#endif // _MEDIA_FS_FAT32_POSITION_HISTORY_H_
