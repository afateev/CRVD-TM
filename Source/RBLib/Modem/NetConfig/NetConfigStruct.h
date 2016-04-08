///////////////////////////////////////////////////////////////////////////////
//	Структура сетевой конфигурации
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_NETCONFIG_NETCONFIGSTRUCT_H
#define MODEM_NETCONFIG_NETCONFIGSTRUCT_H

#define NETCONFIG_DEFAULT_MOBILECOMPLEX
#define RBLIB_MODEM_INCLUDE_NETID_TO_PACKET_HEADER

namespace Rblib
{
namespace Modem
{
namespace NetConfig
{
#pragma pack(push, 1)

	struct NetConfigStructVer1
	{
		unsigned int Id;
		unsigned char FrequencyChannel;
		unsigned short MainTimeSlotCount;
		unsigned short AdditionalTimeSlotCount;
		unsigned short SyncPeriod;
		unsigned short SyncTimeout;
		unsigned short ActivityTableDistributionPeriod;
		
		NetConfigStructVer1()
		{
			Reset();
		}
		
		void Reset()
		{
#ifdef NETCONFIG_DEFAULT_MOBILECOMPLEX
			/*Id = 0x12345678;
			FrequencyChannel = 3;
			MainTimeSlotCount = 20;
			AdditionalTimeSlotCount = 10;
			SyncPeriod = 1 * 16;
			SyncTimeout = 1 * 5;
			ActivityTableDistributionPeriod = 2;*/
			Id = 0;
			FrequencyChannel = 0;
			MainTimeSlotCount = 20;
			AdditionalTimeSlotCount = 10;
			SyncPeriod = 10;
			SyncTimeout = 5;
			ActivityTableDistributionPeriod = 2;
			/*
			Id = 1;
			FrequencyChannel = 1;
			MainTimeSlotCount = 20;
			AdditionalTimeSlotCount = 10;
			SyncPeriod = 16;
			SyncTimeout = 5;
			ActivityTableDistributionPeriod = 2;*/
#else
			Id = 0;
			FrequencyChannel = 0;
			MainTimeSlotCount = 1;
			AdditionalTimeSlotCount = 0;
			SyncPeriod = 1;
			SyncTimeout = 1;
			ActivityTableDistributionPeriod = 1;
#endif
		}
		
		friend bool operator==(const NetConfigStructVer1& left, const NetConfigStructVer1& right)
		{
			bool res = true;
			res &= left.Id == right.Id;
			res &= left.FrequencyChannel == right.FrequencyChannel;
			res &= left.MainTimeSlotCount == right.MainTimeSlotCount;
			res &= left.AdditionalTimeSlotCount == right.AdditionalTimeSlotCount;
			res &= left.SyncPeriod == right.SyncPeriod;
			res &= left.SyncTimeout == right.SyncTimeout;
			res &= left.ActivityTableDistributionPeriod == right.ActivityTableDistributionPeriod;
			
			return res;
		}
		
		friend bool operator!=(const NetConfigStructVer1& left, const NetConfigStructVer1& right)
		{
			bool res = left == right;
			return !res;
		}
	};
	
	struct NetConfigStructVer2
	{
		unsigned int Id;
		unsigned char FrequencyChannel;
		unsigned short MainTimeSlotCount;
		unsigned short AdditionalTimeSlotCount;
		unsigned short SyncPeriod;
		unsigned short SyncTimeout;
		unsigned short ActivityTableDistributionPeriod;
		unsigned char AdditionalTimeSlotUsedCount;
		unsigned char AdditionalTimeSlotStart;
		
		NetConfigStructVer2()
		{
			Reset();
		}
		
		void Reset()
		{
#ifdef NETCONFIG_DEFAULT_MOBILECOMPLEX
			Id = 0;
			FrequencyChannel = 0;
			MainTimeSlotCount = 20;
			AdditionalTimeSlotCount = 10;
			SyncPeriod = 10;
			SyncTimeout = 5;
			ActivityTableDistributionPeriod = 2;
			AdditionalTimeSlotUsedCount = 0;
			AdditionalTimeSlotStart = 0;
#else
			Id = 0;
			FrequencyChannel = 0;
			MainTimeSlotCount = 1;
			AdditionalTimeSlotCount = 0;
			SyncPeriod = 1;
			SyncTimeout = 1;
			ActivityTableDistributionPeriod = 1;
			AdditionalTimeSlotUsedCount = 0;
			AdditionalTimeSlotStart = 0;
#endif
		}
		
		friend bool operator==(const NetConfigStructVer2& left, const NetConfigStructVer2& right)
		{
			bool res = true;
			res &= left.Id == right.Id;
			res &= left.FrequencyChannel == right.FrequencyChannel;
			res &= left.MainTimeSlotCount == right.MainTimeSlotCount;
			res &= left.AdditionalTimeSlotCount == right.AdditionalTimeSlotCount;
			res &= left.SyncPeriod == right.SyncPeriod;
			res &= left.SyncTimeout == right.SyncTimeout;
			res &= left.ActivityTableDistributionPeriod == right.ActivityTableDistributionPeriod;
			
			return res;
		}
		
