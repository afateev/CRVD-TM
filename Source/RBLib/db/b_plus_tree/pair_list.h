
#ifndef _RBLIB_BD_BPLUS_PAIR_LIST_H_
#define _RBLIB_BD_BPLUS_PAIR_LIST_H_

#include "pair.h"
#include "min_uint.h"
#include "../../static_check.h"

namespace Rblib
{
namespace Db
{
namespace BPlusTree
{

namespace _private
{
	//
	// ��������������� ����� ��� ����������� ����. ������� ������
	// � ����������� �� ������� �������� � ������ ���������� ����� � ������ ��� ������ "������"
	// � ������ ����, ��� ������� ������ ������ ��������� � ���������� ������������ �������
	//
	template<unsigned long PairSize, unsigned long MaxSpace>
	class MaxSizeCalc
	{
		// ����. ���-�� ���������, ���� ���������� ��� �������� �������� ���-�� ��������� ����� �������� ������� (1 ����)
		static const unsigned long MaxSize1 = (MaxSpace - 1) / PairSize;
		// ������� �� ����� ���� ����� �������� ���������� ��� �������� ������ �������� (���-�� ���������)
		static const unsigned char TypeSize1 = sizeof(MinUInt<MaxSize1>::T);

		// ������������� ����. ���-�� ���������, ������ �� ������ ������� ����������
		static const unsigned long MaxSize2 = (MaxSpace - TypeSize1) / PairSize;
		// ������ ���������� ��� �������� ������ ��������
		static const unsigned char TypeSize2 = sizeof(MinUInt<MaxSize2>::T);

	public:

		// ����������� ������ ������
		static const unsigned long MaxSize = (TypeSize2 < TypeSize1) ? (MinUInt<MaxSize2>::MaxVal) : (MaxSpace - TypeSize2) / PairSize;
	};
}

//
// ��������������� �� �������� ����� ������ ��� ��������
//
#pragma pack(push, 1)
// MaxSpace - ������������ ������, ������� ����� ����� ������ ����� ������
template<class Key, class Value, unsigned long MaxSpace>
class PairList
{
public:

	typedef Pair<Key, Value> PairType; // ��� �������� ������
	static const unsigned long MaxSize = _private::MaxSizeCalc<sizeof(PairType), MaxSpace>::MaxSize; // ������������ ������ ������
	typedef typename MinUInt<MaxSize>::T SizeType;

public:

	PairList() : _size(0)
	{
		StaticCheck();
	}

	// �������� ���������� ������ �� ����� �����������
	static void StaticCheck()
	{
		// !!! ������ ������ ������� �� ����� 4-� ��������� (����� �� ������� B-������) !!!
		RBLIB_STATIC_CHECK(MaxSize < 4, PAIRLIST_MAXSIZE_LESS_THEN_4);
	}

	//
	// ������
	//

	inline unsigned char Size() const
	{
		return _size;
	}

	inline bool Empty()
	{
		return _size == 0;
	}

	inline void SetSize(unsigned char size)
	{
		_size = size;
	}

	inline void Clear()
	{
		_size = 0;
	}

	inline bool Full() const
	{
		return _size == MaxSize;
	}

	static inline bool ThinSize(SizeType sz)
	{
		return sz < (MaxSize >> 1);
	}

	inline bool Thin() const
	{
		return ThinSize(_size);
	}

	inline bool HalfFull() const
	{
		return _size > (MaxSize >> 1);
	}

	//
	// ������ � ������
	//

	inline PairType& operator[](unsigned long i)
	{
		return _list[i];
	}

	//
	// ����������� �������
	//

	inline Key MinKey()
	{
		return _list[_size - 1].Key();
	}

	inline Value MinValue()
	{
		return _list[_size - 1].Value();
	}

	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	inline void SetMinKey(Key key)
	{
		if (_size == 0)
			return;
		_list[_size - 1].SetKey(key);
	}

