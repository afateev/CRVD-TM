///////////////////////////////////////////////////////////////////////////////
//	Класс реализации работы с функцие устройтсва "служба подписки" по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_FUNCTIONS_SUBSCRIPTIONSERVICE_H
#define G1_FUNCTIONS_SUBSCRIPTIONSERVICE_H

#include "../Function.h"
#include "../../../Protocol/G1/Function/SubscriptionService/FunctionData.h"
#include "../../../Protocol/G1/Function/Camera/FunctionData.h"
#include "../../../Protocol/G1/Function/WirebreakSensor/FunctionData.h"
#include "../../../Protocol/G1/Function/Relay/FunctionData.h"

namespace Rblib
{
namespace StdDevice
{
namespace G1
{
namespace Functions
{
	template
		<
			class Base,
			int timerTickCountPerSecond
		>
	class SubscriptionService : public Base
	{
	public:
		static const int TimerTickCountPerSecond = timerTickCountPerSecond;
		static const int KeepAlivePeriod = 60; // Период рассылки "я жив" подписчикам
		
		enum State
		{
			StateDisabled = 0,
			StateEnabled,
			StateTxGetSubscriptionsAndSubscribersList,
			StateTxGetSubscriptionsAndSubscribersListWaitDelivery,
			StateRxSubscriptionsAndSubscribersListWait,
			StateTxAddSubscription,
			StateTxAddSubscriptionWaitDelivery,
			StateRxAddSubscriptionResultWait,
			StateTxRemoveSubscription,
			StateTxRemoveSubscriptionWaitDelivery,
			StateRxRemoveSubscriptionResultWait,
			StateTxAddSubscribers,
			StateTxAddSubscribersWaitDelivery,
			StateRxAddSubscribersResultWait,
			StateTxRemoveSubscribers,
			StateTxRemoveSubscribersWaitDelivery,
			StateRxRemoveSubscribersResultWait,
			StateIdle
		};
		
		static const int MaxSubscriptionCount = 10;
		static const int MaxSubscribersCount = 10;
		
		struct Subscription
		{
			unsigned char FunctionCode;
			unsigned char MessageCode;
			unsigned char Subscribers[MaxSubscribersCount];
			unsigned char SubscribersCount;
			
			Subscription()
			{
				FunctionCode = 0;
				MessageCode = 0;
				SubscribersCount = 0;
			}
			
			Subscription(unsigned char functionCode, unsigned char messageCode, unsigned char *subscribers, unsigned char subscribersCount)
			{
				FunctionCode = functionCode;
				MessageCode = messageCode;
				SubscribersCount = subscribersCount;
				
				if (SubscribersCount > MaxSubscribersCount)
				{
					SubscribersCount = MaxSubscribersCount;
				}
				
				if (subscribers == 0)
				{
					SubscribersCount = 0;
				}
				
				for (int i = 0; i < SubscribersCount; i++)
				{
					Subscribers[i] = subscribers[i];
				}
			}
			
			void UpdateSubscribersList(unsigned char *subscribers, unsigned char subscribersCount)
			{
				SubscribersCount = subscribersCount;
				
				if (SubscribersCount > MaxSubscribersCount)
				{
					SubscribersCount = MaxSubscribersCount;
				}
				
				if (subscribers == 0)
				{
					SubscribersCount = 0;
				}
				
				for (int i = 0; i < SubscribersCount; i++)
				{
					Subscribers[i] = subscribers[i];
				}
			}
			
			void AppendSubscribers(unsigned char *subscribers, unsigned char subscribersCount)
			{
				if (subscribers == 0)
				{
					return;
				}
				
				for (int i = 0; i < subscribersCount; i++)
				{
					bool present = false;
					
					for (int j = 0; j < SubscribersCount; j++)
					{
						if (Subscribers[j] == subscribers[i])
						{
							present = true;
							break;
						}
					}
					
					if (!present)
					{
						if (SubscribersCount < MaxSubscribersCount)
						{
							Subscribers[SubscribersCount] = subscribers[i];
							SubscribersCount++;
						}
					}
				}
			}
			
			bool ContainsSubscriber(unsigned char subscriber)
			{
				for (int i = 0; i < SubscribersCount; i++)
				{
					if (Subscribers[i] == subscriber)
					{
						return true;
					}
				}
				
				return false;
			}
			
			bool operator == (Subscription &other)
			{
				return FunctionCode == other.FunctionCode && MessageCode == other.MessageCode;
			}
		};
		
		struct SubscriptionShort
		{
			unsigned char FunctionCode;
			unsigned char MessageCode;
			
