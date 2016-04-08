///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Лог тревог
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_ALARMLOG_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_ALARMLOG_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"
#include "ClearAlarmLogResultCode.h"
#include "ResultCode.h"
#include "../Storage/BitFlagsHolder.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace AlarmLog
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeGetLogInfo						= 1,
		MessageCodeLogInfo							= 2,
		MessageCodeClearAlarmLog					= 3,
		MessageCodeClearAlarmLogResult				= 4,
		MessageCodeReadAlarmLog						= 5,
		MessageCodeReadAlarmLogResult				= 6,
		MessageCodePushAlarmLog						= 7,
		MessageCodePushAlarmLogResult				= 8,
		MessageCodePushAlarmLogDone					= 9, // отправлены все части
		MessageCodeGenerateAlarmLog					= 10,
		MessageCodeGenerateAlarmLogResult			= 11,
		
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
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeAlarmLog, MessageFormat>
	{
	};
	
	// Входной формат данных функции Модем
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeAlarmLog, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeAlarmLog, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};

	class MessageLogInfo
	{
	public:
		unsigned int StartTime;

		unsigned int NumStringInLog;
		unsigned int FirstTime;
		unsigned int LastTime;
	protected:
		bool _valid;
	public:
		MessageLogInfo()
		{
			StartTime = 0;
			FirstTime = 0;
			LastTime = 0;
			NumStringInLog = 0;
		}
	
		MessageLogInfo(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(StartTime))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			StartTime = *(unsigned int *)&src[0];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(FirstTime) + sizeof(LastTime) + sizeof(NumStringInLog))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = NumStringInLog;
			dstIndex++;
			dstPtr[dstIndex] = (NumStringInLog >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (NumStringInLog >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (NumStringInLog >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = FirstTime;
			dstIndex++;
			dstPtr[dstIndex] = (FirstTime >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FirstTime >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FirstTime >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = LastTime;
			dstIndex++;
			dstPtr[dstIndex] = (LastTime >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (LastTime >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (LastTime >> 24) & 0xFF;
			dstIndex++;
			
			return dstIndex;
		}
	};

	class MessageClearAlarmLogResult
	{
	public:
		ClearAlarmLogResultCode Result;
	public:
		MessageClearAlarmLogResult()
		{
			Result = ClearAlarmLogResultError;
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
	
	class MessageReadAlarmLog
	{
	public:
		unsigned int StartTime;
		unsigned int StartOffset;
		unsigned int Size;
	protected:
		bool _valid;
	public:
		MessageReadAlarmLog()
		{
			StartTime = -1;
			StartOffset = -1;
			Size = 0;
		}
		
		MessageReadAlarmLog(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(StartTime) + sizeof(StartOffset) + sizeof(Size))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			StartTime = *(unsigned int *)&src[0];
			StartOffset = *(unsigned int *)&src[sizeof(StartTime)];
			Size = *(unsigned int *)&src[sizeof(StartTime) + sizeof(StartOffset)];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(StartTime) + sizeof(StartOffset) + sizeof(Size))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = StartTime;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = StartOffset;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = Size;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 24) & 0xFF;
			dstIndex++;
			
			return dstIndex;
		}
	};

	class MessageReadAlarmLogResult
	{
	public:
		ResultCode Result;
		unsigned int StartTime;
		unsigned int StartOffset;
		unsigned int Size;
		unsigned int SizeData;
		unsigned char *Data;
	public:
		MessageReadAlarmLogResult()
		{
			Result = ResultUnknown;
			StartTime = -1;
			StartOffset = -1;
			Size = 0;
			SizeData = 0;
			Data = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Result) + sizeof(StartTime) + sizeof(StartOffset) + sizeof(Size) + Size || (Data == 0 && Size > 0))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Result;
			dstIndex++;
			
			dstPtr[dstIndex] = StartTime;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = StartOffset;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = Size;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 24) & 0xFF;
			dstIndex++;
			
			for (int i = 0; i < SizeData; i++)
			{
				dstPtr[dstIndex] = Data[i];
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	class MessagePushAlarmLog
	{
	public:
		unsigned int PushAddress;
		unsigned int StartTime;
		unsigned int PartSize;
		BitFlagsHolder RequestedParts;
	protected:
		bool _valid;
	public:
		MessagePushAlarmLog()
		{
			PushAddress = -1;
			StartTime = 0;
			PartSize = 0;
		}
		
		MessagePushAlarmLog(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(PushAddress) + sizeof(StartTime) + sizeof(PartSize))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			PushAddress = *(unsigned int *)&src[0];
			StartTime = *(unsigned int *)&src[sizeof(PushAddress)];
			PartSize = *(unsigned int *)&src[sizeof(PushAddress) + sizeof(StartTime)];
			
			unsigned char filedsLen = sizeof(PushAddress) + sizeof(StartTime) + sizeof(PartSize);

			RequestedParts.Deserialize(src + filedsLen, dataCount - filedsLen);

			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(PushAddress) + sizeof(StartTime) + sizeof(PartSize) + RequestedParts.SerializedSize())
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = PushAddress;
			dstIndex++;
			dstPtr[dstIndex] = (PushAddress >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PushAddress >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PushAddress >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = StartTime;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = PartSize;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 24) & 0xFF;
			dstIndex++;
			
			dstIndex += RequestedParts.Serialize(dstPtr + dstIndex);

			return dstIndex;
		}
	};
	
	class MessagePushAlarmLogResult
	{
	public:
		ResultCode Result;
		unsigned int StartTime;
		unsigned int NumStringInLog;
		unsigned int PartSize;
		unsigned int PartsCount;
	public:
		MessagePushAlarmLogResult()
		{
			Result = ResultUnknown;
			StartTime = 0;
			NumStringInLog = 0;
			PartSize = 0;
			PartsCount = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Result) + sizeof(StartTime) + sizeof(NumStringInLog) + sizeof(PartSize) + sizeof(PartsCount))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Result;
			dstIndex++;
			
			dstPtr[dstIndex] = StartTime;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = NumStringInLog;
			dstIndex++;
			dstPtr[dstIndex] = (NumStringInLog >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (NumStringInLog >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (NumStringInLog >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = PartSize;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = PartsCount;
			dstIndex++;
			dstPtr[dstIndex] = (PartsCount >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartsCount >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartsCount >> 24) & 0xFF;
			dstIndex++;

			return dstIndex;
		}
	};

	class MessageGenerateAlarmLog
	{
	public:
		unsigned int StartTime;
		unsigned int CountString;
	protected:
		bool _valid;
	public:
		MessageGenerateAlarmLog()
		{
			StartTime = -1;
			CountString = -1;
		}
		
		MessageGenerateAlarmLog(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(StartTime) + sizeof(CountString))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			StartTime = *(unsigned int *)&src[0];
			CountString = *(unsigned int *)&src[sizeof(StartTime)];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(StartTime) + sizeof(CountString))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = StartTime;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartTime >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = CountString;
			dstIndex++;
			dstPtr[dstIndex] = (CountString >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (CountString >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (CountString >> 24) & 0xFF;
			dstIndex++;
			
			return dstIndex;
		}
	};

	class MessageGenerateAlarmLogResult
	{
	public:
		ResultCode Result;
	public:
		MessageGenerateAlarmLogResult()
		{
			Result = ResultUnknown;
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