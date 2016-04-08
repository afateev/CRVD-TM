
#ifndef _RBLIB_BD_BPLUS_TREE_H_
#define _RBLIB_BD_BPLUS_TREE_H_

#include "file_client.h"
#include "b_plus_tree/node.h"
#include "b_plus_tree/node_wrapper.h"
#include "digit_selector.h"

namespace Rblib
{
// �����, ����������� B+ ������
// FS - ������ � �������� �������
// Key - ��� �����, �� �������� ����� ����������� � ������ ������
// Value - ��� ��������� �������� (������ Value ������ ��������� ������� �� �������� �� ����� 4-� ��� ����-��������)
// _64bit - ����� ����������� �������� � ����� (������������ ������ ����� � ������� 2^32 ��� 2^64), ��� FAT32 ������ _64bit=false
// _pageSize ������ ������7���� ��������, ������� ������ ��������� �� 1 ���
template<class FS, class Key, class Value, bool _64bit = false, unsigned long _pageSize = 512>
class BPlusTree : private Db::FileClient<FS>
{

private:

	//
	// ���������� ���� ������
	//
	typedef Db::BPlusTree::Node<Key, Value, _64bit, _pageSize> NodeType; // ��� ����
	typedef Db::BPlusTree::NodeWrapper<NodeType> NodeWrapperType; // ������ ����, �������� ��� ���������, ������ ��� ���������
	typedef typename NodeType::OffsetType OffsetType; // ��� ��� ������������� �������� ����� � �����
	typedef typename NodeType::SizeType SizeType; // ��� ��� ������������� ������� ������ � ����
	typedef Db::FileClient<FS> FileClient; // ������ � �����

public:

	//
	// ���� ������, ������� ����������� �������� ������
	//
	// ���� ����-�������, �������� - ������ ������������, ��������������� � ������
	typedef typename NodeType::LeafPairType PairType;

public:

	BPlusTree(FS& fs)
	: FileClient(fs)
	, _currentOffset(0)
	, _currentElement(-1) // ������ ������������ ��������
	, _needLastKeyCheck(false)
	, _rootModified(false)
	, _rootUnvalid(true) // � ������ ������ �� ������
	{
	}

	//
	// ������� ���� � ������� ��� ������� ����� ������
	//
	bool Open(char* fileName)
	{
		if (!FileClient::Open(fileName))
			return false;

		if (LoadRoot()) // ��������� ������
			return true;

		if (FileClient::IoFault()) // ���� �� ��������� � ���� ������ ������
			return false;

		_root.Reset(); // ������ ����, ������� ����� ������
		_root.SetRoot();
		_root.SetLeaf(); // � ������ ������ �������� ������

		if (!SaveRoot())
			return false; // ����� ����� ������
		return FileClient::Flush(); // ��������� FAT
	}

	//
	// ������� ���� � �������
	//
	bool Close()
	{ 
		return FileClient::Close();
	}

	//
	// ��������� ������������
	//
	inline bool Flush()
	{
		bool rootOk = FlushRoot(); // ��������� ������
		bool fatOk = FileClient::Flush(); // ��������� �� ��������� (������������ FAT)
		return rootOk && fatOk;
	}

	//
	// ������ � ������
	//
	bool Insert(Key key, Value value)
	{
		if (!CheckRoot()) // ������ ������ ���� ����������
			return false;

		// ��� ���������� ����� ����� 2 �������-���� (�������� � �������)
		NodeType node1; // ������ ������-����
		NodeType* curNode = &_root; // ������� ���� - ������
		OffsetType curNodeOffset = 0;

		NodeType* curParent = &_root; // �������� �������� ����
		OffsetType curParentOffset = 0; // �������� �������� �������� ����
		SizeType curIndex = 0; // ������ �������� � ��������, ������������ �� ������� ����

		if (_root.Full()) // ���� ������ ��� ������, �� ��� ���� ���������
		{
			// ����� ��������� ���������� ����,
			// ����� � ������� ���� (curNode) ����� ���������� �������� ������ ���� (�������� �����) � �� ���� ���� ��� ����� ���������
			SizeType pathIndex = _root.FindPath(key);
			curIndex = _root.MajorIndex(pathIndex) ? 0 : 1; // ���������� ������ ������ ���� � ����� �����, ����������� ��� �������
			curNode = &node1; // ������ ������� ���� - ������ ������, ���������� � SplitRoot
			if ((curNodeOffset = SplitRoot(*curNode, _root.MajorIndex(pathIndex))) == 0) // ��������� ������
				return false;
		}

		NodeType node2; // ������ ������-����
		NodeType* child = &node2; // ������� - ������ ������, ���������� ��� ���������� (LoadNode)

		// ���� ����-����, � ������� ���� ���������� �������,
		// �� ���� ��������� ����, ���� �����
		while(!curNode->Leaf())
		{
			// ���������� ������� (���������), � ������� ������� ����
			SizeType pathInParentIndex = curNode->FindPath(key); // ����� � �������
			OffsetType childOffset = curNode->Offset(pathInParentIndex); // �������� � �����

			// ���������� ��������, ����������� ��� �������
			curParent = curNode;
			curParentOffset = curNodeOffset;
			curIndex = pathInParentIndex;

			if (!LoadNodeWise(*child, childOffset, pathInParentIndex > 0, curNode->Key(pathInParentIndex - 1))) // ��������� ����
				return false;

			// ���� ���� ��� ��������, �� ��� ���� ���������
			if (child->Full())
			{
				// ����� ��������� ���������� ����,
				// ����� � ������� ������� (child) ����� ���������� �������� ������ ���� (�������� �������� �������)
				//  � �� ���� ���� ��� ����� ���������
				SizeType pathInChildIndex = child->FindPath(key);
				// ��������� �������� ������� (child)
				curIndex += child->MajorIndex(pathInChildIndex) ? 0 : 1;
				if ((childOffset = SplitNodeWise(*curNode, curNodeOffset, pathInParentIndex, *child, childOffset, child->MajorIndex(pathInChildIndex))) == 0)
					return false;
			}

			// �� ��������� �������� ��������� �������� ���� ����� �������� �������� �������
			curNodeOffset = childOffset;

			if (curNode->Root()) // ���� ������� ���� ������ (������, ���������� ����� �� ����)
			{
				// ������� ���� ����� ��������� �� ������, � ������� �������
				curNode = child;
				// � ������� ����� ��������� �� ��� �� �������������� ������ node1
				child = &node1;
			}
			else
			{
				// ������ ������ �� ������� �����
				// ������� ����� ����� ��������� �� ������, � ������� �������
				// � ������� ����� ��������� �� ������, � ������� ��� �������� ������� ����
				NodeType* tmp = curNode;
				curNode = child;
				child = tmp;
			}
		}
		
		curNode->InsertValue(key, value); // ��������� ����� ������� � ����-����
		
		// ���� ������� �� � ������ � ��������� ���. ����, ���� ������� �������� � ��������� ��������
		if (!curNode->Root() && curParent->Key(curIndex) > curNode->MinKey())
		{
			curParent->SetKey(curIndex, curNode->MinKey());
			if (!SaveNode(*curParent, curParentOffset))
				return false; // ���� �������� �������� �� ������, ������� �� �������
		}

		return SaveNode(*curNode, curNodeOffset); // ��������� ���� � �������
	}

