
#ifndef _RBLIB_BD_BPLUS_NODE_H_
#define _RBLIB_BD_BPLUS_NODE_H_

#include "header.h"
#include "pair_list.h"
#include "type_selector.h"
#include "../../static_check.h"
#include "../size_addition.h"
#include "../digit_selector.h"

namespace Rblib
{
namespace Db
{
namespace BPlusTree
{

#pragma pack(push, 1)
template<class KeyT, class ValueT, bool _64bit = false, unsigned long _pageSize = 512>
class Node : public Header<typename DigitSelector<_64bit>::T> // ���������
{
private:

	//
	// ���������� ���� ������
	//
	typedef Header<typename DigitSelector<_64bit>::T> Header; // ��� ���������
	// ������ ��� ����-�������� ��� ����������� ���� (�������� ����� � �������� �����-��������)
	typedef PairList<KeyT, typename Header::OffsetType, _pageSize - sizeof(Header)> InternalPairListType;
	// ������ ��� ����-�������� ��� ����-����� (�������� ����� � ��������������� �� ���������������� ������)
	typedef PairList<KeyT, ValueT, _pageSize - sizeof(Header)> LeafPairListType;

	typedef typename InternalPairListType::SizeType InternalSizeType; // ��� ��� �������� ������� ����������� ����
	typedef typename LeafPairListType::SizeType LeafSizeType; // ��� ��� �������� ������� ����-�����

public:

	//
	// ���� ������, ������� ����� ������������ �������� ������
	//
	typedef KeyT KeyType; // ��� �����
	typedef ValueT ValueType; // ��� ��������� ��������
	typedef typename Header::OffsetType OffsetType;

	// ��� ��� �������� ������� ���� - ������� �� ���� (��� ����������� ���� � ����-�����)
	typedef typename TypeSelector<InternalSizeType, LeafSizeType, sizeof(InternalSizeType) >= sizeof(LeafSizeType)>::T SizeType;

	typedef typename InternalPairListType::PairType InternalPairType; // ���� ����-�������� ��� ����������� ����
	typedef typename LeafPairListType::PairType LeafPairType; // ���� ����-�������� ��� ����-�����

	static const unsigned long LeafMaxSize = LeafPairListType::MaxSize; // ������������ ������ ����-����� (������� �� ����� ��� ������ ������)

public:

	Node()
	{
		// !!! ������������ ������ ��������, ������ ���� ������� 512 !!!
		RBLIB_STATIC_CHECK(_pageSize == 0 || _pageSize % 512, INVALID_PAGE_SIZE);
		// !!! ���� ������ ��������� �� 1 ��������, ���-�� ������ !!!
		RBLIB_STATIC_CHECK(sizeof(Node<KeyT, ValueT, _64bit, _pageSize>) != _pageSize, INVALID_NODE_SIZE);

		InternalPairListType::StaticCheck(); // ��������� ��������� ����������� ����
		LeafPairListType::StaticCheck();  // ��������� ��������� ����-�����

		InternalPairList().Clear(); // ������� ������ ������ (internal ��� leaf �������, �.�. ��� �� ����� ������)
	}

	//
	// ��, ��� ������� � �������� ������
	//

	// ������ (���������� ��������� � ������ ��� ����-��������)
	inline SizeType Size()
	{
		return Header::Leaf() ? LeafPairList().Size() : InternalPairList().Size();
	}

	// ������������ ������ ������ ��� ����-��������
	inline SizeType MaxSize()
	{
		return Header::Leaf() ? LeafPairListType::MaxSize : InternalPairListType::MaxSize;
	}

	// �������� �� ������ � �������� ������ � �������� ����������?
	inline bool MajorIndex(SizeType i)
	{
		return Header::Leaf() ? LeafPairList().MajorIndex(i) : InternalPairList().MajorIndex(i);
	}

	// ������ �������� (������� ����������)?
	inline bool Full()
	{
		return Header::Leaf() ? LeafPairList().Full() : InternalPairList().Full();
	}

	// ������ ���������� (������� ��������������� ��� ������� � ������ �����)?
	inline bool Thin()
	{
		return Header::Leaf() ? LeafPairList().Thin() : InternalPairList().Thin();
	}

