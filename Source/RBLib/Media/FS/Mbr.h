
#ifndef _MEDIA_FS_MBR_H_
#define _MEDIA_FS_MBR_H_

#include "partition.h"

namespace Rblib
{
namespace Media
{
namespace Fs
{

#pragma pack(push, 1)

class Mbr
{
public:

	static const unsigned char MaxPartitionsNumber = 4;

public:

	inline bool Valid()
	{
		return _signature == 0xaa55;
	}

	inline Partition& GetPartition(unsigned char p)
	{
		return _partition[p];
	}
	
protected:

	unsigned char _notUsed[446];
	Partition _partition[MaxPartitionsNumber];
	unsigned short _signature;
};

#pragma pack(pop)

}
}
}

#endif // _MEDIA_FS_MBR_H_