	inline void SetMinValue(Value value)
	{
		if (_size == 0)
			return;
		_list[_size - 1].SetValue(value);
	}

	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	inline void SetMin(Key key, Value value)
	{
		if (_size == 0)
			return;
		_list[_size - 1].SetKey(key);
		_list[_size - 1].SetValue(value);
	}

	//
	// ������������ �������
	//

	inline Key MaxKey()
	{
		return _list[0].Key();
	}

	inline Value MaxValue()
	{
		return _list[0].Value();
	}

	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	inline void SetMaxKey(Key key)
	{
		if (_size == 0)
			return;
		_list[0].SetKey(key);
	}

	inline void SetMaxValue(Value value)
	{
		if (_size == 0)
			return;
		_list[0].SetValue(value);
	}

	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	inline void SetMax(Key key, Value value)
	{
		if (_size == 0)
			return;
		_list[0].SetKey(key);
		_list[0].SetValue(value);
	}

	//
	// �������
	//
	bool Insert(Key key, Value value)
	{
		if (_size >= MaxSize)
			return false;

		SizeType pos = FindPos(key);

		// ���� ��� ����� ������, ��������� � �����
		if (pos == _size)
		{
			_list[_size].Set(key, value);
			_size++;
		}
		// ���� ����� ����� �� ����, ������ �����. �������
		else if (_list[pos].Key() == key)
		{
			_list[pos].Set(value);
		}
		else
		{
			// �������� ������� ����� �� 1 �������
			MoveElements(pos);
			// ��������� ����� ������� �� �������������� �����
			_list[pos].Set(key, value);
		}

		return true;
	}

	// ������� � ����� ������
	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	bool InsertMin(Key key, Value value)
	{
		if (_size >= MaxSize)
			return false;
		_list[_size++].Set(key, value);
		return true;
	}

	//
	// �����
	//
	SizeType Find(Key key)
	{
		for(SizeType i = 0; i < _size; i++)
			if (_list[i].Key() == key)
				return i;
		return _size;
	}

	//
	// �����������
	//
	// ������ �������� � �������� � ������ key
	bool SetValueForKey(Key key, Value value)
	{
		SizeType i = Find(key);
		if (i >= _size)
			return false;
		_list[i].SetValue(value);
		return true;
	}

	//
	// ��������
	//
	// �������� �� �������
	void Delete(SizeType index)
	{
		for(SizeType i = index; i < (_size - 1); i++)
			_list[i] = _list[i + 1];
		if (_size)
			_size--;
	}

	// ����� �������� �� ����� (����� ������� -> ��������)
	bool DeleteWise(Key key)
	{
		SizeType index = Find(key);
		if (index >= _size)
			return false;
		Delete(index);
		return true;
	}

	// ������� ����������� �������
	inline void DeleteMin()
	{
		_size--;
	}

	// ������� ������������ �������
	inline void DeleteMax()
	{
		Delete(0);
	}

	// ������� ������� ��������, ������� � index
	void DeleteMajorFrom(SizeType index)
	{
		if (index >= _size)
			return;
		_size = _size - index - 1; // ��������� ������ ������
		// ������� ��������
		for (SizeType i = 0; i < _size; i++)
			_list[i] = _list[index + 1 + i];
	}

	// ������� �� �������, �������������� ���������� ������� �����
	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	bool MergeWithMinor(PairList& pairList)
	{
		if (pairList._size == 0)
			return true;
		if (pairList._size > MaxSize - _size)
			return false;
		// ��������� ����� �������� � ����� ������
		for(SizeType i = 0; i < pairList._size; i++)
			_list[_size + i] = pairList._list[i];
		_size += pairList._size;
		return true;
	}

	// ������� �� �������, �������������� ���������� ������� �����
	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	bool MergeWithMajor(PairList& pairList)
	{
		if (pairList._size == 0)
			return true;
		if (pairList._size > MaxSize - _size)
			return false;
		// �������� ������� ������ �� pairList._size ����
		for(SizeType i = 0; i < _size; i++)
			_list[pairList._size + _size - 1 - i] = _list[_size - 1 - i];
		// �������� ����� �������� � ������� ������
		for(SizeType i = 0; i < pairList._size; i++)
			_list[i] = pairList._list[i];
		_size += pairList._size; // ������ ������
		return true;
	}

