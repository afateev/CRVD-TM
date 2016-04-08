///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Служба подписки
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"

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
	enum MessageCode
	{
		MessageCodeInvalid									= 0,
		MessageCodeGetSubscriptionsList						= 1,
		MessageCodeSubscriptionsList						= 2,
		MessageCodeGetSubscribersList						= 3,
		MessageCodeSubscribersList							= 4,
		MessageCodeAddSubscription							= 5,
		MessageCodeAddSubscriptionResult 					= 6,
		MessageCodeRemoveSubscription						= 7,
		MessageCodeRemoveSubscriptionResult					= 8,
		MessageCodeKeepAlive								= 9,
		MessageCodeAddSubscribers							= 10,
		MessageCodeAddSubscribersResult 					= 11,
		MessageCodeRemoveSubscribers						= 12,
		MessageCodeRemoveSubscribersResult 					= 13,
		MessageCodeGetSubscriptionsAndSubscribersList		= 14,
		MessageCodeSubscriptionsAndSubscribersList			= 15,
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
	
	// Выходной формат данных функции Сейсмический датчик
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeSubscriptionService, MessageFormat>
	{
	};
	
	// Входной формат данных функции Сейсмический датчик
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeSubscriptionService, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeSubscriptionService, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	class MessageSubscriptionsList
	{
	public:
		unsigned char SubscriptionsCount;
		unsigned char *FunctionCodes;
		unsigned char *MessageCodes;
	public:
		MessageSubscriptionsList()
		{
			SubscriptionsCount = 0;
			FunctionCodes = 0;
			MessageCodes = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(SubscriptionsCount))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			int cnt = SubscriptionsCount;
			
			if (FunctionCodes == 0 || MessageCodes == 0)
			{
				cnt = 0;
			}
			
			if (dstSize - 1 < cnt * (sizeof(unsigned char) + sizeof(unsigned char)))
			{
				cnt = (dstSize - 1) / (sizeof(unsigned char) + sizeof(unsigned char));
			}
			
			if (cnt < 0)
			{
				cnt = 0;
			}
			
			dstPtr[dstIndex] = cnt;
			dstIndex++;
			
			for (int i = 0; i < cnt; i++)
			{
				dstPtr[dstIndex] = FunctionCodes[i];
				dstIndex++;
			}
			
			for (int i = 0; i < cnt; i++)
			{
				dstPtr[dstIndex] = MessageCodes[i];
				dstIndex++;
			}

			return dstIndex;
		}
	};
	
	class MessageGetSubscribersList
	{
	public:		
		unsigned char FunctionCode;
		unsigned char MessageCode;
	protected:
		bool _valid;
	public:
		MessageGetSubscribersList(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(FunctionCode) + sizeof(MessageCode))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			FunctionCode = src[0];
			MessageCode = src[1];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageSubscribersList
	{
	public:
		unsigned char FunctionCode;
		unsigned char MessageCode;
		unsigned char SubscribersCount;
		unsigned char *Subscribers;
	public:
		MessageSubscribersList()
		{
			SubscribersCount = 0;
			Subscribers = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(FunctionCode) + sizeof(MessageCode) + sizeof(SubscribersCount))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			int cnt = SubscribersCount;
			
			if (Subscribers == 0)
			{
				cnt = 0;
			}
			
			if (dstSize - 1 < cnt)
			{
				cnt = dstSize - 1;
			}
			
			if (cnt < 0)
			{
				cnt = 0;
			}
			
			dstPtr[dstIndex] = FunctionCode;
			dstIndex++;
			
			dstPtr[dstIndex] = MessageCode;
			dstIndex++;
			
			dstPtr[dstIndex] = cnt;
			dstIndex++;
			
			for (int i = 0; i < cnt; i++)
			{
				dstPtr[dstIndex] = Subscribers[i];
				dstIndex++;
			}

			return dstIndex;
		}
	};
	
	class MessageAddSubscription
	{
	public:		
		unsigned char Subscriber;
		unsigned char SubscriptionsCount;
		unsigned char *FunctionCodes;
		unsigned char *MessageCodes;
	protected:
		bool _valid;
	public:
		MessageAddSubscription(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 2)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Subscriber = src[0];
			SubscriptionsCount = src[1];
			
			if (dataCount < sizeof(Subscriber) + sizeof(SubscriptionsCount) + SubscriptionsCount * 2 * sizeof(unsigned char))
			{
				return;
			}
			
			FunctionCodes = &src[2];
			MessageCodes = &src[2 + SubscriptionsCount];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageAddSubscriptionResult
	{
	public:
		enum ErrorCode
		{
			ErrorCodeOk							= 0,
			ErrorCodeInvalidFunctionCode		= 1,
			ErrorCodeInvalidMessageCode			= 2,
			ErrorCodeNoMoreSpace				= 3,
			ErrorCodeInvalidDataFormat			= 4,
		};
		unsigned char Count;
		ErrorCode *Error;
	public:
		MessageAddSubscriptionResult()
		{
			Count = 0;
			Error = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Error))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			int cnt = Count;
			
			if (Error == 0)
			{
				cnt = 0;
			}
			
			if (dstSize - 1 < cnt)
			{
				cnt = dstSize - 1;
			}
			
			if (cnt < 0)
			{
				cnt = 0;
			}
			
			dstPtr[dstIndex] = cnt;
			dstIndex++;
			
			for (int i = 0; i < cnt; i++)
			{
				dstPtr[dstIndex] = Error[i];
				dstIndex++;
			}

			return dstIndex;
		}
	};
	
	class MessageRemoveSubscription
	{
	public:		
		unsigned char Subscriber;
		unsigned char SubscriptionsCount;
		unsigned char *FunctionCodes;
		unsigned char *MessageCodes;
	protected:
		bool _valid;
	public:
		MessageRemoveSubscription(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 2)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Subscriber = src[0];
			SubscriptionsCount = src[1];
			
			if (dataCount < sizeof(Subscriber) + sizeof(SubscriptionsCount) + SubscriptionsCount * 2 * sizeof(unsigned char))
			{
				return;
			}
			
			FunctionCodes = &src[2];
			MessageCodes = &src[2 + SubscriptionsCount];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageRemoveSubscriptionResult
	{
	public:
		enum ErrorCode
		{
			ErrorCodeOk							= 0,
			ErrorCodeInvalidDataFormat			= 1,
		};
		unsigned char Count;
		ErrorCode *Error;
	public:
		MessageRemoveSubscriptionResult()
		{
			Count = 0;
			Error = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Error))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			int cnt = Count;
			
			if (Error == 0)
			{
				cnt = 0;
			}
			
			if (dstSize - 1 < cnt)
			{
				cnt = dstSize - 1;
			}
			
			if (cnt < 0)
			{
				cnt = 0;
			}
			
			dstPtr[dstIndex] = cnt;
			dstIndex++;
			
			for (int i = 0; i < cnt; i++)
			{
				dstPtr[dstIndex] = Error[i];
				dstIndex++;
			}

			return dstIndex;
		}
	};
	
	class MessageAddSubscribers
	{
	public:		
		unsigned char FunctionCode;
		unsigned char MessageCode;
		unsigned char SubscribersCount;
		unsigned char *Subscribers;
	protected:
		bool _valid;
	public:
		MessageAddSubscribers(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 2)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			FunctionCode = src[0];
			MessageCode = src[1];
			SubscribersCount = src[2];
			
			if (dataCount < sizeof(FunctionCode) + sizeof(MessageCode) + sizeof(SubscribersCount) + SubscribersCount * sizeof(unsigned char))
			{
				return;
			}
			
			Subscribers = &src[3];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageAddSubscribersResult
	{
	public:
		enum ErrorCode
		{
			ErrorCodeOk							= 0,
			ErrorCodeInvalidFunctionCode		= 1,
			ErrorCodeInvalidMessageCode			= 2,
			ErrorCodeNoMoreSpace				= 3,
			ErrorCodeInvalidDataFormat			= 4,
		};
		ErrorCode Error;
	public:
		MessageAddSubscribersResult()
		{
			Error = ErrorCodeOk;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Error))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Error;
			dstIndex++;

			return dstIndex;
		}
	};
	
	class MessageRemoveSubscribers
	{
	public:		
		unsigned char FunctionCode;
		unsigned char MessageCode;
		unsigned char SubscribersCount;
		unsigned char *Subscribers;
	protected:
		bool _valid;
	public:
		MessageRemoveSubscribers(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 2)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			FunctionCode = src[0];
			MessageCode = src[1];
			SubscribersCount = src[2];
			
			if (dataCount < sizeof(FunctionCode) + sizeof(MessageCode) + sizeof(SubscribersCount) + SubscribersCount * sizeof(unsigned char))
			{
				return;
			}
			
			Subscribers = &src[3];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageRemoveSubscribersResult
	{
	public:
		enum ErrorCode
		{
			ErrorCodeOk							= 0,
			ErrorCodeInvalidFunctionCode		= 1,
			ErrorCodeInvalidMessageCode			= 2,
			ErrorCodeInvalidDataFormat			= 3,
		};
		ErrorCode Error;
	public:
		MessageRemoveSubscribersResult()
		{
			Error = ErrorCodeOk;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Error))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Error;
			dstIndex++;

			return dstIndex;
		}
	};
	
	template
		<
			int maxSubscriptionsCount,
			int maxSubscribersCount
		>
	class MessageSubscriptionsAndSubscribersList
	{
	public:
		static const int MaxSubscriptionsCount = maxSubscriptionsCount;
		static const int MaxSubscribersCount = maxSubscribersCount;
		static const int SubscribersBufferSize = MaxSubscriptionsCount * MaxSubscribersCount;
	public:
		struct Subscription
		{
			unsigned char FunctionCode;
			unsigned char MessageCode;
			unsigned char SubscribersCount;
			unsigned char *SubscribesList;
			
			Subscription()
			{
				FunctionCode = -1;
				MessageCode = -1;
				SubscribersCount = 0;
				SubscribesList = 0;
			}
		};
	protected:	
		unsigned short _subscriptionsCount;
		Subscription _subscriptionsList[MaxSubscriptionsCount];
		unsigned char _subscribersBuffer[SubscribersBufferSize];
		int _subscribersBufferUsed;
	public:
		MessageSubscriptionsAndSubscribersList()
		{
			_subscriptionsCount = 0;
			_subscribersBufferUsed = 0;
		}
		
		void Add(unsigned char functionCode, unsigned char messageCode, unsigned char *subscribesList, unsigned char subscribersCount)
		{
			if (_subscriptionsCount >= MaxSubscriptionsCount)
			{
				return;
			}
			
			_subscriptionsList[_subscriptionsCount].FunctionCode = functionCode;
			_subscriptionsList[_subscriptionsCount].MessageCode = messageCode;
			_subscriptionsList[_subscriptionsCount].SubscribersCount = subscribersCount;
			
			_subscriptionsList[_subscriptionsCount].SubscribersCount = 0;
			for (int i = 0; i < subscribersCount; i++)
			{
				if (_subscribersBufferUsed >= SubscribersBufferSize)
				{
					break;
				}
				
				if (i >= MaxSubscribersCount)
				{
					break;
				}
				
				if (i == 0)
				{
					_subscriptionsList[_subscriptionsCount].SubscribesList = &_subscribersBuffer[_subscribersBufferUsed];
				}
				
				_subscribersBuffer[_subscribersBufferUsed] = subscribesList[i];
				_subscriptionsList[_subscriptionsCount].SubscribersCount++;
				_subscribersBufferUsed++;
			}
			
			_subscriptionsCount++;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(_subscriptionsCount))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			int cnt = _subscriptionsCount;
			
			
			dstPtr[dstIndex] = cnt;
			dstIndex++;
			
			if (dstIndex >= dstSize)
			{
				return dstIndex;
			}
			
			for (int i = 0; i < cnt; i++)
			{
				dstPtr[dstIndex] = _subscriptionsList[i].FunctionCode;
				dstIndex++;
				if (dstIndex >= dstSize)
				{
					return dstIndex;
				}
				
				dstPtr[dstIndex] = _subscriptionsList[i].MessageCode;
				dstIndex++;
				if (dstIndex >= dstSize)
				{
					return dstIndex;
				}
				
				// пока 0, потом по факту заполним
				int subscribersCountIndex = dstIndex;
				dstPtr[dstIndex] = 0;
				dstIndex++;

				if (dstIndex >= dstSize)
				{
					return dstIndex;
				}
				
				for (int j = 0; j < _subscriptionsList[i].SubscribersCount; j++)
				{
					dstPtr[dstIndex] = _subscriptionsList[i].SubscribesList[j];
					dstPtr[subscribersCountIndex] = j + 1; // фактическое число подписчиков
					
					dstIndex++;
					if (dstIndex >= dstSize)
					{
						return dstIndex;
					}
				}
			}
			
			return dstIndex;
		}
	};
}
}
}
}
}

#endif