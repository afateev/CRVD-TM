///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Диагостика
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_DIAGNOSTICS_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_DIAGNOSTICS_IMPLEMENTER_H

#include "FunctionData.h"
#include "../../../../EventDispetcher.h"
#include "../../../../Utils/Singletone.h"
#include "../../../../Utils/CallbackWrapper.h"
#include "../../../../Utils/CallbackListWrapper.h"
#include "EventCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Diagnostics
{
	template
		<
			int eventId,								// код событий модема
			int maxDataSize								// максимальный размер полезных пользовательских данных
		>
	class Implementer
	{
	public:
		static const int EventId = eventId;
		static const int MaxDataSize = maxDataSize;
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
		typedef CallbackWrapper<int, DiagnosticCode *, DiagnosticCode *, DiagnosticValueType *, int, int &> GetStateCallbackType;
		typedef CallbackWrapper<int, DiagnosticCode *, DiagnosticCode *, DiagnosticValueType *, int, int &> GetSavedStateCallbackType;
		typedef CallbackWrapper<bool &> ResetSavedStateCallbackType;
	protected:
		unsigned char _messageData[MaxDataSize + 20];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		GetStateCallbackType GetStateCallback;
		GetSavedStateCallbackType GetSavedStateCallback;
		ResetSavedStateCallbackType ResetSavedStateCallback;
	public:
		Implementer()
		{
			Reset();
			Singletone<EventDispetcher>::Instance().AddHandler(EventId, EventDispetcher::EventFunctor(*this), false);
		}
		
		void Reset()
		{
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
					case EventCodeDeviceStateChanged:
						{
							_messageData[0] = event.Param2;
							
							FromFunction(MessageCodeDeviceStateChanged, _messageData, 1);
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
			
			FromFunctionCallback.Call(Device::FunctionCodeDiagnostics, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeDiagnostics, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeDiagnostics, code, _functionData, functionDataLen, enqueued, packetId);
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
								DiagnosticCode codes[MaxDiagnosticCodesCount];
								DiagnosticValueType values[MaxDiagnosticCodesCount];
								int actualCount = 0;
								GetStateCallback.Call(message.Count, message.Codes, codes, values, MaxDiagnosticCodesCount, actualCount);
								
								MessageState messageState;
								messageState.Count = actualCount;
								messageState.Codes = codes;
								messageState.Values = values;
								int messageDataLen = messageState.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeState, _messageData, messageDataLen);
							}
						}
						break;
					case MessageCodeGetSavedState:
						{
							MessageGetSavedState message(command.Data.Data, command.Data.DataLen);
							
							if (message.Valid())
							{
								DiagnosticCode codes[MaxDiagnosticCodesCount];
								DiagnosticValueType values[MaxDiagnosticCodesCount];
								int actualCount = 0;
								GetSavedStateCallback.Call(message.Count, message.Codes, codes, values, MaxDiagnosticCodesCount, actualCount);
								
								MessageSavedState messageSavedState;
								messageSavedState.Count = actualCount;
								messageSavedState.Codes = codes;
								messageSavedState.Values = values;
								int messageDataLen = messageSavedState.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeSavedState, _messageData, messageDataLen);
							}
						}
						break;
					case MessageCodeResetSavedState:
						{
							bool result = false;
							ResetSavedStateCallback.Call(result);
							
							_messageData[0] = (unsigned char)result;
							
							FromFunction(MessageCodeResetSavedStateResult, _messageData, 1);
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