
#ifndef _RBLIB_BD_INDEX_BPLUS_WRAPPER_H_
#define _RBLIB_BD_INDEX_BPLUS_WRAPPER_H_

namespace Rblib
{
namespace Db
{
namespace BPlusTree
{

template<class NodeType>
class NodeWrapper
{
public:

	NodeType* Node;
	typename NodeType::KeyType Key;
	typename NodeType::SizeType Index;
	unsigned long Offset;

public:
	
	NodeWrapper(NodeType* node
				, typename NodeType::SizeType index
				, typename NodeType::KeyType key
				, unsigned long offset)
	: Node(node)
	, Key(key)
	, Offset(offset)
	, Index(index)
	{
	}

};

}
}
}

#endif // _RBLIB_BD_INDEX_BPLUS_WRAPPER_H_

