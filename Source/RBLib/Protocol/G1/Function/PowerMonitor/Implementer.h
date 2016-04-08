///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройства
//	Монитор питания
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_POWERMONITOR_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_POWERMONITOR_IMPLEMENTER_H

#include "FunctionData.h"
#include "EventCode.h"
#include "../../../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace PowerMonitor
{
	template
		<
			int eventId,								// код событий монитора питания
			int maxDataSize,							// максимальный размер полезных пользовательских данных
			int maxStateCount,							// максимальное число кодов состояний
			int maxParamsCount							// максимальное число параметров
		>
	class Implementer
	{
	public:
		static const int EventId = eventId;
		static const int MaxDataSize = maxDataSize;
		static const int MaxStateCount = maxStateCount;
		static const int MaxParamsCount = maxParamsCount;
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
		typedef CallbackWrapper<int, StateCode *, StateCode *, StateValueType *, int, int &> GetStateCallbackType;
		typedef CallbackWrapper<int, ParamCode *, ParamCode *, ParamValueType *, int, int &> GetParamsCallbackType;
		typedef CallbackWrapper<int, ParamCode *, ParamValueType *, bool *, int, int &> SetParamsCallbackType;
	protected:
		unsigned char _messageData[MaxDataSize + 50];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		GetStateCallbackType GetStateCallback;
		GetParamsCallbackType GetParamsCallback;
		SetParamsCallbackType SetParamsCallback;
	public:
		Implementer()
		{
			Singletone<EventDispetcher>::Instance().AddHandler(EventId, EventDispetcher::EventFunctor(*this), false);
		}
		
		// Обработчик событий
		void operator()(const Event &event)
		{
			switch (event.Code)
			{
			case EventId:
				{
					EventCode eventCode = (EventCode)event.Param1;
					switch(eventCode)
					{
					case EventCodeParamsChanged:
						{
							ParamCode ids[MaxParamsCount];
							ParamValueType values[MaxParamsCount];
							int actualCount = 0;
							GetParamsCallback.Call(0, 0, ids, values, MaxParamsCount, actualCount);
								
							MessageParams messageParams;
							messageParams.Count = actualCount;
							messageParams.Ids = ids;
							messageParams.Values = values;
							int messageDataLen = messageParams.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeParamsChanged, _messageData, messageDataLen);
						}
						break;
					}
					
				}
				break;
			}
		}
		
		// Отправить сообщение от имени функции
		void FromFunction(MessageCode code, void *data, int dataCount)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionCallback.Call(Device::FunctionCodePowerMonitor, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodePowerMonitor, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodePowerMonitor, code, _functionData, functionDataLen, enqueued, packetId);
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
					case MessageCodeGetState:
						{
							MessageGetState message(command.Data.Data, command.Data.DataLen);
							
							if (message.Valid())
							{
								StateCode ids[MaxStateCount];
								StateValueType values[MaxStateCount];
								int actualCount = 0;
								GetStateCallback.Call(message.Count, message.Ids, ids, values, MaxStateCount, actualCount);
								
								MessageState messageState;
								messageState.Count = actualCount;
								messageState.Ids = ids;
								messageState.Values = values;
								int messageDataLen = messageState.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeState, _messageData, messageDataLen);
							}
						}
						break;
					case MessageCodeGetParams:
						{
							MessageGetParams message(command.Data.Data, command.Data.DataLen);
							
							if (message.Valid())
							{
								ParamCode ids[MaxParamsCount];
								ParamValueType values[MaxParamsCount];
								int actualCount = 0;
								GetParamsCallback.Call(message.Count, message.Ids, ids, values, MaxParamsCount, actualCount);
								
								MessageParams messageParams;
								messageParams.Count = actualCount;
								messageParams.Ids = ids;
								messageParams.Values = values;
								int messageDataLen = messageParams.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeParams, _messageData, messageDataLen);
							}
						}
						break;
					case MessageCodeSetParams:
						{
							MessageSetParams message(command.Data.Data, command.Data.DataLen);
							
							if (message.Valid())
							{
								bool setResults[MaxParamsCount];
								int actualCount = 0;
								SetParamsCallback.Call(message.Count, message.Ids, message.Values, setResults, MaxParamsCount, actualCount);
								
								MessageSetParamsResult messageSetParamsResult;
								messageSetParamsResult.Count = actualCount;
								messageSetParamsResult.Ids = message.Ids;
								messageSetParamsResult.Results = setResults;
								int messageDataLen = messageSetParamsResult.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeSetParamsResult, _messageData, messageDataLen);
							}
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