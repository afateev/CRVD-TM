///////////////////////////////////////////////////////////////////////////////
//	Класс реализации работы с функций устройтсва "служба действия" по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_FUNCTIONS_ACTIONSERVICE_H
#define G1_FUNCTIONS_ACTIONSERVICE_H

#include "../Function.h"
#include "../../../Protocol/G1/Function/ActionService/FunctionData.h"

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
	class ActionService : public Base
	{
	public:
		static const int TimerTickCountPerSecond = timerTickCountPerSecond;
		static const int MaxSlaveCount = 20;
		
		enum State
		{
			StateDisabled = 0,
			StateEnabled,
			StateTxGetSlaveList,
			StateTxGetSlaveListWaitDelivery,
			StateRxSlaveListWait,
			StateTxAddSlave,
			StateTxAddSlaveWaitDelivery,
			StateRxAddSlaveResultWait,
			StateTxRemoveSlave,
			StateTxRemoveSlaveWaitDelivery,
			StateRxRemoveSlaveResultWait,
			StateIdle
		};
		
		typedef vector_static<unsigned char, MaxSlaveCount> ListType;
	protected:
		bool _getSlaveList;
		ListType _currentSlaveList;
		ListType _addSlave;
		ListType _addingSlave;
		ListType _removeSlave;
		ListType _removingSlave;
		State _state;
		
		static const int BufferSize = 1 + MaxSlaveCount;
		static unsigned char _buffer[BufferSize];
	public:
		ActionService()
		{
			_getSlaveList = false;
		}
		
		virtual Protocol::G1::Device::FunctionCode FunctionCode()
		{
			return Protocol::G1::Device::FunctionCodeActionService;
		}
		
		virtual void Reset()
		{
			Base::Reset();
			
			_state = StateDisabled;
			_currentSlaveList.clear();
		}
		
		virtual void SetEnabled()
		{
			Base::SetEnabled();
			
			_state = StateEnabled;
			
			RequestSlaveList();
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
					if (_getSlaveList)
					{
						_state = StateTxGetSlaveList;
						break;
					}
					
					if (!_addSlave.empty())
					{
						_state = StateTxAddSlave;
						break;
					}
					
					if (!_removeSlave.empty())
					{
						_state = StateTxRemoveSlave;
						break;
					}
				}
				break;
			case StateTxGetSlaveList:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = Base::DefaultTimeout;
					
					unsigned char paramsCount = 0;
					
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::ActionService::MessageCodeGetSlaveList, &paramsCount, sizeof(paramsCount), deliveryTimeout);
					_state = StateTxGetSlaveListWaitDelivery;
				}
				break;
			case StateTxGetSlaveListWaitDelivery:
			case StateRxSlaveListWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						_state = StateTxGetSlaveList;
					}
				}
				break;
			case StateTxAddSlave:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = Base::DefaultTimeout;
					
					unsigned char count = _addSlave.size();
					_addingSlave.clear();
					
					_buffer[0] = count;
					
					for (int i = 0; i < _addSlave.size(); i++)
					{
						_buffer[1 + i] = _addSlave[i];
						// запоминаем для какого списка отправили команду, вдруг пока будем доставлять список изменится
						_addingSlave.push_back(_addSlave[i]);
					}
					
					int dataLen = 1 + count;
					
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::ActionService::MessageCodeAddSlave, _buffer, dataLen, deliveryTimeout);
					_state = StateTxAddSlaveWaitDelivery;
				}
				break;
			case StateTxAddSlaveWaitDelivery:
			case StateRxAddSlaveResultWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						_state = StateTxAddSlave;
					}
				}
				break;
			case StateTxRemoveSlave:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = Base::DefaultTimeout;
					
					unsigned char count = _removeSlave.size();
					_removingSlave.clear();
					
					_buffer[0] = count;
					
					for (int i = 0; i < _removeSlave.size(); i++)
					{
						_buffer[1 + i] = _removeSlave[i];
						// запоминаем для какого списка отправили команду, вдруг пока будем доставлять список изменится
						_removingSlave.push_back(_removeSlave[i]);
					}
					
					int dataLen = 1 + count;
					
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::ActionService::MessageCodeRemoveSlave, _buffer, dataLen, deliveryTimeout);
					_state = StateTxRemoveSlaveWaitDelivery;
				}
				break;
			case StateTxRemoveSlaveWaitDelivery:
			case StateRxRemoveSlaveResultWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						_state = StateTxRemoveSlave;
					}
				}
				break;
			case StateIdle:
				{
					_state = StateEnabled;
				}
				break;
			}
		}
		
		int SendCommandToFunction(Protocol::G1::Function::ActionService::MessageCode code, void *data, int dataCount, int timeout)
		{
			Protocol::G1::Function::ActionService::OutputMessageFormat message;
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
				case StateTxGetSlaveListWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxSlaveListWait;
						}
						else
						{
							_state = StateTxGetSlaveList;
						}
					}
					break;
				case StateTxAddSlaveWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxAddSlaveResultWait;
						}
						else
						{
							_state = StateTxAddSlave;
						}
					}
					break;
				case StateTxRemoveSlaveWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxRemoveSlaveResultWait;
						}
						else
						{
							_state = StateTxRemoveSlave;
						}
					}
					break;
				}
			}
		}
		
		virtual void OnCommandFromFunction(unsigned char *data, int dataCount)
		{
			Base::OnCommandFromFunction(data, dataCount);
			
			Protocol::G1::Function::ActionService::InputMessageFormat command(data, dataCount);
			if (command.Valid())
			{
				if (command.Data.Valid())
				{
					switch(command.Data.Code)
					{
					case Protocol::G1::Function::ActionService::MessageCodeSlaveList:
						{
							// Обновляем хранящийся у нас список параметров
							_currentSlaveList.clear();
							
							if (command.Data.DataLen >= 1)
							{
								unsigned char *data = (unsigned char *)command.Data.Data;
								if (data)
								{
									int count = data[0];
									
									for (int i = 0; i < count; i++)
									{
										unsigned char item = data[1 + i];
										
										if (!Contains(item, _currentSlaveList))
										{
											_currentSlaveList.push_back(item);
										}
									}
								}
							}
							
							switch(_state)
							{
							case StateRxSlaveListWait:
								{
									_getSlaveList = false;
									_state = StateIdle;
								}
								break;
							}
						}
						break;
					case Protocol::G1::Function::ActionService::MessageCodeAddSlaveResult:
						{
							switch(_state)
							{
							case StateRxAddSlaveResultWait:
								{
									if (command.Data.DataLen >= 1)
									{
										unsigned char *data = (unsigned char *)command.Data.Data;
										if (data)
										{
											bool result = data[0];
											
											if (result)
											{
												// удаляем из псиска "требуется добавить" элементы списка "добавляли"
												for (int i = 0; i < _addingSlave.size(); i++)
												{
													Erase(_addingSlave[i], _addSlave);
												}
												
												_addingSlave.clear();
											}
										}
									}
									
									_state = StateIdle;
								}
								break;
							}
						}
						break;
					case Protocol::G1::Function::ActionService::MessageCodeRemoveSlaveResult:
						{
							switch(_state)
							{
							case StateRxRemoveSlaveResultWait:
								{
									if (command.Data.DataLen >= 1)
									{
										unsigned char *data = (unsigned char *)command.Data.Data;
										if (data)
										{
											bool result = data[0];
											
											if (result)
											{
												// удаляем из псиска "требуется удалить" элементы списка "удаляли"
												for (int i = 0; i < _removingSlave.size(); i++)
												{
													Erase(_removingSlave[i], _removeSlave);
												}
												
												_removingSlave.clear();
											}
										}
									}
									
									_state = StateIdle;
								}
								break;
							}
						}
						break;
					case Protocol::G1::Function::ActionService::MessageCodeSlaveListChanged:
						{
							_currentSlaveList.clear();
							
							if (command.Data.DataLen >= 1)
							{
								unsigned char *data = (unsigned char *)command.Data.Data;
								if (data)
								{
									int count = data[0];
									
									for (int i = 0; i < count; i++)
									{
										unsigned char item = data[1 + i];
										
										if (!Contains(item, _currentSlaveList))
										{
											_currentSlaveList.push_back(item);
										}
									}
								}
							}
						}
						break;
					}
				}
			}
		}
		
		void RequestSlaveList()
		{
			_getSlaveList = true;
		}
		
		int GetSlaveList(unsigned char *result, int maxCount)
		{
			if (result == 0 || maxCount < 1)
			{
				return 0;
			}
			
			int count = _currentSlaveList.size();
			// вдруг приемный буфер не может столько вместить
			if (count > maxCount)
			{
				count = maxCount;
			}
			
			for (int i = 0; i < count; i++)
			{
				result[i] = _currentSlaveList[i];
			}

			return count;
		}
		
		bool AddSlave(unsigned char *list, int count)
		{
			if (list == 0)
			{
				return false;
			}
			
			for (int i = 0; i < count; i++)
			{
				if (!Contains(list[i], _addSlave))
				{
					_addSlave.push_back(list[i]);
				}
			}

			return true;
		}
				  
		bool RemoveSlave(unsigned char *list, int count)
		{
			if (list == 0)
			{
				return false;
			}
			
			for (int i = 0; i < count; i++)
			{
				if (!Contains(list[i], _removeSlave))
				{
					_removeSlave.push_back(list[i]);
				}
			}
			
			return true;
		}
		
		virtual bool WaitResponseFlag()
		{
			bool res = Base::WaitResponseFlag();
			
			res |= _state > StateEnabled && _state < StateIdle;
			
			return res;
		}
	protected:
		bool Contains(unsigned char item, ListType &list)
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
		
		void Erase(unsigned char item, ListType &list)
		{
			ListType::iterator i;
			
			for (i = list.begin(); i != list.end();)
			{
				if (*i == item)
				{
					i = list.erase(i);
				}
				else
				{
					i++;
				}
			}
		}
		
		int SecondsToTimerTicks(int seconds)
		{
			return TimerTickCountPerSecond * seconds;
		}
	};
	
	template <class Base, int timerTickCountPerSecond>
		unsigned char ActionService<Base,timerTickCountPerSecond>::_buffer[ActionService<Base, timerTickCountPerSecond>::BufferSize];
}
}
}
}

#endif