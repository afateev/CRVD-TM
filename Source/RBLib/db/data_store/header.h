
#ifndef _RBLIB_BD_DATA_STORE_HEADER_H_
#define _RBLIB_BD_DATA_STORE_HEADER_H_

#include "../../static_check.h"

namespace Rblib
{
namespace Bd
{
namespace DataStore
{

#pragma pack(1)
template<class Offset>
class Header
{
public:

public:

	Header() : _firstFreeRecord(0)
	{
		RBLIB_STATIC_CHECK(sizeof(Offset) != 4 && sizeof(Offset) != 8, OFFSET_SIZE_SHOULD_BE_4_OR_8_BYTES);
	}

	inline unsigned long FirstFreeRecord()
	{
		return _firstFreeRecord;
	}

	inline void SetFirstFreeRecord(unsigned long firstFreeRecord)
	{
		_firstFreeRecord = firstFreeRecord;
	}

private:

	Offset _firstFreeRecord;
	unsigned char _reserved[32 - sizeof(Offset)]; // размер заголовка должун быть 32 байта
};

}
}
}

#endif // _RBLIB_BD_DATA_STORE_HEADER_H_

