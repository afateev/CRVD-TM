#ifndef RTC_STM32_F4_H
#define RTC_STM32_F4_H

#include "Register.h"
#include "../../IdObjectBase.h"
#include "../../Utils/CallbackWrapper.h"
#include <time.h>

namespace Rblib
{
namespace Stm32
{
namespace F4
{
namespace RtcImplementation
{
	static const unsigned int BaseAddress = 0x40002800;
	
	// RTC time register (RTC_TR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TimeRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x00;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType SU			: 4;
			Core::RegisterValueType ST			: 3;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType MNU			: 4;
			Core::RegisterValueType MNT			: 3;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType HU			: 4;
			Core::RegisterValueType HT			: 2;
			Core::RegisterValueType PM			: 1;
			Core::RegisterValueType 			: 9;
		};
		
		typedef Register<Address, Core::RegisterValueType> RegisterForWrite;
		typedef Register<Address, RegStruct> Register;
	public:
		static int GetSeconds()
		{
			int res = Register::Ptr()->ST;
			res *= 10;
			res += Register::Ptr()->SU;
			
			return res;
		}
		
		static int GetMinutes()
		{
			int res = Register::Ptr()->MNT;
			res *= 10;
			res += Register::Ptr()->MNU;
			
			return res;
		}
		
		static int GetHours()
		{
			int res = Register::Ptr()->HT;
			res *= 10;
			res += Register::Ptr()->HU;
			
			return res;
		}
		
		static bool GetPM()
		{
			return Register::Ptr()->PM;
		}
		
		static void SetTimeValue(int hours, int minutes, int seconds)
		{
			RegStruct newVal;
			newVal.HT = hours / 10;
			newVal.HU = hours % 10;
			newVal.MNT = minutes / 10;
			newVal.MNU = minutes % 10;
			newVal.ST = seconds / 10;
			newVal.SU = seconds % 10;
			
			*RegisterForWrite::Ptr() = *((Core::RegisterValueType *)&newVal);
		}
	};
	
	// RTC date register (RTC_DR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class DateRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x04;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType DU			: 4;
			Core::RegisterValueType DT			: 2;
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType MU			: 4;
			Core::RegisterValueType MT			: 1;
			Core::RegisterValueType WDU			: 3;
			Core::RegisterValueType YU			: 4;
			Core::RegisterValueType YT			: 4;
			Core::RegisterValueType 			: 8;
		};
		
		typedef Register<Address, Core::RegisterValueType> RegisterForWrite;
		typedef Register<Address, RegStruct> Register;
	public:
		static int GetDay()
		{
			int res = Register::Ptr()->DT;
			res *= 10;
			res += Register::Ptr()->DU;
			
			return res;
		}
		
		static int GetMonth()
		{
			int res = Register::Ptr()->MT;
			res *= 10;
			res += Register::Ptr()->MU;
			
			return res;
		}
		
		static int GetYear()
		{
			int res = Register::Ptr()->YT;
			res *= 10;
			res += Register::Ptr()->YU;
			
			return res;
		}
		
		static bool GetDayOfWeek()
		{
			return Register::Ptr()->WDU;
		}
		
