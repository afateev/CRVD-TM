////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		18/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RTCPARTS_STM32_F107_H
#define RTCPARTS_STM32_F107_H

#include "..\..\RegisterReference.h"

template<RegisterPointerType RegisterAddr>
class RtcControlLowRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType SecondFlag					: 1;
		RegisterDataType AlarmFlag					: 1;
		RegisterDataType OverflowFlag				: 1;
		RegisterDataType RegistersSynchronizedFlag	: 1;
		RegisterDataType ConfigurationFlag			: 1;
		RegisterDataType RtcOff						: 1;
		RegisterDataType Reserved0					: 10;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned short>, RegisterAddr> RegRef;
public:
	inline static bool GetRtcOff()
	{
		return RegRef::_register->RtcOff;
	}
	
	inline static void EnableConfig(bool enable = true)
	{
		RegRef::_register->ConfigurationFlag = enable;
	}
};

template<RegisterPointerType RegisterAddr>
class RtcCounterHiRegister
{
protected:
	typedef RegisterReference<unsigned short, RegisterAddr> RegRef;
public:
	inline static unsigned short GetCounterHi()
	{
		return *RegRef::_register;
	}
	
	inline static void SetCounterHi(unsigned short value)
	{
		*RegRef::_register = value;
	}
};

template<RegisterPointerType RegisterAddr>
class RtcCounterLowRegister
{
protected:
	typedef RegisterReference<unsigned short, RegisterAddr> RegRef;
public:
	inline static unsigned short GetCounterLow()
	{
		return *RegRef::_register;
	}
	
	inline static void SetCounterLow(unsigned short value)
	{
		*RegRef::_register = value;
	}
};

#endif