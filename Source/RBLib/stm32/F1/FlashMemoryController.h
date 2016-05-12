#ifndef FLASHMEMORYCONTROLLER_STM32_F1_H
#define FLASHMEMORYCONTROLLER_STM32_F1_H

#include "Register.h"
#include "../../IdObjectBase.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Stm32
{
namespace F1
{
namespace FlashMemoryControllerImplementation
{
	static const unsigned int BaseAddress = 0x40022000;
	
	// Flash access control register (FLASH_ACR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class AccessControlRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x00;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
	protected:
		struct RegStruct
		{
			Core::RegisterValueType LATENCY				: 3;
			Core::RegisterValueType HLFCYA				: 1;
			Core::RegisterValueType PRFTBE				: 1;
			Core::RegisterValueType PRFTBS				: 1;
			Core::RegisterValueType 					: 26;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		// 000 Zero wait state, if 0 < SYSCLK <= 24 MHz
		// 001 One wait state, if 24 MHz < SYSCLK <= 48 MHz
		// 010 Two wait states, if 48 MHz < SYSCLK <= 72 MHz
		static void SetLatency(unsigned char waitStateCount)
		{
			Register::Ptr()->LATENCY = waitStateCount;
		}
	};
	
	// FPEC key register (FLASH_KEYR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class KeyRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x04;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		static const unsigned int KEY1 = 0x45670123;
		static const unsigned int KEY2 = 0xCDEF89AB;
	protected:
		typedef Register<Address, unsigned int> Register;
	public:
		static void Unlock()
		{
			*Register::Ptr() = KEY1;
			*Register::Ptr() = KEY2;
		}
	};
	
	// Flash status register (FLASH_SR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class StatusRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x0C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType BSY					: 1;
			Core::RegisterValueType 					: 1;
			Core::RegisterValueType PGERR				: 1;
			Core::RegisterValueType 					: 1;
			Core::RegisterValueType WRPRTERR			: 1;
			Core::RegisterValueType EOP					: 1;
			Core::RegisterValueType 					: 26;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static bool Busy()
		{
			return Register::Ptr()->BSY;
		}
	};
	
	// Flash control register (FLASH_CR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class ControlRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x10;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType PG					: 1;
			Core::RegisterValueType PER					: 1;
			Core::RegisterValueType MER					: 1;
			Core::RegisterValueType 					: 1;
			Core::RegisterValueType OPTPG				: 1;
			Core::RegisterValueType OPTER				: 1;
			Core::RegisterValueType STRT				: 1;
			Core::RegisterValueType LOCK				: 1;
			Core::RegisterValueType 					: 1;
			Core::RegisterValueType OPTWRE				: 1;
			Core::RegisterValueType ERRIE				: 1;
			Core::RegisterValueType 					: 1;
			Core::RegisterValueType EOPIE				: 1;
			Core::RegisterValueType 					: 19;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableProgramming(bool enable = true)
		{
			Register::Ptr()->PG = enable;
		}
		
		static bool Locked()
		{
			return Register::Ptr()->LOCK;
		}
		
		static void EnablePageErase(bool enable = true)
		{
			Register::Ptr()->PER = enable;
		}
		
		static void Start()
		{
			Register::Ptr()->STRT = 1;
		}
	};
	
	// Flash address register (FLASH_AR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class AddressRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x14;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, unsigned int> Register;
	public:
		static void SetAddress(unsigned int value)
		{
			*Register::Ptr() = value;
		}
	};
	
	class FlashMemoryController :
		public AccessControlRegister<BaseAddress>,
		public KeyRegister<BaseAddress>,
		public StatusRegister<BaseAddress>,
		public ControlRegister<BaseAddress>,
		public AddressRegister<BaseAddress>
	{
	public:
		typedef KeyRegister<BaseAddress> KeyRegister;
		typedef StatusRegister<BaseAddress> StatusRegister;
		typedef ControlRegister<BaseAddress> ControlRegister;
		typedef AddressRegister<BaseAddress> AddressRegister;
	public:
		static void Unlock()
		{
			if (ControlRegister::Locked())
			{
				KeyRegister::Unlock();
			}
		}
		
		static void Erase(unsigned int pageAddress)
		{
			while(StatusRegister::Busy())
			{
				;
			}
			
			ControlRegister::EnablePageErase(true);
			AddressRegister::SetAddress(pageAddress);
			ControlRegister::Start();
			
			while(StatusRegister::Busy())
			{
				;
			}
			
			ControlRegister::EnablePageErase(false);
		}
	};
}

typedef FlashMemoryControllerImplementation::FlashMemoryController FlashMemoryController;

}
}
}

#endif
