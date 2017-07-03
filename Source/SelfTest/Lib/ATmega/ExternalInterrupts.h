///////////////////////////////////////////////////////////////////////////////
//	 ласс управлени€ внешними прерывани€ми
//	ATMEGA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef EXTERNALINTERRUPTS_ATMEGA_H
#define EXTERNALINTERRUPTS_ATMEGA_H

#include "../IdObjectBase.h"
#include "../RegisterReference.h"

namespace Lib
{
namespace ATmega
{
namespace ExternalInterrupts
{
	// “ип значени€ порта
	typedef unsigned char ValueDataType;
	
	// PCICR Ц Pin Change Interrupt Control Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class PinChangeInterruptControlRegister
	{
	public:
		struct RegisterStruct
		{
			ValueDataType Enable0 		: 1;
			ValueDataType Enable1 		: 1;
			ValueDataType Enable2 		: 1;
			ValueDataType Enable3 		: 1;
			ValueDataType 				: 4;
		};
		
		enum Interrupt
		{
			Interrupt0,
			Interrupt1,
			Interrupt2,
			Interrupt3
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void SetInterruptEnable(Interrupt interrupt, bool enable = true)
		{
			switch(interrupt)
			{
			case Interrupt0:
				RegRef::_register->Enable0 = enable;
				break;
			case Interrupt1:
				RegRef::_register->Enable1 = enable;
				break;
			case Interrupt2:
				RegRef::_register->Enable2 = enable;
				break;
			case Interrupt3:
				RegRef::_register->Enable3 = enable;
				break;
			}
		}
		
		static void InterruptEnable(Interrupt interrupt)
		{
			SetInterruptEnable(interrupt, true);
		}
		
		static void InterruptDisable(Interrupt interrupt)
		{
			SetInterruptEnable(interrupt, false);
		}
	};
	
	// PCMSK3 Ц Pin Change Mask Register 3
	template
		<
			RegisterPointerType RegisterAddr
		>
	class PinChangeMaskRegister3
	{
	public:
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void PinChangeMask3SetBit(unsigned char bit)
		{
			*RegRef::_register |= (1 << bit);
		}
		
		static void PinChangeMask3ClearBit(unsigned char bit)
		{
			*RegRef::_register &= ~(1 << bit);
		}
	};
	
	// PCMSK2 Ц Pin Change Mask Register 2
	template
		<
			RegisterPointerType RegisterAddr
		>
	class PinChangeMaskRegister2
	{
	public:
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void PinChangeMask2SetBit(unsigned char bit)
		{
			*RegRef::_register |= (1 << bit);
		}
		
		static void PinChangeMask2ClearBit(unsigned char bit)
		{
			*RegRef::_register &= ~(1 << bit);
		}
	};
	
	// PCMSK1 Ц Pin Change Mask Register 1
	template
		<
			RegisterPointerType RegisterAddr
		>
	class PinChangeMaskRegister1
	{
	public:
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void PinChangeMask1SetBit(unsigned char bit)
		{
			*RegRef::_register |= (1 << bit);
		}
		
		static void PinChangeMask1ClearBit(unsigned char bit)
		{
			*RegRef::_register &= ~(1 << bit);
		}
	};
	
	// PCMSK0 Ц Pin Change Mask Register 0
	template
		<
			RegisterPointerType RegisterAddr
		>
	class PinChangeMaskRegister0
	{
	public:
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void PinChangeMask0SetBit(unsigned char bit)
		{
			*RegRef::_register |= (1 << bit);
		}
		
		static void PinChangeMask0ClearBit(unsigned char bit)
		{
			*RegRef::_register &= ~(1 << bit);
		}
	};
	
	template
		<
			unsigned int IdObj,
			RegisterPointerType ExternalInterruptControlRegisterAAddr,
			RegisterPointerType ExternalInterruptMaskRegisterAddr,
			RegisterPointerType ExternalInterruptFlagRegisterAddr,
			RegisterPointerType PinChangeInterruptControlRegisterAddr,
			RegisterPointerType PinChangeInterruptFlagRegisterAddr,
			RegisterPointerType PinChangeMaskRegister3Addr,
			RegisterPointerType PinChangeMaskRegister2Addr,
			RegisterPointerType PinChangeMaskRegister1Addr,
			RegisterPointerType PinChangeMaskRegister0Addr
		>
	class ExternalInterrupts :
		public PinChangeInterruptControlRegister<PinChangeInterruptControlRegisterAddr>,
		public PinChangeMaskRegister3<PinChangeMaskRegister3Addr>,
		public PinChangeMaskRegister2<PinChangeMaskRegister2Addr>,
		public PinChangeMaskRegister1<PinChangeMaskRegister1Addr>,
		public PinChangeMaskRegister0<PinChangeMaskRegister0Addr>
	{
	};
}
}
}

#endif