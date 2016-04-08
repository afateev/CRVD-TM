
#ifndef _RBLIB_BD_BPLUS_HEADER_H_
#define _RBLIB_BD_BPLUS_HEADER_H_

#include "../../static_check.h"

namespace Rblib
{
namespace Db
{
namespace BPlusTree
{

#pragma pack(push, 1)
template<class Offset>
class Header
{
public:

	// �����, ������������ ��� ����
	enum Flag
	{
		FlagRoot = 1, // ������
		FlagLeaf = 2 // ����-����
	};

	// ��� ������ ��� �������� �������� � �����
	typedef Offset OffsetType;

public:

	Header() : _firstFreePage(0), _flags(0)
	{
		// !!! �������� ����� ���� 32- ��� 64-������ !!!
		RBLIB_STATIC_CHECK(sizeof(Offset) != 4 && sizeof(Offset) !=8, FILE_OFFSET_SHOULD_BE_32_OR_64_BIT);
	}

	inline void Reset()
	{
		_firstFreePage = 0;
		ClearFlags();
	}

	//
	// ������ ������ �������� (��� �����-������)
	// 
	inline Offset FirstFreePage()
	{
		return _firstFreePage;
	}

	inline void SetFirstFreePage(Offset firstFreePage)
	{
		_firstFreePage = firstFreePage;
	}

	//
	// ��������� ������ �������� (��� ������ �������)
	//
	inline Offset NextFreePage()
	{
		return _nextFreePage;
	}

	inline void SetNextFreePage(Offset nextFreePage)
	{
		_nextFreePage = nextFreePage;
	}

	//
	// ��������� ���� ���� (��� ����-�����, �� ����������� ������)
	//

	inline Offset NextLeafPage()
	{
		return _nextLeafPage;
	}

	inline void SetNextLeafPage(Offset nextLeafPage)
	{
		_nextLeafPage = nextLeafPage;
	}

	//
	// ���������� �� ����
	//

	inline bool Root()
	{
		return _flags & FlagRoot;
	}

	inline bool Leaf()
	{
		return (_flags & FlagLeaf) ? true : false;
	}

	inline void ClearFlags()
	{
		_flags = 0;
	}

	inline void ClearLeaf()
	{
		_flags &= ~FlagLeaf;
	}

	inline void ClearRoot()
	{
		_flags &= ~FlagRoot;
	}

	inline void SetRoot()
	{
		_flags |= FlagRoot;
	}

	inline void SetLeaf()
	{
		_flags |= FlagLeaf;
	}

private:

	union
	{
		Offset _firstFreePage;
		Offset _nextFreePage;
		Offset _nextLeafPage;
	};

	unsigned char _flags;
	unsigned short _reserve;
};
#pragma pack (pop)

}
}
}

#endif //  _RBLIB_BD_BPLUS_HEADER_H_

