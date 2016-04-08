///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Часы
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_CLOCK_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_CLOCK_IMPLEMENTER_H

#include "FunctionData.h"
#include "../../../../Utils/Singletone.h"
#include "../../../../Utils/CallbackWrapper.h"
#include "../../../../Utils/CallbackListWrapper.h"

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
	template
		<
			int maxDataSize								// максимальный размер полезных пользовательских данных
		>
	class Implementer
	{
	public:
		static const int MaxDataSize = maxDataSize;
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
		typedef CallbackWrapper<int &, int &, int &, int &, int &, int &> GetDateTimeCallbackType;
		typedef CallbackWrapper<int, int, int, int, int, int> SetDateTimeCallbackType;
	protected:
		unsigned char _messageData[MaxDataSize + 20];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		
		GetDateTimeCallbackType GetDateTimeCallback;
		SetDateTimeCallbackType SetDateTimeCallback;
	public:
		Implementer()
		{
			Reset();
		}
		
		void Reset()
		{
		}
		
		// Отправить сообщение от имени функции
		void FromFunction(MessageCode code, void *data, int dataCount)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionCallback.Call(Device::FunctionCodeClock, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeClock, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeClock, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		void ToFunction(unsigned char *data, unsigned int dataCount, bool wirelessRequest, int requesterId)
		{
			InputMessageFormat command(data, dataCount);
			if (command.Valid())
			{
				if (command.Data.Valid())
				{
					switch(command.Data.Code)
					{
					case MessageCodeGetDateTime:
						{
							int year = 1970;
							int month = 1;
							int dayOfMonth = 1;
							int hour = 0;
							int minute = 0;
							int second = 0;
							
							GetDateTimeCallback.Call(year, month, dayOfMonth, hour, minute, second);
							
							MessageDateTime messageDateTime;
							messageDateTime.Year = year;
							messageDateTime.Month = month;
							messageDateTime.DayOfMonth = dayOfMonth;
							messageDateTime.Hour = hour;
							messageDateTime.Minute = minute;
							messageDateTime.Second = second;
							
							int messageDataLen = messageDateTime.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeDateTime, _messageData, messageDataLen);
						}
						break;
					case MessageCodeSetDateTime:
						{
							MessageDateTime message(command.Data.Data, command.Data.DataLen);
							
							MessageSetDateTimeResult messageSetDateTimeResult;
							messageSetDateTimeResult.Result = ResultUnknown;
							
							if (message.Valid())
							{
								unsigned int year = message.Year;
								unsigned int month = message.Month;
								unsigned int dayOfMonth = message.DayOfMonth;
								unsigned int hour = message.Hour;
								unsigned int minute = message.Minute;
								unsigned int second = message.Second;
								
								SetDateTimeCallback.Call(year, month, dayOfMonth, hour, minute, second);
								
								messageSetDateTimeResult.Result = ResultOk;
							}
							else
							{
								messageSetDateTimeResult.Result = ResultInvalidRequest;
							}
							
							int messageDataLen = messageSetDateTimeResult.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeSetDateTimeResult, _messageData, messageDataLen);
						}
						break;
					}
				}
			}
		}
		
		static void ToFunction(void *objectPtr, unsigned char *data, unsigned int dataCount, bool wirelessRequest, int requesterId)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Implementer *im = (Implementer *)objectPtr;
			im->ToFunction(data, dataCount, wirelessRequest, requesterId);
		}
	};
}
}
}
}
}

#endif