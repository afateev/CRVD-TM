////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		18/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RTC_STM32_F107_H
#define RTC_STM32_F107_H

#include "RtcParts.h"

const unsigned int RtcBase = 0x40002800;

class Rtc :
	public RtcControlLowRegister<RtcBase + 0x04>,
	public RtcCounterHiRegister<RtcBase + 0x18>,
	public RtcCounterLowRegister<RtcBase + 0x1C>
{
public:	
	static unsigned int GetTime()
	{
		unsigned int res = GetCounterHi();
		res <<= 16;
		res |= GetCounterLow();
		return res;
	}
	
	static void SetTime(unsigned int time)
	{
		unsigned int low = time & 0xFFFF;
		unsigned int hi = (time >> 16) & 0xFFFF;
		
		PowerControl::DisableBackupDomainWriteProtection(true);
		
		bool rtcoff = GetRtcOff();
		
		while(rtcoff == 0)
			rtcoff = GetRtcOff();
		
		EnableConfig(true);
		
		SetCounterLow(low);
		SetCounterHi(hi);
		
		EnableConfig(false);
		
		rtcoff = GetRtcOff();
		while(rtcoff == 0)
			rtcoff = GetRtcOff();
		
		PowerControl::DisableBackupDomainWriteProtection(false);
	}
};

#endif