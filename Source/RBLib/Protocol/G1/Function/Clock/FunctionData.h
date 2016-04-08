///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Часы
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_CLOCK_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_CLOCK_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"
#include "ResultCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Clock
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeGetDateTime						= 1,
		MessageCodeDateTime							= 2,
		MessageCodeSetDateTime						= 3,
		MessageCodeSetDateTimeResult				= 4,
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
	
	// Выходной формат данных функции Модем
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeClock, MessageFormat>
	{
	};
	
	// Входной формат данных функции Модем
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeClock, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeClock, MessageFormat> Base;
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

	class MessageSetDateTimeResult
	{
	public:
		unsigned char Result;
	public:
		MessageSetDateTimeResult()
		{
			Result = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Result))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Result;
			dstIndex++;
			
			return dstIndex;
		}
	};
	
}
}
}
}
}

#endif