	//
	// ����� �������� �� �����
	// value - ��������� ��������
	//
	bool Find(Key key, Value& value)
	{
		if (!CheckRoot()) // ������ ������ ���� ����������
			return false;

		NodeType* curNode = &_root;
		NodeType node;
		
		while(!curNode->Leaf()) // ���������� �� ������ �� �����
		{
			SizeType index = curNode->FindPath(key); // ���������� ��������� (���� �������) � ������� ������
			OffsetType nextOffset = curNode->Offset(index);
			curNode = &node;
			if (!LoadNodeWise(*curNode, nextOffset, index > 0, curNode->Key(index - 1))) // ��������� �������
				return false;
		}
		
		// � curNode ����-����, � ������� ������ ���� ������� ����
		SizeType index = curNode->Find(key);
		if (index == curNode->Size()) // ���� �� ����� � ���� �����
			return false; // �������

		// ����� ���������� �������� � value
		value = curNode->Value(index);
		return true;
	}

	//
	// �������� �� �����
	//
	bool Delete(Key key)
	{
		_lostElementsCount = 0; // ���������� ���-�� ���������� ��������� (��� �������)

		if (!CheckRoot()) // ������ ������ ���� ����������
			return false;

		// ���� ������ - ����
		if (_root.Leaf())
		{
			// �� ������ ������� �������
			if (!_root.DeleteWise(key))
				return false;
			return SaveRoot(); // ��������� ������
		}

		// 2 �������, � ������� ����� ��������� ������ ����
		NodeType node1;
		NodeType node2;
		NodeType* parent = &_root; // ������� ��������
		OffsetType parentOffset = 0;

		NodeType* child = &node1; // ������� �������
		NodeType* childSibling = &node2; // ������� ���� ������� (�������� ����, ������� parent)

		SizeType index;
		typename NodeType::InternalPairType nodePair;

		// ���� ����-���� � ������� ������, ������� ����������������� ����, ���� ����
		do 
		{
			index = parent->FindPath(key); // ���������� ��������� � ������� ������
			nodePair = parent->ChildPair(index); // ��� ���������� � ������ �������

			bool root = parent->Root();
			// ����������, ����� ���� ��� ����� (����� ��� ������)
			bool right = index > 0;
			SizeType index2 = right ? index - 1 : index + 1;
			typename NodeType::InternalPairType nodePair2 = parent->ChildPair(index2); // ��� ���������� � �����

			if (!LoadNodeWise(*child, nodePair.Value(), index > 0, parent->Key(index - 1))) // ��������� ������� �������
				return false;

			if (child->Thin()) // ���� ������� ����������, �� ����������� ��������������� � ������
			{
				if (!LoadNodeWise(*childSibling, nodePair2.Value(), index2 > 0, parent->Key(index2 - 1))) // ��������� �����
					return false;

				// ����� ���������������
				if (root) // ���� �������� ������� ������� � ����� �������� �������
				{
					NodeWrapperType childWrapper(child, index, nodePair.Key(), nodePair.Value());
					NodeWrapperType siblingWrapper(childSibling, index2, nodePair2.Key(), nodePair2.Value());
					if (!Redistribute(childWrapper, siblingWrapper, right))
						return false;
					nodePair.SetValue(childWrapper.Offset); // �������� ���� ����������, ��������� ���
				}
				else
				{
					NodeWrapperType childWrapper(child, index, nodePair.Key(), nodePair.Value());
					NodeWrapperType siblingWrapper(childSibling, index2, nodePair2.Key(), nodePair2.Value());
					if (!Redistribute(childWrapper, siblingWrapper, parentOffset, right))
						return false;
					nodePair.SetValue(childWrapper.Offset); // �������� ���� ����������, ��������� ���
				}
			}

			if (child->Leaf()) // ���� ������� ����� - ����, �� ����� �������
				break;

			// ������� ������� ����� ���������
			// ���� ������� ���� ������, �� ��������� ����� ������
			// ! ������� ��� ����� ������, ���� �� ��� ����� �����,
			// ! �  � ����� ���� ����� 2 �������, � ��� ���� ����� � ���� ��� ���������������
			parent = child->Root() ? &_root : child;
			parentOffset = child->Root() ? 0 : nodePair.Value();

			if (!child->Root())
			{
				child = childSibling; // ��������� ������� ����� ������ �� ����� �����
				childSibling = parent; // ���� ����� ������ �� ����� ��������
			}

		} while (1);

		SizeType i = child->Find(key); // ���� ���� � �����
		bool minKeyRemoved = i == (child->Size() - 1);
		if (i >= child->Size()) // ���� �� �����
			return false;

		// ���� ������� ���� ������, �� ������� �� ����� � �������
		if (child->Root())
		{
			_root.Delete(i);
			return SaveRoot();
		}

		// ������� ������� � ��������� ����
		child->Delete(i);
		if (!SaveNode(*child, nodePair.Value()))
			return false;

		// ��������� �� �������������� ��������
		// ! ����������� �������� ���� �������������, ���� ������� ������� � ����������� ������,
		if (!minKeyRemoved)
			return true;

		// ������ ���� � �������� � ���������
		parent->SetKey(index, child->MinKey());
		SaveNode(*parent, parentOffset); // ��������� �� �����, �.�. ������� ��� �����
		return true;
	}

