///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Радиомаяк
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_RADIOBEACON_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_RADIOBEACON_IMPLEMENTER_H

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
namespace Radiobeacon
{
	template
		<
			int eventId,								// код событий модема
			int maxDataSize,							// максимальный размер полезных пользовательских данных
			class EventDataType,						// тип данных сопутствующих событию EventCodeEventDataUpdated
			bool processEventCodeDataReceived			// обрабатывать события о входящих данных
		>
	class Implementer
	{
	public:
		static const int EventId = eventId;
		static const int MaxDataSize = maxDataSize;
		static const int MaxConfigParamsCount = 32;
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
		typedef CallbackWrapper<EventDataType &> GetEventDataCallbackType;
		typedef CallbackWrapper<int &> GetStateCallbackType;
		typedef CallbackWrapper<void *, unsigned int, unsigned int &, unsigned char &, bool &> GetInputDataCallbackType;
		typedef CallbackWrapper<unsigned char *, unsigned int, void *, unsigned int, bool &, int & > DataEnqueueCallbackType;
		typedef CallbackWrapper<int, ConfigParamCode *, ConfigParamCode *, ConfigParamValueType *, int, int &> GetConfigParamsCallbackType;
		typedef CallbackWrapper<int, ConfigParamCode *, ConfigParamValueType *, bool *, int, int &> SetConfigParamsCallbackType;
		typedef CallbackWrapper<> ApplyConfigParamsCallbackType;
		typedef CallbackWrapper<>EnableDebugDataOutputCallbackType;
	protected:
		EventDataType _eventData;
		unsigned char _messageData[MaxDataSize + 20];
		unsigned char _functionData[MaxDataSize + 20];
		unsigned char _dataFromModemBuffer[MaxDataSize];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		GetEventDataCallbackType GetEventDataCallback;
		GetStateCallbackType GetStateCallback;
		GetInputDataCallbackType GetInputDataCallback;
		DataEnqueueCallbackType DataEnqueueCallback;
		GetConfigParamsCallbackType GetConfigParamsCallback;
		SetConfigParamsCallbackType SetConfigParamsCallback;
		ApplyConfigParamsCallbackType ApplyConfigParamsCallback;
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
					case EventCodeEventDataUpdated:
						{
							GetEventDataCallback.Call(_eventData);
							
							unsigned char eventDataId = event.Param2;
					
							if (eventDataId != _eventData.EventDataId())
							{
								break;
							}

							for (int i = 0; i < _eventData.StateHistorySize; i++)
							{
								_messageData[0] = _eventData.GetStateFromHistory(i);
								FromFunction(MessageCodeStateChanged, _messageData, 1);
							}
							
							for (int i = 0; i < _eventData.DeliveryHistorySize; i++)
							{
								unsigned char packetId = -1;
								unsigned char node = -1;
								unsigned char deliveryResultCode = -1;
								
								if (_eventData.GetDeliveryHistory(i, packetId, node, deliveryResultCode))
								{
									MessageDataDeliveryResult resultMessage;
									resultMessage.PacketId = packetId;
									resultMessage.Node = node;
									resultMessage.Result = (MessageDataDeliveryResult::DeliveryResult) deliveryResultCode;
									int messageDataLen = resultMessage.ToByteArray(_messageData, sizeof(_messageData));
									
									FromFunction(MessageCodeDataDeliveryResult, _messageData, messageDataLen);
								}
							}
							
							if (_eventData.DataReceived)
							{
								if (processEventCodeDataReceived)
								{
									while(1)
									{
										unsigned int dataCount = 0;
										unsigned char sourceAddress = 0;
										bool empty = true;
										GetInputDataCallback.Call(_dataFromModemBuffer, sizeof(_dataFromModemBuffer), dataCount, sourceAddress, empty);
										
										if (!empty)
										{
											SendDataReceived(sourceAddress, _dataFromModemBuffer, dataCount);
										}
										else
										{
											break;
										}
									}
								}
							}
						}
					break;
					}
				}
				break;
			}
		}
		
		void SendDataReceived(unsigned char sourceAddress, unsigned char *data, int dataCount)
		{
			MessageDataReceived dataMessage;
			dataMessage.SourceAddress = sourceAddress;
			dataMessage.DataCount = dataCount;
			dataMessage.Data = data;
			int messageDataLen = dataMessage.ToByteArray(_messageData, sizeof(_messageData));
			
			FromFunction(MessageCodeDataReceived, _messageData, messageDataLen);
		}
	
		// Отправить сообщение от имени функции
		void FromFunction(MessageCode code, void *data, int dataCount)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionCallback.Call(Device::FunctionCodeRadiobeacon, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeRadiobeacon, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeRadiobeacon, code, _functionData, functionDataLen, enqueued, packetId);
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
							int state = 0;
							GetStateCallback.Call(state);
							
							_messageData[0] = (unsigned char)state;
							
							FromFunction(MessageCodeState, _messageData, 1);
						}
						break;
					case MessageCodeDataEnqueue:
						{
							MessageDataEnqueue message(command.Data.Data, command.Data.DataLen);
							if (message.Valid())
							{
								int packetId = 0;
								bool enqueued = false;
								DataEnqueueCallback.Call(message.DstList, message.DstCount, message.Data, message.DataCount, enqueued, packetId);
								
								Function::WirelessModem::MessageDataEnqueueResult resultMessage;
								
								if (enqueued)
								{
									resultMessage.DataId = message.DataId;
									resultMessage.Result = Function::WirelessModem::MessageDataEnqueueResult::EqueueResultEnqueued;
									resultMessage.PacketId = packetId;
								}
								else
								{
									resultMessage.DataId = message.DataId;
									resultMessage.Result = Function::WirelessModem::MessageDataEnqueueResult::EqueueResultNoSpaceInQueue;
								}
								
								int messageDataLen = resultMessage.ToByteArray(_messageData, sizeof(_messageData));
								FromFunction(MessageCodeDataEnqueueResult, _messageData, messageDataLen);
							}
						}
						break;
					case MessageCodeGetConfigParams:
						{
							MessageGetConfigParams message(command.Data.Data, command.Data.DataLen);
							
							if (message.Valid())
							{
								ConfigParamCode ids[MaxConfigParamsCount];
								ConfigParamValueType values[MaxConfigParamsCount];
								int actualCount = 0;
								GetConfigParamsCallback.Call(message.Count, message.Ids, ids, values, MaxConfigParamsCount, actualCount);
								
								MessageConfigParams messageConfigParams;
								messageConfigParams.Count = actualCount;
								messageConfigParams.Ids = ids;
								messageConfigParams.Values = values;
								int messageDataLen = messageConfigParams.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeConfigParams, _messageData, messageDataLen);
							}
						}
						break;
					case MessageCodeSetConfigParams:
						{
							MessageSetConfigParams message(command.Data.Data, command.Data.DataLen);
							
							if (message.Valid())
							{
								bool setResults[MaxConfigParamsCount];
								int actualCount = 0;
								SetConfigParamsCallback.Call(message.Count, message.Ids, message.Values, setResults, MaxConfigParamsCount, actualCount);
								
								MessageSetConfigParamsResult messageSetConfigParamsResult;
								messageSetConfigParamsResult.Count = actualCount;
								messageSetConfigParamsResult.Ids = message.Ids;
								messageSetConfigParamsResult.Results = setResults;
								int messageDataLen = messageSetConfigParamsResult.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeSetConfigParamsResult, _messageData, messageDataLen);
							}
						}
						break;
					case MessageCodeApplyConfigParams:
						{
							ApplyConfigParamsCallback.Call();
							
							FromFunction(MessageCodeApplyConfigParamsAck, _messageData, 0);
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