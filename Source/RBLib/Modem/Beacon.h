#ifndef MODEM_BEACON_H
#define MODEM_BEACON_H

#include "EventData.h"
#include "NodeConfig/NodeConfig.h"
#include "Protocol/Parser.h"
#include "Protocol/Builder.h"
#include "DataQueue/Queue.h"
#include "Transport/RingBuffer.h"
#include "../Protocol/G1/Function/Radiobeacon/DeliveryResult.h"
#include "../Utils/vector_static.h"

namespace Rblib
{
namespace Modem
{
	template
		<
			class Api,									// API аппаратной части
			class Config,								// Средство конфигурирования аппаратной чати
			class TickSource,							// Источник тактовых импульсов
			int eventId,								// Идентификатор события модема
			class libConfig = LibConfig::BaseConfig		// Статическая конфигурация модуля (например, максимумы параметров)
		>
	class Beacon
	{
	public:
		static const int EventId = eventId;
		static const unsigned int MaxDestinationCount = 255;
		static const unsigned int InputQueueSize = 1024 * 5;
		
		// коды должны совпадать с кодами, утвержденными в протокле
		enum EventCode
		{
			// внешние коды
			EventUnknown = Rblib::Protocol::G1::Function::Radiobeacon::EventCodeUnknown,
			EventDataUpdated = Rblib::Protocol::G1::Function::Radiobeacon::EventCodeEventDataUpdated
		};
		
		enum StateCode
		{
			StateNotInited,
			StateDisabled,
			StatePowerUp,
			StatePowerDown,
			StateIdle,
			StateStartTx,
			StateWaitTxComplete,
			StateLocked,
		};
		
		typedef libConfig LibConfig;
		typedef NodeConfig::NodeConfig NodeConfigType;
		typedef Protocol::Parser<LibConfig::ApiMaxPacketSize> ParserType;
		typedef Protocol::Builder<LibConfig::ApiMaxPacketSize> BuilderType;
		typedef typename ParserType::AddressType AddressType;
		typedef EventData<LibConfig::DeliveryHistoryMaxCount> EventDataType;
		typedef DataQueue::Queue<AddressType> DataQueueType;
		typedef typename DataQueueType::ItemType DataQueueItemType;
		
		static const unsigned int MaxDataCountPerSession = BuilderType::MaxSessionPacketSize;
		
		enum DataTypeCode
		{
			DataTypeOutput,		// исходящие данные
			DataTypeRelay,		// ретранслируемые данные
		};
		
#pragma pack(push, 1)
		struct TicketType
		{
			AddressType SourceAddress;
			unsigned char PacketId;
			unsigned short TickCount;
			
			TicketType(AddressType sourceAddress, unsigned char packetId)
			{
				SourceAddress = sourceAddress;
				PacketId = packetId;
				TickCount = 0;
			}
		};
		
		struct InputQueueHeader
		{
		public:
			unsigned char PacketId;
			unsigned char InitialAddress;
			DataTypeCode DataType; 
			int TransmitCount;
		public:
			InputQueueHeader()
			{
				PacketId = 0;
				InitialAddress = -1;
				TransmitCount = 0;
				DataType = DataTypeOutput;
			}
		};
#pragma pack (pop)
		
		typedef InputQueueHeader InputQueueHeaderType;
		typedef Transport::RingBuffer<InputQueueHeaderType, InputQueueSize> InputQueueType;
		static const int TickListMaxCount = 256 * 10;
		typedef vector_static<TicketType, TickListMaxCount> TicketListType;
		
		typedef CallbackWrapper<int> SetTickPeriodCallbackType;
		typedef void (OnTxStartCallbackType)();
		typedef void (OnTxDoneCallbackType)();
		typedef CallbackWrapper<NodeConfigType::NodeConfigStruct &> GetCurrentConfigCallbackType;
		typedef CallbackWrapper<> LoadConfigCallbackType;
		typedef CallbackWrapper<bool, bool, bool> PowerUpResultCallbackType;
	public:
		SetTickPeriodCallbackType SetTickPeriodCallback;
		OnTxStartCallbackType *OnTxStartCallback;
		OnTxDoneCallbackType *OnTxDoneCallback;
		GetCurrentConfigCallbackType GetCurrentConfigCallback;
		LoadConfigCallbackType LoadConfigCallback;
		PowerUpResultCallbackType PowerUpResultCallback;
	protected:
		int _myAddress;
		unsigned int _myNetId;
		bool _netIdFilterEnabled;
		int _frequencyChannel;
		int _transmitCount;
		int _relayMode;
		unsigned int _listenigInterval;
		unsigned int _listeningCounter;
		Api _api;
		EventDataType _eventInfo;
		EventDataType _eventInfoOutput;
		volatile bool _eventInfoLocked;
		DataQueueType _inputDataQueue;
		InputQueueType _inputQueue;
		ParserType _parser;
		BuilderType _builder;
		StateCode _state;
		int _currentTxPacketId;
		TicketListType _ticketList;
		bool _powerDown;
	public:
		Beacon()
		{
			_eventInfoLocked = false;
			_state = StateNotInited;
			_powerDown = false;
			_myAddress = 0;
			_myNetId = 0xFFFFFFFF;
			_netIdFilterEnabled = false;
			_frequencyChannel = 10;
			_transmitCount = 10;
			_relayMode = 0;
			_listenigInterval = 10;
			_listeningCounter = 0;
		}
		
