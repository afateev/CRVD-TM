
#ifndef _MEDIA_SD_RESPONSE_CSD_V1_H_
#define _MEDIA_SD_RESPONSE_CSD_V1_H_

namespace Rblib
{
namespace Media
{
namespace Sd
{
namespace Response
{
namespace CSD
{

#pragma pack(push, 1)
class V1
{
public:

	inline unsigned long Size() const
	{
		unsigned long size = _size + 1;
		
		unsigned char multPow = _sizeMult + 2;

		for (unsigned char i = 0; i < multPow; i++)
		{
			size <<= 1;
		}

		for (unsigned char i = 0; i < _readBlLen; i++)
		{
			size <<= 1;
		}

		return size;
	}

	unsigned char Type() const
	{
		return _type;
	}

private:

	// описаны только поля, нужные для определения размера флешки и типа структыры (v1.0 или v2.0)

	unsigned char _r1;
	unsigned char _r2;
	unsigned short _r3;
	unsigned short _r4 : 7;
	unsigned short _sizeMult : 3;
	unsigned short _r5 : 6;
	unsigned long _r6 : 6;
	unsigned long _size : 12;
	unsigned long _r7 : 6;
	unsigned long _readBlLen : 4;
	unsigned long _r8 : 4;
	unsigned long _r9;
	unsigned char _r10 : 6;
	unsigned char _type : 2;
};
#pragma pack (pop)

}
}
}
}
}

#endif // _MEDIA_SD_RESPONSE_CSD_V1_H_