		static void SetDateValue(int year, int month, int day)
		{
			RegStruct newVal;
			newVal.YT = year / 10;
			newVal.YU = year % 10;
			newVal.MT = month / 10;
			newVal.MU = month % 10;
			newVal.DT = day / 10;
			newVal.DU = day % 10;
			
			*RegisterForWrite::Ptr() = *((Core::RegisterValueType *)&newVal);
		}
	};
	
	// RTC initialization and status register (RTC_ISR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class InitializationAndStatusRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x0C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType ALRAWF		: 1;
			Core::RegisterValueType ALRBWF		: 1;
			Core::RegisterValueType WUTWF		: 1;
			Core::RegisterValueType SHPF		: 1;
			Core::RegisterValueType INITS		: 1;
			Core::RegisterValueType RSF			: 1;
			Core::RegisterValueType INITF		: 1;
			Core::RegisterValueType INIT		: 1;
			Core::RegisterValueType ALRAF		: 1;
			Core::RegisterValueType ALRBF		: 1;
			Core::RegisterValueType WUTF		: 1;
			Core::RegisterValueType TSF			: 1;
			Core::RegisterValueType TSOVF		: 1;
			Core::RegisterValueType TAMP1F		: 1;
			Core::RegisterValueType TAMP2F		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType RECALPF		: 1;
			Core::RegisterValueType 			: 15;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void WriteEnable(bool enable = true)
		{
			Register::Ptr()->INIT = enable;
		}
		
		static bool IsWriteAllowed()
		{
			return Register::Ptr()->INITF;
		}
	};
	
	// RTC write protection register (RTC_WPR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class WriteProtectionRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x24;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType KEY			: 8;
			Core::RegisterValueType 			: 24;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void WriteProtectionEnable(bool enable = true)
		{
			if (enable)
			{
				Register::Ptr()->KEY = 0;
			}
			else
			{
				Register::Ptr()->KEY = 0xCA;
				Register::Ptr()->KEY = 0x53;
			}
			
		}
	};
	
	template
		<
			unsigned int IdObj
		>
	class Rtc :
		public IdObjectBase<IdObj>,
		public TimeRegister<BaseAddress>,
		public DateRegister<BaseAddress>,
		public InitializationAndStatusRegister<BaseAddress>,
		public WriteProtectionRegister<BaseAddress>
	{
	public:
		typedef TimeRegister<BaseAddress> TimeRegister;
		typedef DateRegister<BaseAddress> DateRegister;
		typedef InitializationAndStatusRegister<BaseAddress> InitializationAndStatusRegister;
		typedef WriteProtectionRegister<BaseAddress> WriteProtectionRegister;
	public:
		static unsigned int GetTime()
		{
			struct tm *timeinfo = localtime(0);
		
			int year = DateRegister::GetYear();
			int month = DateRegister::GetMonth();
			int day = DateRegister::GetDay();
			
			int hours = TimeRegister::GetHours();
			int minutes = TimeRegister::GetMinutes();
			int seconds = TimeRegister::GetSeconds();
			
			timeinfo->tm_year = year;
			timeinfo->tm_mon = month - 1;
			timeinfo->tm_mday = day;
			timeinfo->tm_hour = hours;
			timeinfo->tm_min = minutes;
			timeinfo->tm_sec = seconds;
			timeinfo->tm_isdst = -1;
			
			time_t res = mktime(timeinfo);
			
			return res;
		}
		
		static void SetTime(unsigned int time)
		{
			WriteProtectionRegister::WriteProtectionEnable(false);
			InitializationAndStatusRegister::WriteEnable(true);
			
			unsigned int waitTimeOut = 1000;
			while(!InitializationAndStatusRegister::IsWriteAllowed() && waitTimeOut > 0)
			{
				waitTimeOut--;
			}
			
			struct tm *timeinfo = localtime(&time);
			/*
			DateRegister::SetYear(timeinfo->tm_year);
			DateRegister::SetMonth(timeinfo->tm_mon + 1);
			DateRegister::SetDay(timeinfo->tm_mday);*/
			
			DateRegister::SetDateValue(timeinfo->tm_year, timeinfo->tm_mon + 1, timeinfo->tm_mday);
			TimeRegister::SetTimeValue(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
			/*
			TimeRegister::SetHours(timeinfo->tm_hour);
			TimeRegister::SetMinutes(timeinfo->tm_min);
			TimeRegister::SetSeconds(timeinfo->tm_sec);
			*/
			
			InitializationAndStatusRegister::WriteEnable(false);
			WriteProtectionRegister::WriteProtectionEnable(true);
		}
	};
}

typedef RtcImplementation::Rtc<1> Rtc;

}
}
}

#endif