///////////////////////////////////////////////////////////////////////////////
//	 ласс реализации работы с радиосетью по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_NETWORK_H
#define G1_NETWORK_H

#include "..\..\EventDispetcher.h"
#include "..\..\Modem.h"
#include "WirelessDevice.h"

namespace Rblib
{
namespace StdDevice
{
namespace G1
{
	template
		<
			class radioType,
			int maxDataSize,
			int eventCode,
			int tickCountPerSecond,
			bool processEventDataReceived = true // разрешить классу извлекать данные из вход€щей очереди модема
		>
	class Network
	{
	public:
		typedef radioType RadioType;
		static const int MaxNodeCount = RadioType::LibConfig::MaxNodeCount;
		static const unsigned int TickCountPerSecond = tickCountPerSecond;
		static const int EventCode = eventCode;
		typedef WirelessDevice<maxDataSize, EventCode, TickCountPerSecond> DeviceType;
	protected:
		EventHandlerClass<Network> _eventHandler;		//ќбъект который перенаправл€ет обработку событий на метод EventHandler
		RadioType *_radio;
		typename RadioType::EventDataType _eventData;
		DeviceType _deviceList[MaxNodeCount];
		bool _processRouteTables;
		unsigned char onlineNodesBuffer[MaxNodeCount];
		unsigned char dataBuffer[maxDataSize];
	public:
		Network() : _eventHandler(this)
		{
			_radio = 0;
			InitDevices();
			Reset();
		}
		
		void Init(RadioType *radio)
		{
			_radio = radio;
			Singletone<EventDispetcher>::Instance().AddHandler(RadioType::EventId, EventDispetcher::EventFunctor(_eventHandler), false);
		}
		
