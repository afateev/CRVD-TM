
#ifndef _MEDIA_SD_RESPONSE_INTERFACE_CONDITION_H_
#define _MEDIA_SD_RESPONSE_INTERFACE_CONDITION_H_

namespace Rblib
{
namespace Media
{
namespace Sd
{
namespace Response
{

#pragma pack(push, 1)
class InterfaceCondition
{
public:

	unsigned char Voltage()
	{
		return _voltage;
	}

	unsigned char CheckPattern()
	{
		return _checkPattern;
	}

protected:

	unsigned char _checkPattern;
	unsigned char _voltage : 4;
	unsigned char : 4;
	unsigned short : 16;

};
#pragma pack (pop)

}
}
}
}

#endif // _MEDIA_SD_RESPONSE_INTERFACE_CONDITION_H_

