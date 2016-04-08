///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Служба действий
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_ACTIONSERVICE_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_ACTIONSERVICE_IMPLEMENTER_H

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
namespace ActionService
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
		typedef MessageDoAction::ReasonCodeType ReasonCode;
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char **, int &> GetSlaveListCallbackType;
		typedef CallbackWrapper<unsigned char *, int, bool &> AddSlaveCallbackType;
		typedef CallbackWrapper<unsigned char *, int, bool &> RemoveSlaveCallbackType;
		typedef CallbackWrapper<int, ReasonCode, unsigned int> ActionInitiatedCallbackType;
	protected:
		unsigned char _messageData[MaxDataSize + 20];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		GetSlaveListCallbackType GetSlaveListCallback;
		AddSlaveCallbackType AddSlaveCallback;
		RemoveSlaveCallbackType RemoveSlaveCallback;
		ActionInitiatedCallbackType OnActionInitiatedCallback;
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
					case EventCodeSlaveListChanged:
						{
							unsigned char *list = 0;
							int count = 0;
							GetSlaveListCallback.Call(&list, count);
							
							MessageSlaveList messageSlaveList;
							messageSlaveList.Count = count;
							messageSlaveList.List = list;
							
							int messageDataLen = messageSlaveList.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeSlaveListChanged, _messageData, messageDataLen);
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
			
			FromFunctionCallback.Call(Device::FunctionCodeActionService, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeActionService, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeActionService, code, _functionData, functionDataLen, enqueued, packetId);
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
					case MessageCodeGetSlaveList:
						{
							unsigned char *list = 0;
							int count = 0;
							GetSlaveListCallback.Call(&list, count);
							
							MessageSlaveList messageSlaveList;
							messageSlaveList.Count = count;
							messageSlaveList.List = list;
							
							int messageDataLen = messageSlaveList.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeSlaveList, _messageData, messageDataLen);
						}
						break;
					case MessageCodeAddSlave:
						{
							MessageAddSlave message(command.Data.Data, command.Data.DataLen);
							
							bool result = false;
							
							if (message.Valid())
							{
								unsigned char *list = message.List;
								int count = message.Count;
								AddSlaveCallback.Call(list, count, result);
							}
							
							_messageData[0] = (unsigned char)result;
							
							FromFunction(MessageCodeAddSlaveResult, _messageData, 1);
						}
						break;
					case MessageCodeRemoveSlave:
						{
							MessageRemoveSlave message(command.Data.Data, command.Data.DataLen);
							
							bool result = false;
							
							if (message.Valid())
							{
								unsigned char *list = message.List;
								int count = message.Count;
								RemoveSlaveCallback.Call(list, count, result);
							}
							
							_messageData[0] = (unsigned char)result;
							
							FromFunction(MessageCodeRemoveSlaveResult, _messageData, 1);
						}
						break;
					case MessageCodeDoAction:
						{
							MessageDoAction messageDoAction(command.Data.Data, command.Data.DataLen);
							if (messageDoAction.Valid())
							{
								// входящая команды "Выполнить действие"
								OnActionInitiatedCallback.Call(messageDoAction.InitiatorId, messageDoAction.ReasonCode, messageDoAction.ReasonId);
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
		
		// Исходящая команды "Выполнить действие"
		void SendDoAction(int node, int initiatorId, ReasonCode reasonCode, unsigned int reasonId, bool & enqueued, int & packetId)
		{
			MessageDoAction messageDoAction;

			messageDoAction.InitiatorId = initiatorId;
			messageDoAction.ReasonCode = reasonCode;
			messageDoAction.ReasonId = reasonId;
			
			int messageDataLen = messageDoAction.ToByteArray(_messageData, sizeof(_messageData));

			ToFunctionToNode(node, MessageCodeDoAction, _messageData, messageDataLen, enqueued, packetId);
		}
		
		static void SendDoAction(void *callbackParam, int node, int initiatorId, ReasonCode reasonCode, unsigned int reasonId, bool & enqueued, int & packetId)
		{
			Implementer *im = (Implementer *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->SendDoAction(node, initiatorId, reasonCode, reasonId, enqueued, packetId);
		}
	};
}
}
}
}
}

#endif