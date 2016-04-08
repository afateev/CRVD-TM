///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Служба подписки
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_IMPLEMENTER_H

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
namespace SubscriptionService
{
	template
		<
			int eventId,								// код событий службы подписки
			int maxDataSize,							// максимальный размер полезных пользовательских данных
			int maxSubscriptionsCount,					// максимальное число подписок
			int maxSubscribersCount						// максимальное число подписчиков
		>
	class Implementer
	{
	public:
		static const int EventId = eventId;
		static const int MaxDataSize = maxDataSize;
		static const int MaxSubscriptionsCount = maxSubscriptionsCount;
		static const int MaxSubscribersCount = maxSubscribersCount;
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char *, unsigned char *, int, int &> GetSubscriptionsListCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char *, int, int &> GetSubscribersListCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, bool &, bool &, bool &> AddSubscriptionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char> RemoveSubscriptionCallbackType;
		typedef CallbackWrapper<int, int> OnKeepAliveTransmitedCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char *, int, bool &, bool &, bool &> AddSubscribersCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char *, int, bool &, bool &> RemoveSubscribersCallbackType;
	protected:
		unsigned char _messageData[MaxDataSize + 50];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		GetSubscriptionsListCallbackType GetSubscriptionsListCallback;
		GetSubscribersListCallbackType GetSubscribersListCallback;
		AddSubscriptionCallbackType AddSubscriptionCallback;
		RemoveSubscriptionCallbackType RemoveSubscriptionCallback;
		OnKeepAliveTransmitedCallbackType OnKeepAliveTransmitedCallback;
		AddSubscribersCallbackType AddSubscribersCallback;
		RemoveSubscribersCallbackType RemoveSubscribersCallback;
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
					case EventCodeKeepAlive:
						{
							int subscriberAddress = event.Param2;
							bool enqueued = false;
							int packetId = -1;
							
							FromFunctionToNode(subscriberAddress, MessageCodeKeepAlive, 0, 0, enqueued, packetId);
							
							if (enqueued)
							{
								OnKeepAliveTransmitedCallback.Call(subscriberAddress, packetId);
							}
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
			
			FromFunctionCallback.Call(Device::FunctionCodeSubscriptionService, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeSubscriptionService, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeSubscriptionService, code, _functionData, functionDataLen, enqueued, packetId);
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
					case MessageCodeGetSubscriptionsList:
						{
							int count = 0;
							unsigned char functionCodes[MaxSubscriptionsCount];
							unsigned char messageCodes[MaxSubscriptionsCount];
							GetSubscriptionsListCallback.Call(functionCodes, messageCodes, MaxSubscriptionsCount, count);
							
							MessageSubscriptionsList message;
							message.SubscriptionsCount = count;
							message.FunctionCodes = functionCodes;
							message.MessageCodes = messageCodes;
							int messageDataLen = message.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeSubscriptionsList, _messageData, messageDataLen);
						}
						break;
					case MessageCodeGetSubscribersList:
						{
							MessageGetSubscribersList message(command.Data.Data, command.Data.DataLen);
							
							if (message.Valid())
							{
								unsigned char subscribers[MaxSubscribersCount];
								
								int count = 0;
								GetSubscribersListCallback.Call(message.FunctionCode, message.MessageCode, subscribers, MaxSubscribersCount, count);
								
								MessageSubscribersList messageResult;
								messageResult.FunctionCode = message.FunctionCode;
								messageResult.MessageCode = message.MessageCode;
								messageResult.SubscribersCount = count;
								messageResult.Subscribers = subscribers;
								int messageDataLen = messageResult.ToByteArray(_messageData, sizeof(_messageData));
							
								FromFunction(MessageCodeSubscribersList, _messageData, messageDataLen);
							}
						}
						break;
					case MessageCodeAddSubscription:
						{
							MessageAddSubscription message(command.Data.Data, command.Data.DataLen);
							
							int count = 0;
							MessageAddSubscriptionResult::ErrorCode error[MaxSubscriptionsCount];
							
							if (message.Valid())
							{
								bool invalidFunction = false;
								bool invalidMessage = false;
								bool noMoreSapce = false;
								
								count = message.SubscriptionsCount;
								
								for (int i = 0; i < message.SubscriptionsCount; i++)
								{
									error[i] = MessageAddSubscriptionResult::ErrorCodeOk;
									AddSubscriptionCallback.Call(message.Subscriber, message.FunctionCodes[i], message.MessageCodes[i], invalidFunction, invalidMessage, noMoreSapce);
									
									if (invalidFunction)
									{
										error[i] = MessageAddSubscriptionResult::ErrorCodeInvalidFunctionCode;
									}
									else
									{
										if (invalidMessage)
										{
											error[i] = MessageAddSubscriptionResult::ErrorCodeInvalidMessageCode;
										}
										else
										{
											if (noMoreSapce)
											{
												error[i] = MessageAddSubscriptionResult::ErrorCodeNoMoreSpace;
											}
										}
									}
								}
							}
							else
							{
								count = 1;
								error[0] = MessageAddSubscriptionResult::ErrorCodeInvalidDataFormat;
							}
							
							MessageAddSubscriptionResult messageResult;
							messageResult.Count = count;
							messageResult.Error = error;
							int messageDataLen = messageResult.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeAddSubscriptionResult, _messageData, messageDataLen);
						}
						break;
					case MessageCodeRemoveSubscription:
						{
							MessageRemoveSubscription message(command.Data.Data, command.Data.DataLen);
							
							int count = 0;
							MessageRemoveSubscriptionResult::ErrorCode error[MaxSubscriptionsCount];
							
							if (message.Valid())
							{
								count = message.SubscriptionsCount;
								
								for (int i = 0; i < message.SubscriptionsCount; i++)
								{
									error[i] = MessageRemoveSubscriptionResult::ErrorCodeOk;
									RemoveSubscriptionCallback.Call(message.Subscriber, message.FunctionCodes[i], message.MessageCodes[i]);
								}
							}
							else
							{
								count = 1;
								error[0] = MessageRemoveSubscriptionResult::ErrorCodeInvalidDataFormat;
							}
							
							MessageRemoveSubscriptionResult messageResult;
							messageResult.Count = count;
							messageResult.Error = error;
							int messageDataLen = messageResult.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeRemoveSubscriptionResult, _messageData, messageDataLen);
						}
						break;
					case MessageCodeAddSubscribers:
						{
							MessageAddSubscribers message(command.Data.Data, command.Data.DataLen);
							
							MessageAddSubscribersResult::ErrorCode error = MessageAddSubscribersResult::ErrorCodeOk;
							
							if (message.Valid())
							{
								bool invalidFunction = false;
								bool invalidMessage = false;
								bool noMoreSapce = false;
								
								AddSubscribersCallback.Call(message.FunctionCode, message.MessageCode, message.Subscribers, message.SubscribersCount, invalidFunction, invalidMessage, noMoreSapce);
								
								if (invalidFunction)
								{
									error = MessageAddSubscribersResult::ErrorCodeInvalidFunctionCode;
								}
								else
								{
									if (invalidMessage)
									{
										error = MessageAddSubscribersResult::ErrorCodeInvalidMessageCode;
									}
									else
									{
										if (noMoreSapce)
										{
											error = MessageAddSubscribersResult::ErrorCodeNoMoreSpace;
										}
									}
								}
							}
							else
							{
								error = MessageAddSubscribersResult::ErrorCodeInvalidDataFormat;
							}
							
							MessageAddSubscribersResult messageResult;
							messageResult.Error = error;
							int messageDataLen = messageResult.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeAddSubscribersResult, _messageData, messageDataLen);
						}
						break;
					case MessageCodeRemoveSubscribers:
						{
							MessageRemoveSubscribers message(command.Data.Data, command.Data.DataLen);
							
							MessageRemoveSubscribersResult::ErrorCode error = MessageRemoveSubscribersResult::ErrorCodeOk;
							
							if (message.Valid())
							{
								bool invalidFunction = false;
								bool invalidMessage = false;
								
								RemoveSubscribersCallback.Call(message.FunctionCode, message.MessageCode, message.Subscribers, message.SubscribersCount, invalidFunction, invalidMessage);
								
								if (invalidFunction)
								{
									error = MessageRemoveSubscribersResult::ErrorCodeInvalidFunctionCode;
								}
								else
								{
									if (invalidMessage)
									{
										error = MessageRemoveSubscribersResult::ErrorCodeInvalidMessageCode;
									}
								}
							}
							else
							{
								error = MessageRemoveSubscribersResult::ErrorCodeInvalidDataFormat;
							}
							
							MessageRemoveSubscribersResult messageResult;
							messageResult.Error = error;
							int messageDataLen = messageResult.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeRemoveSubscribersResult, _messageData, messageDataLen);
						}
						break;
					case MessageCodeGetSubscriptionsAndSubscribersList:
						{
							int subscriptionsCount = 0;
							unsigned char functionCodes[MaxSubscriptionsCount];
							unsigned char messageCodes[MaxSubscriptionsCount];
							GetSubscriptionsListCallback.Call(functionCodes, messageCodes, MaxSubscriptionsCount, subscriptionsCount);
							
							MessageSubscriptionsAndSubscribersList<MaxSubscriptionsCount, MaxSubscribersCount> message;
							
							for (int i = 0; i < subscriptionsCount; i++)
							{
								unsigned char subscribers[MaxSubscribersCount];
								
								int subscribersCount = 0;
								GetSubscribersListCallback.Call(functionCodes[i], messageCodes[i], subscribers, MaxSubscribersCount, subscribersCount);
								
								message.Add(functionCodes[i], messageCodes[i], subscribers, subscribersCount);
							}
							
							int messageDataLen = message.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeSubscriptionsAndSubscribersList, _messageData, messageDataLen);
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