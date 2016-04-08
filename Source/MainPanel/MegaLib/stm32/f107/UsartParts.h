////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		03/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef USARTPARTS_STM32_F107_H
#define USARTPARTS_STM32_F107_H

#include "..\..\RegisterReference.h"

template<RegisterPointerType RegisterAddr>
class UsartStatusRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType ParityError						: 1;
		RegisterDataType FramingError						: 1;
		RegisterDataType NoiseError							: 1;
		RegisterDataType OverrunError						: 1;
		RegisterDataType Idle								: 1;
		RegisterDataType RxNotEmpty							: 1;
		RegisterDataType TransmissionComplete				: 1;
		RegisterDataType TxEmpty							: 1;
		RegisterDataType LinBreak							: 1;
		RegisterDataType CTS								: 1;
		RegisterDataType Reserved0							: 22;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static bool IsRxNotEmpty()
	{
		return RegRef::_register->RxNotEmpty;
	}
	
	inline static bool IsTxEmpty()
	{
		return RegRef::_register->TxEmpty;
	}
};

template<RegisterPointerType RegisterAddr>
class UsartDataRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Data		: 9;
		RegisterDataType Reserved0	: 23;
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

template<RegisterPointerType RegisterAddr>
class UsartBaudRateRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Fraction	: 4;
		RegisterDataType Mantissa	: 12;
		RegisterDataType Reserved0	: 16;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void SetBaudRate(unsigned short Mantissa, unsigned char Fraction)
	{
		RegRef::_register->Fraction = Fraction;
		RegRef::_register->Mantissa = Mantissa;
	}
};

template<RegisterPointerType RegisterAddr>
class UsartControl1Register
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType SendBreak								: 1;
		RegisterDataType ReceiverWakeup							: 1;
		RegisterDataType ReceiverEnable							: 1;
		RegisterDataType TransmitterEnable						: 1;
		RegisterDataType IdleInterruptEnable					: 1;
		RegisterDataType RxNotEmptyInterruptEnable				: 1;
		RegisterDataType TransmissionCompleteInterruptEnable	: 1;
		RegisterDataType TxEmptyInterruptEnable					: 1;
		RegisterDataType ParityErrorInterruptEnable				: 1;
		RegisterDataType ParitySelection						: 1;
		RegisterDataType ParityControlEnable					: 1;
		RegisterDataType WakeupMethod							: 1;
		RegisterDataType WordLength								: 1;
		RegisterDataType UsartEnable							: 1;
		RegisterDataType Reserved0								: 18;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void Enable(bool enable = true)
	{
		RegRef::_register->TransmitterEnable = enable;
		RegRef::_register->ReceiverEnable = enable;
		RegRef::_register->UsartEnable = enable;
	}
	
	inline static void RxInterruptEnable(bool enable = true)
	{
		RegRef::_register->RxNotEmptyInterruptEnable = enable;
	}
	
	inline static bool IsRxInterruptEnabled()
	{
		return RegRef::_register->RxNotEmptyInterruptEnable;
	}
	
	inline static void TxInterruptEnable(bool enable = true)
	{
		RegRef::_register->TxEmptyInterruptEnable = enable;
	}
	
	inline static bool IsTxInterruptEnabled()
	{
		return RegRef::_register->TxEmptyInterruptEnable;
	}
	
	inline static void ParityControlEnable(bool enable = true)
	{
		RegRef::_register->ParityControlEnable = enable;
	}
	
	inline static void ParitySelection(bool odd)
	{
		// true = odd
		// false = even
		RegRef::_register->ParityControlEnable = odd;
	}
};

#endif