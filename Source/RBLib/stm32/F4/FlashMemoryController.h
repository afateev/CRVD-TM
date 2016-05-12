#ifndef FLASHMEMORYCONTROLLER_STM32_F4_H
#define FLASHMEMORYCONTROLLER_STM32_F4_H

#include "Register.h"
#include "../../IdObjectBase.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Stm32
{
namespace F4
{
namespace FlashMemoryControllerImplementation
{
	static const unsigned int BaseAddress = 0x40023C00;
	
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
			Core::RegisterValueType 					: 5;
			Core::RegisterValueType PRFTEN				: 1;
			Core::RegisterValueType ICEN				: 1;
			Core::RegisterValueType DCEN				: 1;
			Core::RegisterValueType ICRST				: 1;
			Core::RegisterValueType DCRST				: 1;
			Core::RegisterValueType 					: 19;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
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
			Core::RegisterValueType EOP					: 1;
			Core::RegisterValueType OPERR				: 1;
			Core::RegisterValueType 					: 2;
			Core::RegisterValueType WRPRTERR			: 1;
			Core::RegisterValueType PGAERR				: 1;
			Core::RegisterValueType PGPERR				: 1;
			Core::RegisterValueType PGSERR				: 1;
			Core::RegisterValueType RDERR				: 1;
			Core::RegisterValueType 					: 7;
			Core::RegisterValueType BSY					: 1;
			Core::RegisterValueType 					: 15;
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
			Core::RegisterValueType SER					: 1;
			Core::RegisterValueType MER					: 1;
			Core::RegisterValueType SNB					: 5;
			Core::RegisterValueType PSIZE				: 2;
			Core::RegisterValueType 					: 5;
			Core::RegisterValueType MER1				: 1;
			Core::RegisterValueType STRT				: 1;
			Core::RegisterValueType 					: 7;
			Core::RegisterValueType EOPIE				: 1;
			Core::RegisterValueType ERRIE				: 1;
			Core::RegisterValueType 					: 5;
			Core::RegisterValueType LOCK				: 1;
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
		
		static void EnableSectorErase(bool enable = true)
		{
			Register::Ptr()->SER = enable;
		}
		
		static void SetSectorNumber(unsigned char sectorNumber)
		{
			Register::Ptr()->SNB = sectorNumber;
		}
		
		static void Start()
		{
			Register::Ptr()->STRT = 1;
		}
	};
	
	class FlashMemoryController :
		public AccessControlRegister<BaseAddress>,
		public KeyRegister<BaseAddress>,
		public StatusRegister<BaseAddress>,
		public ControlRegister<BaseAddress>
	{
	public:
		typedef KeyRegister<BaseAddress> KeyRegister;
		typedef StatusRegister<BaseAddress> StatusRegister;
		typedef ControlRegister<BaseAddress> ControlRegister;
	public:
		static void Unlock()
		{
			if (ControlRegister::Locked())
			{
				KeyRegister::Unlock();
			}
		}
		
		static void Erase(unsigned int sectorNumber)
		{
			while(StatusRegister::Busy())
			{
				;
			}
			
			ControlRegister::EnableSectorErase(true);
			ControlRegister::SetSectorNumber(sectorNumber);
			ControlRegister::Start();
			
			while(StatusRegister::Busy())
			{
				;
			}
			
			ControlRegister::EnableSectorErase(false);
		}
	};
}

typedef FlashMemoryControllerImplementation::FlashMemoryController FlashMemoryController;

}
}
}

#endif
