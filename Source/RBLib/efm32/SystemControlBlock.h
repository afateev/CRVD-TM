#ifndef EMF32_SYSTEMCONTROLBLOCK_H
#define EMF32_SYSTEMCONTROLBLOCK_H

#include "Register.h"
#include "../IdObjectBase.h"
#include "../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Efm32
{
namespace SystemControlBlock
{
	// ICSR - Interrupt Control and State Register
	class InterruptControlAndStateRegister
	{
	public:
		static const unsigned int Address = 0xE000ED04;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType VECTACTIV	: 9; // Contains the active exception number
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType RETTOBASE	: 1; // Indicates whether there are preempted active exceptions
			Core::RegisterValueType VECTPENDING	: 6; // Indicates the exception number of the highest priority pending enabled exception
			Core::RegisterValueType 			: 4;
			Core::RegisterValueType ISRPENDING	: 1; // Interrupt pending flag, excluding NMI and Faults
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType PENDSTCLR	: 1; // SysTick exception clear-pending bit
			Core::RegisterValueType PENDSTSET	: 1; // SysTick exception set-pending bit
			Core::RegisterValueType PENDSVCLR	: 1; // PendSV clear-pending bit
			Core::RegisterValueType PENDSVSET	: 1; // PendSV set-pending bit
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType NMIPENDSET	: 1; // NMI set-pending bit
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static int GetActiveVector()
		{
			// 0 = Thread mode
			// Nonzero = The exception number1 of the currently active exception.
			// Subtract 16 from this value to obtain the IRQ number required to index
			int vector = Register::Ptr()->VECTACTIV;
			
			if (vector)
			{
				vector -= 16; 
			}
			
			return vector;
		}
	};
	
	// VTOR - Vector Table Offset Register
	class VectorTableOffsetRegister
	{
	public:
		static const unsigned int Address = 0xE000ED08;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType 				: 7;
			Core::RegisterValueType TBLOFF			: 25; // Vector table base offset field. It contains bits[29:7] (bits[31:7] for Cortex-M3 revision r2p1 and
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static unsigned int GetVectorTableOffset(unsigned int offset)
		{
			int offsetValue = Register::Ptr()->TBLOFF;
			offsetValue <<= 7;
			return  offsetValue;
		}
		
		static void SetVectorTableOffset(unsigned int offset)
		{
			int offsetValue = offset;
			offsetValue >>= 7;
			Register::Ptr()->TBLOFF = offsetValue;
		}
	};
	
	// AIRCR - Application Interrupt and Reset Control Register
	class ApplicationInterruptAndResetControlRegister
	{
	public:
		static const unsigned int Address = 0xE000ED0C;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType VECTRESET		: 1; // Reserved for Debug use
			Core::RegisterValueType VECTCLRACTIVE	: 1; // Reserved for Debug use
			Core::RegisterValueType SYSRESETREQ		: 1; // System reset request
			Core::RegisterValueType 				: 5;
			Core::RegisterValueType PRIGROUP		: 3; // Interrupt priority grouping field
			Core::RegisterValueType 				: 4;
			Core::RegisterValueType ENDIANESS		: 1; // Always read as 0 (Little-endian)
			Core::RegisterValueType VECTKEY			: 16; // Register key: Reads as 0x05FA. On writes, write 0x5FA to VECTKEY, otherwise the write is ignored.
		};
		
		typedef Register<Address, unsigned int> Register;
	public:
		static void Reset()
		{
			// запись с одновременныс указанием ключа на запись
			Core::RegisterValueType regValue = *Register::Ptr();
			RegStruct * val = (RegStruct *)&regValue;
			val->VECTKEY = 0x5FA;
			val->SYSRESETREQ = 1;
			*Register::Ptr() = regValue;
		}
	};
	
	// SCR - System Control Register
	class SystemControlRegister
	{
	public:
		static const unsigned int Address = 0xE000ED10;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType SLEEPONEXIT		: 1; // Indicates sleep-on-exit when returning from Handler mode to Thread mode
			Core::RegisterValueType SLEEPDEEP		: 1; // Controls whether the processor uses sleep or deep sleep as its low power mode
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType SEVONPEND		: 1; // Send Event on Pending bit
			Core::RegisterValueType 				: 27;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SleepOnExit(bool enable, bool deep)
		{
			Register::Ptr()->SLEEPDEEP = deep;
			Register::Ptr()->SLEEPONEXIT = enable;
		}
	};
	
	class SystemControlBlock :
		public InterruptControlAndStateRegister,
		public VectorTableOffsetRegister,
		public ApplicationInterruptAndResetControlRegister,
		public SystemControlRegister
	{
	public:
	};
}
}
}

#endif