	// ���������, ������ �� ������ ����������, ���� ��� ������ ������ ������ sz
	inline bool ThinSize(SizeType sz)
	{
		return Header::Leaf() ? LeafPairListType::ThinSize(sz) : InternalPairListType::ThinSize(sz);
	}

	// ������ ����?
	inline bool Empty()
	{
		return Header::Leaf() ? LeafPairList().Empty() : InternalPairList().Empty();
	}

	// �������� ������ ��� ����-��������
	inline void Clear()
	{
		Header::Leaf() ? LeafPairList().Clear() : InternalPairList().Clear();
	}

	//
	// ������ � ������ � ��������� � ����
	//

	// ���� i-�� �������� � ���� (� ������ ��� ����-��������)
	inline KeyType Key(SizeType i)
	{
		if (i >= Size())
			return KeyType(); // �������, ��� �� ������ (����� �� ������ � ������������ ������� ��� ������ LoadNodeWise)
		return Header::Leaf() ? LeafPairList()[i].Key() : InternalPairList()[i].Key(); 
	}

	// ����������� ����
	inline KeyType MinKey()
	{
		return Header::Leaf() ? LeafPairList().MinKey() : InternalPairList().MinKey();
	}

	// ������������ ����
	inline KeyType MaxKey()
	{
		return Header::Leaf() ? LeafPairList().MaxKey() : InternalPairList().MaxKey();
	}

	// ���������� ���� i-�� ��������
	// ������� �����, �� ������������ ������� ���������!!!
	inline void SetKey(SizeType i, KeyType key)
	{
		Header::Leaf() ? LeafPairList()[i].SetKey(key) : InternalPairList()[i].SetKey(key);
	}

	// ���� ����-�������� ��� i-�� �������
	// ������ ��� ����������� ����, �������� �� ��� ���� ���, ������� �����!!!
	inline InternalPairType ChildPair(SizeType i)
	{
		return InternalPairList()[i];
	}

	// ���� ����-�������� ��� i-�� ��������
	// ������ ��� �����, �������� �� ��� ���� ���, ������� �����!!!
	inline LeafPairType Pair(SizeType i)
	{
		return LeafPairList()[i];
	}

	// c������� ��� i-�� �������
	// ������ ��� ����������� ����, �������� �� ��� ���� ���, ������� �����!!!
	inline OffsetType Offset(SizeType i)
	{
		return InternalPairList()[i].Value();
	}

	// �������� ��� i-�� ��������
	// ������ ��� �����, �������� �� ��� ���� ���, ������� �����!!!
	inline ValueType Value(SizeType i)
	{
		return LeafPairList()[i].Value();
	}

	//
	// ����������� ����
	//

	// ������� �� ���������� ����
	inline bool InsertChild(KeyType key, OffsetType offset)
	{
		return InternalPairList().Insert(key, offset);
	}

	// ������� � ����-����
	inline bool InsertValue(KeyType key, ValueType value)
	{
		return LeafPairList().Insert(key, value);
	}

	// ������� ������� �� �������
	inline void Delete(SizeType i)
	{
		Header::Leaf() ? LeafPairList().Delete(i) : InternalPairList().Delete(i);
	}

	// ������� ������� �� �����
	inline bool DeleteWise(KeyType key)
	{
		return Header::Leaf() ? LeafPairList().DeleteWise(key) : InternalPairList().DeleteWise(key);
	}

	inline void DeleteMajorFrom(SizeType i)
	{
		Header::Leaf() ? LeafPairList().DeleteMajorFrom(i) : InternalPairList().DeleteMajorFrom(i);
	}

	// ������ �������� ��� �������� � �������� ������,
	// ������ ��� ����������� ����
	inline bool SetOffsetForKey(KeyType key, OffsetType offset)
	{
		if (Header::Leaf())
			return false;
		return InternalPairList().SetValueForKey(key, offset);
	}

	// ������ �������� ��� �������� � �������� ��������,
	// ������ ��� ����������� ����
	inline bool SetOffset(SizeType i, OffsetType offset)
	{
		if (Header::Leaf())
			return false;
		InternalPairList()[i].SetValue(offset);
		return true;
	}

	//
	// �����
	//

	// ����� ���� (� ������ ������� ����)
	inline SizeType FindPath(KeyType key)
	{
		return Header::Leaf() ? FindPath<LeafPairListType>(key, LeafPairList()) : FindPath<InternalPairListType>(key, InternalPairList());
	}

