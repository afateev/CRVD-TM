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
	};
	
	template
		<
			unsigned int IdObj
		>
	class Rtc :
		public IdObjectBase<IdObj>,
		public TimeRegister<BaseAddress>,
		public DateRegister<BaseAddress>
	{
	public:
		typedef TimeRegister<BaseAddress> TimeRegister;
		typedef DateRegister<BaseAddress> DateRegister;
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
			
		}
	};
}

typedef RtcImplementation::Rtc<1> Rtc;

}
}
}

#endif