			SubscriptionShort(unsigned char functionCode, unsigned char messageCode)
			{
				FunctionCode = functionCode;
				MessageCode = messageCode;
			}
			
			bool operator == (Subscription &other)
			{
				return FunctionCode == other.FunctionCode && MessageCode == other.MessageCode;
			}
		};
		
		typedef vector_static<Subscription, MaxSubscriptionCount> SubscriptionListType;
		typedef vector_static<SubscriptionShort, MaxSubscriptionCount> ShortSubscriptionListType;
	protected:
		int _gateId;
		State _state;
		bool _getSubscriptionsList;
		ShortSubscriptionListType _necessary;
		SubscriptionListType _current;
		SubscriptionListType _add;
		Subscription _adding;
		SubscriptionListType _remove;
		Subscription _removing;
		SubscriptionListType _deleting;
		int _keepAliveCounter;
		static const int BufferSize = 2 + 2 * MaxSubscriptionCount;
		static unsigned char _buffer[BufferSize];
	public:
		SubscriptionService()
		{
			_gateId = -1;
		}
		
		virtual Protocol::G1::Device::FunctionCode FunctionCode()
		{
			return Protocol::G1::Device::FunctionCodeSubscriptionService;
		}
		
		virtual void Reset()
		{
			Base::Reset();
			
			_getSubscriptionsList = false;
			
			_necessary.clear();
			_current.clear();
			_add.clear();
			_remove.clear();
			_deleting.clear();
			_keepAliveCounter = false;
			_state = StateDisabled;
		}
		
		virtual void SetEnabled()
		{
			Base::SetEnabled();
			_state = StateEnabled;
			
			RequestSubscriptions();
		}
		
		int Get(unsigned char functionCode, unsigned char messageCode, unsigned char *result, int maxCount)
		{
			if (result == 0 || maxCount < 1)
			{
				return 0;
			}
			
			for (int i = 0; i < _current.size(); i++)
			{
				if (_current[i].FunctionCode == functionCode && _current[i].MessageCode == messageCode)
				{
					int count = _current[i].SubscribersCount;
					// вдруг приемный буфер не может столько вместить
					if (count > maxCount)
					{
						count = maxCount;
					}
					
					for (int j = 0; j < count; j++)
					{
						result[j] = _current[i].Subscribers[j];
					}
					
					return count;
				}
			}
			
			return 0;
		}
		
		bool Add(unsigned char functionCode, unsigned char messageCode, unsigned char *subscribers, int subscribersCount)
		{
			if (subscribers == 0)
			{
				return false;
			}
			
			Subscription subscrinption(functionCode, messageCode, subscribers, subscribersCount);
			Append(subscrinption, _add);
			
			return true;
		}
				  
		bool Remove(unsigned char functionCode, unsigned char messageCode, unsigned char *subscribers, int subscribersCount)
		{
			if (subscribers == 0)
			{
				return false;
			}
			
			Subscription subscrinption(functionCode, messageCode, subscribers, subscribersCount);
			Append(subscrinption, _remove);
			
			return true;
		}
		
		void AddPermanentSubscription(unsigned char functionCode, unsigned char messageCode)
		{
			// проверяем что ещё нет в списке такой подписки
			for (int i = 0; i < _necessary.size(); i++)
			{
				if (_necessary[i].FunctionCode == functionCode && _necessary[i].MessageCode == messageCode)
				{
					return;
				}
			}
			
			_necessary.push_back(SubscriptionShort(functionCode, messageCode));
		}
		
		void RemovePermanentSubscription(unsigned char functionCode, unsigned char messageCode)
		{
			for (int i = 0; i < _necessary.size(); i++)
			{
				if (_necessary[i].FunctionCode == functionCode && _necessary[i].MessageCode == messageCode)
				{
					_necessary.erase(&_necessary[i]);
				}
			}
		}
		
		void SetGateId(int id)
		{
			_gateId = id;
		}
		
		virtual bool WaitResponseFlag()
		{
			bool res = Base::WaitResponseFlag();
			
			res |= _getSubscriptionsList || !_add.empty() || !_remove.empty();
			
			return res;
		}
		