		void EventHandler(const Rblib::Event &event)
		{
			switch (event.Code)
			{
			case RadioType::EventId:
				{
					typename RadioType::EventCode code = (typename RadioType::EventCode)event.Param1;
					switch(code)
					{
					case RadioType::EventDataUpdated:
						{
							if (!_radio)
							{
								break;
							}
							
							_eventData = _radio->GetEventData();
							
							unsigned char eventDataId = event.Param2;
					
							if (eventDataId != _eventData.EventDataId())
							{
								break;
							}
							
							for (int i = 0; i < _eventData.StateHistorySize; i++)
							{
								Rblib::Modem::State::State modemState = (Rblib::Modem::State::State)_eventData.GetStateFromHistory(i);
								switch(modemState)
								{
								case Rblib::Modem::State::StateDisabled:
								case Rblib::Modem::State::StateLocked:
								case Rblib::Modem::State::StateSearchigSyncSources:
									{
										OnDown();
									}
									break;
								case Rblib::Modem::State::StateSync:
									{
										OnUp();
									}
									break;
								case Rblib::Modem::State::StateReady:
									{
										OnReady();
									}
									break;
								}
							}
							
							if (_eventData.RouteTableChanged)
							{
								if (_processRouteTables)
								{
									if (_radio)
									{
										int buffersSize = sizeof(onlineNodesBuffer);
										int onlineNodesCount = _radio->GetRouteTable(onlineNodesBuffer, 0, buffersSize);
										
										// вырубаем тех кого нет в сети
										for (int i = 0; i < MaxNodeCount; i++)
										{
											if (_deviceList[i].Online())
											{
												if (!Contains(_deviceList[i].Id(), onlineNodesBuffer, onlineNodesCount))
												{
													_deviceList[i].Reset();
												}
											}
										}
										
										// активируемтех кто сейчас есть в сети
										for (int i = 0; i < onlineNodesCount; i++)
										{
											DeviceType *dev = Get(onlineNodesBuffer[i]);
											if (dev)
											{
												dev->SetOnline();
											}
										}
									}
								}
							}
							
							for (int i = 0; i < _eventData.DeliveryHistorySize; i++)
							{
								unsigned char packetId = -1;
								unsigned char node = -1;
								unsigned char deliveryResultCode = -1;
								
								if (_eventData.GetDeliveryHistory(i, packetId, node, deliveryResultCode))
								{
									typename RadioType::DataTxResult result = (typename RadioType::DataTxResult)deliveryResultCode;
									bool delivered = result == RadioType::DataTxResultDelivered;

									OnTxComplete(node, packetId, delivered);
								}
							}
							
							if (_eventData.DataReceived)
							{
								if (_radio && processEventDataReceived)
								{
									// вычитываем все накопленные данные
									while(1)
									{
										unsigned int dataCount = 0;
										unsigned char sourceAddress = 0;
										int bufferSize = sizeof(dataBuffer);
										
										typename RadioType::DataQueueItemType dataQueueItem;
										if (_radio->GetInputData(dataQueueItem, dataBuffer, bufferSize))
										{
											dataCount = dataQueueItem.DataCount;
											sourceAddress = dataQueueItem.SourceAddress;
											
											OnDataReceived(sourceAddress, dataBuffer, dataCount);
										}
										else
										{
											// нет больше данных
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
		
		void OnTick()
		{
			for (int i = 0; i < MaxNodeCount; i++)
			{
				_deviceList[i].OnTick();
			}
		}

		DeviceType *Get(int id)
		{
			if (id >= MaxNodeCount)
			{
				return 0;
			}
			
			// пр€ма€ адресаци€, индекс соответсвует идентификатору
			DeviceType *res = &_deviceList[id];
			
			// вдруг кос€к, индекс не соответсвует идентификатору
			if (res->Id() != id)
			{
				res = 0;
			}
			
			return res;
		}
		
		// получить экземпл€р устройтсва с которым есть св€зь и номер которого больше currentId
		// используетс€ дл€ перебора устройтсв сети
		DeviceType *GetNextOnline(int currentId)
		{
			DeviceType *res = 0;
			
			for (int i = currentId + 1; i <MaxNodeCount; i++)
			{
				if (_deviceList[i].Online())
				{
					res = &_deviceList[i];
					break;
				}
			}
			
			return res;
		}
		
		static void OnDataReceived(void *callbackParam, unsigned char sourceAddress, unsigned char *data, int dataCount)
		{
			Network *im = (Network *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnDataReceived(sourceAddress, data, dataCount);
		}
		
		int MyAddress()
		{
			if (_radio)
				return _radio->MyAddress();
			
			return -1;
		}

	protected:
		void Reset()
		{
			_processRouteTables = false;
		}
		
		void InitDevices()
		{
			for (int i = 0; i < MaxNodeCount; i++)
			{
				_deviceList[i].Reset();
				// пр€ма€ адресаци€, индекс соответсвует идентификатору
				_deviceList[i].SetId(i);
				_deviceList[i].SendDataCallback.Set(SendData, this);
				_deviceList[i].InitFunctions();
			}
		}
		
		void ResetDevices()
		{
			for (int i = 0; i < MaxNodeCount; i++)
			{
				_deviceList[i].Reset();
			}
		}
		
		void SetDevicesNetworkParams()
		{
			// идентификатор модема через который выходим в сеть, т.е. наш идентификатор
			int gateId = -1;
			if (_radio)
			{
				gateId = _radio->MyAddress();
			}
			
			for (int i = 0; i < MaxNodeCount; i++)
			{
				_deviceList[i].SetGateId(gateId);
			}
		}
		
		void OnDown()
		{
			_processRouteTables = false;
			ResetDevices();
			
			Event e(EventCode, EventCodeGlobalOffline);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
		}
		
		void OnUp()
		{
			_processRouteTables = true;
			SetDevicesNetworkParams();
			
			Event e(EventCode, EventCodeGlobalSync);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
		}
		
		void OnReady()
		{
			Event e(EventCode, EventCodeGlobalOnline);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
		}
		
		bool Contains(unsigned char item, unsigned char *list, int listSize)
		{
			if (list == 0)
			{
				return false;
			}
			
			for (int i = 0; i < listSize; i++)
			{
				if (list[i] == item)
				{
					return true;
				}
			}
			
			return false;
		}
		
		void SendData(unsigned char address, unsigned char *data, unsigned int dataCount, int timeout, int &packetId)
		{
			if (_radio)
			{
				unsigned char dstList[] = {address};
				int dstCount = sizeof(dstList);
				_radio->EnqueueOutputData(dstList, dstCount, data, dataCount, timeout, packetId);
			}
		}
		
		static void SendData(void *objectPtr, unsigned char address, unsigned char *data, unsigned int dataCount, int timeout, int &packetId)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Network *im = (Network *)objectPtr;
			im->SendData(address, data, dataCount, timeout, packetId);
		}
		
		void OnTxComplete(unsigned char node, int packetId, bool delivered)
		{
			DeviceType *dev = Get(node);
			if (dev)
			{
				dev->OnTxComplete(packetId, delivered);
			}
		}
		
		void OnDataReceived(unsigned char sourceAddress, unsigned char *data, int dataCount)
		{
			DeviceType *dev = Get(sourceAddress);
			if (dev)
			{
				dev->OnDataReceived(data, dataCount);
			}
		}
	};
}
}
}

#endif