	//
	// ��������� �� ������ �������� � ���������� ������,
	// �������������� ����� ����� ������
	//
	bool First(PairType& value)
	{
		if (!CheckRoot()) // ������ ������ ���� ����������
			return false;

		if (_root.Empty())
		{
			// ���� ������ ��������
			SetIteratorInvalid();
			return false;
		}

		if (_root.Leaf()) // ���� ������ - ����
			return FirstRoot(value);

		return FirstGeneral(value);
	}

	// ��� �� First, ������ ���� � �������� ���������� �� �����������, � �� �����
	inline bool First(Key& key, Value& value)
	{
		if (!CheckRoot()) // ������ ������ ���� ����������
			return false;

		PairType pair;
		bool res = First(pair);
		key = pair.Key();
		value = pair.Value();
		return res;
	}

	//
	// ��������� �� ������ �������� � ���������� ������ ������� ��� ������ key,
	// �������������� ����� ������, ������� � ����� �����
	//
	bool First(Key key, PairType& value)
	{
		if (!CheckRoot()) // ������ ������ ���� ����������
			return false;

		if (_root.Empty())
		{
			// ���� ������ ��������
			SetIteratorInvalid();
			return false;
		}

		if (_root.Leaf()) // ���� ������ - ����
			return FirstRoot(key, value);

		return FirstGeneral(key, value);
	}

	// ��� �� First, ������ ���� � �������� ���������� �� �����������, � �� �����
	bool First(Key keyIn, Key& key, Value& value)
	{
		if (!CheckRoot()) // ������ ������ ���� ����������
			return false;

		PairType pair;
		bool res = First(keyIn, pair);
		key = pair.Key();
		value = pair.Value();
		return res;
	}

	//
	// ����� ������, ��������� ��������� �������
	//
	bool Next(PairType& value)
	{
		if (!CheckRoot()) // ������ ������ ���� ����������
			return false;

		if (IteratorInvalid()) // ���� ����� ��� �������
			return false;

		if (_currentOffset == 0) // ���� ������-���� � ����� � ��
			return NextRoot(value);

		return NextGeneral(value);
	}

	// ��� �� Next, ������ ���� � �������� ���������� �� �����������, � �� �����
	bool Next(Key& key, Value& value)
	{
		if (!CheckRoot()) // ������ ������ ���� ����������
			return false;

		PairType pair;
		bool res = Next(pair);
		key = pair.Key();
		value = pair.Value();
		return res;
	}

	//
	// ������ ��� �������
	//

	// ���������� ���������� ��������� ��� ��������� �������� (���� ���� ������ ��� ���������������)
	inline SizeType LostCount()
	{
		return _lostElementsCount;
	}

	// ���������� �������� �������� ���� (�����)
	inline OffsetType CurrentOffset()
	{
		return _currentOffset;
	}

private:

	inline bool CheckRoot()
	{
		if (_rootUnvalid == false) // ���� ������ ����������, �� ������ �� ���� ������
			return true;
		return LoadRoot(); // ����� ��������� � �����
	}

	// ��������� ������
	inline bool LoadRoot()
	{
		_rootModified = false;
		if (LoadNodeRaw(_root, 0))
		{
			_rootUnvalid = false;
			return true;
		}
		return false;
	}

	// ��������� ������
	inline bool SaveRoot()
	{
		return SaveNode(_root, 0);
	}

	// ��������� ��������� � �����, ���� ��� ����
	inline bool FlushRoot()
	{
		// ���� ������ ������ � �� ���������, �������� ���������
		if (!_rootModified)
			return true;
		_rootModified = false; // �������� ��������� ������ 1 ���, ����� ��������� ������������
		return SaveRoot();
	}

	// ������� ���� �� ���������� ��������
	bool LoadNodeRaw(NodeType& node, OffsetType offset)
	{
		if (!FileClient::SeekFile(offset))
			return false;
		return FileClient::ReadFile((unsigned char*) &node, sizeof(NodeType));
	}

	// ����� ��������� ����, ��������� � ������������ ������ ��������������� (��. �������� ������ RedistributeLeaf)
	bool LoadNodeWise(NodeType& node, OffsetType offset, bool checkError, Key rightKey)
	{
		if (!LoadNodeRaw(node, offset))
			return false;

		// ������ ����� ���� ������ � �������, �� ����� ���� � �����
		if (!checkError || !node.Leaf() || node.Root())
			return true;

		// ��������� �� ������ � ������������, ���� ����
		CorrectRedistributionError(node, rightKey);

		return true;
	}

	// ���������� ��������� �� ����, ��������������� ��������;
	// ���� �������� = 0, �� ������ ������������ ��������� �� _root,
	// ����� � ������ tempNode ����������� ���� �� �������� � ����������� ��������� �� tempNode
	NodeType* GetNode(OffsetType offset, NodeType& tempNode)
	{
		// ���� ����� �� ������, ��������� ��� � ������ tempNode
		if (offset)
		{
			if (!LoadNodeRaw(tempNode, offset)) 
				return 0;
			return &tempNode;
		}
		return &_root; // ����� ���������� ��������� �� ��� ��������� ������
	}

	// ��������� ���� �� ���������� ��������
	bool SaveNode(NodeType& node, OffsetType offset)
	{
		if (offset == 0) // ���� ��������� ������
			_rootModified = false; // �������� ��������� ������ 1 ���, ����� ��������� ������������

		if (SaveNodeRaw(node, offset)) // ���� ���������, �� �������
			return true;

		if (offset == 0) // ���� ������
			_rootUnvalid = true; // ���������� ���� �������������� �����

		return false;
	}

	// ��������� ����, ���������� ��������
	OffsetType SaveNode(NodeType& node)
	{
		OffsetType offset = FileClient::FileSize(); // ���� ��� ������ ��������, �� �������� � ����� �����
		
		if (_root.FirstFreePage()) // ���� ���� ��������� �������� (����� �������� ����)
		{
			offset = _root.FirstFreePage(); // ���� ������ ������ ��������

			OffsetType nextFree = 0;
			// ��������� ��� ������ ��������, ����� ������ �������� ��������� ������ ��������
			if (!LoadNextFreeRecordId(offset, nextFree))
				return 0;

			_root.SetFirstFreePage(nextFree); // ������ �������� ������ ������ ��������
			if (!SaveRoot()) // ����� ��������� ������
				return false;
		}

		if (!SaveNode(node, offset)) // ��������� ����
			return 0;

		return offset;
	}

