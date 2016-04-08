////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		11/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_BASIC_STM32_F107_H
#define TIMER_BASIC_STM32_F107_H

#include "..\..\RegisterReference.h"

template<RegisterPointerType RegisterAddr>
class TimerBasicControl1Register
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType CounterEnable 				: 1;
		RegisterDataType UpdateDisable 				: 1;
		RegisterDataType UpdateRequestSource 		: 1;
		RegisterDataType OnePulseMode 				: 1;
		RegisterDataType Reserved0					: 4;
		RegisterDataType AutoReloadPreloadEnable 	: 1;
		RegisterDataType Reserved1 					: 8;
		RegisterDataType Reserved2 					: 16;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void Start()
	{
		RegRef::_register->CounterEnable = 1;
	}
	
	inline static void Stop()
	{
		RegRef::_register->CounterEnable = 0;
	}
};

template<RegisterPointerType RegisterAddr>
class TimerBasicInterruptEnableRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType UpdateInterruptEnable 		: 1;
		RegisterDataType Reserved0					: 7;
		RegisterDataType UpdateDmaRequestEnable 	: 1;
		RegisterDataType Reserved1 					: 7;
		RegisterDataType Reserved2 					: 16;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void InterruptEnable()
	{
		RegRef::_register->UpdateInterruptEnable = 1;
	}
	
	inline static void InterruptDisable()
	{
		RegRef::_register->UpdateInterruptEnable = 0;
	}
};

template<RegisterPointerType RegisterAddr>
class TimerBasicStatusRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType UpdateInterruptFlag 		: 1;
		RegisterDataType Reserved0					: 15;
		RegisterDataType Reserved1 					: 16;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void ClearInterruptFlag()
	{
		RegRef::_register->UpdateInterruptFlag = 0;
	}
};

template<RegisterPointerType RegisterAddr>
class TimerBasicPrescalerRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Prescaler					: 16;
		RegisterDataType Reserved0 					: 16;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void SetPrescaler(unsigned short value)
	{
		RegRef::_register->Prescaler = value;
	}
};

template<RegisterPointerType RegisterAddr>
class TimerBasicAutoReloadRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType AutoReload					: 16;
		RegisterDataType Reserved0 					: 16;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void SetMaxCount(unsigned short value)
	{
		RegRef::_register->AutoReload = value;
	}
};

template<unsigned int BaseAddress>
class TimerBasic :
	public TimerBasicControl1Register			<BaseAddress + 0x00>,
	public TimerBasicInterruptEnableRegister	<BaseAddress + 0x0C>,
	public TimerBasicStatusRegister				<BaseAddress + 0x10>,
	public TimerBasicPrescalerRegister			<BaseAddress + 0x28>,
	public TimerBasicAutoReloadRegister			<BaseAddress + 0x2C>
{
protected:
	typedef void (CallbackType)(void);
	static CallbackType *_interruptHandler;
public:
	inline static void InterruptHandler()
	{
		TimerBasicStatusRegister<BaseAddress + 0x10>::ClearInterruptFlag();
		if (_interruptHandler)
			_interruptHandler();
	}
	inline static void SetInterruptHandler(CallbackType handler)
	{
		_interruptHandler = handler;
	}
};

template<unsigned int BaseAddress>
TimerBasic<BaseAddress>::CallbackType *
TimerBasic<BaseAddress>::_interruptHandler = 0;


#endif