	// ����� �������� � ������� ������
	inline SizeType Find(KeyType key)
	{
		return Header::Leaf() ? LeafPairList().Find(key) : InternalPairList().Find(key);
	}

	//
	// �������, ������������ ��� ����������, ��������������� � ������� �����
	// ������� ������ ���������� ������ ��� ������� �������!!!
	//
	// �������� �������� ���������, ���������� ������� �����
	inline void CopyMinor(Node& node)
	{
		if (Header::Leaf())
			LeafPairList().CopyMinor(node.LeafPairList());
		else
			InternalPairList().CopyMinor(node.InternalPairList());
	}

	// �������� �������� ���������, ���������� ������� �����
	inline void CopyMajor(Node& node)
	{
		if (Header::Leaf())
			LeafPairList().CopyMajor(node.LeafPairList());
		else
			InternalPairList().CopyMajor(node.InternalPairList());
	}

	// ��������� � ���� ������ �������� ������� (�� �����) ���������
	inline void MakeMinorPart()
	{
		Header::Leaf() ? LeafPairList().MakeMinorPart() : InternalPairList().MakeMinorPart();
	}

	// ��������� � ���� ������ �������� ������� (�� �����) ���������
	inline void MakeMajorPart()
	{
		Header::Leaf() ? LeafPairList().MakeMajorPart() : InternalPairList().MakeMajorPart();
	}

	// ���������� ���� ������������ ��� ����� ������ MakeMinorPart
	inline void RestoreFromMinorPart()
	{
		Header::Leaf() ? LeafPairList().RestoreFromMinorPart() : InternalPairList().RestoreFromMinorPart();
	}

	// ���������� ���� ������������ ��� ����� ������ MakeMajorPart
	inline void RestoreFromMajorPart()
	{
		Header::Leaf() ? LeafPairList().RestoreFromMajorPart() : InternalPairList().RestoreFromMajorPart();
	}

	// ���������� num ������� ��������� � node, ���������� ������� ��������, ��� � ���
	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	inline bool MoveMinor2Minor(Node& node, SizeType num)
	{
		if (Header::Leaf())
			return LeafPairList().MoveMinor2Minor(node.LeafPairList(), num);
		else
			return InternalPairList().MoveMinor2Minor(node.InternalPairList(), num);
	}

	// ���������� num ������� ��������� � node, ���������� ������� ��������, ��� � ���
	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	inline bool MoveMajor2Major(Node& node, SizeType num)
	{
		if (Header::Leaf())
			return LeafPairList().MoveMajor2Major(node.LeafPairList(), num);
		else
			return InternalPairList().MoveMajor2Major(node.InternalPairList(), num);
	}

	// ������� � �����, �������������� ���������� ������� �����
	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	inline bool MergeWithMajor(Node& node)
	{
		if (Header::Leaf())
			return LeafPairList().MergeWithMajor(node.LeafPairList());
		else
			return InternalPairList().MergeWithMajor(node.InternalPairList());
	}

	// ������� � �����, �������������� ���������� ������� �����
	// ������� �����, �� ������������ ������� ������, �������� � ����!!!
	inline bool MergeWithMinor(Node& node)
	{
		if (Header::Leaf())
			return LeafPairList().MergeWithMinor(node.LeafPairList());
		else
			return InternalPairList().MergeWithMinor(node.InternalPairList());
	}

private:

	// ������������� ����� ���� ���������� �� ����, ��� �������� � PairList
	template<class PairList>
	static SizeType FindPath(KeyType key, PairList& list)
	{
		SizeType lastIndex = list.Size() - 1;

		for(SizeType i = 0; i < lastIndex; i++)
		{
			if (key >= list[i].Key())
				return i;
		}

		return lastIndex;
	}

	// ���������� ������ �� ������ ��� ����-�������� ��� ����������� ����
	inline InternalPairListType& InternalPairList()
	{
		return (InternalPairListType&)*_buffer;
	}

	// ���������� ������ �� ������ ��� ����-�������� ��� ����-�����
	inline LeafPairListType& LeafPairList()
	{
		return (LeafPairListType&)*_buffer;
	}

private:

	unsigned char _buffer[_pageSize - sizeof(Header)];
};
#pragma pack (pop)

}
}
}

#endif // _RBLIB_BD_BPLUS_NODE_H_