		bool Init()
		{
			bool apiInitOk = _api.Init();
			_api.OnTxCompleteCallback.Set(OnTxCompleteCallback, this);
			_api.OnRxCompleteCallback.Set(OnRxCompleteCallback, this);
			
			_parser.OnBeaconDataCallback.Set(OnRxBeaconDataCallback, this);
			
			if (apiInitOk)
			{
				_state = StateDisabled;
			}
			else
			{
				_state = StateLocked;
			}
			
			return apiInitOk;
		}
		
		void LoadConfig()
		{
			NodeConfigType::NodeConfigStruct nodeConfig;
			GetCurrentConfigCallback.Call(nodeConfig);
			
			_frequencyChannel = nodeConfig.NetConfig.FrequencyChannel;
			_myAddress = nodeConfig.Id;
			_myNetId = nodeConfig.NetConfig.Id;
			_netIdFilterEnabled = nodeConfig.NetConfig.UseIdFilter;
			_relayMode = nodeConfig.NetConfig.RelayMode;
			
			_transmitCount = nodeConfig.NetConfig.PacketRepeatCount;
			if (_transmitCount < 1)
			{
				_transmitCount = 1;
			}
			
			_listenigInterval = nodeConfig.NetConfig.RadioListetingInterval;
			if (_listenigInterval < 10)
			{
				_listenigInterval = 10;
			}
			
			if (_listenigInterval > 100000)
			{
				_listenigInterval = 100;
			}
		}
		
		AddressType MyAddress()
		{
			return _myAddress;
		}
		
		unsigned int MyNetId()
		{
			return _myNetId;
		}
		
		// Включить функцию
		void Enable()
		{
			if (Enabled())
			{
				return;
			}
			
			TickSource::OnTickCallback.Set(OnTickCallback, this);
			
			LoadConfig();
			
			_eventInfo.Reset();
			TickSource::SetFrequency(100);
			TickSource::Enable();
			
			_ticketList.clear();
			SetState(StatePowerUp);
		}
		
		// Отключить функцию
		void Disable()
		{
			if (!Enabled())
			{
				return;
			}
			
			_powerDown = true;
		}
		
		void OnIrq()
		{
			_api.OnIrq();
		}
		
		Rblib::Protocol::G1::Function::WirelessModem::StateCode GetState()
		{
			return ConvertToProtocolStateCode(_state);
		}
		
		static void GetState(void *callbackParam, int &state)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Beacon *im = (Beacon *)callbackParam;
			
			state = im->GetState();
		}
		
		//  Поместить выходные данные в очередь
		bool EnqueueOutputData(AddressType *destination, int destinationCount, void *data, unsigned int dataCount, int &packetId)
		{
			static unsigned char nextPacketId = 0;
			
			if (destination == 0 || destinationCount < 1 || data == 0 || dataCount < 1)
			{
				return false;
			}
			
			if (destinationCount > MaxDestinationCount)
			{
				return false;
			}
			
			if (dataCount > MaxDataCountPerSession)
			{
				return false;
			}
			
			void *dataChunks[2] = {destination, data};
			typename InputQueueType::DataCountType dataChunkSizes[2] = {destinationCount, dataCount};
			
			if (!_inputQueue.IsFits(dataChunks, dataChunkSizes, 2))
			{
				return false;
			}
			
			packetId = nextPacketId++;
			
			InputQueueHeaderType header;
			header.PacketId = packetId;
			header.InitialAddress = _myAddress;
			header.TransmitCount = 0;
			header.DataType = DataTypeOutput;
			_inputQueue.Push(&header, dataChunks, dataChunkSizes, 2);
			
			return true;
		}
		