	// ��������� ���� �� ���������� ��������,
	// ����������� SaveNode��, �.�. ���� ����� �� ������������ ����� _rootModified � _rootUnvalid
	bool SaveNodeRaw(NodeType& node, OffsetType offset)
	{
		if (!FileClient::SeekFile(offset))
			return false;
		return FileClient::WriteFile((unsigned char*) &node, sizeof(NodeType));
	}

	//
	// ������ ������
	//

	// ��������� ������
	// node - ������, � ������� ����� ��������� ���� �� ������������ �����
	// major - ����, ������������, ����� ���� ������� (� �������� ��� �������� �������)
	// ���������� �������� ������ �� ����� �����
	OffsetType SplitRoot(NodeType& node, bool major)
	{
		if (_root.Leaf()) // ���� ������ ����, �� � ����� ���� ����� ��������
			node.SetLeaf();

		// ������� �������� �� ����� �������� ������� ������ � ���������
		_root.CopyMajor(node);
		Key majorMinKey = node.MinKey(); // ����������� ���� � ����
		OffsetType majorOffset = SaveNode(node); // ���������, �������� �������� � �����
		if (majorOffset == 0)
			return 0;

		// ����� �������� �������� ������� ������ � ���������
		_root.CopyMinor(node);
		// ���� ����� ���� ������, �� ���� ������� �� � ������
		if (node.Leaf())
			node.SetNextLeafPage(majorOffset);
		Key minorMinKey = node.MinKey(); // ����������� ���� � ����
		OffsetType minorOffset = SaveNode(node); // ���������, �������� �������� � �����
		if (minorOffset == 0)
			return 0;

		// ���� ���� ��������� ����� ��������, �� ���� ����� ��������� ������ ����� � FAT-�������
		if (!FileClient::Flush())
			return false;

		// ���� �� ������ ����� �������� � �������� �������, ����� �������� �
		if (major)
		{
			_root.CopyMajor(node);
			if (node.Leaf())
				node.SetNextFreePage(0);
		}

		// ��������� ������
		_root.Clear();
		_root.ClearLeaf(); // ������ ����� �� ����
		_root.InsertChild(majorMinKey, majorOffset);
		_root.InsertChild(minorMinKey, minorOffset);

		if (!SaveRoot()) // ��������� ������
			return 0;

		return major ? majorOffset : minorOffset;
	}

	// ��������� ����-����������, ������������� �����
	// node - ����, ������� ���� ��������� � � ������� ����� ��������� ���� �� ������������ �����
	// major - ����, ������������, ����� ���� ������� (� �������� ��� �������� �������)
	// offset - �������� ������������ ���� (node)
	// parentOffset - �������� ��������
	// ���������� �������� ������ �� ���� �����, ���������� ��� ����������
	inline OffsetType SplitNodeWise(NodeType& parent, OffsetType parentOffset, SizeType indexInParent, NodeType& node, OffsetType offset, bool major)
	{
		if (node.Leaf())
			return SplitLeafNode(parent, parentOffset, indexInParent, node, offset, major);
		return SplitInternalNode(parent, parentOffset, indexInParent, node, offset, major);
	}

	// ��������� ����-���������� (�� ����)
	// node - ����, ������� ���� ��������� � � ������� ����� ��������� ���� �� ������������ �����
	// major - ����, ������������, ����� ���� ������� (� �������� ��� �������� �������)
	// offset - �������� ������������ ���� (node)
	// parentOffset - �������� ��������
	// ���������� �������� ������ �� ���� �����, ���������� ��� ����������
	OffsetType SplitInternalNode(NodeType& parent, OffsetType parentOffset, SizeType indexInParent, NodeType& node, OffsetType offset, bool major)
	{
		// ������� ���� �������� ������� �������� � ��������� � ����� ����
		node.MakeMajorPart();
		Key majorMinKey = node.MinKey(); // ����������� ���� ������ ����
		OffsetType majorOffset = SaveNode(node); // ��������� �� ����� �����, �������� �������� ������ ����
		if (majorOffset == 0)
			return 0;

		// ���������� ���� ������������ ���
		node.RestoreFromMajorPart();

		// ���� ������ �������� �������� (�������) � ��������� � ����� ����
		node.MakeMinorPart();

		Key minorMinKey = node.MinKey(); // ����������� ���� ������ ����
		OffsetType minorOffset = SaveNode(node); // ��������� �� ����� �����, �������� �������� ������ ����
		if (minorOffset == 0)
			return 0;

		// ���� ���� ��������� ����� ��������, �� ���� ����� ��������� ������ ����� � FAT-�������
		if (!FileClient::Flush())
			return false;

		// ������ ���� ������ ��������� � �������� (�������� ����� ����, ������� ������)
		// �������� ������ �� ������ ����, ������� �� ����� (� �������� ����������),
		// ����������� ������������ ���� �� (!!!) ������� ������ ��������
		parent.SetKey(indexInParent, node.MinKey());
		parent.SetOffset(indexInParent, minorOffset);
		parent.InsertChild(majorMinKey, majorOffset); // ��������� ����� ���� (������� ��������)
		if (!SaveNode(parent, parentOffset)) // ��������� ��������
		{
			// TODO: ����� ����� ���������� ���������� ������� ������ ������ ��������,
			// � ������� ��������� ��� ������ �� ����� ������ ��������������
			return false; // ���� �� ���������, �� �������
		}

		//
		// ������ ���� ���������� ������ ��������, ��� ����� node, � ������� � offset,
		// ��� ����� ���� � �������� � ������ ������ �������
		//

		// ��������� ������ �� ����� ������ ��������,
		// ����� � ���� NextFreePage ������ �������� ��������� �������� ���� FirstFreePage �����
		if (!SaveNode(_root, offset))
			return false;

		// ������ ����� ������ �������� ������ � ������ ������ ������� � ��������� ������
		_root.SetFirstFreePage(offset);
		if (!SaveRoot())
			return false;

		// �� �����������, ��������������� ���������;
		// ���� �� ������ ����� �������� � �������� �������, ����� �������������� ����
		if (major)
		{
			node.RestoreFromMinorPart();
			node.MakeMajorPart();
		}

		return major ? majorOffset : minorOffset;
	}

