///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Хранилище
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_STORAGE_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_STORAGE_IMPLEMENTER_H

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
namespace Storage
{
	struct PushFileParams
	{
		unsigned int DestinationNodeId;
		unsigned int FileId;
		unsigned int FileSize; // возвращается
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
		typedef CallbackWrapper<unsigned int, unsigned int, unsigned int, unsigned char * &, bool &> ReadFileCallbackType;
		
		typedef CallbackWrapper<PushFileParams &> PushFileCallbackType;
	protected:
		unsigned char _messageData[MaxDataSize + 50];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		ReadFileCallbackType ReadFileCallback;
		PushFileCallbackType PushFileCallback;
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
			
			FromFunctionCallback.Call(Device::FunctionCodeStorage, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool& enqueued, int& packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeStorage, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeStorage, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// отправить часть файла либо конкретному узлу либо стандартным способом в зависимости от параметра wireless
		void SendReadFileResult(bool wireless, unsigned char node, unsigned int fileId, unsigned int startOffset, unsigned char *data, unsigned int size, bool& enqueued, int& packetId)
		{
			MessageReadFileResult messageReadFileResult;
			messageReadFileResult.Result = ResultOk;
			messageReadFileResult.FileId = fileId;
			messageReadFileResult.StartOffset = startOffset;
			messageReadFileResult.Size = size;
			messageReadFileResult.Data = data;

			int messageDataLen = messageReadFileResult.ToByteArray(_messageData, sizeof(_messageData));
			
			if (wireless)
			{
				FromFunctionToNode(node, MessageCodeReadFileResult, _messageData, messageDataLen, enqueued, packetId);
			}
			else
			{
				FromFunction(MessageCodeReadFileResult, _messageData, messageDataLen);
			}
		}
		
		// отправить результат выполнения PushFile
		void SendPushFileResult(PushFileParams pushFileParams)
		{
			MessagePushFileResult messagePushFileResult;
			messagePushFileResult.Result = pushFileParams.Started ? ResultOk : ResultFileNotExist;
			
			if (pushFileParams.Busy)
			{
				messagePushFileResult.Result = ResultBusy;
			}
			
			messagePushFileResult.FileId = pushFileParams.FileId;
			messagePushFileResult.FileSize = pushFileParams.FileSize;
			messagePushFileResult.PartSize = pushFileParams.PartSize;
			messagePushFileResult.PartsCount = pushFileParams.PartsCount;
			
			int messageDataLen = messagePushFileResult.ToByteArray(_messageData, sizeof(_messageData));
			
			if (pushFileParams.WirelessRequest)
			{
				bool enqueued = false;
				int packetId = -1;
				FromFunctionToNode(pushFileParams.RequesterId, MessageCodePushFileResult, _messageData, messageDataLen, enqueued, packetId);
			}
			else
			{
				FromFunction(MessageCodePushFileResult, _messageData, messageDataLen);
			}
		}

		void SendPushDoneNotification(bool wireless, unsigned char node, unsigned int fileId, bool& enqueued, int& packetId)
		{
			if (wireless)
			{
				FromFunctionToNode(node, MessageCodePushFileDone, &fileId, sizeof(unsigned int), enqueued, packetId);
			}
			else
			{
				FromFunction(MessageCodePushFileDone, &fileId, sizeof(unsigned int));
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
					case MessageCodeReadFile:
						{
							MessageReadFile message(command.Data.Data, command.Data.DataLen);
							
							MessageReadFileResult messageReadFileResult;
							messageReadFileResult.Result = ResultUnknown;
							
							// TODO функция временно заблокирована, как "не потоко-безопасная"
							if (message.Valid() && 0)
							{
								unsigned int fileId = message.FileId;
								unsigned int startOffset = message.StartOffset;
								unsigned int size = message.Size;
								unsigned char *data = 0;
								bool readed = false;
								ReadFileCallback.Call(fileId, startOffset, size, data, readed);
								
								messageReadFileResult.Result = readed ? ResultOk : ResultFileNotExist;

								messageReadFileResult.FileId = fileId;
								messageReadFileResult.StartOffset = startOffset;
								messageReadFileResult.Size = size;
								messageReadFileResult.Data = data;
							}
							else
							{
								messageReadFileResult.Result = ResultInvalidRequest;
							}
							
							int messageDataLen = messageReadFileResult.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeReadFileResult, _messageData, messageDataLen);
						}
						break;
					case MessageCodePushFile:
						{
							MessagePushFile message(command.Data.Data, command.Data.DataLen);
							
							MessagePushFileResult messagePushFileResult;
							messagePushFileResult.Result = ResultUnknown;
							
							PushFileParams pushFileParams;
							
							if (message.Valid())
							{
								pushFileParams.DestinationNodeId = message.PushAddress;
								pushFileParams.FileId = message.FileId;
								pushFileParams.PartSize = message.PartSize;
								pushFileParams.WirelessRequest = wirelessRequest;
								pushFileParams.RequesterId = requesterId;
								
								if (message.RequestedParts.Size())
								{
									pushFileParams.RequestedParts = message.RequestedParts;
									pushFileParams.RequestedPartsUsed = true;
								}

								PushFileCallback.Call(pushFileParams);
								
								if (!pushFileParams.DelayResponse)
								{
									SendPushFileResult(pushFileParams);
								}
							}
							else
							{
								messagePushFileResult.Result = ResultInvalidRequest;
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
		
		void BuidFilePart(unsigned int fileId, unsigned int partNumber, unsigned char * &data, unsigned int partSize)
		{
			_messageData[0] = 5;	// FromFunction
			_messageData[1] = 
			_messageData[0] = 6;	// Storage
		}
	};
}
}
}
}
}

#endif