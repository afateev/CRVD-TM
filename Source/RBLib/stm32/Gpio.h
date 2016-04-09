///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами ввода-вывода
//	STM32
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_STM32_H
#define GPIO_STM32_H

#include "Register.h"
#include "../IdObjectBase.h"
#include "../Utils/CallbackListWrapper.h"

namespace Rblib
{
namespace Stm32
{
namespace GpioImplementation
{
	// Port configuration register low (GPIOx_CRL) (x=A..G)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffsetLow = 0x00;
		static const Core::RegisterAddressType AddressOffsetHi = 0x04;
		static const Core::RegisterAddressType AddressLow = BaseAddress + AddressOffsetLow;
		static const Core::RegisterAddressType AddressHi = BaseAddress + AddressOffsetHi;
		
		enum Mode
		{
			ModeInput			= 0,
			ModeOutput10Mhz		= 1,
			ModeOutput2Mhz		= 2,
			ModeOutput50Mhz		= 3,
			ModeOutput 			= ModeOutput10Mhz
		};
		
		enum Config
		{
			ConfigInputAnalog				= 0,
			ConfigInputFloating				= 1,
			ConfigInputPull					= 2,
			ConfigOutputPushPull			= 0,
			ConfigOutputOpenDrain			= 1,
			ConfigOutputAlternatePushPull	= 2,
			ConfigOutputAlternateOpenDrain	= 3,
		};
	protected:
		typedef Register<AddressLow, Core::RegisterValueType> RegisterLow;
		typedef Register<AddressHi, Core::RegisterValueType> RegisterHi;
	public:
		static void SetMode(unsigned char pin, Mode val)
		{
			volatile Core::RegisterValueType *ptr = 0;
			unsigned char offset = 0;
			
			if (pin <= 7)
			{
				ptr = RegisterLow::Ptr();
				offset = pin * 4;
			}
			
			if (pin >= 8 && pin <= 15)
			{
				ptr = RegisterHi::Ptr();
				offset = (pin - 8) * 4;
			}
			
			if (ptr)
			{
				Core::RegisterValueType regVal = *ptr;
				regVal &= ~(0x03 << offset);
				regVal |= val << offset;
				
				*ptr = regVal;
			}
		}
		
		static void SetConfig(unsigned char pin, Config val)
		{
			volatile Core::RegisterValueType *ptr = 0;
			unsigned char offset = 0;
			
			if (pin <= 7)
			{
				ptr = RegisterLow::Ptr();
				offset = pin * 4;
			}
			
			if (pin >= 8 && pin <= 15)
			{
				ptr = RegisterHi::Ptr();
				offset = (pin - 8) * 4;
			}
			
			offset += 2;
			
			if (ptr)
			{
				Core::RegisterValueType regVal = *ptr;
				regVal &= ~(0x03 << offset);
				regVal |= val << offset;
				
				*ptr = regVal;
			}
		}
		
		static void SetOutputPin(unsigned char pin)
		{
			SetMode(pin, ModeOutput);
			SetConfig(pin, ConfigOutputPushPull);
		}
	};
	
	// Port input data register (GPIOx_IDR) (x=A..G)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortInputDataRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x08;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static Core::RegisterValueType Read()
		{
			return *Register::Ptr();
		}
		
		static Core::RegisterValueType Read(unsigned char pin)
		{
			return *Register::Ptr() & (1 << pin);
		}
	};
	
	// Port output data register (GPIOx_ODR) (x=A..G)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortOutputDataRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x0C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static void Write(Core::RegisterValueType val)
		{
			*Register::Ptr() = val;
		}
	};
	
	// Port bit set/reset register (GPIOx_BSRR) (x=A..G)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortBitSetResetRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x10;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static void SetBit(unsigned char pin)
		{
			*Register::Ptr() = 1 << pin;
		}
	};
	
	// Port bit reset register (GPIOx_BRR) (x=A..G)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortBitResetRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x14;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static void ClearBit(unsigned char pin)
		{
			*Register::Ptr() = 1 << pin;
		}
	};
	
	template
		<
			unsigned int IdObj,
			Core::RegisterAddressType BaseAddress
		>
	class Port :
		public IdObjectBase<IdObj>,
		public PortConfigurationRegister<BaseAddress>,
		public PortInputDataRegister<BaseAddress>,
		public PortOutputDataRegister<BaseAddress>,
		public PortBitSetResetRegister<BaseAddress>,
		public PortBitResetRegister<BaseAddress>
	{
	};
	
	class Gpio
	{
	public:
		typedef Port<1, 0x40010800> A;
		typedef Port<2, 0x40010C00> B;
		typedef Port<3, 0x40011000> C;
		typedef Port<4, 0x40011400> D;
		typedef Port<5, 0x40011800> E;
		typedef Port<6, 0x40011C00> F;
		typedef Port<7, 0x40012000> G;
	};
}

typedef GpioImplementation::Gpio Gpio;
}
}

#endif