	// ��������� ����-����������, ���������� ������
	// node - ����, ������� ���� ��������� � � ������� ����� ��������� ���� �� ������������ �����
	// major - ����, ������������, ����� ���� ������� (� �������� ��� �������� �������)
	// offset - �������� ������������ ���� (node)
	// parentOffset - �������� ��������
	// ���������� �������� ������ �� ���� �����, ���������� ��� ����������
	OffsetType SplitLeafNode(NodeType& parent, OffsetType parentOffset, SizeType indexInParent, NodeType& node, OffsetType offset, bool major)
	{
		// ������� ���� �������� ������� �������� � ��������� � ����� ����
		node.MakeMajorPart();
		Key majorMinKey = node.MinKey();
		OffsetType majorOffset = SaveNode(node);
		if (majorOffset == 0)
			return 0;

		// ���� ���� ��������� ����� ��������, �� ���� ����� ��������� ������ ����� � FAT-�������
		if (!FileClient::Flush())
			return false;

		// ������ ���� �������� ����� ���� � ������� �������,
		// ��� ����� ������� ���� ������ �� ���� ���������

		// ���������� ���� ������������ ���
		node.RestoreFromMajorPart();
		OffsetType majorNextLeaf = node.NextLeafPage(); // ���������� ������� ���� NextLeaf ��� �������� ����
		node.SetNextLeafPage(majorOffset); // ������ ������� ���� ������ ��������� �� ����� ��������� ���� ����
		if (!SaveNode(node, offset)) // ��������� ���� �� ������ �����
		{
			// ���� � ���� ����� ��������� �� �������, ������ ���� ��������,
			// ����� ���������� � ���������� (��������� � ������ ������)
			return 0;
		}

		// ��������� � �������� ������ �� ���������� ����� ���� � �������� ����������
		parent.InsertChild(majorMinKey, majorOffset);
		if (!SaveNode(parent, parentOffset)) // ��������� ��������
		{
			// ���� � ���� ����� ��������� �� �������, �������� �������� � ������ �������,
			// ���������� ������� ������ ������������ ��� ������
			return 0;
		}

		// ������ ���� ������ �������� �������� (�������) � ����� ��������� �� ������ �����
		// ���� NextLeafPage ��� ����������
		node.MakeMinorPart();
		if (!SaveNode(node, offset)) // ���������
		{
			// ���� ��� ������ �� �������� ����, ���������� �����,
			// ����������� ����� �� ��������� ����� (major �����),
			// ��� ����� ������ ������������ ��� ������,
			// � ������ ���������������� ��� ����������� ��� ����������� � ��������,
			// ��� ����� �������� ����������� ������� LoadNodeWise
			return 0;
		}

		// ���� �� ������ ����� �������� � �������� �������, ����� �������������� ����
		if (major)
		{
			node.RestoreFromMinorPart();
			node.MakeMajorPart();
			node.SetNextLeafPage(majorNextLeaf); // ��������������� ������ �� ��������� ����
		}

		return major ? majorOffset : offset;
	}

	//
	// ��������������� �����
	//
	// ��������� 1 ��� 2 ����������� ���� �� ������ ����������� � ��� ������� �����.
	// ���� � ������ ���� ������ ������ ��������, �� ���� �� ���� ���� ����
	// ���� ������ ���� ���� ����������, ������� ��� ���� � ���� node1
	bool RedistributeWithRight(NodeType& node, NodeType& rightNode)
	{
		SizeType elementsToMove = (rightNode.Size() - node.Size()) >> 1;
		// ���� ������ ���� �� ������ ����������, ����� ������������ ��������, �� �����������
		if (!rightNode.ThinSize(rightNode.Size() - elementsToMove))
		{
			_lastMovedCount = elementsToMove; // ���������� ���-�� ������������ ��������� (��� �������)
			// ����������� ������� ����� (���������� �������� ������� ��������)
			return rightNode.MoveMinor2Minor(node, elementsToMove);
		}

		_lastMovedCount = rightNode.Size(); // ���������� ���-�� ������������ ��������� (��� �������)

		// ����� ������� � ���� ����
		if (!node.MergeWithMajor(rightNode))
			return false; // ������ ���� �� �������, ����� ������� ����� � ������������� ������!!!

		rightNode.Clear();

		return true;
	}

	// ��������� 1 ��� 2 ����������� ���� �� ������ ����������� � ��� ������ �����.
	// ���� � ����� ���� ������ ������ ��������, �� ���� �� ���� ���� ����
	// ���� ����� ���� ���� ����������, ������� ��� ���� � ���� leftNode
	bool RedistributeWithLeft(NodeType& node, NodeType& leftNode)
	{
		SizeType elementsToMove = (leftNode.Size() - node.Size()) >> 1;
		// ���� ����� ���� �� ������ ����������, ����� ������������ ��������, �� �����������
		if (!leftNode.ThinSize(leftNode.Size() - elementsToMove))
		{
			_lastMovedCount = elementsToMove; // ���������� ���-�� ������������ ��������� (��� �������)
			// ����������� ������� ����� (���������� �������� ������� ��������)
			return leftNode.MoveMajor2Major(node, elementsToMove);
		}

		_lastMovedCount = leftNode.Size(); // ���������� ���-�� ������������ ��������� (��� �������)

		// ����� ������� � ���� ����
		if (!node.MergeWithMinor(leftNode))
			return false; // ������ ���� �� ������, ����� ������� ����� � ������������� ������!!!

		leftNode.Clear();

		return true;
	}

	//
	// ��������������� �����
	//
	// ��������������� �������� �����
	inline bool Redistribute(NodeWrapperType& node, NodeWrapperType& nodeSibling, bool right)
	{
		if (node.Node->Leaf())
			return RedistributeLeaf(node, nodeSibling, 0, right);
		return RedistributeInternal(node, nodeSibling, 0, right);
	}

	// ��������������� �������� �������� ����
	bool inline Redistribute(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset, bool right)
	{
		if (node.Node->Leaf())
			return RedistributeLeaf(node, nodeSibling, parentOffset, right);
		return RedistributeInternal(node, nodeSibling, parentOffset, right);
	}

