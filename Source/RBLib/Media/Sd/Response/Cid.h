
#ifndef _MEDIA_SD_RESPONSE_CID_H_
#define _MEDIA_SD_RESPONSE_CID_H_

namespace Rblib
{
namespace Media
{
namespace Sd
{
namespace Response
{

#pragma pack(push, 1)
class Cid
{
public:

	unsigned long Serial()
	{
		return _serial;
	}

	char* Name()
	{
		return _name;
	}

	unsigned char ManufacturerId()
	{
		return _manufacturerId;
	}

	unsigned short ManufacturingDate()
	{
		return _date;
	}

	unsigned char Revision()
	{
		return _revision;
	}

	char* ApplicationId()
	{
		return _appId;
	}

protected:

	unsigned short _date : 12;
	unsigned short : 4;
	unsigned long _serial;
	unsigned char _revision;
	char _name[5];
	char _appId[2];
	unsigned char _manufacturerId;

};
#pragma pack (pop)

}
}
}
}

#endif // _MEDIA_SD_RESPONSE_CID_H_

