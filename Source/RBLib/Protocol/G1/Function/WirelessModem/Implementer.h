///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Беспроводной модем
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_WIRELESSMODEM_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_WIRELESSMODEM_IMPLEMENTER_H

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
namespace WirelessModem
{
	template
		<
			int eventId,								// код событий модема
			int maxDataSize,							// максимальный размер полезных пользовательских данных
			class EventDataType,						// тип данных сопутствующих событию EventCodeEventDataUpdated
			bool processEventCodeDataReceived,			// обрабатывать события о входящих данных
			bool processEventCodeDebugDataRefilled,		// обрабатывать события об отладочных данных
			bool processEventCodeTestData				// обрабатывать события о тестовых данных
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
		typedef CallbackWrapper<unsigned char *, unsigned char *, unsigned int, int &> GetSelfActivityTableCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char *, unsigned char *, unsigned int, int &> GetRemoteNodeActivityTableCallbackType;
		typedef CallbackWrapper<unsigned char *, unsigned char *, unsigned int, int &> GetRouteTableCallbackType;
		typedef CallbackWrapper<void *, unsigned int, unsigned int &, unsigned char &, bool &> GetInputDataCallbackType;
		typedef CallbackWrapper<void *, unsigned int, unsigned int &, unsigned short &, bool &> GetDebugDataCallbackType;
		typedef CallbackWrapper<unsigned char *, unsigned int, void *, unsigned int, unsigned int, bool &, int & > DataEnqueueCallbackType;
		typedef CallbackWrapper<int, ConfigParamCode *, ConfigParamCode *, ConfigParamValueType *, int, int &> GetConfigParamsCallbackType;
		typedef CallbackWrapper<int, ConfigParamCode *, ConfigParamValueType *, bool *, int, int &> SetConfigParamsCallbackType;
		typedef CallbackWrapper<> ApplyConfigParamsCallbackType;
		typedef CallbackWrapper<>EnableDebugDataOutputCallbackType;
		typedef CallbackWrapper<unsigned char, int, unsigned char *, unsigned int, int &> TestCommandCallbackType;
		typedef CallbackWrapper<void *, unsigned int, unsigned int &, unsigned char &, int &, bool &> GetTestDataCallbackType;
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
		GetSelfActivityTableCallbackType GetSelfActivityTableCallback;
		GetRemoteNodeActivityTableCallbackType GetRemoteNodeActivityTableCallback;
		GetRouteTableCallbackType GetRouteTableCallback;
		GetInputDataCallbackType GetInputDataCallback;
		GetDebugDataCallbackType GetDebugDataCallback;
		DataEnqueueCallbackType DataEnqueueCallback;
		GetConfigParamsCallbackType GetConfigParamsCallback;
		SetConfigParamsCallbackType SetConfigParamsCallback;
		ApplyConfigParamsCallbackType ApplyConfigParamsCallback;
		EnableDebugDataOutputCallbackType EnableDebugDataOutputCallback;
		TestCommandCallbackType TestCommandCallback;
		GetTestDataCallbackType GetTestDataCallback;
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
							