	// ��������������� ���� � ��� �����, ��������� ����������� ������
	// ��� ���������� ����������� � ������ ������ ���������������� ���� ����������� �� ����� ��������,
	// ����� �������������� ��������, ����� ������������� ������ ����
	bool RedistributeInternal(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset, bool right)
	{
		// �������������� �������� ����� ������
		if (right)
			RedistributeWithRight(*node.Node, *nodeSibling.Node); // �������������� ������� ���� � ���� ������ �� ����
		else
			RedistributeWithLeft(*node.Node, *nodeSibling.Node); // �������������� ������� ���� � ���� ����� �� ����

		// �������� - ������, � ���������� ��������������� � ����� ������� ������ ���� �������,
		// ������ ����� ������� ������
		if (parentOffset == 0 && _root.Size() == 2 && nodeSibling.Node->Empty())
			return RedistributeRoot(node, nodeSibling);

		// ��������� ������� ���� �� ����� �����
		OffsetType nodeNewOffset = SaveNode(*node.Node);
		if (nodeNewOffset == 0)
			return false;

		// ���� ���� �� �������, ��������� �� ����� �����
		OffsetType siblingNewOffset;
		if (!nodeSibling.Node->Empty())
		{
			siblingNewOffset = SaveNode(*nodeSibling.Node);
			if (siblingNewOffset == 0)
				return false;
		}

		// ���� ����� ����������� �� ������������ ��������,
		// � ���� ������� �� ������ ����������� ������������� ��������� � ����� (������ ������ �������) �� ��������� ������������� ����;
		// ��� ��������-����� �� ����
		if (parentOffset != 0)
		{
			if (!FlushRoot())
				return false;
		}
		// ���� ���� ��������� ����� ��������, �� ���� ����� ��������� ������ ����� � FAT-�������
		if (!FileClient::Flush())
			return false;

		// ���������� ���. ���� �����, �.�. ���� ������ ����������� (���� �������� �� ������)
		Key siblingMinKey = nodeSibling.Node->MinKey();
		bool siblingEmpty = nodeSibling.Node->Empty(); // ���������� ������ �����

		// ���� �������� �� ������, ��������� ��� � ��� �������� ������ nodeSibling,
		// ����� ���� _root
		NodeType* nodeParent = GetNode(parentOffset, *nodeSibling.Node);
		if (nodeParent == 0)
			return false;

		// ������������ ��������

		// ���������� �� ������� ����
		nodeParent->SetKey(node.Index, node.Node->MinKey());
		nodeParent->SetOffset(node.Index, nodeNewOffset);

		// ���������� � �����
		if (!siblingEmpty)
		{
			nodeParent->SetKey(nodeSibling.Index, siblingMinKey);
			nodeParent->SetOffset(nodeSibling.Index, siblingNewOffset);
		}
		else
			nodeParent->Delete(nodeSibling.Index);

		// ��������� ��������
		if (!SaveNode(*nodeParent, parentOffset))
			return false;

		// ������ ���� ���������� ������ �������� ����� (�������� � ������ ������)

		// ������ �������� ����� ����� ��������� �� ������ ����� �������� � ������,
		// ��� ����� ������ ������ ��������� ������ �� ���������� ��������
		if (!SaveNode(_root, nodeSibling.Offset))
			return false;

		// ������ �������� �������� ���� ����� ��������� �� ������ �������� �����,
		// ���������� ������ nodeSibling.Node, �.�. �� ������ �� �����
		nodeSibling.Node->SetNextFreePage(nodeSibling.Offset);
		if (!SaveNode(*nodeSibling.Node, node.Offset))
			return false;

		// ������ �������� �������� ���� ����� ������ � ������ ������
		_root.SetFirstFreePage(node.Offset);
		node.Offset = nodeNewOffset; // !!! ������ ������ ������� ���������� �������� �������� ����
		return SaveRoot();
	}

	// ��������������� �������� �����, ���������� ��������
	// � ������ ������� �� �� ����� ��� ���� ��������� �� ����� ��������,
	// ����� ���� (���� ��� �������) ������ �������� �� ����� ��������, �.�. �� �� ����� ��������� ���� �� ��������� ���������,
	// � ����� � ����, ������� ���������� ������� ������ ��� ������ ��������,
	// ��� ���� � ������� �� ���������� �����, ������ ��� ���������� ��� ��������������� ������ ����� �������� � ������ � ������:
	// ���� ����� ���������� ������ �����, ����������� ����� ��������� ���� � �������� �������, ��� ������ ������ ���������������� ��� ����������� ��������,
	// ������� ��� ������������� ���������� ����
	bool RedistributeLeaf(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset, bool right)
	{
		// �������������� �������� ����� ������
		if (right)
			RedistributeWithRight(*node.Node, *nodeSibling.Node); // �������������� ������� ���� � ���� ������ �� ����
		else
			RedistributeWithLeft(*node.Node, *nodeSibling.Node); // �������������� ������� ���� � ���� ����� �� ����

		// ���� � ���������� ��������������� � ����� ������� ������ ���� �������,
		// ������ ����� ������� ������
		if (parentOffset == 0 && _root.Size() == 2 && nodeSibling.Node->Empty())
			return RedistributeRoot(node, nodeSibling);

		// ���� ���� ������� ����� ���������������
		if (nodeSibling.Node->Empty())
			return SaveRedistributeLeafSiblingEmpty(node, nodeSibling, parentOffset, right);

		// ���� ���� �� �������
		if (right)
			return SaveRedistributeLeafSiblingRigth(node, nodeSibling, parentOffset); // ���� ���� ������
		return SaveRedistributeLeafSiblingLeft(node, nodeSibling, parentOffset); // ���� ���� �����
	}

	// ����������, ���� � ����� ������� ������ ���� ������� ����� ���������������
	// ������ ����� ������� ������
	bool RedistributeRoot(NodeWrapperType& node, NodeWrapperType& nodeSibling)
	{
		OffsetType nextFree = _root.FirstFreePage();
		node.Node->SetRoot(); // ����� ������
		_root = *node.Node; // ������ ������������� ����������� ������� ������
		_root.SetFirstFreePage(nextFree); // ����� ����������� ��� ���� ��������, ���������������

		if (!SaveRoot()) // ����� ��������� ������, � ������ ����� ������������ ������ ������ �������
			return false;

		// ����� ��������� ������ ��������,
		// �������� ���������� ������ �� ��������� ������ ��������
		nodeSibling.Node->SetNextFreePage(nextFree);
		if (SaveNode(*nodeSibling.Node, nodeSibling.Offset))
			nextFree = nodeSibling.Offset;

		node.Node->SetNextFreePage(nextFree);
		if (SaveNode(*node.Node, node.Offset))
			nextFree = node.Offset;

		// ������������� ������ �� ������ ������ ��������
		_root.SetFirstFreePage(nextFree);
		// node ������ ��������� ������ �� ������
		node.Node->SetFirstFreePage(_root.FirstFreePage());
		node.Offset = 0;

		// ��������� ������
		return SaveRoot();
	}