		static void EnqueueOutputData(void *callbackParam, AddressType *dstList, unsigned int dstCount, void *data, unsigned int dataCount, bool &enqueued, int &packetId)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Beacon *im = (Beacon *)callbackParam;
			enqueued = im->EnqueueOutputData(dstList, dstCount, data, dataCount, packetId);
		}
		
		// Извлечь входные данные из очереди
		bool GetInputData(DataQueueItemType &item, void *buffer, int bufferSize)
		{
			if (_inputDataQueue.Empty())
			{
				return false;
			}
			
			item = _inputDataQueue.Pop(buffer, bufferSize);
		
			return true;
		}
		
		// Извлечь входные данные из очереди
		static void GetInputData(void *callbackParam, void *buffer, unsigned int bufferSize, unsigned int &count, unsigned char &sourceAddress, bool &empty)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Beacon *im = (Beacon *)callbackParam;
			
			empty = im->_inputDataQueue.Empty();
			if (empty)
			{
				return;
			}
			
			DataQueueItemType item = im->_inputDataQueue.Pop(buffer, bufferSize);
			count = item.DataCount;
			sourceAddress = item.SourceAddress;
		}
		
		void LockEventData()
		{
			_eventInfoLocked = true;
		}
		
		void UnlockEventData()
		{
			_eventInfoLocked = false;
		}
		
		EventDataType &GetEventData()
		{
			return _eventInfoOutput;
		}
		
		static void GetEventData(void *callbackParam, EventDataType &eventData)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Beacon *im = (Beacon *)callbackParam;
			
			eventData = im->GetEventData();
		}
		
		static void ApplyConfigParams(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Beacon *im = (Beacon *)callbackParam;
			
			im->LoadConfigCallback.Call();
			im->LoadConfig();
		}
		
		bool Enabled()
		{
			return _state != StateDisabled && _state != StateNotInited;
		}
	protected:
		void SetState(StateCode newState)
		{
			if (_state != newState)
			{
				Rblib::Protocol::G1::Function::WirelessModem::StateCode oldProtocolStateCode = ConvertToProtocolStateCode(_state);
				Rblib::Protocol::G1::Function::WirelessModem::StateCode newProtocolStateCode = ConvertToProtocolStateCode(newState);
				
				_state = newState;
				
				// в терминах протокола может ничего не поменяться, проверяем отдельно
				if (oldProtocolStateCode != newProtocolStateCode)
				{
					_eventInfo.AddStateToHistory(newProtocolStateCode);
				}
				
				if (_state == StateIdle)
				{
					_listeningCounter = 0;
				}
			}
		}
		
		Rblib::Protocol::G1::Function::WirelessModem::StateCode ConvertToProtocolStateCode(StateCode state)
		{
			switch(state)
			{
			case StateNotInited:
			case StateDisabled:
				return Rblib::Protocol::G1::Function::WirelessModem::StateCodeDisabled;
			case StatePowerUp:
				return Rblib::Protocol::G1::Function::WirelessModem::StateCodeInitializing;
			case StateLocked:
				return Rblib::Protocol::G1::Function::WirelessModem::StateCodeLocked;
			}
			
			return Rblib::Protocol::G1::Function::WirelessModem::StateCodeReady;
		}
		
		void SetTickPeriod(int period)
		{
			
		}
		
		static void OnTickCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Beacon *im = (Beacon *)callbackParam;
			im->OnTick();
		}
		
		void OnTick()
		{
			switch(_state)
			{
			case StatePowerUp:
				{
					bool powerUp = _api.PowerUp();
					bool done = false;
					bool apiTestOk = false;
					
					if (powerUp)
					{
						int tryCount = 0;
						while (!apiTestOk && tryCount < 3)
						{
							tryCount++;
							apiTestOk = _api.Test();
							
							if (apiTestOk)
							{
								done = Config::ApplyConfig(&_api);
								if (done)
								{
									// максимальный таймаут
									Config::SetRxPreambleTimeout(&_api, 0x0F);
								}
							}
						}
					}
					
					PowerUpResultCallback.Call(powerUp, apiTestOk, done);
					
					if (done)
					{
						SetState(StateIdle);
						_api.AllowInterruptHandling(true);
					}
					else
					{
						SetState(StateLocked);
					}
				}
				break;
			case StatePowerDown:
				{
					_api.AllowInterruptHandling(false);
					_api.PowerDown();
					
					TickSource::OnTickCallback.Reset();
					TickSource::Disable();
					SetState(StateDisabled);
					
					// Сообщим о последних событиях
					PushEventInfo();
				}
				break;
			case StateIdle:
				{
					_currentTxPacketId = -1;
					
					if (TxData(_myAddress, _myNetId, _currentTxPacketId))
					{
						SetState(StateWaitTxComplete);
						TickSource::SetFrequency(1000 - (rand() % 5) * 100);
					}
					else
					{
						TickSource::SetFrequency(100);
						
						if (_powerDown)
						{
							_powerDown = false;
							SetState(StatePowerDown);
							break;
						}
						
						unsigned int maxCount = _listenigInterval / 10 - 1;
						
						if (_listeningCounter == 0)
						{
							EnableRx();
						}
						
						if (_listeningCounter >= maxCount)
						{
							_listeningCounter = 0;
						}
						else
						{
							_listeningCounter++;
						}
					}
					
					// сообщим о произошедших событиях
					PushEventInfo();
				}
				break;
			}
			
			// обновить цикл жизни сохраненных идентификаторов пакетов
			UpdateTicketCounters(100);
		}
		
		bool TxPacket(void * data, int len)
		{
			if (0 == data)
			{
				return false;
			}
			
			if (len < 1)
			{
				return false;
			}
			
			if (_api.IsRx())
			{
				_api.CancelRx();
			}

			_api.TxPrepare(data, len);
				
			if (OnTxStartCallback)
			{
				OnTxStartCallback();
			}

			return _api.TxStart(_frequencyChannel);
		}
		
		static void OnTxCompleteCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Beacon *im = (Beacon *)callbackParam;
			if (im->OnTxDoneCallback)
			{
				im->OnTxDoneCallback();
			}
			
			if (im->_state == StateWaitTxComplete)
			{
				im->SetState(StateIdle);
				im->EnableRx();
			}
		}
		
		void EnableRx()
		{
			if (!_api.IsRx())
			{
				_api.Rx(true, _frequencyChannel);
			}
		}
		
		static void OnRxCompleteCallback(void *callbackParam, void *data, unsigned int dataCount, int rssi)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Beacon *im = (Beacon *)callbackParam;
			im->_parser.ProcessData(im->_myNetId, im->_netIdFilterEnabled, data, dataCount, rssi);
		}
		
		void PushEventInfo()
		{
			if (_eventInfo.Event)
			{
				if (_eventInfoLocked)
				{
					return;
				}
				
			  	// копируем то что накопилось
				_eventInfoOutput = EventDataType(_eventInfo);
				// освобождаем место под новые
				_eventInfo.Reset();
				
				Event e(EventId, EventDataUpdated, _eventInfoOutput.EventDataId());
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
		}
		
		// Извлечь данные из очереди и отправить
		bool TxData(AddressType selfAddress, unsigned int selfNetId, int &packetId)
		{
			bool res = false;
			
			if (_inputQueue.NowPush())
			{
				return res;
			}
			
			if (_inputQueue.Empty())
			{
				return res;
			}
			
			// заголовок
			InputQueueHeaderType header;
			// список адресатов
			AddressType destination[MaxDestinationCount];
			// данные
			unsigned char data[MaxDataCountPerSession];
			
			void *dataChunks[2] = {destination, data};
			// кол-во адресатов
			typename InputQueueType::DataCountType destinationCount;
			// кол-во данных
			typename InputQueueType::DataCountType dataCount;
			typename InputQueueType::DataCountType *dataChunkSizes[2] = {&destinationCount, &dataCount};
			// Извлекаем из очереди
			typename InputQueueType::DataCountType dataChunksCount = 0;
			_inputQueue.Pop(&header, dataChunks, dataChunkSizes, &dataChunksCount);
			
			res = _builder.MakeBeaconData(selfAddress, selfNetId, header.PacketId, header.InitialAddress, destination, destinationCount, data, dataCount);
			
			if (res)
			{
				res &= TxPacket(_builder.PacketData, _builder.PacketLen);
			}
			
			header.TransmitCount++;
			
			if (res)
			{
				packetId = header.PacketId;
				
				TicketListPush(header.InitialAddress, packetId);
				
				if (header.TransmitCount < _transmitCount)
				{
					// возвращаем в хвост очереди
					typename InputQueueType::DataCountType dataChunkPushSizes[2] = {destinationCount, dataCount};
					_inputQueue.Push(&header, dataChunks, dataChunkPushSizes, dataChunksCount);
				}
				else
				{
					if (header.DataType == DataTypeOutput)
					{
						if (destinationCount > 0)
						{
							for (int i = 0; i < destinationCount; i++)
							{
								_eventInfo.SetDeliveryComplete(_currentTxPacketId, destination[i], Rblib::Protocol::G1::Function::Radiobeacon::DeliveryResultTransmited);
							}
						}
						else
						{
							_eventInfo.SetDeliveryComplete(_currentTxPacketId, -1, Rblib::Protocol::G1::Function::Radiobeacon::DeliveryResultTransmited);
						}
					}
				}
			}
			else
			{
				if (header.DataType == DataTypeOutput)
				{
					if (destinationCount > 0)
					{
						for (int i = 0; i < destinationCount; i++)
						{
							_eventInfo.SetDeliveryComplete(_currentTxPacketId, destination[i], Rblib::Protocol::G1::Function::Radiobeacon::DeliveryResultImpossible);
						}
					}
					else
					{
						_eventInfo.SetDeliveryComplete(_currentTxPacketId, -1, Rblib::Protocol::G1::Function::Radiobeacon::DeliveryResultImpossible);
					}
				}
			}
			
			return res;
		}
		
		static void OnRxBeaconDataCallback(void *callbackParam, AddressType sourceAddress, unsigned char packetId, AddressType initialAddress, unsigned char *destination, int destinationCount, unsigned char *data, int dataCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Beacon *im = (Beacon *)callbackParam;
			im->OnRxBeaconData(sourceAddress, packetId, initialAddress, destination, destinationCount, data, dataCount);
		}
		
		void OnRxBeaconData(AddressType sourceAddress, unsigned char packetId, AddressType initialAddress, unsigned char *destination, int destinationCount, unsigned char *data, int dataCount)
		{
			if (EarlierReceived(initialAddress, packetId))
			{
				return;
			}
			
			TicketListPush(initialAddress, packetId);
			
			// обрабатываем если "широковещательный" пакет, т.е. без адресатов
			bool processPacket = destinationCount == 0 || destination == 0;
			// ретранслируем если "широковещательный" пакет, т.е. без адресатов
			bool relayPacket = destinationCount == 0 || destination == 0; 
			for (int i = 0; i < destinationCount; i++)
			{
				// обрабатываем, если мы есть в списке адресатов
				processPacket |= destination[i] == _myAddress;
				// ретранслируем, если в списке адресатов есть кто-то кроме нас
				relayPacket |= destination[i] != _myAddress;
			}
			
			if (processPacket)
			{
				_inputDataQueue.Push(DataQueueItemType(initialAddress, dataCount), data);
				_eventInfo.SetDataReceived();
			}
			
			// если ретранслировать нужно и можно
			if (relayPacket && _relayMode > 0)
			{
				void *dataChunks[2] = {destination, data};
				typename InputQueueType::DataCountType dataChunkSizes[2] = {destinationCount, dataCount};
				
				if (_inputQueue.IsFits(dataChunks, dataChunkSizes, 2))
				{
					InputQueueHeaderType header;
					header.PacketId = packetId;
					header.InitialAddress = initialAddress;
					header.TransmitCount = 0;
					header.DataType = DataTypeRelay;
					// если очередь занята, то бессмысленно ждать когда она освободится, т.к. у этого потока приоритет выше
					_inputQueue.Push(&header, dataChunks, dataChunkSizes, 2, false);
				}
			}
		}
		
		bool EarlierReceived(AddressType sourceAddress, unsigned char packetId)
		{
			return TicketListContains(sourceAddress, packetId);
		}
		
		void TicketListPush(AddressType sourceAddress, unsigned char packetId)
		{
			if (TicketListContains(sourceAddress, packetId))
			{
				return;
			}
			
			_ticketList.push_back(TicketType(sourceAddress, packetId));
		}
		
		bool TicketListContains(AddressType sourceAddress, unsigned char packetId)
		{
			for (int i = 0; i < _ticketList.size(); i++)
			{
				if (_ticketList[i].SourceAddress == sourceAddress && _ticketList[i].PacketId == packetId)
				{
					return true;
				}
			}
			
			return false;
		}
		
		void UpdateTicketCounters(int maxCount)
		{
			for (int i = 0; i < _ticketList.size();)
			{
				if (_ticketList[i].TickCount >= maxCount)
				{
					_ticketList.erase(&_ticketList[i]);
				}
				else
				{
					_ticketList[i].TickCount++;
					i++;
				}
			}
		}
	};
	
}
}

#endif