	//
	// ��������� ���������� ����
	// ������� ������ ���������� ������ ��� ������� �������!!!
	//
	// �������� �������� ���������, ���������� ������� �����
	void CopyMajor(PairList& pairList)
	{
		static const unsigned long sizeToMove = MaxSize >> 1; // ������� ����� �����������

		for(unsigned long i = 0; i < sizeToMove; i++)
		{
			pairList._list[i] = _list[i];
		}

		pairList._size = sizeToMove;
	}

	// �������� �������� ���������, ���������� ������� �����
	void CopyMinor(PairList& pairList)
	{
		static const unsigned long sizeMajor = MaxSize >> 1; // ������ �������� � �������� �������
		static const unsigned long sizeToMove = MaxSize - sizeMajor; // ������� ����� �����������

		for(unsigned long i = 0; i < sizeToMove; i++)
		{
			pairList._list[i] = _list[sizeMajor + i];
		}

		pairList._size = sizeToMove;
	}

	// �������� �������� ������, � �������� ��� �������� ������� � ����������� �� ���������
	inline void CopyPart(bool major, PairList& pairList)
	{
		if (major)
			CopyMajor(pairList);
		else
			CopyMinor(pairList);
	}

	// ���������� num ������� ��������� � pairList, ���������� ������� ��������, ��� � ���
	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	bool MoveMinor2Minor(PairList& pairList, SizeType num)
	{
		// ���� ������ ������ ������ ������ num, �� ��������� ���� ��� ������
		num = _size >= num ? num : _size;
		// ���� �� ������� ������ �� ����� - ��� ������
		if (pairList._size + num > MaxSize)
			return false;
		// �������� �������� �� num ���� �� ������� ������
		for (SizeType i = 0; i < pairList._size; i++)
			pairList._list[pairList._size + num - 1 - i] = pairList._list[pairList._size - 1 - i];
		// �������� �������� � ������ �������� ������
		for (SizeType i = 0; i < num; i++)
			pairList._list[i] = _list[_size - num + i];
		// ������ ������� �������
		_size -= num;
		pairList._size += num;
		return true;
	}

	// ���������� num ������� ��������� � pairList, ���������� ������� ��������, ��� � ���
	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	bool MoveMajor2Major(PairList& pairList, SizeType num)
	{
		// ���� ������ ������ ������ ������ num, �� ��������� ���� ��� ������
		num = _size >= num ? num : _size;
		// ���� �� ������� ������ �� ����� - ��� ������
		if (pairList._size + num > MaxSize)
			return false;
		// �������� �������� � ����� �������� ������
		for(SizeType i = 0; i < num; i++)
			pairList._list[pairList._size + i] = _list[i];
		// ������ ������� �������
		_size -= num;
		pairList._size += num;
		// �������� �������� �� num ����� � ����� ������
		for(SizeType i = 0; i < _size ;i++)
			_list[i] = _list[i + num];
		return true;
	}

	//
	// ��������� ���������� ����
	// ������� ������ ���������� ������ ��� ������� �������!!!
	//
	// ��������� � ������� ������ �������� ������� ��������
	inline void MakeMajorPart()
	{
		// ������ ������������ �� ��������, ������� ������ ������ ������
		_size = MaxSize >> 1;
	}

	// ���������� ������� ������������ ��� ����� ������ ���������� �������
	inline void RestoreFromMajorPart()
	{
		_size = MaxSize; // ���������� ������
	}

	// ��������� � ������� ������ �������� ������� ��������
	inline void MakeMinorPart()
	{
		SwitchParts<MaxSize % 2>(); // ������ ������� �������� �������
		_size = MaxSize - (MaxSize >> 1); // ������ ������
	}

