
#ifndef _MEDIA_FS_PARTITION_H_
#define _MEDIA_FS_PARTITION_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{

#pragma pack(push, 1)

class Partition
{ 	
public:

	inline bool Valid()
	{
		return _status == 0x80 || _status == 0;
	}

	inline bool Active()
	{
		return _status == 0x80;
	}

	inline unsigned long Offset()
	{
		return _firstSector;
	}

	inline unsigned long SectorsCount()
	{
		return _sectorsTotal;
	}

	inline bool Fat32()
	{
		return _type == 0x0B || _type == 0x0C;
	}

protected:

	unsigned char _status;
	unsigned char _headStart;
	unsigned short _cylSectStart;
	unsigned char _type;
	unsigned char _headEnd;
	unsigned short _cylSectEnd;
	unsigned long _firstSector;
	unsigned long _sectorsTotal;
};

#pragma pack(pop)

}
}
}

#endif // _MEDIA_FS_PARTITION_H_