		friend bool operator!=(const NetConfigStructVer2& left, const NetConfigStructVer2& right)
		{
			bool res = left == right;
			return !res;
		}
	};
	
	struct NetConfigStruct3
	{
		unsigned int Id;
		unsigned char FrequencyChannel;
		unsigned short MainTimeSlotCount;
		unsigned short AdditionalTimeSlotCount;
		unsigned short SyncPeriod;
		unsigned short SyncTimeout;
		unsigned short ActivityTableDistributionPeriod;
		unsigned char AdditionalTimeSlotUsedCount;
		unsigned char AdditionalTimeSlotStart;
		unsigned char RelayMode;
		
		NetConfigStruct3()
		{
			Reset();
		}
		
		void Reset()
		{
#ifdef NETCONFIG_DEFAULT_MOBILECOMPLEX
			Id = 0;
			FrequencyChannel = 0;
			MainTimeSlotCount = 20;
			AdditionalTimeSlotCount = 0;
			SyncPeriod = 10;
			SyncTimeout = 5;
			ActivityTableDistributionPeriod = 2;
			AdditionalTimeSlotUsedCount = 0;
			AdditionalTimeSlotStart = 0;
			RelayMode = 0;
#else
			Id = 0;
			FrequencyChannel = 0;
			MainTimeSlotCount = 1;
			AdditionalTimeSlotCount = 0;
			SyncPeriod = 1;
			SyncTimeout = 1;
			ActivityTableDistributionPeriod = 1;
			AdditionalTimeSlotUsedCount = 0;
			AdditionalTimeSlotStart = 0;
			RelayMode = 0;
#endif
		}
		
		friend bool operator==(const NetConfigStruct3& left, const NetConfigStruct3& right)
		{
			bool res = true;
			res &= left.Id == right.Id;
			res &= left.FrequencyChannel == right.FrequencyChannel;
			res &= left.MainTimeSlotCount == right.MainTimeSlotCount;
			res &= left.AdditionalTimeSlotCount == right.AdditionalTimeSlotCount;
			res &= left.SyncPeriod == right.SyncPeriod;
			res &= left.SyncTimeout == right.SyncTimeout;
			res &= left.ActivityTableDistributionPeriod == right.ActivityTableDistributionPeriod;
			
			return res;
		}
		
		friend bool operator!=(const NetConfigStruct3& left, const NetConfigStruct3& right)
		{
			bool res = left == right;
			return !res;
		}
	};
	
	struct NetConfigStruct
	{
#ifdef RADIODOZOR
		// включить фильтрацию по идентификатору сети
		static const bool UseIdFilter = true;
		// должно быть также включено RBLIB_MODEM_INCLUDE_NETID_TO_PACKET_HEADER
#else
		static const bool UseIdFilter = false;
#endif
		
		unsigned int Id;
		unsigned char FrequencyChannel;
		unsigned short MainTimeSlotCount;
		unsigned short AdditionalTimeSlotCount;
		unsigned short SyncPeriod;
		unsigned short SyncTimeout;
		unsigned short ActivityTableDistributionPeriod;
		unsigned char AdditionalTimeSlotUsedCount;
		unsigned char AdditionalTimeSlotStart;
		unsigned char RelayMode;
		unsigned char PacketRepeatCount;
		unsigned int RadioListetingInterval;
		
		NetConfigStruct()
		{
			Reset();
		}
		
		void Reset()
		{
#ifdef NETCONFIG_DEFAULT_MOBILECOMPLEX
			Id = 0;
			FrequencyChannel = 0;
			MainTimeSlotCount = 12;
			AdditionalTimeSlotCount = 5;
			SyncPeriod = 10;
			SyncTimeout = 5;
			ActivityTableDistributionPeriod = 2;
			AdditionalTimeSlotUsedCount = 0;
			AdditionalTimeSlotStart = 0;
			RelayMode = 0;
			PacketRepeatCount = 10;
			RadioListetingInterval = 10;
#else
			Id = 0;
			FrequencyChannel = 0;
			MainTimeSlotCount = 1;
			AdditionalTimeSlotCount = 0;
			SyncPeriod = 1;
			SyncTimeout = 1;
			ActivityTableDistributionPeriod = 1;
			AdditionalTimeSlotUsedCount = 0;
			AdditionalTimeSlotStart = 0;
			RelayMode = 0;
			PacketRepeatCount = 10;
			RadioListetingInterval = 10;
#endif
		}
		
		friend bool operator==(const NetConfigStruct& left, const NetConfigStruct& right)
		{
			bool res = true;
			res &= left.Id == right.Id;
			res &= left.FrequencyChannel == right.FrequencyChannel;
			res &= left.MainTimeSlotCount == right.MainTimeSlotCount;
			res &= left.AdditionalTimeSlotCount == right.AdditionalTimeSlotCount;
			res &= left.SyncPeriod == right.SyncPeriod;
			res &= left.SyncTimeout == right.SyncTimeout;
			res &= left.ActivityTableDistributionPeriod == right.ActivityTableDistributionPeriod;
			
			return res;
		}
		
		friend bool operator!=(const NetConfigStruct& left, const NetConfigStruct& right)
		{
			bool res = left == right;
			return !res;
		}
	};

#pragma pack (pop)
}
}
}

#endif