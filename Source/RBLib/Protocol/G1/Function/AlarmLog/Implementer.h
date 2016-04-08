///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Лог тревог
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_ALARMLOG_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_ALARMLOG_IMPLEMENTER_H

#include "FunctionData.h"
#include "EventCode.h"
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
namespace AlarmLog
{
	struct PushFileParams
	{
		unsigned int DestinationNodeId;
		unsigned int FileId;	// StartTime
		unsigned int FileSize; //NumStringInLog возвращается
		unsigned int PartSize;
		unsigned int PartsCount; // возвращается
		bool Started; // возвращается
		bool Busy; // возвращается
		BitFlagsHolder RequestedParts;
		bool RequestedPartsUsed;	// запрашивались ли конкретные части
		bool WirelessRequest; // Признак запроса по радио
		int RequesterId; // иденетификатор запросившего узла
		bool DelayResponse; // возвращается: true - ответ будет выдан позже, false - возвращаемые поля заполнены и ответ можно отправлять
	public:
		PushFileParams()
		{
			Reset();
		}
		
		void Reset()
		{
			DestinationNodeId = -1;
			FileId = -1;
			FileSize = 0;
			PartSize = 0;
			PartsCount = 0;
			Started = false;
			Busy = false;
			RequestedParts.Clear();
			RequestedPartsUsed = false;
			WirelessRequest = false;
			RequesterId = -1;
			DelayResponse = false;
		}
	};

	template
		<
			int eventId,								// код событий
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
		typedef CallbackWrapper<unsigned int, unsigned int, unsigned int, unsigned char * &, bool &> ReadAlarmLogCallbackType;	// AlarmLog Implementer
		typedef CallbackWrapper<PushFileParams &> PushAlarmLogCallbackType;	// AlarmLog Implementer
		
		typedef CallbackWrapper<unsigned int, unsigned int &, unsigned int &, unsigned int &, unsigned int &> GetLogInfoCallbackType;
		typedef CallbackWrapper<unsigned int, unsigned int> GenerateAlarmLogCallbackType;
		typedef CallbackWrapper<bool &> ClearAlarmLogCallbackType;
	protected:
		unsigned char _messageData[MaxDataSize + 50];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		
		ReadAlarmLogCallbackType ReadAlarmLogCallback;	// AlarmLog Implementer
		PushAlarmLogCallbackType PushAlarmLogCallback;	// AlarmLog Implementer

		GetLogInfoCallbackType GetLogInfoCallback;
		ClearAlarmLogCallbackType ClearAlarmLogCallback;
	
		GenerateAlarmLogCallbackType GenerateAlarmLogCallback;
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
			