	// ���������� ������� ������������ ��� ����� ������ ���������� �������
	inline void RestoreFromMinorPart()
	{
		SwitchPartsBack<MaxSize % 2>(); // ������� ������ ������� �������� �������
		_size = MaxSize; // ������ ������
	}

	// ��������� � ������� �������� ��������, ������� ��� ������� � ����������� �� ���������
	inline void MakePart(bool major)
	{
		if (major)
			MakeMajorPart();
		else
			MakeMinorPart();
	}

	// ���������� ������� ������������ ��� ����� ������ ���������� �������
	inline void RestoreFromPart(bool major)
	{
		if (major)
			RestoreFromMajorPart();
		else
			RestoreFromMinorPart();
	}

	// � ����� �������� (������� ��� ������� ��������) ��������� ������?
	static inline bool MajorIndex(SizeType index)
	{
		return index < (MaxSize >> 1);
	}

private:

	//
	// ������� ������� �������� �������
	// ������� ������������ ������ ��� ������������ ������, ������� ���������� ��������� MaxSize
	//
  	
  	template<bool val> class StaticBool{};
  
	template<bool odd> void SwitchParts()
	{
		SwitchParts(StaticBool<odd>());
	}

	// ��� ������� ���������� ���������
	void SwitchParts(StaticBool<false>)
	{
		static const unsigned long halfSize = MaxSize >> 1;
		for(unsigned long i = 0; i < halfSize; i++)
		{
			PairType tmp = _list[i];
			_list[i] = _list[halfSize + i];
			_list[halfSize + i] = tmp;
		}
	}

	// ��� ��������� ���������� ���������
	// "��������" ����� ������ ����� (����� ���������� �� 1)
	void SwitchParts(StaticBool<true>)
	{
		static const unsigned long halfSize = MaxSize >> 1;

		PairType tmp1 = _list[halfSize];

		for(unsigned long i = 0; i < halfSize; i++)
		{
			PairType tmp2 = _list[i];
			_list[i] = tmp1;
			tmp1 = _list[halfSize + 1 + i];
			_list[halfSize + 1 + i] = tmp2;
		}

		_list[halfSize] = tmp1;
	}

	//
	// ������� ������ � �������� ��� ����� ������ ���������� �������
	// ������� ������������ ������ ��� ������������ ������, ������� ���������� ��������� MaxSize
	//
	template<bool odd> void SwitchPartsBack()
	{
		SwitchPartsBack(StaticBool<odd>());
	}

	// ��� ��������� ���-�� ���������
	// "��������" ������ ����� (����� ���������� �� 1)
	void SwitchPartsBack(StaticBool<true>)
	{
		static const unsigned long halfSize = MaxSize >> 1;

		PairType tmp1 = _list[halfSize];

		for(unsigned long i = 0; i < halfSize; i++)
		{
			PairType tmp2 = _list[MaxSize - 1 - i];
			_list[MaxSize - 1 - i] = tmp1;
			tmp1 = _list[halfSize - i - 1];
			_list[halfSize - i - 1] = tmp2;
		}

		_list[halfSize] = tmp1;
	}

	// ��� ������� ���-�� ���������
	inline void SwitchPartsBack(StaticBool<false>)
	{
		SwitchParts<false>();
	}

	//
	// ��������������� ������� ��� ������� � ������
	//

	// ���� ������� ��� ������� �����
	SizeType FindPos(Key key)
	{
		for(SizeType i = 0; i < _size; i++)
			if (_list[i].Key() <= key)
				return i;
		return _size;
	}

	// �������� �������� ������ �� ���� �������, ������� � pos
	// ���� ������ ������, �������� ����� ������!
	void MoveElements(SizeType pos)
	{
		for(SizeType i = _size++; i > pos; i--)
			_list[i] = _list[i - 1];
	}

protected:

	SizeType _size; // ������� ������
	PairType _list[MaxSize]; // ������

};
#pragma pack (pop)

}
}
}

#endif // _RBLIB_BD_BPLUS_PAIR_LIST_H_