	//
	// ���������������, ��������������� �������
	//
	// ��������� ���������� ���������������,
	// ���� ���� - ������, � ���� �������
	bool SaveRedistributeLeafSiblingEmpty(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset, bool right)
	{
		// ���� ���� �����, �� ������������ ���������� ���� ���� ��������� �� ��� �����
		if (!right)
		{
			// ����� �������
			OffsetType offset = node.Offset;
			node.Offset = nodeSibling.Offset;
			nodeSibling.Offset = offset;
			SizeType index = node.Index;
			node.Index = nodeSibling.Index;
			nodeSibling.Index = index;
		}

		// ������� ���� ��������� �� ���� �� ������ ������� (���� ���� �����, �� ���� ���������� �� ��� ��������) 
		if (!SaveNode(*node.Node, node.Offset))
			return false; // ���� �� ����������, ������ ������� �� ����� �����, ���������� false

		// ���������� �������� ���� NextLeaf �����, �.�. �� ����� ���������, �������� ���� ����� ������������
		OffsetType SiblingNextLeaf = nodeSibling.Node->NextLeafPage();

		// ���� ��������: ���� �������� �� ������, ��������� ��� � ��� �������� ������ nodeSibling,
		// ����� ���� _root
		NodeType* nodeParent = GetNode(parentOffset, *nodeSibling.Node);
		if (nodeParent == 0)
			return false;

		// ������� ������ �� ���������� ���� �� ����� � ���������
		// ���� �� ���� ��������������, �� � ��� ���������� � ����� ������
		nodeParent->Delete(nodeSibling.Index);
		if (!SaveNode(*nodeParent, parentOffset))
			return false; //����� ��������� ������ ��������������� (��. �������� ������ RedistributeLeaf)

		// ���� ���� ��� ������, �� ������ ���� ��������� ���� NextLeaf ����������� ����
		if (right)
		{
			// ���� ����� ��������� �� ��������, �� ������� �������� ����
			node.Node->SetNextLeafPage(SiblingNextLeaf);
			if (!SaveNode(*node.Node, node.Offset))
				return false; // ����� ���������� ������ ����� � ������� �������, � ������������ �������
		}

		// ������������ ���������� ��������, ��������� � � ������ ������
		// � ���� NextFreePage ������ ���� �������� FirstFreePage �� �����
		if (!SaveNode(_root, nodeSibling.Offset))
			return false;
		// ���������� �������� ���������� ������ � ������ ������
		_root.SetFirstFreePage(nodeSibling.Offset);
		return SaveRoot();
	}

	// ��������� ���������� ���������������,
	// ���� ���� - ������, ���� ������
	bool SaveRedistributeLeafSiblingRigth(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset)
	{
		// ����� ��������� �� ������ ��������
		if (!SaveNode(*nodeSibling.Node, nodeSibling.Offset))
			return false; // ������ ���

		_lostElementsCount = _lastMovedCount; // ���������� ���������� ���������, ���� ����� ������ ��� ����������

		// ���������� ���. ���� �����, �.�. ���� ������ ����������� (���� �������� �� ������)
		Key siblingMinKey = nodeSibling.Node->MinKey();

		// ���� ��������: ���� �������� �� ������, ��������� ��� � ��� �������� ������ nodeSibling,
		// ����� ���� _root
		NodeType* nodeParent = GetNode(parentOffset, *nodeSibling.Node);
		if (nodeParent == 0)
			return false; // ������

		// ������������ ��������: ������������ ����
		nodeParent->SetKey(nodeSibling.Index, siblingMinKey);
		if (!SaveNode(*nodeParent, parentOffset))
			return false; // ������

		// ������� ���� ��������� �� ������ �����
		if (!SaveNode(*node.Node, node.Offset))
			return false; // ������

		_lostElementsCount = 0; // ������ ���

		return true;
	}

	// ��������� ���������� ���������������,
	// ���� ���� - ������, � ���� �����
	bool SaveRedistributeLeafSiblingLeft(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset)
	{
		// ��������� ����� �� ������ �����
		if (!SaveNode(*nodeSibling.Node, nodeSibling.Offset))
			return false; // ������ ���

		_lostElementsCount = _lastMovedCount; // ���������� ���������� ���������, ���� ����� ������ ��� ����������

		// ���� ��������: ���� �������� �� ������, ��������� ��� � ��� �������� ������ nodeSibling.Node,
		// ����� ���� _root
		NodeType* nodeParent = GetNode(parentOffset, *nodeSibling.Node);
		if (nodeParent == 0)
			return false; // ������

		// ������ � �������� ���� �������� ����
		nodeParent->SetKey(node.Index, node.Node->MinKey());
		if (!SaveNode(*nodeParent, parentOffset))
			return false; // ������
		
		// ��������� ������� ���� �� ������ ��������
		if (!SaveNode(*node.Node))
			return false; // ������

		_lostElementsCount = 0; // ������ ���

		return true;
	}

	// ����� ��� ������������� ������ ����������������� ��������� ����� (��. �������� ������ RedistributeLeaf)
	// node - ����������� � �������������� ����
	// key - ����������� ���� ����
	// rightKey - ����������� ���� ������� ����� ���� ����
	void CorrectRedistributionError(NodeType& node, Key rightKey)
	{
		if (!node.Leaf())
			return;

		SizeType i = node.FindPath(rightKey);

		if (node.Key(i) != rightKey)
		{
			if (i == 0)
				return; // ������ ������ ���, �������
			else
				i--; // ����� �������, ��� Key[i], ��� ������
		}

		// ���� ������ �����, ������� � i, ���� ������� ��� �������� � ��������� [0,i], �.�. ����� >= Key[i]
		node.DeleteMajorFrom(i);
	}

	//
	// ����� ������
	//

	// ��������� �������� �� ��������� ���� � �������
	inline void SetIteratorNextNode(NodeType& currentNode)
	{
		_currentOffset = currentNode.NextLeafPage();
		if (_currentOffset)
			SetIteratorBegin();
	}

	// ��������� �������� �� ��������� ���� � ������� � ��������� ��������� ���� � ���� (����� �������)
	inline void SetIteratorNextNodeEx(NodeType& currentNode)
	{
		SetIteratorNextNode(currentNode);
		_currentLastKey = currentNode.Key(0);
		_needLastKeyCheck = true;
	}

	// ������������ ���������, ���������� ����� �� ��������
	inline bool IteratorInvalid()
	{
		return _currentElement > NodeType::LeafMaxSize;
	}
	
