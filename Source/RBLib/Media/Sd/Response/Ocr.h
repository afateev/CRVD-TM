
#ifndef _MEDIA_SD_RESPONSE_OCR_H_
#define _MEDIA_SD_RESPONSE_OCR_H_

namespace Rblib
{
namespace Media
{
namespace Sd
{
namespace Response
{

#pragma pack(push, 1)
class Ocr
{
public:

	inline bool SwitchTo1_8()
	{
		return _switchTo1_8;
	}

	inline bool SdhcCard()
	{
		return _cardCapacityStatus;
	}

	inline bool PowerUpFinished()
	{
		return _cardPowerUpStatus;
	}

	inline bool Uhs2()
	{
		return _uhs2CardStatus;
	}

protected:

	unsigned char : 8;
	unsigned short : 7;
	unsigned short _voltage : 9;
	unsigned char _switchTo1_8 : 1;
	unsigned char : 4;
	unsigned char _uhs2CardStatus : 1;
	unsigned char _cardCapacityStatus : 1;
	unsigned char _cardPowerUpStatus : 1;
};
#pragma pack (pop)

}
}
}
}

#endif // _MEDIA_SD_RESPONSE_OCR_H_

