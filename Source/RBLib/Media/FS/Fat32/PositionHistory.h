
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

	// добавить позицию
	void Add(unsigned long pos, unsigned long cluster)
	{
		for (unsigned long i = 0; i < _size; i++)
		{
			// если позиция уже есть в списке, просто поднимаем её наверх
			if (_info[i].Pos == pos)
			{
				// на всякий случай сравниваем кластера и обновляем, если разные,
				// по идее не должно такого быть
				if (_info[i].Cluster != cluster)
					_info[i].Cluster = cluster;
				Move2Top(i); // поднимаем запись наверх
				return;
			}
		}
		MoveDown(_size); // сдвигаем записи на 1 вниз
		_info[0] = PosInfo(pos, cluster); // новую запись помещаем в начало списка
		_size += (_size < _maxSize) ? 1 : 0; // увеличиваем размер, если надо
	}

	// удаляет все сохранённые позиции, начиная с pos
	void RemoveAllFrom(unsigned long pos)
	{
		for(unsigned long i = 0; i < _size;)
		{
			if (_info[i].Pos >= pos)
				MoveUp(i); // двигаем всё на 1 вверх
			else
				i++;
		}
	}

	// найти позицию
	bool Find(unsigned long pos, unsigned long& savedPos, unsigned long& savedCluster)
	{
		unsigned long p = 0;
		unsigned long index = 0;

		// подбираем самую близкую позицию из сохранённых
		for(unsigned long i = 0; i < _size; i++)
		{
			if (_info[i].Pos <= pos && _info[i].Pos > p)
			{
				p = _info[i].Pos;
				index = i;
			}
		}

		// если не нашли ничего подходящего
		if (p == 0)
			return false;

		// возвращаем найдённое
		savedPos = _info[index].Pos;
		savedCluster = _info[index].Cluster;

		Move2Top(index); // поднимаем запись наверх

		return true;
	}

private:

	// поднять сохранённую позицию наверх
	inline void Move2Top(unsigned long index)
	{
		PosInfo i = _info[index];
		MoveDown(index);
		_info[0] = i;
	}

	// двигает элементы на 1 позицию (в массиве) вниз от 0 до index
	inline void MoveDown(unsigned long index)
	{
		if (index == 0)
			return;
		if (index >= _maxSize)
			index = _maxSize - 1;
		for(unsigned long i = index; i > 0; i--)
			_info[i] = _info[i - 1];
	}

	// двигает элементы на 1 позицию (в массиве) вверх, начиная с index,
	// фактически это удаление записи
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