	// ���������� ���������, ���������� ����� ��������
	inline bool IteratorValid()
	{
		return _currentElement <= NodeType::LeafMaxSize;
	}

	// ��������� � ������������ ���������
	inline void SetIteratorInvalid()
	{
		_needLastKeyCheck = false;
		_currentElement = -1;
	}

	// ����� �� ������ ����� �������� ���� � ������?
	inline bool IteratorBegin()
	{
		return _currentElement == NodeType::LeafMaxSize;
	}

	// ���������� �������� �� ������ ����
	inline void SetIteratorBegin()
	{
		_needLastKeyCheck = false;
		_currentElement = NodeType::LeafMaxSize;
	}

	// ������ ������� ������, ���� ������ - ����
	bool FirstRoot(PairType& value)
	{
		_currentOffset = 0;
		_currentElement = _root.Size() - 1;
		value = _root.Pair(_currentElement--);
		return true;
	}

	// ������ ������� ������ ������� ��� ������ key, ���� ������ - ����
	bool FirstRoot(Key key, PairType& value)
	{
		_currentOffset = 0;
		_currentElement = _root.FindPath(key);
		if (_root.Key(_currentElement) < key) // ���� ������ � ����� ����� ������� (�����������), ���� ��� ����� ���� < key
			_currentElement--; // ����� ��������� ������ (� �������� ��������) �� 1 �������
		if (IteratorInvalid())
			return false;
		value = _root.Pair(_currentElement--);
		return true;
	}

	// ������ ������� ������, ���� ������ �� �������� ������
	bool FirstGeneral(PairType& value)
	{
		SetIteratorInvalid();

		OffsetType nextOffset = _root.Offset(_root.Size() - 1);
		NodeType node;

		do
		{
			if (!LoadNodeRaw(node, nextOffset) || node.Size() == 0)
				return false;
			_currentOffset = nextOffset;
			nextOffset = node.Offset(node.Size() - 1);
		} while (!node.Leaf());

		_currentElement = node.Size() - 1;
		value = node.Pair(_currentElement--);

		return true;
	}

	// ������ ������� ������ ������� ��� ������ key, ���� ������ �� �������� ������
	bool FirstGeneral(Key key, PairType& value)
	{
		_currentElement = _root.FindPath(key);
		_currentOffset = _root.Offset(_currentElement);

		NodeType node;

		do
		{
			if (!LoadNodeRaw(node, _currentOffset) || node.Size() == 0)
			{
				SetIteratorInvalid();
				return false;
			}

			_currentElement = node.FindPath(key);

			if (node.Leaf())
				break;

			_currentOffset = node.Offset(_currentElement);
		} while (1);

		// ���� ������ � ����� ����� ������� (�����������), ��� ����� ���� ����� < key, ���������� ��
		while (node.Key(_currentElement) < key)
		{
			_currentElement--; // ��������� ������ (� �������� ��������) �� 1 �������
			// ���� ����������� ������� � ������� ����, ������������� �� ��������� � �������
			if (IteratorInvalid())
			{
				SetIteratorNextNode(node); // ���� ���������� �� ����
				return Next(value);
			}
		}

		value = node.Pair(_currentElement--);

		// ���� ����� ��������� ������� � ����, �� ������������� �� ���������
		if (IteratorInvalid())
			SetIteratorNextNodeEx(node); // ��������� �� ���� ����, ���������� ��������� ����, ��� ������������� ������, ���� ��� ����

		return true;
	}

	// ��������� �������, ���� ������ - ����
	bool NextRoot(PairType& value)
	{
		if (!_root.Leaf())
			return false;

		if (_currentElement >= _root.Size())
			return false;

		value = _root.Pair(_currentElement--);
		return true;
	}

	// ��������� �������, ���� ������ �� �������� ������
	bool NextGeneral(PairType& value)
	{
		NodeType node;

		do
		{
			if (!LoadNodeRaw(node, _currentOffset))
				return false;

			if (node.Size() == 0)
				return false;

			if (!IteratorBegin())
				break;

			_currentElement = node.Size() - 1;
			SkipDuplicateKeys(node);

			// ���� ����������� ������� � ������� ����, ������������� �� ��������� � �������
			if (IteratorInvalid())
			{
				SetIteratorNextNode(node); // ���� ��������� �� ����
				_needLastKeyCheck = true;
			}
			else
				break;

		} while(IteratorValid());

		if (IteratorInvalid()) // ���� �� �������� ���������, �������
			return false;

		value = node.Pair(_currentElement--);

		// ���� ����� ��������� ������� � ����, �� ������������� �� ���������
		if (IteratorInvalid())
			SetIteratorNextNodeEx(node); // ��������� ���� ���� ���������

		return true;
	}

	// ���������� ����������� �����, ������� ����� ���������� ��� ������ ���������� �� ����� ���������������,
	// ��. �������� ������ RedistributeLeaf
	void SkipDuplicateKeys(NodeType& currentNode)
	{
		if (_needLastKeyCheck == false)
			return;
		while (IteratorValid() && _currentLastKey >= currentNode.Key(_currentElement))
			_currentElement--;
	}

	//
	// ������ ������ �������
	//

	// ������� ������ 4 ����� ������ ������, � ������� id (��������) ��������� ������ ������
	bool LoadNextFreeRecordId(OffsetType id, OffsetType& nextId)
	{
		if (!FileClient::SeekFile(id))
			return false;
		return FileClient::ReadFile((unsigned char*) &nextId, sizeof(unsigned long));
	}

private:

	NodeType _root; // ������ ������, ������ ����������

	// ������� ������� ������������ ��������� � ����� (������, ���� ��������� ������ � ������ Flush()),
	// ������ ������������ ������ ��� ��������� ���� FirstFree ��� ���������� ���� �� ����� ��������� 
	bool _rootModified;
	bool _rootUnvalid; // ���� ������������, ���� �� ������ ��������� ��������� � �����

	OffsetType _currentOffset; // ������� �������-���� ��� ������ ������
	SizeType _currentElement; // ������� ������� � ������� ����
	Key _currentLastKey;
	bool _needLastKeyCheck;

	SizeType _lastMovedCount; // ���������� ������������ ��������� ��� ��������� ��������������� (��� �������)
	SizeType _lostElementsCount; // ���������� ���������� ��� ��������� ��������������� ��������� (��� �������)
};

}

#endif // _RBLIB_BD_BPLUS_TREE_H_