							if (_eventData.SelfActivityTableChanged)
							{
								unsigned char activityTableNodes[255];
								unsigned char activityTableRates[255];
								
								int buffersSize = sizeof(activityTableNodes);
								int tableSize = 0;
								GetSelfActivityTableCallback.Call(activityTableNodes, activityTableRates, buffersSize, tableSize);
								
								MessageSelfActivityTable tableMessage;
								tableMessage.TableSize = tableSize;
								tableMessage.Nodes = activityTableNodes;
								tableMessage.Rates = activityTableRates;
								int messageDataLen = tableMessage.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeSelfActivityTableChanged, _messageData, messageDataLen);
							}
							
							if (_eventData.RemoteNodeActivityTableReceived)
							{
								for (int i = 0; i < 255; i++)
								{
									if (_eventData.GetRemoteNodeActivityTableReceived(i))
									{
										unsigned char activityTableNodes[255];
										unsigned char activityTableRates[255];
										
										int node = i;
										int buffersSize = sizeof(activityTableNodes);
										int tableSize = 0;
										GetRemoteNodeActivityTableCallback.Call(node, activityTableNodes, activityTableRates, buffersSize, tableSize);
										
										MessageRemoteNodeActivityTable tableMessage;
										tableMessage.RemoteNodeId = node;
										tableMessage.TableSize = tableSize;
										tableMessage.Nodes = activityTableNodes;
										tableMessage.Rates = activityTableRates;
										int messageDataLen = tableMessage.ToByteArray(_messageData, sizeof(_messageData));
										
										FromFunction(MessageCodeRemoteNodeActivityTableReceived, _messageData, messageDataLen);
									}
								}
							}
							
							if (_eventData.RemoteNodeActivityTableChanged)
							{
								for (int i = 0; i < 255; i++)
								{
									if (_eventData.GetRemoteNodeActivityTableChanged(i))
									{
										unsigned char activityTableNodes[255];
										unsigned char activityTableRates[255];
										
										int node = i;
										int buffersSize = sizeof(activityTableNodes);
										int tableSize = 0;
										GetRemoteNodeActivityTableCallback.Call(node, activityTableNodes, activityTableRates, buffersSize, tableSize);
										
										MessageRemoteNodeActivityTable tableMessage;
										tableMessage.RemoteNodeId = node;
										tableMessage.TableSize = tableSize;
										tableMessage.Nodes = activityTableNodes;
										tableMessage.Rates = activityTableRates;
										int messageDataLen = tableMessage.ToByteArray(_messageData, sizeof(_messageData));
										
										FromFunction(MessageCodeRemoteNodeActivityTableChanged, _messageData, messageDataLen);
									}
								}
							}
							
							if (_eventData.RouteTableChanged)
							{
								unsigned char routeTableToNodes[255];
								unsigned char routeTableFromNodes[255];
								
								int buffersSize = sizeof(routeTableToNodes);
								int tableSize = 0;
								GetRouteTableCallback.Call(routeTableToNodes, routeTableFromNodes, buffersSize, tableSize);
								
								MessageRouteTable tableMessage;
								tableMessage.TableSize = tableSize;
								tableMessage.ToNodes = routeTableToNodes;
								tableMessage.FromNodes = routeTableFromNodes;
								int messageDataLen = tableMessage.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeRouteTableChanged, _messageData, messageDataLen);
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
							
							if (_eventData.DebugDataRefilled)
							{
								if (processEventCodeDebugDataRefilled)
								{
									while(1)
									{
										unsigned int dataCount = 0;
										unsigned short code = 0;
										bool empty = true;
										GetDebugDataCallback.Call(_dataFromModemBuffer, sizeof(_dataFromModemBuffer), dataCount, code, empty);
										
										if (!empty)
										{
											MessageDebugData dataMessage;
											dataMessage.Code = code;
											dataMessage.DataCount = dataCount;
											dataMessage.Data = _dataFromModemBuffer;
											int messageDataLen = dataMessage.ToByteArray(_messageData, sizeof(_messageData));
											
											FromFunction(MessageCodeDebugData, _messageData, messageDataLen);
										}
										else
										{
											break;
										}
									}
								}
							}
							
							if (_eventData.TestDataPresent)
							{
								if (processEventCodeTestData)
								{
									while(1)
									{
										unsigned char testType = 0;
										int testId = 0;
										unsigned int dataCount = 0;
										bool empty = true;
										GetTestDataCallback.Call(_dataFromModemBuffer, sizeof(_dataFromModemBuffer), dataCount, testType, testId, empty);
										
										if (!empty)
										{
											MessageTestData dataMessage;
											dataMessage.TestType = testType;
											dataMessage.TestId = testId;
											dataMessage.DataCount = dataCount;
											dataMessage.Data = _dataFromModemBuffer;
											int messageDataLen = dataMessage.ToByteArray(_messageData, sizeof(_messageData));
											
											FromFunction(MessageCodeTestData, _messageData, messageDataLen);
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
		
		static void SendTestDataToNode(void *callbackParam, int nodeId, unsigned char testType, int testId, unsigned char *data, int dataCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Implementer *im = (Implementer *)callbackParam;
			
			MessageTestData dataMessage;
			dataMessage.TestType = testType;
			dataMessage.TestId = testId;
			dataMessage.DataCount = dataCount;
			dataMessage.Data = data;
			int messageDataLen = dataMessage.ToByteArray(im->_messageData, sizeof(_messageData));
			
			bool enqueued = false;
			int packetId = -1;
			im->FromFunctionToNode(nodeId, MessageCodeTestData, im->_messageData, messageDataLen, enqueued, packetId);
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
			
			FromFunctionCallback.Call(Device::FunctionCodeWirelessModem, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeWirelessModem, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeWirelessModem, code, _functionData, functionDataLen, enqueued, packetId);
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
					case MessageCodeGetSelfActivityTable:
						{
							unsigned char activityTableNodes[255];
							unsigned char activityTableRates[255];
							
							int buffersSize = sizeof(activityTableNodes);
							int tableSize = 0;
							GetSelfActivityTableCallback.Call(activityTableNodes, activityTableRates, buffersSize, tableSize);
							
							MessageSelfActivityTable tableMessage;
							tableMessage.TableSize = tableSize;
							tableMessage.Nodes = activityTableNodes;
							tableMessage.Rates = activityTableRates;
							int messageDataLen = tableMessage.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeSelfActivityTable, _messageData, messageDataLen);
						}
						break;
					case MessageCodeGetRemoteNodeActivityTable:
						{
							if (command.Data.Data != 0 && command.Data.DataLen >= 1)
							{
								int node = ((unsigned char *)command.Data.Data)[0];
								
								unsigned char activityTableNodes[255];
								unsigned char activityTableRates[255];
								
								int buffersSize = sizeof(activityTableNodes);
								int tableSize = 0;
								GetRemoteNodeActivityTableCallback.Call(node, activityTableNodes, activityTableRates, buffersSize, tableSize);
								
								MessageRemoteNodeActivityTable tableMessage;
								tableMessage.RemoteNodeId = node;
								tableMessage.TableSize = tableSize;
								tableMessage.Nodes = activityTableNodes;
								tableMessage.Rates = activityTableRates;
								int messageDataLen = tableMessage.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeRemoteNodeActivityTable, _messageData, messageDataLen);
							}
						}
						break;
					case MessageCodeGetRouteTable:
						{
							unsigned char routeTableToNodes[255];
							unsigned char routeTableFromNodes[255];
							
							int buffersSize = sizeof(routeTableToNodes);
							int tableSize = 0;
							GetRouteTableCallback.Call(routeTableToNodes, routeTableFromNodes, buffersSize, tableSize);
							
							MessageRouteTable tableMessage;
							tableMessage.TableSize = tableSize;
							tableMessage.ToNodes = routeTableToNodes;
							tableMessage.FromNodes = routeTableFromNodes;
							int messageDataLen = tableMessage.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeRouteTable, _messageData, messageDataLen);
						}
						break;
					case MessageCodeDataEnqueue:
						{
							MessageDataEnqueue message(command.Data.Data, command.Data.DataLen);
							if (message.Valid())
							{
								int packetId = 0;
								bool enqueued = false;
								DataEnqueueCallback.Call(message.DstList, message.DstCount, message.Data, message.DataCount, message.Timeout, enqueued, packetId);
								
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
					case MessageCodeEnableDebugDataOutput:
						{
							EnableDebugDataOutputCallback.Call();
						}
						break;
					case MessageCodeTestCommand:
						{
							MessageTestCommand message(command.Data.Data, command.Data.DataLen);
							if (message.Valid())
							{
								int result = 0;
								TestCommandCallback.Call(message.TestType, message.TestId, message.Data, message.DataCount, result);
								
								Function::WirelessModem::MessageTestCommandResult resultMessage;
								
								resultMessage.TestType = message.TestType;
								resultMessage.TestId = message.TestId;
								resultMessage.Result = result;
								
								int messageDataLen = resultMessage.ToByteArray(_messageData, sizeof(_messageData));
								FromFunction(MessageCodeTestCommandResult, _messageData, messageDataLen);
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