		virtual void OnTick()
		{
			Base::OnTick();
			
			switch(_state)
			{
			case StateDisabled:
				{
				}
				break;
			case StateEnabled:
				{
					if (!_add.empty())
					{
						bool oneSubcriber = true;
						int _prev = -1;
						for (int i = 0; i < _add.size(); i++)
						{
							if (_add[i].SubscribersCount > 1)
							{
								oneSubcriber = false;
								break;
							}
							
							if (i == 0)
							{
								_prev = _add[i].Subscribers[0];
							}
							else
							{
								if (_prev != _add[i].Subscribers[0])
								{
									oneSubcriber = false;
									break;
								}
							}
						}
						
						if (oneSubcriber)
						{
							_state = StateTxAddSubscription;
						}
						else
						{
							_state = StateTxAddSubscribers;
						}
						break;
					}
					
					if (!_remove.empty())
					{
						bool oneSubcriber = true;
						int _prev = -1;
						for (int i = 0; i < _remove.size(); i++)
						{
							if (_remove[i].SubscribersCount > 1)
							{
								oneSubcriber = false;
								break;
							}
							
							if (i == 0)
							{
								_prev = _remove[i].Subscribers[0];
							}
							else
							{
								if (_prev != _remove[i].Subscribers[0])
								{
									oneSubcriber = false;
									break;
								}
							}
						}
						
						if (oneSubcriber)
						{
							_state = StateTxRemoveSubscription;
						}
						else
						{
							_state = StateTxRemoveSubscribers;
						}
						break;
					}
					
					if (_getSubscriptionsList)
					{
						_state = StateTxGetSubscriptionsAndSubscribersList;
						break;
					}
					
					for (int i = 0; i < _necessary.size(); i++)
					{
						if (!Contains(_necessary[i].FunctionCode, _necessary[i].MessageCode, _gateId, _current))
						{
							unsigned char subscribers[] = {_gateId};
							Add(_necessary[i].FunctionCode, _necessary[i].MessageCode, subscribers, sizeof(subscribers));
							RequestSubscriptions();
						}
					}
					
					for (int i = 0; i < _current.size(); i++)
					{
						if (_current[i].ContainsSubscriber(_gateId))
						{
							bool contains = false;
							for (int j = 0; j < _necessary.size(); j++)
							{
								if (_current[i].FunctionCode == _necessary[j].FunctionCode && _current[i].MessageCode == _necessary[j].MessageCode)
								{
									contains = true;
									break;
								}
							}
							
							if (!contains)
							{
								unsigned char subscribers[] = {_gateId};
								Remove(_current[i].FunctionCode, _current[i].MessageCode, subscribers, sizeof(subscribers));
								RequestSubscriptions();
							}
						}
					}
					
					// давно не было пакетов от отправителя, а нас должны быть подписки
					if ((_keepAliveCounter > TimerTickCountPerSecond * KeepAlivePeriod * 1.5) && _current.size() > 0)
					{
						// удостоверимся что подписки существуют
						RequestSubscriptions();
					}
					else
					{
						_keepAliveCounter++;
					}
				}
				break;
			case StateTxGetSubscriptionsAndSubscribersList:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = Base::DefaultTimeout;
					
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::SubscriptionService::MessageCodeGetSubscriptionsAndSubscribersList, 0, 0, deliveryTimeout);
					_state = StateTxGetSubscriptionsAndSubscribersListWaitDelivery;
				}
				break;
			case StateTxGetSubscriptionsAndSubscribersListWaitDelivery:
			case StateRxSubscriptionsAndSubscribersListWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						_state = StateTxGetSubscriptionsAndSubscribersList;
					}
				}
				break;
			case StateTxAddSubscription:
				{
					int count = _add.size();
					if (count > 0)
					{
						if (_add[0].SubscribersCount > 0)
						{
							Base::_waitTimeout = 60 * TimerTickCountPerSecond;
							int deliveryTimeout = Base::DefaultTimeout;
							
							_buffer[0] = _add[0].Subscribers[0];
							_buffer[1] = count;
							for (int i = 0; i < count; i++)
							{
								_buffer[2 + i] = _add[i].FunctionCode;
								_buffer[2 + count + i] = _add[i].MessageCode;
							}
							
							int dataLen = 2 + 2 * count;
							
							Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::SubscriptionService::MessageCodeAddSubscription, _buffer, dataLen, deliveryTimeout);
							_state = StateTxAddSubscriptionWaitDelivery;
						}
						else
						{
							_state = StateIdle;
						}
					}
					else
					{
						_state = StateIdle;
					}
				}
				break;
			case StateTxAddSubscriptionWaitDelivery:
			case StateRxAddSubscriptionResultWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						_state = StateTxAddSubscription;
					}
				}
				break;
			case StateTxRemoveSubscription:
				{
					int count = _remove.size();
					if (count > 0)
					{
						if (_remove[0].SubscribersCount > 0)
						{
							Base::_waitTimeout = 60 * TimerTickCountPerSecond;
							int deliveryTimeout = Base::DefaultTimeout;
							
							_buffer[0] = _remove[0].Subscribers[0];
							_buffer[1] = count;
							for (int i = 0; i < count; i++)
							{
								_buffer[2 + i] = _remove[i].FunctionCode;
								_buffer[2 + count + i] = _remove[i].MessageCode;
							}
							
							int dataLen = 2 + 2 * count;
							
							Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::SubscriptionService::MessageCodeRemoveSubscription, _buffer, dataLen, deliveryTimeout);
							_state = StateTxRemoveSubscriptionWaitDelivery;
						}
						else
						{
							_state = StateIdle;
						}
					}
					else
					{
						_state = StateIdle;
					}
				}
				break;
			case StateTxRemoveSubscriptionWaitDelivery:
			case StateRxRemoveSubscriptionResultWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						_state = StateTxRemoveSubscription;
					}
				}
				break;
			case StateTxAddSubscribers:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = Base::DefaultTimeout;
					
					_adding = _add.front();
					_add.erase(_add.begin());
					
					_buffer[0] = _adding.FunctionCode;
					_buffer[1] = _adding.MessageCode;
					_buffer[2] = _adding.SubscribersCount;
					
					for (int i = 0; i < _adding.SubscribersCount; i++)
					{
						_buffer[3 + i] = _adding.Subscribers[i];
					}
					
					int dataLen = 3 + _adding.SubscribersCount;
					
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::SubscriptionService::MessageCodeAddSubscribers, _buffer, dataLen, deliveryTimeout);
					_state = StateTxAddSubscribersWaitDelivery;
				}
				break;
			case StateTxAddSubscribersWaitDelivery:
			case StateRxAddSubscribersResultWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						Append(_adding, _add);
						_state = StateTxAddSubscribers;
					}
				}
				break;
			case StateTxRemoveSubscribers:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = Base::DefaultTimeout;
					
					_removing = _remove.front();
					_remove.erase(_remove.begin());
					
					_buffer[0] = _removing.FunctionCode;
					_buffer[1] = _removing.MessageCode;
					_buffer[2] = _removing.SubscribersCount;
					
					for (int i = 0; i < _removing.SubscribersCount; i++)
					{
						_buffer[3 + i] = _removing.Subscribers[i];
					}
					
					int dataLen = 3 + _removing.SubscribersCount;
					
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::SubscriptionService::MessageCodeRemoveSubscribers, _buffer, dataLen, deliveryTimeout);
					_state = StateTxRemoveSubscribersWaitDelivery;
				}
				break;
			case StateTxRemoveSubscribersWaitDelivery:
			case StateRxRemoveSubscribersResultWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						Append(_removing, _remove);
						_state = StateTxRemoveSubscribers;
					}
				}
				break;
			case StateIdle:
				{
					_state = StateEnabled;
					// что-то делали с подписками, логично начать отсчёт заново
					_keepAliveCounter = 0;
				}
				break;
			}
		}
		
		int SendCommandToFunction(Protocol::G1::Function::SubscriptionService::MessageCode code, void *data, int dataCount, int timeout)
		{
			Protocol::G1::Function::SubscriptionService::OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(Base::_functionData, sizeof(Base::_functionData));
			
			int packetId = 0;
			Base::SendCommandToFunctionCallback.Call(Base::_functionData, functionDataLen, timeout, packetId);
			return packetId;
		}
		
		virtual void OnTxComplete(int packetId, bool delivered)
		{
			Base::OnTxComplete(packetId, delivered);
			
			bool waitComplete = packetId == Base::_waitPacketId;
			// дождались завершения передачи
			if (waitComplete)
			{
				switch(_state)
				{
				case StateTxGetSubscriptionsAndSubscribersListWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxSubscriptionsAndSubscribersListWait;
						}
						else
						{
							_state = StateTxGetSubscriptionsAndSubscribersList;
						}
					}
					break;
				case StateTxAddSubscriptionWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxAddSubscriptionResultWait;
						}
						else
						{
							_state = StateTxAddSubscription;
						}
					}
					break;
				case StateTxRemoveSubscriptionWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxRemoveSubscriptionResultWait;
						}
						else
						{
							_state = StateTxRemoveSubscription;
						}
					}
					break;
				case StateTxAddSubscribersWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxAddSubscribersResultWait;
						}
						else
						{
							Append(_adding, _add);
							_state = StateTxAddSubscribers;
						}
					}
					break;
				case StateTxRemoveSubscribersWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxRemoveSubscribersResultWait;
						}
						else
						{
							Append(_removing, _remove);
							_state = StateTxRemoveSubscribers;
						}
					}
					break;
				}
			}
		}
		
		virtual void OnCommandFromFunction(unsigned char *data, int dataCount)
		{
			Base::OnCommandFromFunction(data, dataCount);
			
			Protocol::G1::Function::SubscriptionService::InputMessageFormat command(data, dataCount);
			if (command.Valid())
			{
				if (command.Data.Valid())
				{
					switch(command.Data.Code)
					{
					case Protocol::G1::Function::SubscriptionService::MessageCodeSubscriptionsAndSubscribersList:
						{
							if (command.Data.DataLen >= 1)
							{
								unsigned char *data = (unsigned char *)command.Data.Data;
								if (data)
								{
									int j = 0;
									int subscriptionsCount = data[j++];
									
									_current.clear();
									
									for (int i = 0; i < subscriptionsCount; i++)
									{
										unsigned char functionCode = data[j++];
										unsigned char messageCode = data[j++];
										unsigned char subscribersCount = data[j++];
										
										_current.push_back(Subscription(functionCode, messageCode, &data[j], subscribersCount));
										j += subscribersCount;
									}
								}
							}
							
							switch(_state)
							{
							case StateRxSubscriptionsAndSubscribersListWait:
								{
									_getSubscriptionsList = false;
									_state = StateIdle;
								}
								break;
							}
						}
						break;
					case Protocol::G1::Function::SubscriptionService::MessageCodeKeepAlive:
						{
							_keepAliveCounter = 0;
						}
						break;
					case Protocol::G1::Function::SubscriptionService::MessageCodeAddSubscriptionResult:
						{
							// TODO Надо проанализировать результат, но сейчас не ясно что делать в случае ошибок,
							// поэтому будем считать что если ответ пришел, то всё ОК
							
							switch(_state)
							{
							case StateRxAddSubscriptionResultWait:
								{
									_add.clear();
									_state = StateIdle;
								}
								break;
							}
						}
						break;
					case Protocol::G1::Function::SubscriptionService::MessageCodeRemoveSubscriptionResult:
						{
							// TODO Надо проанализировать результат, но сейчас не ясно что делать в случае ошибок,
							// поэтому будем считать что если ответ пришел, то всё ОК
							
							switch(_state)
							{
							case StateRxRemoveSubscriptionResultWait:
								{
									_remove.clear();
									_state = StateIdle;
								}
								break;
							}
						}
						break;
					case Protocol::G1::Function::SubscriptionService::MessageCodeAddSubscribersResult:
						{
							// TODO Надо проанализировать результат, но сейчас не ясно что делать в случае ошибок,
							// поэтому будем считать что если ответ пришел, то всё ОК
							
							switch(_state)
							{
							case StateRxAddSubscribersResultWait:
								{
									_state = StateIdle;
								}
								break;
							}
						}
						break;
					case Protocol::G1::Function::SubscriptionService::MessageCodeRemoveSubscribersResult:
						{
							// TODO Надо проанализировать результат, но сейчас не ясно что делать в случае ошибок,
							// поэтому будем считать что если ответ пришел, то всё ОК
							
							switch(_state)
							{
							case StateRxRemoveSubscribersResultWait:
								{
									_state = StateIdle;
								}
								break;
							}
						}
						break;
					}
				}
			}
		}
		
		void RequestSubscriptions()
		{
			_getSubscriptionsList = true;
		}
		
		bool Contains(Subscription item, SubscriptionListType list)
		{
			for (int i = 0; i < list.size(); i++)
			{
				if (list[i] == item)
				{
					return true;
				}
			}
			
			return false;
		}
		
		bool Contains(unsigned char functionCode, unsigned char messageCode, unsigned char subscriber, SubscriptionListType list)
		{
			for (int i = 0; i < list.size(); i++)
			{
				if (list[i].FunctionCode == functionCode && list[i].MessageCode == messageCode)
				{
					for (int j = 0; j < list[i].SubscribersCount; j++)
					{
						if (list[i].Subscribers[j] == subscriber)
						{
							return true;
						}
					}
				}
			}
			
			return false;
		}
		
		void Append(Subscription item, SubscriptionListType &list)
		{
			bool found = false;
			
			// если ест похожий элемент, то дополним
			for (int i = 0; i < list.size(); i++)
			{
				if (list[i] == item)
				{
					list[i].AppendSubscribers(item.Subscribers, item.SubscribersCount);
					found = true;
				}
			}
			
			// если нет, то добавим
			if (!found)
			{
				list.push_back(item);
			}
		}
	};
	
	template <class Base, int timerTickCountPerSecond>
		unsigned char SubscriptionService<Base, timerTickCountPerSecond>::_buffer[SubscriptionService<Base, timerTickCountPerSecond>::BufferSize];
}
}
}
}

#endif