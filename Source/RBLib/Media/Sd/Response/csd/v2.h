
#ifndef _MEDIA_SD_RESPONSE_CSD_V2_H_
#define _MEDIA_SD_RESPONSE_CSD_V2_H_

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
class V2
{
public:

	inline unsigned long Size() const
	{
		return _size * _512KByte;
	}

	unsigned char Type() const
	{
		return _type;
	}

private:

	static const unsigned long _512KByte = 512 * 1024;

private:

	// описаны только поля, нужные для определения размера флешки и типа структуры (v1.0 или v2.0)

	unsigned char _r1;
	unsigned long _r2;
	unsigned long _size : 22;
	unsigned long _r3 : 10;
	unsigned long _r4;
	unsigned char _r5;
	unsigned char _r6 : 6;
	unsigned char _type : 2;
};
#pragma pack (pop)

}
}
}
}
}

#endif // _MEDIA_SD_RESPONSE_CSD_V2_H_