			FromFunctionCallback.Call(Device::FunctionCodeAlarmLog, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeAlarmLog, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeAlarmLog, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// отправить часть "лога тревог" либо конкретному узлу, либо стандартным способом в зависимости от параметра wireless
		void SendReadAlarmLogResult(bool wireless, unsigned char node, unsigned int startTime, unsigned int startOffset, unsigned char *data, unsigned int size, bool& enqueued, int& packetId)//s
		{
			MessageReadAlarmLogResult messageReadAlarmLogResult;
			messageReadAlarmLogResult.Result = ResultOk;
			messageReadAlarmLogResult.StartTime = startTime;
			messageReadAlarmLogResult.StartOffset = startOffset;
			messageReadAlarmLogResult.Size = size;
			messageReadAlarmLogResult.SizeData = size * 17;
			messageReadAlarmLogResult.Data = data;

			int messageDataLen = messageReadAlarmLogResult.ToByteArray(_messageData, sizeof(_messageData));

			if (wireless)
			{
				FromFunctionToNode(node, MessageCodeReadAlarmLogResult, _messageData, messageDataLen, enqueued, packetId);
			}
			else
			{
				FromFunction(MessageCodeReadAlarmLogResult, _messageData, messageDataLen);
			}
		}
		// отправить результат выполнения PushFile
		void SendPushAlarmLogResult(PushFileParams pushFileParams)
		{
			MessagePushAlarmLogResult messagePushAlarmLogResult;
			messagePushAlarmLogResult.Result = pushFileParams.Started ? ResultOk : ResultAlarmLogNotExist;
			
			if (pushFileParams.Busy)
			{
				messagePushAlarmLogResult.Result = ResultBusy;
			}
			
			messagePushAlarmLogResult.StartTime = pushFileParams.FileId;//StartTime;
			messagePushAlarmLogResult.NumStringInLog = pushFileParams.FileSize; //NumStringInLog;
			messagePushAlarmLogResult.PartSize = pushFileParams.PartSize;
			messagePushAlarmLogResult.PartsCount = pushFileParams.PartsCount;
			
			int messageDataLen = messagePushAlarmLogResult.ToByteArray(_messageData, sizeof(_messageData));
			
			if (pushFileParams.WirelessRequest)
			{
				bool enqueued = false;
				int packetId = -1;
				FromFunctionToNode(pushFileParams.RequesterId, MessageCodePushAlarmLogResult, _messageData, messageDataLen, enqueued, packetId);
			}
			else
			{
				FromFunction(MessageCodePushAlarmLogResult, _messageData, messageDataLen);
			}
		}

		void SendPushDoneNotification(bool wireless, unsigned char node, unsigned int fileId, bool& enqueued, int& packetId)
		{
			if (wireless)
			{
				FromFunctionToNode(node, MessageCodePushAlarmLogDone, &fileId, sizeof(unsigned int), enqueued, packetId);
			}
			else
			{
				FromFunction(MessageCodePushAlarmLogDone, &fileId, sizeof(unsigned int));
			}
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
					case MessageCodeGetLogInfo:
						{
							MessageLogInfo message(command.Data.Data, command.Data.DataLen);
							
							MessageLogInfo messageLogInfo;
							
							if (message.Valid())
							{
								unsigned int firstTime = 0;
								unsigned int lastTime = 0;
								unsigned int numStringInLog = 0;
								
								unsigned int startTime =  message.StartTime;
								unsigned int numStringInLogFirstElement = 0;
								GetLogInfoCallback.Call(startTime, firstTime, lastTime, numStringInLog, numStringInLogFirstElement);
								
								messageLogInfo.FirstTime = firstTime;
								messageLogInfo.LastTime = lastTime;
								messageLogInfo.NumStringInLog = numStringInLog;
							}
							int messageDataLen = messageLogInfo.ToByteArray(_messageData, sizeof(_messageData));

							FromFunction(MessageCodeLogInfo, _messageData, messageDataLen);
						}
						break;
					case MessageCodeClearAlarmLog:
						{
							bool result = false;
							ClearAlarmLogCallback.Call(result);
							
							MessageClearAlarmLogResult messageClearAlarmLogResult;
							messageClearAlarmLogResult.Result = result ? ClearAlarmLogResultOk : ClearAlarmLogResultError;
							int messageDataLen = messageClearAlarmLogResult.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeClearAlarmLogResult, _messageData, messageDataLen);
						}
						break;
					case MessageCodeReadAlarmLog:
						{
							MessageReadAlarmLog message(command.Data.Data, command.Data.DataLen);
							
							MessageReadAlarmLogResult messageReadAlarmLogResult;
							messageReadAlarmLogResult.Result = ResultUnknown;
							
							if (message.Valid())
							{
								unsigned int startTime = message.StartTime;
								unsigned int startOffset = message.StartOffset;
								unsigned int size = message.Size;
								unsigned char *data = 0;
								bool readed = false;
								ReadAlarmLogCallback.Call(startTime, startOffset, size, data, readed);
								
								messageReadAlarmLogResult.Result = readed ? ResultOk : ResultAlarmLogNotExist;

								messageReadAlarmLogResult.StartTime = startTime;
								messageReadAlarmLogResult.StartOffset = startOffset;
								messageReadAlarmLogResult.Size = size;
								messageReadAlarmLogResult.SizeData = size * 17;	// 17 - размер структуры АлармЛог
								messageReadAlarmLogResult.Data = data;
							}
							else
							{
								messageReadAlarmLogResult.Result = ResultInvalidRequest;
							}
							
							int messageDataLen = messageReadAlarmLogResult.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeReadAlarmLogResult, _messageData, messageDataLen);
						}
						break;
					case MessageCodePushAlarmLog:
						{
							MessagePushAlarmLog message(command.Data.Data, command.Data.DataLen);
							
							MessagePushAlarmLogResult messagePushAlarmLogResult;
							messagePushAlarmLogResult.Result = ResultUnknown;
							
							PushFileParams pushFileParams;
							
							if (message.Valid())
							{
								pushFileParams.DestinationNodeId = message.PushAddress;
								pushFileParams.FileId = message.StartTime;			// начиная с какого времени смотреть лог
								pushFileParams.PartSize = message.PartSize;				// количество строк помещаемых в пакет
								pushFileParams.WirelessRequest = wirelessRequest;
								pushFileParams.RequesterId = requesterId;

								if (message.RequestedParts.Size())
								{
									pushFileParams.RequestedParts = message.RequestedParts;
									pushFileParams.RequestedPartsUsed = true;
								}
								
								PushAlarmLogCallback.Call(pushFileParams);
								
								if (!pushFileParams.DelayResponse)
								{
									SendPushAlarmLogResult(pushFileParams);
								}
							}
							else
							{
								messagePushAlarmLogResult.Result = ResultInvalidRequest;
							}
						}
						break;
					case MessageCodeGenerateAlarmLog:
						{
							MessageGenerateAlarmLog message(command.Data.Data, command.Data.DataLen);
							
							MessageGenerateAlarmLogResult messageGenerateAlarmLogResult;
							messageGenerateAlarmLogResult.Result = ResultUnknown;
							
							if (message.Valid())
							{
								unsigned int startTime = message.StartTime;
								unsigned int countString = message.CountString;

								GenerateAlarmLogCallback.Call(startTime, countString);

								messageGenerateAlarmLogResult.Result = ResultOk;

							}
							else
							{
								messageGenerateAlarmLogResult.Result = ResultInvalidRequest;
							}
							
							int messageDataLen = messageGenerateAlarmLogResult.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeGenerateAlarmLogResult, _messageData, messageDataLen);
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

		void BuidFilePart(unsigned int fileId, unsigned int partNumber, unsigned char * &data, unsigned int partSize)
		{
			_messageData[0] = 3;	// FromFunction SeismicSensor
			_messageData[1] = 
			_messageData[0] = 12;	// AlarmLog
		}
	};
}
}
}
}
}

#endif