///////////////////////////////////////////////////////////////////////////////
//	������ ������ ��� ������ � �������� ����������
//	Location
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_LOCATION_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_LOCATION_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Location
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeGetState							= 1,
		MessageCodeState							= 2,
		MessageCodeSetState							= 3,
		MessageCodeSetStateResult					= 4,
		MessageCodeStateChanged						= 5,
		MessageCodeGetLocation 						= 6,
		MessageCodeLocation							= 7,
		MessageCodeGetSatelliteInfo					= 8,
		MessageCodeSatelliteInfo					= 9,
		MessageCodeGetDateTime						= 10,
		MessageCodeDateTime							= 11
	};
	
	class MessageFormat
	{
	public:
		MessageCode Code;
		unsigned short DataLen;
		void *Data;
	protected:
		bool _valid;
	public:
		MessageFormat()
		{
			Code = MessageCodeInvalid;
			DataLen = 0;
			Data = 0;
			_valid = true;
		}
		
		int Size()
		{
			return sizeof(Code) + sizeof(DataLen) + sizeof(unsigned char) * DataLen;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < (3 + DataLen))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			unsigned char *dataPtr = (unsigned char *)Data;
			
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Code;
			dstIndex++;
			
			if (DataLen > 0 && Data != 0)
			{
				dstPtr[dstIndex] = DataLen;
				dstIndex++;
				dstPtr[dstIndex] = (DataLen >> 8) & 0xFF;
				dstIndex++;
				
				for (int i = 0; i < DataLen; i ++)
				{
					dstPtr[dstIndex] = dataPtr[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
		
		void FromByteArray(unsigned char *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 3)
			{
				return;
			}
			
			Code = (MessageCode) data[0];
			DataLen = data[1];
			DataLen += data[2] << 8;
			
			if (dataCount < Size())
			{
				return;
			}
			
			Data = &data[3];
			
			_valid = true;
		}
	};
	
	// �������� ������ ������ �������
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeLocation, MessageFormat>
	{
	};
	
	// ������� ������ ������ �������
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeLocation, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeLocation, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	class MessageDateTime
	{
	public:
		unsigned int Year;
		unsigned int Month;
		unsigned int DayOfMonth;
		unsigned int Hour;
		unsigned int Minute;
		unsigned int Second;
	protected:
		bool _valid;
	public:
		MessageDateTime(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(Year) + sizeof(Month) + sizeof(DayOfMonth) + sizeof(Hour) + sizeof(Minute) + sizeof(Second))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Year = *(unsigned int *)&src[0];
			Month = *(unsigned int *)&src[sizeof(Year)];
			DayOfMonth = *(unsigned int *)&src[sizeof(Year) + sizeof(Month)];
			Hour = *(unsigned int *)&src[sizeof(Year) + sizeof(Month) + sizeof(DayOfMonth)];
			Minute = *(unsigned int *)&src[sizeof(Year) + sizeof(Month) + sizeof(DayOfMonth) + sizeof(Hour)];
			Second = *(unsigned int *)&src[sizeof(Year) + sizeof(Month) + sizeof(DayOfMonth) + sizeof(Hour) + sizeof(Minute)];
			
			if((Year < 2000 || Year > 2399) || (Month < 1 || Month > 12) || (DayOfMonth < 1 || DayOfMonth > 31) || (Hour > 23) || (Minute > 59) || (Second > 59))
			{
				return;
			}

			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}

		MessageDateTime()
		{
			Year = 1970;
			Month = 1;
			DayOfMonth = 1;
			Hour = 0;
			Minute = 0;
			Second = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Year) + sizeof(Month) + sizeof(DayOfMonth) + sizeof(Hour) + sizeof(Minute) + sizeof(Second))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Year;
			dstIndex++;
			dstPtr[dstIndex] = (Year >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Year >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Year >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = Month;
			dstIndex++;
			dstPtr[dstIndex] = (Month >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Month >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Month >> 24) & 0xFF;
			dstIndex++;

			dstPtr[dstIndex] = DayOfMonth;
			dstIndex++;
			dstPtr[dstIndex] = (DayOfMonth >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (DayOfMonth >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (DayOfMonth >> 24) & 0xFF;
			dstIndex++;

			dstPtr[dstIndex] = Hour;
			dstIndex++;
			dstPtr[dstIndex] = (Hour >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Hour >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Hour >> 24) & 0xFF;
			dstIndex++;

			dstPtr[dstIndex] = Minute;
			dstIndex++;
			dstPtr[dstIndex] = (Minute >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Minute >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Minute >> 24) & 0xFF;
			dstIndex++;

			dstPtr[dstIndex] = Second;
			dstIndex++;
			dstPtr[dstIndex] = (Second >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Second >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Second >> 24) & 0xFF;
			dstIndex++;

			return dstIndex;
		}
	};
	
	class MessageLocation
	{
	public:
		float Latitude;
		float Longitude;
		float Altitude;
		bool Active;
	protected:
		bool _valid;
	public:
		MessageLocation()
		{
			Latitude  = 0.0;
			Longitude = 0.0;
			Altitude  = 0.0;
			Active = false;
		}
		
		MessageLocation(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(Latitude) + sizeof(Longitude) + sizeof(Altitude) + sizeof(Active))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Latitude  = *(float *)&src[0];
			Longitude = *(float *)&src[sizeof(Latitude)];
			Altitude  = *(float *)&src[sizeof(Latitude) + sizeof(Longitude)];
			Active    = *(bool  *)&src[sizeof(Latitude) + sizeof(Longitude) + sizeof(Altitude)];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Latitude) + sizeof(Longitude) + sizeof(Altitude) + sizeof(Active))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			unsigned char *lat = (unsigned char *)&Latitude;
			dstPtr[dstIndex] = lat[0];
			dstIndex++;
			dstPtr[dstIndex] = lat[1];
			dstIndex++;
			dstPtr[dstIndex] = lat[2];
			dstIndex++;
			dstPtr[dstIndex] = lat[3];
			dstIndex++;
			
			unsigned char *lon = (unsigned char *)&Longitude;
			dstPtr[dstIndex] = lon[0];
			dstIndex++;
			dstPtr[dstIndex] = lon[1];
			dstIndex++;
			dstPtr[dstIndex] = lon[2];
			dstIndex++;
			dstPtr[dstIndex] = lon[3];
			dstIndex++;
			
			unsigned char *alt = (unsigned char *)&Altitude;
			dstPtr[dstIndex] = alt[0];
			dstIndex++;
			dstPtr[dstIndex] = alt[1];
			dstIndex++;
			dstPtr[dstIndex] = alt[2];
			dstIndex++;
			dstPtr[dstIndex] = alt[3];
			dstIndex++;
			
			dstPtr[dstIndex] = Active;
			dstIndex++;
			
			return dstIndex;
		}
	};
	
	class MessageSatelliteInfo
	{
	public:
		unsigned int SatelliteVisibleCount;
		unsigned int SatelliteUsedCount;
	protected:
		bool _valid;
	public:
		MessageSatelliteInfo()
		{
			SatelliteVisibleCount  = 0;
			SatelliteUsedCount = 0;
		}
		
		MessageSatelliteInfo(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(SatelliteVisibleCount) + sizeof(SatelliteUsedCount))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			SatelliteVisibleCount  = *(unsigned int *)&src[0];
			SatelliteUsedCount = *(unsigned int *)&src[sizeof(SatelliteVisibleCount)];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(SatelliteVisibleCount) + sizeof(SatelliteUsedCount))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			

			dstPtr[dstIndex] = SatelliteVisibleCount;
			dstIndex++;
			dstPtr[dstIndex] = (SatelliteVisibleCount >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (SatelliteVisibleCount >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (SatelliteVisibleCount >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = SatelliteUsedCount;
			dstIndex++;
			dstPtr[dstIndex] = (SatelliteUsedCount >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (SatelliteUsedCount >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (SatelliteUsedCount >> 24) & 0xFF;
			dstIndex++;
			
			return dstIndex;
		}
	};
	
	class MessageState
	{
	public:
		bool State;
	protected:
		bool _valid;
	public:
		MessageState()
		{
			State = false;
		}
		
		MessageState(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(State))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			State  = *(unsigned int *)&src[0];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(State))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = State;
			dstIndex++;
			
			return dstIndex;
		}
	};
	
	class MessageSetState
	{
	public:
		bool State;
	protected:
		bool _valid;
	public:
		MessageSetState(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 1)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			if (dataCount < sizeof(State))
			{
				return;
			}
			
			State = src[0];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
}
}
}
}
}

#endif