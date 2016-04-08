///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Служба подписки
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_H
#define PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_H

#include "EventCode.h"

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
			int eventId,								// Идентификатор события
			int maxSubscribersCount
		>
	class SubscriptionItem
	{
	public:
		static const int EventId = eventId;
		typedef unsigned char FunctionCodeType;
		typedef unsigned char MessageCodeType;
		typedef unsigned char AddressType;
		static const int MaxSubscribersCount = maxSubscribersCount;
	protected:
		bool _used;
		FunctionCodeType _functionCode;
		MessageCodeType _messageCode;
		AddressType _subscribersList[MaxSubscribersCount];
		int _subscribersCount;
	public:
		SubscriptionItem()
		{
			Reset();
		}
		
		void Reset()
		{
			_used = false;
			_functionCode = 0;
			_messageCode = 0;
			_subscribersCount = 0;
		}
		
		bool Empty()
		{
			return !_used;
		}
		
		FunctionCodeType FunctionCode()
		{
			return _functionCode;
		}
		
		MessageCodeType MessageCode()
		{
			return _messageCode;
		}
		
		bool Equal(FunctionCodeType functionCode, MessageCodeType messageCode)
		{
			return _used && _functionCode == functionCode && _messageCode == messageCode;
		}
		
		void Set(FunctionCodeType functionCode, MessageCodeType messageCode)
		{
			if (_used)
			{
				return;
			}
			
			_functionCode = functionCode;
			_messageCode = messageCode;
			_used = true;
		}
		
		int Count()
		{
			return _subscribersCount;
		}
		
		bool Contains(AddressType subscriber)
		{
			for (int i = 0; i < _subscribersCount; i++)
			{
				if (_subscribersList[i] == subscriber)
				{
					return true;
				}
			}
			
			return false;
		}
		
		bool Add(AddressType subscriber)
		{
			if (Contains(subscriber))
			{
				// subscriber уже есть в списке
				return true;
			}
			
			// есть ли место
			int remain = MaxSubscribersCount - _subscribersCount;
			if (remain < 1)
			{
				return false;
			}
			
			_subscribersList[_subscribersCount] = subscriber;
			_subscribersCount++;
			
			Rblib::Event e1(EventId, EventCodeSubscriberAdded, _functionCode, _messageCode, subscriber);
			Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e1);
			
			Rblib::Event e2(EventId, EventCodeSubscribersListChanged, _functionCode, _messageCode);
			Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e2);
			
			return true;
		}
		
		bool Remove(AddressType subscriber)
		{
			bool changed = false;
			
			for (int i = 0; i < _subscribersCount; i++)
			{
				if (_subscribersList[i] == subscriber)
				{
					for (int j = i; j < _subscribersCount - 1; j ++)
					{
						_subscribersList[j] = _subscribersList[j + 1];
					}
					_subscribersCount--;
					changed = true;
					
					Rblib::Event e1(EventId, EventCodeSubscriberRemoved, _functionCode, _messageCode, subscriber);
					Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e1);
					
					Rblib::Event e2(EventId, EventCodeSubscribersListChanged, _functionCode, _messageCode);
					Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e2);
					
					break;
				}
			}
			
			// нет больше подписчиков, освободим элмент для последующего использования
			if (_subscribersCount < 1)
			{
				Rblib::Event e1(EventId, EventCodeSubscriptionRemoved, _functionCode, _messageCode);
				Reset();
				Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e1);
				
				Rblib::Event e2(EventId, EventCodeSubscriptionsListChanged);
				Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e2);
			}
			
			return changed;
		}
		
		bool Remove(AddressType *subscribers, int count)
		{
			if (subscribers == 0)
			{
				return false;
			}
			
			bool changed = false;
			
			for (int i = 0; i < count; i++)
			{
				changed |= Remove(subscribers[i]);
			}
			
			return changed;
		}
		
		int Get(AddressType *buffer, int maxCount)
		{
			int count = _subscribersCount;
			if (count > maxCount)
			{
				count = maxCount;
			}
			
			for (int i = 0; i < count; i++)
			{
				buffer[i] = _subscribersList[i];
			}
			
			return count;
		}
		
		// удалить всех подписчиков кроме указанных
		void RemoveAllSubscribersButThese(AddressType *subscribers, int count)
		{
			bool changed = false;
			
			for (int i = 0; i < _subscribersCount; i++)
			{
				if (!ItemAtList(_subscribersList[i], subscribers, count))
				{
					AddressType deleted = _subscribersList[i];
					
					for (int j = i; j < _subscribersCount - 1; j ++)
					{
						_subscribersList[j] = _subscribersList[j + 1];
					}
					_subscribersCount--;
					i--;
					
					Rblib::Event e(EventId, EventCodeSubscriberRemoved, _functionCode, _messageCode, deleted);
					Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e);
					
					changed = true;
				}
			}
			
			if (changed)
			{
				Rblib::Event e(EventId, EventCodeSubscribersListChanged, _functionCode, _messageCode);
				Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e);
			}
			
			// нет больше подписчиков, освободим элмент для последующего использования
			if (_subscribersCount < 1)
			{
				Rblib::Event e1(EventId, EventCodeSubscriptionRemoved, _functionCode, _messageCode);
				Reset();
				Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e1);
				
				Rblib::Event e2(EventId, EventCodeSubscriptionsListChanged);
				Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e2);
			}
		}
	protected:
		bool ItemAtList(AddressType item, AddressType *list, int count)
		{
			if (list == 0)
			{
				return false;
			}
			
			for (int i = 0; i < count; i++)
			{
				if (item == list[i])
				{
					return true;
				}
			}
			
			return false;
		}
	};
	
	template
		<
			class addressType,
			int maxSubscribersCount						// максимальное число подписчиков
		>
	class SubscribersCounters
	{
	public:
		typedef addressType AddressType;
		static const int MaxSubscribersCount = maxSubscribersCount;
		
		struct InfoType
		{
			AddressType Address;
			int PacketId;
			int Counter;
			
			InfoType()
			{
				Address = -1;
				PacketId = -1;
				Counter = 0;
			}
		};
	protected:
		InfoType _subscribersList[MaxSubscribersCount];
		int _subscribersCount;
	public:
		SubscribersCounters()
		{
			_subscribersCount = 0;
		}
		
		void Add(int id)
		{
			if (Exists(id))
			{
				return;
			}
			
			if (_subscribersCount >= MaxSubscribersCount)
			{
				return;
			}
			
			_subscribersList[_subscribersCount] = InfoType();
			_subscribersList[_subscribersCount].Address = id;
			_subscribersCount++;
		}
		
		bool Remove(int id)
		{
			bool removed = false;
			
			for (int i = 0; i < _subscribersCount; i++)
			{
				if (_subscribersList[i].Address == id)
				{
					_subscribersCount--;
					for (int j = i; j < _subscribersCount; j++)
					{
						_subscribersList[j] = _subscribersList[j + 1];
					}
					
					removed = true;
					break;
				}
			}
			
			return removed;
		}
		
		void Update(int id, int packetId)
		{
			// если нет, добавим
			Add(id);
			
			for (int i = 0; i < _subscribersCount; i++)
			{
				if (_subscribersList[i].Address == id)
				{
					_subscribersList[i].PacketId = packetId;
					break;
				}
			}
		}
		
		int Update(int id, int packetId, bool delivered)
		{
			int current = 0;
			for (int i = 0; i < _subscribersCount; i++)
			{
				if (_subscribersList[i].Address == id)
				{
					if (_subscribersList[i].PacketId == packetId)
					{
						// если доставлено сбрасываем счётчик, если нет, увеличиваем
						if (delivered)
						{
							_subscribersList[i].Counter = 0;
						}
						else
						{
							_subscribersList[i].Counter++;
						}
					}
					current = _subscribersList[i].Counter;
					break;
				}
			}
			
			return current;
		}
		
		bool Exists(int id)
		{
			for (int i = 0; i < _subscribersCount; i++)
			{
				if (_subscribersList[i].Address == id)
				{
					return true;
				}
			}
			
			return false;
		}
		
		void RemoveAllButThese(AddressType *list, int count)
		{
			if (list == 0)
			{
				return;
			}
			
			for (int i = 0; i < _subscribersCount;)
			{
				bool found = false;
				
				for (int j = 0; j < count; j++)
				{
					if (_subscribersList[i].Address == list[j])
					{
						found = true;
						break;
					}
				}
				
				if (!found)
				{
					Remove(_subscribersList[i].Address);
					i = 0;
				}
				else
				{
					i++;
				}
			}
		}
	};
	
	template
		<
			int eventId,								// Идентификатор события
			int maxSubscriptionsCount,					// максимальное число подписок
			int maxSubscribersCount						// максимальное число подписчиков
		>
	class SubscriptionService
	{
	public:
		static const int EventId = eventId;
		static const int MaxSubscriptionsCount = maxSubscriptionsCount;
		typedef SubscriptionItem<EventId, maxSubscribersCount> SubscriptionItemType;
		static const int MaxSubscribersCount = SubscriptionItemType::MaxSubscribersCount;
		typedef typename SubscriptionItemType::FunctionCodeType FunctionCodeType;
		typedef typename SubscriptionItemType::MessageCodeType MessageCodeType;
		typedef typename SubscriptionItemType::AddressType AddressType;
		static const int KeepAlivePeriod = 60; // Период рассылки "я жив" подписчикам
		typedef SubscribersCounters<AddressType, MaxSubscribersCount> SubscribersCountersType;
		static const int DeleteThreshold = 5; // Порог числа неполученных ответов, после которого подписчик удаляется
	protected:
		SubscriptionItemType _subscriptions[MaxSubscriptionsCount];
		AddressType _subscribersList[MaxSubscribersCount];	// список подписчиков на кого оформлена хотя бы одна подписка
		int _subscribersCount;
		int _keepAliveCounter;
		SubscribersCountersType _subscribersCounters;
	public:
		SubscriptionService()
		{
			_keepAliveCounter = 0;
			_subscribersCount = 0;
		}
		
		int GetSubscriptionsList(FunctionCodeType *functionCodes, MessageCodeType *messageCodes, int maxCount)
		{
			int count = 0;
			
			for (int i = 0; i < MaxSubscriptionsCount; i++)
			{
				if (!_subscriptions[i].Empty())
				{
					if (functionCodes)
					{
						functionCodes[count] = _subscriptions[i].FunctionCode();
					}
					
					if (messageCodes)
					{
						messageCodes[count] = _subscriptions[i].MessageCode();
					}
					
					count++;
				}
			}
			
			return count;
		}
		
		static void GetSubscriptionsList(void *objectPtr, FunctionCodeType *functionCodes, MessageCodeType *messageCodes, int maxCount, int &count)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			SubscriptionService *im = (SubscriptionService *)objectPtr;
			count = im->GetSubscriptionsList(functionCodes, messageCodes, maxCount);
		}
		
		static void GetSubscribersList(void *objectPtr, FunctionCodeType functionCode, MessageCodeType messageCode, AddressType *subscribers, int maxCount, int &count)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			SubscriptionService *im = (SubscriptionService *)objectPtr;
			count = im->GetSubscribers(functionCode, messageCode, subscribers, maxCount);
		}
		
		void AddSubscription(AddressType subscriber, FunctionCodeType functionCode, MessageCodeType messageCode, bool &invalidFunction, bool &invalidMessage, bool &noMoreSapce)
		{
			int index = Find(functionCode, messageCode);
			if (index < 0)
			{
				index = Add(functionCode, messageCode);
			}
			
			if(index >= 0)
			{
				if (!_subscriptions[index].Add(subscriber))
				{
					noMoreSapce = true;
				}
				else
				{
					UpdateSubscribersList();
				}
			}
			else
			{
				noMoreSapce = true;
			}
		}
		
		static void AddSubscription(void *objectPtr, AddressType subscriber, FunctionCodeType functionCode, MessageCodeType messageCode, bool &invalidFunction, bool &invalidMessage, bool &noMoreSapce)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			SubscriptionService *im = (SubscriptionService *)objectPtr;
			im->AddSubscription(subscriber, functionCode, messageCode, invalidFunction, invalidMessage, noMoreSapce);
		}
		
		void RemoveSubscription(AddressType subscriber, FunctionCodeType functionCode, MessageCodeType messageCode)
		{
			int index = Find(functionCode, messageCode);
			if(index >= 0)
			{
				_subscriptions[index].Remove(subscriber);
				UpdateSubscribersList();
			}
		}
		
		static void RemoveSubscription(void *objectPtr, AddressType subscriber, FunctionCodeType functionCode, MessageCodeType messageCode)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			SubscriptionService *im = (SubscriptionService *)objectPtr;
			im->RemoveSubscription(subscriber, functionCode, messageCode);
		}
		
		int GetSubscribers(FunctionCodeType functionCode, MessageCodeType messageCode, AddressType *buffer, int maxCount)
		{
			int count = 0;
			
			int index = Find(functionCode, messageCode);
			if (index >= 0)
			{
				count = _subscriptions[index].Get(buffer, maxCount);
			}
			
			return count;
		}
		
		void AddSubscribers(FunctionCodeType functionCode, MessageCodeType messageCode, AddressType *subscribers, int sibscribersCount, bool &invalidFunction, bool &invalidMessage, bool &noMoreSapce)
		{
			if (subscribers == 0)
			{
				return;
			}
			
			int index = Find(functionCode, messageCode);
			if (index < 0)
			{
				index = Add(functionCode, messageCode);
			}
			
			if(index >= 0)
			{
				bool update = false;
				
				for (int i = 0; i < sibscribersCount; i++)
				{
					if (!_subscriptions[index].Add(subscribers[i]))
					{
						noMoreSapce = true;
					}
					else
					{
						update = true;
					}
				}
				
				if (update)
				{
					UpdateSubscribersList();
				}
			}
			else
			{
				noMoreSapce = true;
			}
		}
		
		static void AddSubscribers(void *objectPtr, FunctionCodeType functionCode, MessageCodeType messageCode, AddressType *subscribers, int sibscribersCount, bool &invalidFunction, bool &invalidMessage, bool &noMoreSapce)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			SubscriptionService *im = (SubscriptionService *)objectPtr;
			im->AddSubscribers(functionCode, messageCode, subscribers, sibscribersCount, invalidFunction, invalidMessage, noMoreSapce);
		}
		
		void RemoveSubscribers(FunctionCodeType functionCode, MessageCodeType messageCode, AddressType *subscribers, int sibscribersCount, bool &invalidFunction, bool &invalidMessage)
		{
			if (subscribers == 0)
			{
				return;
			}
			
			int index = Find(functionCode, messageCode);
			if (index < 0)
			{
				index = Add(functionCode, messageCode);
			}
			
			if(index >= 0)
			{
				bool update = _subscriptions[index].Remove(subscribers, sibscribersCount);

				if (update)
				{
					UpdateSubscribersList();
				}
			}
		}
		
		static void RemoveSubscribers(void *objectPtr, FunctionCodeType functionCode, MessageCodeType messageCode, AddressType *subscribers, int sibscribersCount, bool &invalidFunction, bool &invalidMessage)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			SubscriptionService *im = (SubscriptionService *)objectPtr;
			im->RemoveSubscribers(functionCode, messageCode, subscribers, sibscribersCount, invalidFunction, invalidMessage);
		}
		
		// удалить всех подписчиков кроме указанных
		void RemoveAllSubscribersButThese(AddressType *subscribers, int count)
		{
			bool changed = false;
			
			for (int i = 0; i < MaxSubscriptionsCount; i++)
			{
				if (!_subscriptions[i].Empty())
				{
					_subscriptions[i].RemoveAllSubscribersButThese(subscribers, count);
					changed = true;
				}
			}
			
			if (changed)
			{
				UpdateSubscribersList();
			}
		}
		
		void OnTick(int tickFrequency)
		{
			_keepAliveCounter++;
			if (_keepAliveCounter >= tickFrequency * KeepAlivePeriod)
			{
				_keepAliveCounter = 0;
				
				for (int i = 0; i < _subscribersCount; i++)
				{
					Rblib::Event e(EventId, EventCodeKeepAlive, _subscribersList[i]);
					Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e);
				}
			}
		}
		
		static void OnKeepAliveTransmited(void *objectPtr, int address, int packetId)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			SubscriptionService *im = (SubscriptionService *)objectPtr;
			im->OnKeepAliveTransmited(address, packetId);
		}
		
		void OnKeepAliveTransmited(int address, int packetId)
		{
			_subscribersCounters.Update(address, packetId);
		}
		
		void OnTransmitComplete(int address, int packetId, bool delivered)
		{
			int currentCounter = _subscribersCounters.Update(address, packetId, delivered);
			if (currentCounter >= DeleteThreshold)
			{
				for (int i = 0; i < MaxSubscriptionsCount; i++)
				{
					if (!_subscriptions[i].Empty())
					{
						_subscriptions[i].Remove(address);
					}
				}
				
				UpdateSubscribersList();
			}
		}
	protected:
		int Find(FunctionCodeType functionCode, MessageCodeType messageCode)
		{
			for (int i = 0; i < MaxSubscriptionsCount; i++)
			{
				if (_subscriptions[i].Equal(functionCode, messageCode))
				{
					return i;
				}
			}
			
			return -1;
		}
		
		int FindEmpty()
		{
			for (int i = 0; i < MaxSubscriptionsCount; i++)
			{
				if (_subscriptions[i].Empty())
				{
					return i;
				}
			}
			
			return -1;
		}
		
		int Add(FunctionCodeType functionCode, MessageCodeType messageCode)
		{
			int index = FindEmpty();
			if (index < 0)
			{
				return -1;
			}
			
			_subscriptions[index].Reset();
			_subscriptions[index].Set(functionCode, messageCode);
			
			Rblib::Event e1(EventId, EventCodeSubscriptionAdded, functionCode, messageCode);
			Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e1);
			
			Rblib::Event e2(EventId, EventCodeSubscriptionsListChanged);
			Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e2);
			
			return index;
		}
		
		void UpdateSubscribersList()
		{
			_subscribersCount = 0;
			
			for (int i = 0; i < MaxSubscriptionsCount; i++)
			{
				if (!_subscriptions[i].Empty())
				{
					AddressType list[MaxSubscribersCount];
					int count = _subscriptions[i].Get(list, MaxSubscribersCount);
					
					for (int j = 0; j < count; j++)
					{
						int id = list[j];
						
						if (!FindSubscriber(id))
						{
							if (_subscribersCount < MaxSubscribersCount)
							{
								_subscribersList[_subscribersCount] = id;
								_subscribersCount++;
							}
						}
					}
				}
			}
			
			_subscribersCounters.RemoveAllButThese(_subscribersList, _subscribersCount);
		}
		
		bool FindSubscriber(int id)
		{
			for (int i = 0; i < _subscribersCount; i++)
			{
				if (_subscribersList[i] == id)
				{
					return true;
				}
			}
			
			return false;
		}
	};
}
}
}
}
}

#endif