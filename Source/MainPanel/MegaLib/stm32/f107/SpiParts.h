////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		05/02/2013
////////////////////////////////////////////////////////////////////////////////

#ifndef SPIPARTS_STM32_F107_H
#define SPIPARTS_STM32_F107_H

#include "..\..\RegisterReference.h"

template<RegisterPointerType RegisterAddr>
class SpiControl1Register
{
public:
	enum Divider
	{
		Div2	= 0,
		Div4	= 1,
		Div8	= 2,
		Div16	= 3,
		Div32	= 4,
		Div64	= 5,
		Div128	= 6,
		Div256	= 7,
	};
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType ClockPhase							: 1;
		RegisterDataType ClockPolarity						: 1;
		RegisterDataType MasterEnable						: 1;
		RegisterDataType BaudRate							: 3;
		RegisterDataType SpiEnable							: 1;
		RegisterDataType FrameFormat						: 1;
		RegisterDataType InternalSlaveSelect				: 1;
		RegisterDataType SoftwareSlaveManagementEnable		: 1;
		RegisterDataType ReceiveOnly						: 1;
		RegisterDataType DataFrameFormat					: 1;
		RegisterDataType CrcTransferNext					: 1;
		RegisterDataType HardwareCrcCalculationEnable		: 1;
		RegisterDataType OutputEnableInBidirectionalMode	: 1;
		RegisterDataType BidirectionalDataModeEnable		: 1;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void Enable(bool enable = true, bool master = true, bool polarity = false)
	{
		RegRef::_register->MasterEnable = master;
		RegRef::_register->SoftwareSlaveManagementEnable = enable;
		RegRef::_register->InternalSlaveSelect = enable;
		RegRef::_register->ClockPolarity = polarity;
		RegRef::_register->SpiEnable = enable;
	}
	
	static void SetDivider(Divider val)
	{
		RegRef::_register->BaudRate = val;
	}
};

template<RegisterPointerType RegisterAddr>
class SpiStatusRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType RxNotEmpty							: 1;
		RegisterDataType TxNotEmpty							: 1;
		RegisterDataType ChannelSide						: 1;
		RegisterDataType Underrun							: 1;
		RegisterDataType CrcError							: 1;
		RegisterDataType ModeFault							: 1;
		RegisterDataType Overrun							: 1;
		RegisterDataType Busy								: 1;
		RegisterDataType Reserved0							: 8;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static bool IsBusy()
	{
		return RegRef::_register->Busy;
	}
	
	inline static bool IsRxNotEmpty()
	{
		return RegRef::_register->RxNotEmpty;
	}
};

template<RegisterPointerType RegisterAddr>
class SpiDataRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Data;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static unsigned char ReadData()
	{
		return RegRef::_register->Data;
	}
	
	inline static void WriteData(unsigned char value)
	{
		RegRef::_register->Data = value;
	}
};

#endif