////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		26/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef GPIOPARTS_STM32_F107_H
#define GPIOPARTS_STM32_F107_H

#include "..\..\RegisterReference.h"

template<RegisterPointerType RegisterAddrLow, RegisterPointerType RegisterAddrHi>
class GpioConfigRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Set 	: 16;
		RegisterDataType Reset	: 16;
	};
	
	typedef RegisterReference<unsigned int, RegisterAddrLow> LowRegRef;
	typedef RegisterReference<unsigned int, RegisterAddrHi> HiRegRef;

protected:
	static inline void SetConfig(unsigned char pinNumber, unsigned char mode, unsigned char cfg)
	{
		if (pinNumber < 8)
		{
			*LowRegRef::_register &= ~(0x0F << pinNumber * 4);
			*LowRegRef::_register |= (mode << pinNumber * 4);
			*LowRegRef::_register |= (cfg << (pinNumber * 4 + 2));
		}
		else
		{
			pinNumber -= 8;
			*HiRegRef::_register &= ~(0x0F << pinNumber * 4);
			*HiRegRef::_register |= (mode << pinNumber * 4);
			*HiRegRef::_register |= (cfg << (pinNumber * 4 + 2));
		}
	}
public:
	static inline void SetInputPin(unsigned char pinNumber)
	{
		unsigned char mode = 0;
		unsigned char cfg = 1;
		SetConfig(pinNumber, mode, cfg);
	}
	
	static inline void SetInputWithPullUpDownPin(unsigned char pinNumber)
	{
		unsigned char mode = 0;
		unsigned char cfg = 2;
		SetConfig(pinNumber, mode, cfg);
	}
	
	static inline void SetOutputPin(unsigned char pinNumber)
	{
		unsigned char mode = 3;
		unsigned char cfg = 0;
		SetConfig(pinNumber, mode, cfg);
	}
	
	static inline void SetAlternateOutputPin(unsigned char pinNumber)
	{
		unsigned char mode = 3;
		unsigned char cfg = 2;
		SetConfig(pinNumber, mode, cfg);
	}
};

template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
class GpioConfigurationRegister
{
protected:
	static volatile RegisterDataType * const _register;
public:
	static inline void SetInputPin(unsigned char pin)
	{
		*_register = 0;
	}
};

template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
class GpioInputDataRegister
{
protected:
	static volatile RegisterDataType * const _register;
public:
	static inline RegisterDataType Read()
	{
		return *_register;
	}
	
	static inline RegisterDataType Read(unsigned char pinNumber)
	{
		return *_register & (1 << pinNumber);
	}
};

template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
class GpioOutputDataRegister
{
protected:
	static volatile RegisterDataType * const _register;
public:
	static inline void Write(RegisterDataType value)
	{
		*_register = value;
	}
	
	static inline void ToggleBit(unsigned char pinNumber)
	{
		*_register ^= (1 << pinNumber);
	}
};

template<RegisterPointerType RegisterAddr>
class GpioBitSetResetRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Set 	: 16;
		RegisterDataType Reset	: 16;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	static inline void SetBit(unsigned char pinNumber)
	{
		RegRef::_register->Set |= (1 << pinNumber);
	}
	
	static inline void ClearBit(unsigned char pinNumber)
	{
		RegRef::_register->Reset |= (1 << pinNumber);
	}
};

template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
volatile RegisterDataType * const GpioConfigurationRegister<RegisterDataType, PointerType, RegisterAddr>
	::_register = reinterpret_cast<volatile RegisterDataType*>(RegisterAddr);


template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
volatile RegisterDataType * const GpioInputDataRegister<RegisterDataType, PointerType, RegisterAddr>
	::_register = reinterpret_cast<volatile RegisterDataType*>(RegisterAddr);

template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
volatile RegisterDataType * const GpioOutputDataRegister<RegisterDataType, PointerType, RegisterAddr>
	::_register = reinterpret_cast<volatile RegisterDataType*>(RegisterAddr);


#endif