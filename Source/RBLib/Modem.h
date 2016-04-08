#ifndef MODEM_H
#define MODEM_H

#include "Utils/map_static.h"
#include "Modem/EventData.h"
#include "Modem/State/StateMachine.h"
#include "Modem/NodeConfig/NodeConfig.h"
#include "Modem/Sync/Sync.h"
#include "Modem/NetTime/NetTime.h"
#include "Modem/Protocol/Parser.h"
#include "Modem/Protocol/Builder.h"
#include "Modem/DataQueue/Queue.h"
#include "Modem/DebugDataQueue/Queue.h"
#include "Modem/Activity/ActivityTable.h"
#include "Modem/Activity/ActivityTableTranslator.h"
#include "Modem/Routing/RouteTable.h"
#include "Modem/Transport/Transport.h"
#include "Modem/Common/SignalTimeout.h"
#include "Modem/LibConfig/BaseConfig.h"
#include "Modem/Tests/Tests.h"
#include "Modem/Tests/DataQueue.h"
#include "Protocol/G1/Function/WirelessModem/EventCode.h"
#include "Protocol/G1/Function/WirelessModem/DeliveryResult.h"
#include "Protocol/G1/Function/WirelessModem/ConfigParamCode.h"
#include "Protocol/G1/Function/WirelessModem/FunctionData.h"

#include "Modem/Beacon.h"

namespace Rblib
{
namespace Modem
{
#define DEBUG_SKIP_INPUT_FROM_SOURCE { if (((unsigned char *)data)[1] == 1  || ((unsigned char *)data)[1] == 11)	{/* return;*/ } }
#define DEBUG_SKIP_ACTIVITY_FROM_SOURCE { if (/*syncInfo.SourceAddress == 7  || syncInfo.SourceAddress == 1  ||*/ syncInfo.SourceAddress == 11)	{ /*return;*/ } }
	
	
	template
		<
			class Api,									// API аппаратной части
			class Config,								// Средство конфигурирования аппаратной чати
			class TickSource,							// Источник тактирования модема
			class DebugSignal,							// Сигналы для отладки
			int eventId,								// Идентификатор события модема
			class libConfig = LibConfig::BaseConfig		// Статическая конфигурация модуля (например, максимумы параметров)
		>
	class Modem
	{
	public:
		static const int EventId = eventId;
		
		// коды должны совпадать с кодами, утвержденными в протокле
		enum EventCode
		{
			// внешние коды
			EventUnknown = Rblib::Protocol::G1::Function::WirelessModem::EventCodeUnknown,
			EventDataUpdated = Rblib::Protocol::G1::Function::WirelessModem::EventCodeEventDataUpdated
		};
		
		enum ActivityTableCompletenessState
		{
			ActivityTableCompletenessStateNotChecked,
			ActivityTableCompletenessStateCheckedPresent,
			ActivityTableCompletenessStateCheckedNotPresent,
		};
		
		// коды должны совпадать с кодами, утвержденными в протокле
		enum DataTxResult
		{
			DataTxResultUnknown = Rblib::Protocol::G1::Function::WirelessModem::DeliveryResultUnknown,
			DataTxResultDelivered = Rblib::Protocol::G1::Function::WirelessModem::DeliveryResultDelivered,
			DataTxResultDeliveryImpossible = Rblib::Protocol::G1::Function::WirelessModem::DeliveryResultImpossible,
			DataTxResultError = Rblib::Protocol::G1::Function::WirelessModem::DeliveryResultError
		};
		
		// Отладочные коды первого уровня
		enum DebugData
		{
			DebugDataRxFlat		= 254,
			DebugDataRx		= 255
		};
		
		// Стартовые значения кодов отладочной информации для разных модулей
		enum DebugDataStartCode
		{
			DebugDataStartCodeSync	= 0,
			DebugDataStartCodeTransport	= 10,
			DebugDataStartCodeActivityTable	= 20,
		};
		
		// Коды причин перезагрузки
		enum ResetReasoneCode
		{
			ResetReasoneStateMachineGroupStart = 100,
			ResetReasoneSyncGroupStart = 200
		};
		
		typedef libConfig LibConfig;
		typedef State::StateMachine StateMachineType;
		typedef NodeConfig::NodeConfig NodeConfigType;
		typedef Sync::Sync<TickSource, LibConfig::SyncSlotCountPerSecond, LibConfig::SyncMaxSlotCountPerCycle, LibConfig::SyncMaxAdditionalSlotCount, DebugDataStartCodeSync, LibConfig::SnifferMode, LibConfig::UseExternalSync> SyncType;
		typedef NetTime::NetTime NetTimeType;
		typedef Protocol::Parser<LibConfig::ApiMaxPacketSize> ParserType;
		typedef Protocol::Builder<LibConfig::ApiMaxPacketSize> BuilderType;
		typedef typename ParserType::AddressType AddressType;
		static const int ActivityTableMaxRemoteNodeLinesCount = LibConfig::MaxNodeCount; // Если не влезаем можно 32
		typedef Activity::ActivityTable<AddressType, LibConfig::MaxNodeCount, ActivityTableMaxRemoteNodeLinesCount, DebugDataStartCodeActivityTable> ActivityTableType;
		static const int ActivityTableTranslatorQueueItemsMaxCount = ActivityTableType::TableLineMaxLen * LibConfig::MaxNodeCount; // если не влезаем можно ActivityTableType::TableLineMaxLen * 10
		typedef Activity::ActivityTableTranslator<ActivityTableType::TableLineMaxLen, ActivityTableTranslatorQueueItemsMaxCount, NetTimeType::NetTimeType> ActivityTableTranslatorType;
		static const unsigned int ActivityTableCompletenessQueueSize = ActivityTableType::TableLineMaxLen;
		typedef map_static<AddressType, ActivityTableCompletenessState, ActivityTableCompletenessQueueSize> ActivityTableCompletenessQueueType;
		typedef Routing::RouteTable<AddressType, ActivityTableType::TableLineMaxLen> RouteTableType;
		typedef Transport::Transport<AddressType, BuilderType::MaxSessionPacketSize, DebugDataStartCodeTransport, LibConfig::MaxSessionsCount, LibConfig::MaxActiveOutputSessionsCount> TransportType;
		static const unsigned int MaxTransportOutputPacketSize = BuilderType::MaxPacketSize;
		static const int MaxOutputPacketsCountInOneCycle = LibConfig::SyncMaxAdditionalSlotCount + 1; // все дополнительные и 1 основной
		typedef DataQueue::Queue<AddressType> DataQueueType;
		typedef typename DataQueueType::ItemType DataQueueItemType;
		typedef typename Tests::Tests TestsType;
		typedef typename Tests::DataQueue TestDataQueueType;
		typedef typename TestDataQueueType::ItemType TestDataQueueItemType;
		
		static const unsigned int MaxDebugDataQueueItemsCount = 100;
		static const unsigned int DebugDataQueueSize = 1024;
		
		typedef DebugDataQueue::Queue<unsigned short, MaxDebugDataQueueItemsCount, DebugDataQueueSize> DebugDataQueueType;
		typedef typename DebugDataQueueType::ItemType DebugDataQueueItemType;
		
		typedef EventData<LibConfig::DeliveryHistoryMaxCount> EventDataType;
		
		typedef CallbackWrapper<NodeConfigType::NodeConfigStruct &> GetCurrentConfigCallbackType;
		typedef void (StartSecondPriorityWorkCallbackType)();
		typedef void (OnTxStartCallbackType)();
		typedef void (OnTxCompleteCallbackType)();
		typedef void (OnTxPrepareCallbackType)();
		typedef void (ResetCallbackType)(int);
		typedef void (PowerUpResultCallbackType)(bool, bool, bool);
		typedef CallbackWrapper<int, unsigned char, int, unsigned char *, int> SendTestDataToNodeCallabckType;
	public:
		GetCurrentConfigCallbackType GetCurrentConfigCallback;
		SendTestDataToNodeCallabckType SendTestDataToNodeCallback;
	protected:
		Api _api;
		StateMachineType _stateMachine;
		SyncType _sync;
		NetTimeType _netTime;
		ParserType _parser;
		BuilderType _builder;
		ActivityTableType _activityTable;
		ActivityTableTranslatorType _activityTableTranslator;
		ActivityTableCompletenessQueueType _activityTableCompletenessQueue;
		bool _activityTableCompletenessCheckFlag;								// пора проверить таблицы слышимости
		RouteTableType _routeTable;
		TransportType _transport;
		TestsType _tests;
		unsigned char _txBuffer[BuilderType::MaxPacketSize];
		int _txBufferCount;
		volatile bool _txBufferLock;
		DataQueueType _inputDataQueue;
		TestDataQueueType _testDataQueue;
		DebugDataQueueType _debugDataQueue;
		bool _debugDataRefilled;
		unsigned int _debugDataOutputCounter;
		int _myAddress;
		unsigned int _myNetId;
		bool _netIdFilterEnabled;
		int _frequencyChannel;
		SignalTimeout<DebugSignal, 2> _signalRx;
		SignalTimeout<DebugSignal, 3> _signalTx;
		SignalTimeout<DebugSignal, 4> _signalSync;
		unsigned char _activityTableNodesBuffer[ActivityTableType::TableLineMaxLen];
		unsigned char _activityTableRatesBuffer[ActivityTableType::TableLineMaxLen];
		bool _startTx;
		EventDataType _eventInfo;
		EventDataType _eventInfoOutput;
		volatile bool _eventInfoLocked;
		StartSecondPriorityWorkCallbackType *_startSecondPriorityWorkCallback;
		OnTxStartCallbackType *_onTxStartCallback;
		OnTxCompleteCallbackType *_onTxCompleteCallback;
		OnTxPrepareCallbackType *_onTxPrepareCallback;
		ResetCallbackType *_resetCallback;
		PowerUpResultCallbackType *_powerUpResultCallback;
	public:
		Modem()
		{
			_startSecondPriorityWorkCallback = 0;
			_onTxStartCallback = 0;
			_onTxCompleteCallback = 0;
			_onTxPrepareCallback = 0;
			_resetCallback = 0;
			_powerUpResultCallback = 0;
			_eventInfoLocked = false;
			_txBufferCount = 0;
			_txBufferLock = false;
		}
		
		bool Init(StartSecondPriorityWorkCallbackType *startSecondPriorityWorkCallback)
		{
			_myAddress = 0xFF;
			_myNetId = 0xFFFFFFFF;
			_netIdFilterEnabled = false;
			_frequencyChannel = 0xFF;
			_stateMachine.Flags.ApiInited = _api.Init();
			_stateMachine.Flags.PowerOn = false;
			_stateMachine.Flags.ApiTestOk = false;
			_debugDataRefilled = false;
			_debugDataOutputCounter = 0;
			_startTx = false;
			
			_stateMachine.Flags.SyncInited = false;
			
			_stateMachine.CommonInitCallback.Set(CommonInitCallback, this);
			_stateMachine.SetPowerUpCallback(PowerUpCallback, this);
			_stateMachine.SetPowerDownCallback(PowerDownCallback, this);
			_stateMachine.SetStartSyncCallback(StartSyncCallback, this);
			_stateMachine.SetStopSyncCallback(StopSyncCallback, this);
			_stateMachine.SetStartActivityProcessingCallback(StartActivityProcessingCallback, this);
			_stateMachine.SetStopActivityProcessingCallback(StopActivityProcessingCallback, this);
			_stateMachine.SetOnStateChangedCallback(OnStateChangedCallback, this);
			_stateMachine.SetStartRoutingCallback(StartRoutingCallback, this);
			_stateMachine.SetStopRoutingCallback(StopRoutingCallback, this);
			_stateMachine.SetStartTransportCallback(StartTransportCallback, this);
			_stateMachine.SetStopTransportCallback(StopTransportCallback, this);
			_stateMachine.SetResetCallback(OnStateMachineResetCallback, this);
			_stateMachine.StartInfinitListeningCallback.Set(StartInfinitListeningCallback, this);
			_stateMachine.StartCarrierTransmitCallback.Set(StartCarrierTransmitCallback, this);
			
			_api.SetOnTxCompleteCallback(OnTxCompleteCallback, this);
			_api.SetOnRxCompleteCallback(OnRxCompleteCallback, this);
			_api.OnErrorCallback.Set(OnApiErrorCallback, this);
			
			_sync.SetOnEndTimeChainCallback(OnEndTimeChainCallback, this);
			_sync.SetRxOnOffCallback(RxOnOffCallback, this);
			_sync.SetTxSyncEquivalentCallback(TxSyncEquivalentCallback, this);
			_sync.SetTxBiggestPacketEquivalentCallback(TxBiggestPacketEquivalentCallback, this);
			_sync.SetTxSyncRequestCallback(TxSyncRequestCallback, this);
			_sync.SetTxSyncCallback(TxSyncCallback, this);
			_sync.SetOnStateChangedCallback(OnSyncStateChangedCallback, this);
			_sync.SetDebugDataCallback(OnDebugDataCallback, this);
			_sync.SetOnTxTimeSlotCallback(OnTxTimeSlotCallback, this);
			_sync.SetResetCallback(OnSyncResetCallback, this);
			_sync.AllowEnergySavingListeningCallback.Set(AllowEnergySavingListeningCallback, this);
			
			_netTime.SetGetCurrentTimeSlotAndCountCallback(GetCurrentTimeSlotAndCountCallback, this);
			
			_parser.OnSyncRequestCallback.Set(OnRxSyncRequestCallback, this);
			_parser.OnNetConfigCallback.Set(OnRxNetConfigCallback, this);
			_parser.OnSyncInfoCallback.Set(OnRxSyncInfoCallback, this);
			_parser.OnNetTimeCallback.Set(OnRxNetTimeCallback, this);
			_parser.OnActivityTableCallback.Set(OnRxActivityTableCallback, this);
			_parser.OnTransportSessionCallback.Set(OnRxTransportSessionCallback, this);
			_parser.OnTestDataCallback.Set(OnRxTestDataCallback, this);
			
			_activityTable.SetSelfActivityTableChangedCallback(OnSelfActivityTableChangedCallback, this);
			_activityTable.SetSelfActivityTableRegularDistributionCallback(OnSelfActivityTableRegularDistributionCallback, this);
			_activityTable.SetRemoteNodeActivityTableChangedCallback(OnRemoteNodeActivityTableChangedCallback, this);
			_activityTable.DebugDataCallback.Set(OnDebugDataCallback, this);
			_activityTableCompletenessCheckFlag = false;
			
			_routeTable.GetLinksCallback.Set(GetLinksCallback, this);
			_routeTable.GetRelayModeCallback.Set(GetRelayModeCallback, this);
			_routeTable.OnRouteTableChangedCallback.Set(OnRouteTableChangedCallback, this);
			
			_transport.SetGetTicketInfoCallback(GetTicketInfoCallbcak, this);
			_transport.SetGetRouteCallback(GetRouteCallback, this);
			_transport.SetOnDataReceivedCallback(OnDataReceivedCallback, this);
			_transport.SetOnTxCompleteCallback(OnTxCompleteCallback, this);
			_transport.SetDebugDataCallback(OnDebugDataCallback, this);
			
			_startSecondPriorityWorkCallback = startSecondPriorityWorkCallback;
			
			_tests.OnTestDataCallback.Set(OnTestDataCallback, this);
			_tests.OnSendTestDataToNodeCallback.Set(OnSendTestDataToNodeCallback, this);
			_tests.GoToListeningModeCallback.Set(GoToListeningModeCallback, this);
			_tests.GoToPowerTestModeCallback.Set(GoToPowerTestModeCallback, this);
			_tests.GoToNormalModeCallback.Set(GoToNormalModeCallback, this);
			_tests.EnqueuePacketCallback.Set(EnqueuePacketCallback, this);
			
			return _stateMachine.Flags.ApiInited;
		}
		
		void SetTxStartCompleteCallback(OnTxStartCallbackType *onTxStartCallback, OnTxCompleteCallbackType *onTxCompleteCallback)
		{
			_onTxStartCallback = onTxStartCallback;
			_onTxCompleteCallback = onTxCompleteCallback;
		}
		
		void SetTxPrepareCallback(OnTxPrepareCallbackType *onTxPrepareCallback)
		{
			_onTxPrepareCallback = onTxPrepareCallback;
		}
		
		void SetResetCallback(ResetCallbackType *resetCallback)
		{
			_resetCallback = resetCallback;
		}
		
		void SetPowerUpResultCallback(PowerUpResultCallbackType *callback)
		{
			_powerUpResultCallback = callback;
		}
		
		void OnIrq()
		{
			_sync.FixTime();
			_api.OnIrq();
		}
		
		static void OnTickCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->OnTick();
		}
		
		void OnTick()
		{
			if (_stateMachine.Flags.ApiError)
			{
				_startSecondPriorityWorkCallback();
				return;
			}
			
			DebugSignal::Set(0);
			_sync.OnTick();
			DebugSignal::Clear(0);
			_signalRx.OnTimer();
			_signalTx.OnTimer();
			
			if (_startTx)
			{
				if (_onTxPrepareCallback)
				{
					_onTxPrepareCallback();
				}
			}
			
			if (_debugDataOutputCounter > 0)
			{
				_debugDataOutputCounter--;
			}
		}
		
		static void OnTickPart1Callback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->OnTickPart1();
		}
		
		void OnTickPart1()
		{
			if (_stateMachine.Flags.ApiError)
			{
				_startSecondPriorityWorkCallback();
				return;
			}
			
			DebugSignal::Set(0);
			if (_startTx)
			{
				if (_onTxStartCallback)
				{
					_onTxStartCallback();
				}
				
				if (_api.IsRx())
				{
					_api.CancelRx();
				}
				
				if (_api.TxStart(_frequencyChannel))
				{
					_startTx = false;
				}
			}
			DebugSignal::Clear(0);
		}
		
		void OnSecondPriorityWork()
		{
			OnEndTimeChainAsync();
		}
		
		// Включить функции модема
		void Enable()
		{
			_eventInfo.Reset();
			TickSource::OnTickCallback.Set(OnTickCallback, this);
			TickSource::OnTickPart1Callback.Set(OnTickPart1Callback, this);
			_stateMachine.Enable();
		}
		
		// Отключить функции модема
		void Disable()
		{
			_stateMachine.Disable();
			TickSource::OnTickCallback.Reset();
			TickSource::OnTickPart1Callback.Reset();
			// Сообщим о последних событиях
			PushEventInfo();
		}
		
		State::State GetState()
		{
			return _stateMachine.GetState();
		}
		
		State::LockReasoneCode GetLockReasone()
		{
			return _stateMachine.GetLockReasone();
		}
		
		static void GetState(void *objectPtr, int &state)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			
			state = im->GetState();
		}
		
		//  Поместить выходные данные в очередь
		bool EnqueueOutputData(AddressType *dstList, int dstCount, void *data, unsigned int dataCount, unsigned int deliveryTimeout, int &packetId)
		{
			float cyclesPerSecond = _sync.GetCyclesCountPerSecond();
			float cyclesCount = (float)deliveryTimeout * cyclesPerSecond;
			
			bool enqueued = _transport.EnqueueData(dstList, dstCount, data, dataCount, (int)cyclesCount, packetId);
			
			return enqueued;
		}
		
		static void EnqueueOutputData(void *objectPtr, AddressType *dstList, unsigned int dstCount, void *data, unsigned int dataCount, unsigned int deliveryTimeout, bool &enqueued, int &packetId)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			enqueued = im->EnqueueOutputData(dstList, dstCount, data, dataCount, deliveryTimeout, packetId);
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
		static void GetInputData(void *objectPtr, void *buffer, unsigned int bufferSize, unsigned int &count, unsigned char &sourceAddress, bool &empty)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			
			empty = im->_inputDataQueue.Empty();
			if (empty)
			{
				return;
			}
			
			DataQueueItemType item = im->_inputDataQueue.Pop(buffer, bufferSize);
			count = item.DataCount;
			sourceAddress = item.SourceAddress;
		}
		
		// Извлечь отладочные данные из очереди
		bool GetDebugData(DebugDataQueueItemType &item, void *buffer, int bufferSize)
		{
			if (_debugDataQueue.Empty())
			{
				return false;
			}
			
			item = _debugDataQueue.Pop(buffer, bufferSize);
		
			return true;
		}
		
		// Извлечь отладочные данные из очереди
		static void GetDebugData(void *objectPtr, void *buffer, unsigned int bufferSize, unsigned int &count, unsigned short &code, bool &empty)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			
			DebugDataQueueItemType item;
			empty = !im->GetDebugData(item, buffer, bufferSize);
			if (empty)
			{
				return;
			}
			
			count = item.DataCount;
			code = item.Code;
		}
		
		// Извлечь тестовые данные из очереди
		bool GetTestData(TestDataQueueItemType &item, void *buffer, int bufferSize)
		{
			if (_testDataQueue.Empty())
			{
				return false;
			}
			
			item = _testDataQueue.Pop(buffer, bufferSize);
		
			return true;
		}
		
		// Извлечь тестовые данные из очереди
		static void GetTestData(void *objectPtr, void *buffer, unsigned int bufferSize, unsigned int &count, unsigned char &testType, int &testId, bool &empty)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			
			TestDataQueueItemType item;
			empty = !im->GetTestData(item, buffer, bufferSize);
			if (empty)
			{
				return;
			}
			
			count = item.DataCount;
			testType = item.TestType;
			testId = item.TestId;
		}
	
		unsigned int GetSerialNumber()
		{
			NodeConfigType::NodeConfigStruct nodeConfig;
			GetCurrentConfigCallback.Call(nodeConfig);
			return nodeConfig.SerialNumber;
		}
		
		unsigned char MyAddress()
		{
			return _myAddress;
		}
		
		unsigned char MyNetId()
		{
			return _myNetId;
		}
		
		// Возвращает реальный размер таблицы
		// Заполняет только то что влезет в буферы
		int GetSelfActivityTable(unsigned char *nodes, unsigned char *rates, unsigned int buffersSize)
		{
			int tableSize = _activityTable.GetSelfActivityTableSize();
			_activityTable.GetSelfActivityTable(nodes, rates, buffersSize);
			
			return tableSize;
		}
		
		static void GetSelfActivityTable(void *objectPtr, unsigned char *nodes, unsigned char *rates, unsigned int buffersSize, int &tableSize)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			
			tableSize = im->_activityTable.GetSelfActivityTableSize();
			im->_activityTable.GetSelfActivityTable(nodes, rates, buffersSize);
		}
		
		int GetRemoteNodeActivityTable(unsigned char remoteNodeId, unsigned char *nodes, unsigned char *rates, unsigned int buffersSize)
		{
			int tableSize = _activityTable.GetRemoteNodeActivityTableSize(remoteNodeId);
			_activityTable.GetRemoteNodeActivityTable(remoteNodeId, nodes, rates, buffersSize);
			
			return tableSize;
		}
		
		static void GetRemoteNodeActivityTable(void *objectPtr, unsigned char remoteNodeId, unsigned char *nodes, unsigned char *rates, unsigned int buffersSize, int &tableSize)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			
			tableSize = im->_activityTable.GetRemoteNodeActivityTableSize(remoteNodeId);
			im->_activityTable.GetRemoteNodeActivityTable(remoteNodeId, nodes, rates, buffersSize);
		}
		
		// Возвращает реальный размер таблицы
		// Заполняет только то что влезет в буферы
		int GetRouteTable(unsigned char *toNodes, unsigned char *fromNodes, unsigned int buffersCount)
		{
			int tableSize = _routeTable.GetRoutesCount();
			_routeTable.GetRouteTable(toNodes, fromNodes, buffersCount);
			return tableSize;
		}
		
		static void GetRouteTable(void *objectPtr, unsigned char *toNodes, unsigned char *fromNodes, unsigned int buffersCount, int &tableSize)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			
			tableSize = im->_routeTable.GetRoutesCount();
			im->_routeTable.GetRouteTable(toNodes, fromNodes, buffersCount);
		}
		
		void LoadConfig()
		{
			// TODO добавить вызов метода загрузки
			_stateMachine.Flags.NodeConfigLoaded = true;
			
			if (_stateMachine.Flags.NodeConfigLoaded)
			{
				NodeConfigType::NodeConfigStruct nodeConfig;
				GetCurrentConfigCallback.Call(nodeConfig);
			
				_frequencyChannel = nodeConfig.NetConfig.FrequencyChannel;
				_myAddress = nodeConfig.Id;
				_myNetId = nodeConfig.NetConfig.Id;
				_netIdFilterEnabled = nodeConfig.NetConfig.UseIdFilter;
			}
		}
		
		static void ApplyConfigParams(void *objectPtr)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			
			im->ApplyConfigParams();
		}
		
		void ApplyConfigParams()
		{
			_stateMachine.ApplyConfigParams();
		}
		
		void Restart()
		{
			_stateMachine.Restart();
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
		
		static void GetEventData(void *objectPtr, EventDataType &eventData)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			
			eventData = im->GetEventData();
		}
		
		void EnableDebugDataOutput()
		{
			_debugDataOutputCounter = 1000.0 / 2.5 * 60; // выдача в течение примерно минуты
		}
		
		static void EnableDebugDataOutput(void *objectPtr)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			im->EnableDebugDataOutput();
		}
		
		Tests::TestCommandResultCode TestCommand(unsigned char testType, int testId, unsigned char *testData, unsigned int testDataCount)
		{
			return _tests.TestCommand(testType, testId, testData, testDataCount);
		}
		
		static void TestCommand(void *objectPtr, unsigned char testType, int testId, unsigned char *testData, unsigned int testDataCount, int &result)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Modem *im = (Modem *)objectPtr;
			
			result = im->TestCommand(testType, testId, testData, testDataCount);
		}
		
		void ServiceTick(int tickFrequency)
		{
			if (GetState() == State::StateLocked)
			{
				PushEventInfo();
			}
			_stateMachine.ServiceTick(tickFrequency);
		}
	protected:
		static void CommonInitCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_stateMachine.Flags.ApiInited = im->_api.Init();
			im->_stateMachine.Flags.SyncInited = im->_sync.Init(5.0, 1.0, im->_myAddress);
			im->LoadConfig();
		}
		
		static void PowerUpCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_stateMachine.Flags.ConfigApplied = false;
			im->_startTx = false;
			
			im->_stateMachine.Flags.PowerOn = im->_api.PowerUp();
			if (im->_stateMachine.Flags.PowerOn)
			{
				int tryCount = 0;
				while (!im->_stateMachine.Flags.ApiTestOk && tryCount < 3)
				{
					tryCount++;
					im->_stateMachine.Flags.ApiTestOk = im->_api.Test();
					
					if (im->_stateMachine.Flags.ApiTestOk)
					{
						im->_stateMachine.Flags.ConfigApplied = Config::ApplyConfig(&im->_api);
					}
				}
			}
			
			if (im->_stateMachine.Flags.ConfigApplied)
			{
				im->_api.AllowInterruptHandling(true);
			}
			
			if (im->_powerUpResultCallback)
			{
				im->_powerUpResultCallback(im->_stateMachine.Flags.PowerOn, im->_stateMachine.Flags.ApiTestOk, im->_stateMachine.Flags.ConfigApplied);
			}
		}
		
		static void PowerDownCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_api.AllowInterruptHandling(false);
			im->_api.PowerDown();
			im->_stateMachine.Flags.PowerOn = false;
			im->_stateMachine.Flags.ApiTestOk = false;
			im->_startTx = false;
		}
		
		static void StartSyncCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			NodeConfigType::NodeConfigStruct nodeConfig;
			im->GetCurrentConfigCallback.Call(nodeConfig);
			
			TimeSlotChainInfo timeSlotChainParams;
			timeSlotChainParams.MainSlotCount = nodeConfig.NetConfig.MainTimeSlotCount;//31;
			timeSlotChainParams.AddtitionalSlotCount = nodeConfig.NetConfig.AdditionalTimeSlotCount;//480;
			timeSlotChainParams.MainTxSlot = im->_myAddress;
			timeSlotChainParams.SyncPacketPeriod = nodeConfig.NetConfig.SyncPeriod;//5;
			timeSlotChainParams.SyncTimeout = nodeConfig.NetConfig.SyncTimeout;//20;
			
			for (int i = 0; i < nodeConfig.NetConfig.AdditionalTimeSlotUsedCount; i++)
			{
				timeSlotChainParams.AdditionalSlotList.push_back(nodeConfig.NetConfig.AdditionalTimeSlotStart + i);
			}

			im->_sync.Start(im->_myAddress, timeSlotChainParams);
			im->_stateMachine.Flags.SyncRunning = true;
		}
		
		static void StopSyncCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_sync.Stop();
			im->_stateMachine.Flags.SyncRunning = false;
		}
		
		static void StartInfinitListeningCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			StopSyncCallback(callbackParam);
			im->_sync.SetInfinitListening(true);
			StartSyncCallback(callbackParam);
		}
		
		static void StartCarrierTransmitCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->StartCarrierTransmit();
			
		}
		
		static void StartActivityProcessingCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_stateMachine.Flags.ActivityTablesCollected = false;
			
			NodeConfigType::NodeConfigStruct nodeConfig;
			im->GetCurrentConfigCallback.Call(nodeConfig);
			
			if (im->_activityTable.Start(nodeConfig.NetConfig.SyncPeriod, nodeConfig.NetConfig.SyncTimeout, nodeConfig.NetConfig.ActivityTableDistributionPeriod))
			{
				im->_stateMachine.Flags.ActivityProcessingRunning = true;
			}
		}
		
		static void StopActivityProcessingCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_stateMachine.Flags.ActivityTablesCollected = false;
			im->_activityTable.Stop();
			im->_stateMachine.Flags.ActivityProcessingRunning = false;
		}
		
		static void StartRoutingCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_stateMachine.Flags.RoutesReady = false;
			
			if (im->_routeTable.Start())
			{
				im->_stateMachine.Flags.RoutingRunning = true;
				
				if (im->_stateMachine.Flags.ActivityTablesCollected)
				{
					// Искусственное побуждение перестроить таблицу маршпутов
					im->_routeTable.OnActivityTablesChanged();
				}
			}
		}
		
		static void StopRoutingCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_stateMachine.Flags.RoutesReady = false;
			im->_routeTable.Stop();
			im->_stateMachine.Flags.RoutingRunning = false;
		}
		
		static void StartTransportCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_txBufferCount = 0;
			im->_txBufferLock = false;
			if (im->_transport.Start())
			{
				im->_stateMachine.Flags.TransportRunning = true;
			}
		}
		
		static void StopTransportCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_transport.Stop();
			im->_txBufferCount = 0;
			im->_txBufferLock = false;
			im->_stateMachine.Flags.TransportRunning = false;
		}
		
		static void OnStateChangedCallback(void *callbackParam, State::State state)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_eventInfo.AddStateToHistory(state);
		}
		
		static void OnApiErrorCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_stateMachine.Flags.ApiError = true;
		}
		
		static void OnTxCompleteCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_sync.OnTxComplete();
			if (im->_onTxCompleteCallback)
			{
				im->_onTxCompleteCallback();
			}
		}
		
		static void OnRxCompleteCallback(void *callbackParam, void *data, unsigned int dataCount)
		{
			OnRxCompleteCallback(callbackParam, data, dataCount, 0);
		}
		
		static void OnRxCompleteCallback(void *callbackParam, void *data, unsigned int dataCount, int rssi)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			DEBUG_SKIP_INPUT_FROM_SOURCE;
			
			im->_sync.OnRxComplete();
			im->_parser.ProcessData(im->_myNetId, im->_netIdFilterEnabled, data, dataCount, rssi);
			im->_signalRx.Set();
			
			static const int sessionDataSize = 1 + 2 + 4;
			
			unsigned char debugData[10 + sessionDataSize];
			
			int debugDataCount = 10;
			
			if (dataCount > 0)
			{
				debugData[0] = ((unsigned char *)data)[0];
				debugData[1] = ((unsigned char *)data)[1];
			}
			else
			{
				debugData[0] = 0xFF;
				debugData[1] = 0xFF;
			}
			debugData[2] = dataCount & 0xFF;
			debugData[3] = (dataCount >> 8) & 0xFF;
			debugData[4] = (dataCount >> 16) & 0xFF;
			debugData[5] = (dataCount >> 24) & 0xFF;
			debugData[6] = rssi & 0xFF;
			debugData[7] = (rssi >> 8) & 0xFF;
			debugData[8] = (rssi >> 16) & 0xFF;
			debugData[9] = (rssi >> 24) & 0xFF;
			
			if (dataCount > 0)
			{
				if (debugData[0] == Protocol::CommandTransportSession)
				{
					for (int i = 0; i < sessionDataSize; i++)
					{
						debugData[10 + i] = ((unsigned char *)data)[sizeof(Protocol::PacketHeader) + i];
					}
					
					debugDataCount += sessionDataSize;
				}
			}
			
			OnDebugDataCallback(im, DebugDataRx, debugData, debugDataCount);
			
			/*
			{
			
				OnDebugDataCallback(im, DebugDataRxFlat, data, dataCount);
			
			}
			*/
		}
		
		static void OnEndTimeChainCallback(void *callbackParam, int slotCount)
		{
			DebugSignal::Set(1);
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_netTime.OnEndTimeChain();
			
			if (im->_startSecondPriorityWorkCallback)
			{
				im->_startSecondPriorityWorkCallback();
			}
			
			DebugSignal::Clear(1);
		}
		
		void OnEndTimeChainAsync()
		{
			DebugSignal::Set(1);
			_activityTable.OnEndTimeSlotChain();
			
			if (_activityTableCompletenessCheckFlag)
			{
				_activityTableCompletenessCheckFlag  = false;
				CheckActivityTableCompleteness();
			}
			
			_routeTable.OnEndTimeSlotChain(_myAddress);
			
			if (_routeTable.GetRoutesCount() > 0)
			{
				_stateMachine.Flags.RoutesReady = true;
			}
			
			UpdateTransportSessions();
			//PrepareTransportData();
			
			_stateMachine.OnEndTimeSlotChain();
			
			// пополнялись отладочные данные
			if (_debugDataRefilled)
			{
				_debugDataRefilled = false;
				_eventInfo.SetDebugDataRefilled();
			}
			
			// сообщим о произошедших за цикл событиях (одни раз в цикл, чтобы не переполнить очередь событий)
			PushEventInfo();
			
			DebugSignal::Clear(1);
		}
		
		static void RxOnOffCallback(void *callbackParam, bool enableRx, bool disableByTimeout)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->RxOnOff(enableRx, disableByTimeout);
		}
		
		void RxOnOff(bool enableRx, bool disableByTimeout)
		{
			if (enableRx)
			{
				if (!_api.IsRx())
				{
					_api.Rx(disableByTimeout, _frequencyChannel);
				}
			}
			else
			{
				if (_api.IsRx())
				{
					_api.CancelRx();
				}
			}
		}
		
		void TxPacket(void * data, int len, bool insertPause = false)
		{
			if (0 == data)
			{
				return;
			}
			
			if (len < 1)
			{
				return;
			}
			
			if (_api.IsRx())
			{
				_api.CancelRx();
			}
			/*
			if (insertPause)
			{
				for (int i = 0; i < 200; i++)
				{
					
				}
			}*/
			if (!_startTx)
			{
				_api.TxPrepare(data, len);
				_startTx = true;
				_signalTx.Set();
			}
		}
		
		static void TxSyncEquivalentCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			if (im->_builder.MakeSyncEquivalent())
			{
				im->TxPacket(im->_builder.PacketData, im->_builder.PacketLen);
			}
		}
		
		static void TxBiggestPacketEquivalentCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			if (im->_builder.MakeBiggestPacketEquivalent(63))
			{
				im->TxPacket(im->_builder.PacketData, im->_builder.PacketLen);
			}
		}
		
		static void TxSyncRequestCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			if (im->_builder.MakeSyncRequest(im->_myAddress, im->_myNetId))
			{
				im->TxPacket(im->_builder.PacketData, im->_builder.PacketLen);
			}
		}
		
		static void TxSyncCallback(void *callbackParam, float txSyncTime, int slot)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			NodeConfigType::NodeConfigStruct nodeConfig;
			im->GetCurrentConfigCallback.Call(nodeConfig);
			
			bool netTimeValid = false;
			unsigned int netTime = (unsigned int)im->_netTime.GetTime(&netTimeValid);
			
			if (im->_builder.MakeSync(im->_myAddress, im->_myNetId, txSyncTime, slot, nodeConfig.NetConfig, netTime, netTimeValid))
			{
				im->TxPacket(im->_builder.PacketData, im->_builder.PacketLen, true);
			}
		}
		
		static void OnRxSyncRequestCallback(void *callbackParam, AddressType sourceAddress)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_sync.OnSyncRequestReceived(sourceAddress);
		}
		
		static void OnRxNetConfigCallback(void *callbackParam, NetConfig::NetConfigStruct netConfig)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			// сравниваем нашу конфигурацию с полученной из эфира
			NodeConfigType::NodeConfigStruct nodeConfig;
			im->GetCurrentConfigCallback.Call(nodeConfig);
			// если не совпадают, выставляется флаг
			// если совпадают, флаг не изменяется, вдруг раньше выставился, а мы затрём
			if (nodeConfig.NetConfig != netConfig)
			{
				im->_stateMachine.Flags.NetConfigInvalid = true;
			}
		}
		
		static void OnRxSyncInfoCallback(void *callbackParam, typename SyncType::SyncInfoType &syncInfo)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_sync.OnSyncInfoReceived(syncInfo);
			DEBUG_SKIP_ACTIVITY_FROM_SOURCE
			im->_activityTable.OnActivityDetected(syncInfo.SourceAddress);
		}
		
		static void OnRxNetTimeCallback(void *callbackParam, AddressType sourceAddress, unsigned int netTime, bool netTimeValid)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			// если пришедшее время действительно
			// и при этом
			//	если у нас нет своего, то используем любое время
			//	или если есть своё, то используем время от узлов с меньшим номером
			if (netTimeValid && (!im->_netTime.TimeValid() || sourceAddress < im->_myAddress))
			{
				im->_netTime.OnNetTimeReceived(sourceAddress, netTime);
			}
		}
		
		static void OnRxActivityTableCallback(void *callbackParam, Protocol::ActivityTableInfo &info)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->OnRxActivityTable(info.Source, info.Owner, info.RelayMode, info.Ticket, info.ItemsCount, info.Nodes, info.Rates);
		}
		
		static void OnRxTransportSessionCallback(void *callbackParam, AddressType sourceAddress, unsigned char *data, int dataCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_transport.OnRxSessionData(im->_myAddress, sourceAddress, data, dataCount);
		}
		
		static void OnRxTestDataCallback(void *callbackParam, AddressType sourceAddress, unsigned char *data, int dataCount, int rssi)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_tests.OnRxData(im->_myAddress, sourceAddress, data, dataCount, rssi);
		}
		
		static void OnSyncStateChangedCallback(void *callbackParam, typename SyncType::State state)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			// синхронизировано
			bool sync = false;
			switch (state)
			{
			case SyncType::StateDisabled:
			case SyncType::StateStarting:
			case SyncType::StateMeasureSyncTxTime:
				{
					im->_stateMachine.Flags.SyncState = State::FlagsStruct::SyncStateUnknown;
				}
				break;
			case SyncType::StateListening:
				{
					im->_stateMachine.Flags.SyncState = State::FlagsStruct::SyncStateSearchigSyncSources;
				}
				break;
			case SyncType::StateSynchronizing:
				{
					im->_stateMachine.Flags.SyncState = State::FlagsStruct::SyncStateStateSynchronizing;
				}
				break;
			case SyncType::StateSync:
				{
					sync = true;
					im->_stateMachine.Flags.SyncState = State::FlagsStruct::SyncStateSync;
					if (im->_sync.IsFirstMaster())
					{
						im->_netTime.SetCurTime(0.0);
					}
					else
					{
						im->_netTime.SetCurTimeByReceivedTime();
					}
				}
				break;
			case SyncType::StateError:
				{
					im->_stateMachine.Flags.SyncState = State::FlagsStruct::SyncStateError;
				}
				break;
			}
			
			if (sync)
			{
				im->_signalSync.Set(0);
			}
			else
			{
				im->_signalSync.Clear();
			}
		}
		
		static void OnTxTimeSlotCallback(void *callbackParam, int slot, bool mainSlot, int dataSlotCounter, bool &txOccured)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			if (mainSlot)
			{
				im->OnMainTxTimeSlot(slot);
			}
			else
			{
				im->OnAdditionalTxTimeSlot(slot, dataSlotCounter);
			}
			
			txOccured = im->_txBufferCount > 0;
		}
		
		void OnMainTxTimeSlot(int slot)
		{
			// В первую очередь данные о слышимости
			if (!_activityTableTranslator.QueueEmpty())
			{
				bool netTimeValid;
				NetTimeType::NetTimeType ticketValue = _netTime.GetTime(&netTimeValid);
				if (netTimeValid)
				{
					unsigned char *owner = 0;
					unsigned char *relayMode = 0;
					unsigned char *itemsCount = 0;
					unsigned char *nodes = 0;
					unsigned char *rates = 0;
					NetTimeType::NetTimeType *ticket = 0;
					unsigned int maxItemsCount = 0;
					
					if (_builder.MakeActivityTable(_myAddress, _myNetId, &ticket, &owner, &relayMode, &itemsCount, &nodes, &rates, &maxItemsCount))
					{
						// для начала наше значение билета
						// но оно может измениться при вызове _activityTableTranslator.OnTxTable
						if (ticket != 0)
						{
							*ticket = ticketValue;
						}
						
						unsigned int forTransmit = _activityTableTranslator.OnTxTable(ticket, owner, relayMode, nodes, rates, maxItemsCount);
						if (forTransmit)
						{
							if (itemsCount != 0)
							{
								*itemsCount = forTransmit;
							}
							
							TxPacket(_builder.PacketData, _builder.PacketLen);
						}
					}
				}
			}
			else
			{
				// передаем тестовые данные если есть
				if (PrepareTestData())
				{
					TxTestData();
				}
				else
				{
					// иначе можно передавать иные данные
					
					// если нет дополнительных слотов, то передаем пользовательские данные в основном слоте
					if (_sync.GetTotalSlotCount() < 2)
					{
						// остальные данные
						// гтовим
						PrepareTransportData(true, 64 + 6 + 20);
						// отпарвляем
						TxDataFromTransport();
					}
				}
			}
		}
		
		void OnAdditionalTxTimeSlot(int slot, int dataSlotCounter)
		{
			if (dataSlotCounter == 0)
			{
				// готовим в первом слоте
				PrepareTransportData(false, 0);
			}
			else
			{
				// отправляем в следующем
				TxDataFromTransport();
			}
		}
		
		void TxDataFromTransport()
		{
			if (_txBufferLock)
			{
				return;
			}
			
			_txBufferLock = true;
			if (_txBufferCount > 0)
			{
				TxPacket(_txBuffer, _txBufferCount);
				_txBufferCount = 0;
			}
			_txBufferLock = false;
		}
		
		static void GetCurrentTimeSlotAndCountCallback(void *callbackParam, int &timeSlot, int& timeSlotCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			timeSlot = im->_sync.GetCurrentTimeSlot();
			timeSlotCount = im->_sync.GetTimeSlotCount();
		}
		
		static void OnSelfActivityTableChangedCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->OnSelfActivityTableChanged();
		}
		
		static void OnSelfActivityTableRegularDistributionCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->OnSelfActivityTableRegularDistribution();
		}
		
		void OnSelfActivityTableChanged()
		{
			int tableSize = _activityTable.GetSelfActivityTableSize();
			if (tableSize <= sizeof(_activityTableNodesBuffer))
			{
				int returned = _activityTable.GetSelfActivityTable(_activityTableNodesBuffer, _activityTableRatesBuffer, sizeof(_activityTableNodesBuffer));
				if (returned > 0)
				{
					NodeConfigType::NodeConfigStruct nodeConfig;
					GetCurrentConfigCallback.Call(nodeConfig);
					
					_activityTableTranslator.Enqueue(_myAddress, nodeConfig.NetConfig.RelayMode, _activityTableNodesBuffer, _activityTableRatesBuffer, returned);
				}
			}
			
			_activityTableCompletenessCheckFlag  = true;
			
			_routeTable.OnActivityTablesChanged();
			
			_eventInfo.SetSelfActivityTableChanged();
		}
		
		void OnSelfActivityTableRegularDistribution()
		{
			int tableSize = _activityTable.GetSelfActivityTableSize();
			if (tableSize <= sizeof(_activityTableNodesBuffer))
			{
				int returned = _activityTable.GetSelfActivityTable(_activityTableNodesBuffer, _activityTableRatesBuffer, sizeof(_activityTableNodesBuffer));
				if (returned > 0)
				{
					NodeConfigType::NodeConfigStruct nodeConfig;
					GetCurrentConfigCallback.Call(nodeConfig);
					
					_activityTableTranslator.Enqueue(_myAddress, nodeConfig.NetConfig.RelayMode, _activityTableNodesBuffer, _activityTableRatesBuffer, returned);
				}
			}
			
			_eventInfo.SetSelfActivityTableRegularDistribution();
		}
		
		void OnRxActivityTable(AddressType source, AddressType owner, unsigned char relayMode, float ticket, unsigned char itemsCount, AddressType *nodes, unsigned char *rates)
		{
			_activityTable.OnRemoteNodeTableReceivedDebug(source, owner, relayMode, ticket, nodes, rates, itemsCount);
			
			// пока не синхронизированы нет смысла обрабатывать
			if (!_stateMachine.IsSync())
			{
				return;
			}
			
			// собственные пакеты нам не интересны (которые переретранслировались)
			if (owner == _myAddress)
			{
				return;
			}
			
			_eventInfo.SetRemoteNodeActivityTableReceived(owner);
			
			// если было принято решение обработать принятую информацию
			if (_activityTableTranslator.OnRxActivityTable(owner, relayMode, ticket, itemsCount, nodes, rates))
			{
				// то обновляем таблицу слышимости удаленного узла
				_activityTable.OnRemoteNodeTableReceived(owner, relayMode, nodes, rates, itemsCount);
			}
		}
		
		static void OnRemoteNodeActivityTableChangedCallback(void *callbackParam, int nodeId)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_activityTableCompletenessCheckFlag  = true;
			
			im->_routeTable.OnActivityTablesChanged();
			
			im->_eventInfo.SetRemoteNodeActivityTableChanged(nodeId);
		}
		
		void CheckActivityTableCompleteness()
		{
			// таблицы уже когда то были полностью собраны, можно не проверять
			if (_stateMachine.Flags.ActivityTablesCollected)
			{
				return;
			}
			
			typename ActivityTableCompletenessQueueType::iterator q;
			_activityTableCompletenessQueue.clear();
			
			int tableSize = _activityTable.GetSelfActivityTableSize();
			
			if (tableSize <= sizeof(_activityTableNodesBuffer))
			{
				int returned = _activityTable.GetSelfActivityTable(_activityTableNodesBuffer, _activityTableRatesBuffer, sizeof(_activityTableNodesBuffer));
				if (returned > 0)
				{
					for (int i = 0; i < returned; i++)
					{
						int node = _activityTableNodesBuffer[i];
						int rate = _activityTableRatesBuffer[i];
						if (rate > 0)
						{
							q = _activityTableCompletenessQueue.find(node);
							if (q == _activityTableCompletenessQueue.end() && _activityTableCompletenessQueue.size() < ActivityTableCompletenessQueueSize)
							{
								typename ActivityTableCompletenessQueueType::value_type item(node, ActivityTableCompletenessStateNotChecked);
								_activityTableCompletenessQueue.insert(item);
							}
						}
					}
				}
			}
			
			// никого не слышим, не сможем принять решение
			if (_activityTableCompletenessQueue.empty())
			{
				return;
			}
			
			bool complete = true;
			// TODO попробуем отказаться от ожидания получения таблиц от всех узлов сети
			// т.к. ждать можно очень долго при плохой связи хотя бы на одном узле
			//q = _activityTableCompletenessQueue.begin();
			q = _activityTableCompletenessQueue.end();
			while(q != _activityTableCompletenessQueue.end())
			{
				// ещё не проверяли
				if (q->second == ActivityTableCompletenessStateNotChecked)
				{
					int node = q->first;
					if (_activityTable.IsRemoteNodeActivityTablePresent(node))
					{
						// проверили - есть таблица
						q->second = ActivityTableCompletenessStateCheckedPresent;
						q++;
						
						// в таблице удаленного узла поищем те узлы, которые мы не слышим напрямую, но слышит проверяемый в данный момент узел
						int tableSize = _activityTable.GetRemoteNodeActivityTableSize(node);
						if (tableSize <= sizeof(_activityTableNodesBuffer))
						{
							int returned = _activityTable.GetRemoteNodeActivityTable(node, _activityTableNodesBuffer, _activityTableRatesBuffer, sizeof(_activityTableNodesBuffer));
							if (returned > 0)
							{
								for (int i = 0; i < returned; i++)
								{
									int nodeTmp = _activityTableNodesBuffer[i];
									int rateTmp = _activityTableRatesBuffer[i];
									// откидываем из содержимого узлы которые не слышно и совпадающий с нашим адрес (для него у нас своя таблица слышимости)
									if (nodeTmp != _myAddress && rateTmp > 0)
									{
										typename ActivityTableCompletenessQueueType::iterator f = _activityTableCompletenessQueue.find(nodeTmp);
										// этот узел встречаем первый раз, добавим его в список
										if (f == _activityTableCompletenessQueue.end() && _activityTableCompletenessQueue.size() < ActivityTableCompletenessQueueSize)
										{
											typename ActivityTableCompletenessQueueType::value_type item(nodeTmp, ActivityTableCompletenessStateNotChecked);
											_activityTableCompletenessQueue.insert(item);
											
											// поновой, т.к. итератор при вставке может сбиться
											q = _activityTableCompletenessQueue.begin();
										}
									}
								}
							}
						}
					}
					else
					{
						// проверили - нет таблицы
						q->second = ActivityTableCompletenessStateCheckedNotPresent;
						// нашелся узел от которого пока не приходило таблицы слышимости
						complete = false;
						// дальше нет смысла проверять
						break;
						// если надо всётаки полностью заполнить список, то закоментируйте break, и раскоментируйте q++
						// q++;
					}
				}
				else
				{
					// уже проверен, переходим к следующему
					q++;
				}
			}
			
			_stateMachine.Flags.ActivityTablesCollected |= complete;
		}
		
		unsigned char GetActivityTableValue(AddressType fromNode, AddressType toNode)
		{
			unsigned char res = 0;
			
			if (toNode == _myAddress)
			{
				res = _activityTable.GetSelfActivityTableValue(fromNode);
			}
			else
			{
				res = _activityTable.GetRemoteActivityTableValue(toNode, fromNode);
			}
			
			return res;
		}
		
		static void GetLinksCallback(void *callbackParam, AddressType fromNode, AddressType* toNodes, unsigned char *lens, int buffersSize, int *itemsCount)
		{
			if (0 == callbackParam || buffersSize < 1 || itemsCount == 0)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			/*
			// это первоначальный вариант получения данных о том, как узел fromNode слышит узлы toNodes
			int tableSize = 0;
			int returned = 0;
			if (fromNode == im->_myAddress)
			{
				tableSize = im->_activityTable.GetSelfActivityTableSize();
				if (tableSize <= buffersSize)
				{
					returned = im->_activityTable.GetSelfActivityTable(toNodes, lens, buffersSize);
				}
			}
			else
			{
				tableSize = im->_activityTable.GetRemoteNodeActivityTableSize(fromNode);
				if (tableSize <= buffersSize)
				{
					returned = im->_activityTable.GetRemoteNodeActivityTable(fromNode, toNodes, lens, buffersSize);
				}
			}
			
			// инвертируем значение слышимости, чтобы оно выглядело как "расстояние" до узла, для формирования таблицы маршрутов
			for (int i = 0; i < returned; i++)
			{
				// мы расчитываем на то, что слышимости не должна превышать 100%
				if (lens[i] > 100)
				{
					lens[i] = 100;
				}
				// инвертируем
				lens[i] = 100 - lens[i];
			}
			*/
			/*
			// это второй вариант
			// получем данные о том как узлы toNodes слышат узел fromNode
			
			// список узлов для котороых есть таблицы слышимости
			int returned = 0;
			for (int i = 0; i < buffersSize; i++)
			{
				// до самого себя пропускаем
				if (i == fromNode)
				{
					continue;
				}
				
				bool present = false;
				
				if (i == im->_myAddress)
				{
					toNodes[returned] = i;
					// одновременно получем данные из слышимости одного и того же узла fromNode в разных узлах
					lens[returned] = im->_activityTable.GetSelfActivityTableValue(fromNode);
					present = true;
				}
				else
				{
					if (im->_activityTable.RemoteNodeActivityTablePresent(i))
					{
						toNodes[returned] = i;
						// одновременно получем данные из слышимости одного и того же узла fromNode в разных узлах
						lens[returned] = im->_activityTable.GetRemoteActivityTableValue(toNodes[returned], fromNode);
						present = true;
					}
				}
				
				if (present)
				{
					// инвертируем значение слышимости, чтобы оно выглядело как "расстояние" до узла, для формирования таблицы маршрутов
					// мы расчитываем на то, что слышимости не должна превышать 100%
					if (lens[returned] > 100)
					{
						lens[returned] = 100;
					}
					// инвертируем
					lens[returned] = 100 - lens[returned];
					returned++;
				}
			}
			// конец второго варианта
			*/
			
			// это новый вариант
			// учитываем данные о том как узлы toNodes слышат узел fromNode и в обратном направлении
			
			// список узлов для котороых есть таблицы слышимости
			int returned = 0;
			for (int i = 0; i < buffersSize; i++)
			{
				// до самого себя пропускаем
				if (i == fromNode)
				{
					continue;
				}
				
				bool present = false;
				
				if (i == im->_myAddress)
				{
					toNodes[returned] = i;
					// как слышат нас
					unsigned char direct = 0;
					if (fromNode == im->_myAddress)
					{
						direct = im->_activityTable.GetSelfActivityTableValue(toNodes[returned]);
					}
					else
					{
						direct = im->_activityTable.GetRemoteActivityTableValue(fromNode, toNodes[returned]);
					}
					// как слышим мы
					unsigned char revers = im->_activityTable.GetSelfActivityTableValue(fromNode);
					
					lens[returned] = direct < revers ? direct : revers;
					present = true;
				}
				else
				{
					if (im->_activityTable.RemoteNodeActivityTablePresent(i))
					{
						toNodes[returned] = i;
						// как слышат нас
						unsigned char direct = 0;
						if (fromNode == im->_myAddress)
						{
							direct = im->_activityTable.GetSelfActivityTableValue(toNodes[returned]);
						}
						else
						{
							direct = im->_activityTable.GetRemoteActivityTableValue(fromNode, toNodes[returned]);
						}
						// как слышим мы
						unsigned char revers = im->_activityTable.GetRemoteActivityTableValue(toNodes[returned], fromNode);
						
						lens[returned] = direct < revers ? direct : revers;
						present = true;
					}
				}
				
				if (present)
				{
					// инвертируем значение слышимости, чтобы оно выглядело как "расстояние" до узла, для формирования таблицы маршрутов
					// мы расчитываем на то, что слышимости не должна превышать 100%
					if (lens[returned] > 100)
					{
						lens[returned] = 100;
					}
					// инвертируем
					lens[returned] = 100 - lens[returned];
					returned++;
				}
			}
			// конец нового варианта
			
			*itemsCount = returned;
		}
		
		unsigned char GetRelayMode(AddressType node)
		{
			unsigned char res = 0;
			
			if (node == _myAddress)
			{
				NodeConfigType::NodeConfigStruct nodeConfig;
				GetCurrentConfigCallback.Call(nodeConfig);
					
				res = nodeConfig.NetConfig.RelayMode;
			}
			else
			{
				res = _activityTable.GetRemoteNodeRelayMode(node);
			}
			
			return res;
		}
		
		static void GetRelayModeCallback(void *callbackParam, AddressType node, unsigned char &relayMode)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;

			relayMode = im->GetRelayMode(node);
		}
		
		static void OnRouteTableChangedCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_eventInfo.SetRouteTableChanged();
		}
		
		static void GetTicketInfoCallbcak(void *callbackParam, int &selfAddress, float &netTime)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			selfAddress = im->_myAddress;
			netTime = im->_netTime.GetTime();
		}
		
		static void GetRouteCallback(void *callbackParam, AddressType src, AddressType dst, AddressType *buffer, int bufferSize, int *routeLen)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			int routeItemsCount = im->_routeTable.GetRouteToNode(src, dst, buffer, bufferSize);
			if (routeLen != 0)
			{
				*routeLen = routeItemsCount;
			}
		}
		
		void UpdateTransportSessions()
		{
			_transport.UpdateSessions();
			_transport.DequeueData(_myAddress);
		}
		
		void PrepareTransportData(bool limited, int limit)
		{
			if (_builder.MakeTransportSession(_myAddress, _myNetId, this, PrepareSessionData, limited, limit))
			{
				while(_txBufferLock)
				{
					;
				}
				
				_txBufferLock = true;
				_txBufferCount = _builder.PacketLen;
				if (_txBufferCount > sizeof(_txBuffer))
				{
					_txBufferCount = sizeof(_txBuffer);
				}
				
				for (int i = 0; i < _txBufferCount; i++)
				{
					_txBuffer[i] = _builder.PacketData[i];
				}
				_txBufferLock = false;
			}
		}
		
		static int PrepareSessionData(void *callbackObject, unsigned char *buffer, int bufferSize)
		{
			if (0 == callbackObject)
			{
				return 0;
			}
			
			Modem *im = (Modem *)callbackObject;
			
			int res = im->_transport.GetOutputData(im->_myAddress, buffer, bufferSize);
			return res;
		}
		
		static void OnDataReceivedCallback(void *callbackParam, AddressType src, unsigned char *data, int dataCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_inputDataQueue.Push(DataQueueItemType(src, dataCount), data);
			
			im->_eventInfo.SetDataReceived();
		}
		
		static void OnDebugDataCallback(void *callbackParam, int code, void *data, int dataCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			if (im->_debugDataOutputCounter > 0)
			{
				im->_debugDataQueue.Push(DebugDataQueueItemType(code, dataCount), data);
				// пока выставим флаг, событие выкинем в конце цикла
				im->_debugDataRefilled = true;
			}
		}
		
		static void OnTestDataCallback(void *callbackParam, unsigned char testType, int testId, unsigned char *data, int dataCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			im->_testDataQueue.Push(TestDataQueueItemType(testType, testId, dataCount), data);
			
			im->_eventInfo.SetTestDataPresent();
		}
		
		static void OnSendTestDataToNodeCallback(void *callbackParam, int nodeId, unsigned char testType, int testId, unsigned char *data, int dataCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->SendTestDataToNodeCallback.Call(nodeId, testType, testId, data, dataCount);
		}
		
		static void OnTxCompleteCallback(void *callbackParam, int packetId, AddressType node, typename TransportType::TxResult txResult)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			DataTxResult dataTxResult = DataTxResultUnknown;
			
			switch(txResult)
			{
			case TransportType::TxResultDelivered:
				dataTxResult = DataTxResultDelivered;
				break;
			case TransportType::TxResultDeliveryImpossible:
				dataTxResult = DataTxResultDeliveryImpossible;
				break;
			case TransportType::TxResultError:
				dataTxResult = DataTxResultError;
				break;
			}
			
			im->_eventInfo.SetDeliveryComplete(packetId, node, dataTxResult);
			im->_tests.OnDeliveryComplete(packetId, node, dataTxResult == DataTxResultDelivered);
		}
		
		static void OnStateMachineResetCallback(void *callbackParam, int resetReasoneCode)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			if (im->_resetCallback)
			{
				im->_resetCallback(ResetReasoneStateMachineGroupStart + resetReasoneCode);
			}
		}
		
		static void OnSyncResetCallback(void *callbackParam, int resetReasoneCode)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
			if (im->_resetCallback)
			{
				im->_resetCallback(ResetReasoneSyncGroupStart + resetReasoneCode);
			}
		}
		
		static void AllowEnergySavingListeningCallback(void *callbackParam, bool &allow)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			
#ifdef RADIODOZOR
			allow = im->_routeTable.RouteToNodePresent(0);
#else
			allow = true;
#endif
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
		
		bool PrepareTestData()
		{
			bool res = false;
			unsigned char *bufferPtr;
			int testDataCount = _tests.PopData(bufferPtr);
			if (testDataCount)
			{
				if (_builder.MakeTestPacket(_myAddress, _myNetId, bufferPtr, testDataCount))
				{
					while(_txBufferLock)
					{
						;
					}
					
					_txBufferLock = true;
					_txBufferCount = _builder.PacketLen;
					if (_txBufferCount > sizeof(_txBuffer))
					{
						_txBufferCount = sizeof(_txBuffer);
					}
					
					for (int i = 0; i < _txBufferCount; i++)
					{
						_txBuffer[i] = _builder.PacketData[i];
					}
					_txBufferLock = false;
					
					res = true;
				}
			}
			
			return res;
		}
		
		void TxTestData()
		{
			if (_txBufferLock)
			{
				return;
			}
			
			_txBufferLock = true;
			if (_txBufferCount > 0)
			{
				TxPacket(_txBuffer, _txBufferCount);
				_txBufferCount = 0;
			}
			_txBufferLock = false;
		}
		
		static void GoToListeningModeCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_stateMachine.Flags.GoToListeningMode = true;
		}
		
		static void GoToPowerTestModeCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->_stateMachine.Flags.GoToPowerTestMode = true;
		}
		
		static void GoToNormalModeCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->Restart();
		}
		
		void StartCarrierTransmit()
		{
			bool powerOn = _api.PowerUp();
			_stateMachine.Flags.ConfigApplied = false;
			if (powerOn)
			{
				int tryCount = 0;
				bool apiTestOk = false;
				while (!apiTestOk && tryCount < 3)
				{
					tryCount++;
					apiTestOk = _api.Test();
					
					if (apiTestOk)
					{
						_stateMachine.Flags.ConfigApplied = Config::ApplyConfig(&_api, Config::ConfigUnmodulatedCarrier);
					}
				}
			}
			
			if (_stateMachine.Flags.ConfigApplied)
			{
				TickSource::Disable();
				TickSource::OnTickCallback.Set(OnCarrierTransmitTickCallback, this);
				TickSource::OnTickPart1Callback.Reset();
				TickSource::Enable();
			}
		}
		
		static void OnCarrierTransmitTickCallback(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			im->OnCarrierTransmitTick();
		}
		
		void OnCarrierTransmitTick()
		{
			static bool pause = true;
			static int timeoutCounter = 0;
			static const int tickCountPerSecond = 400;
			static const int tickCountPer100Millecond = tickCountPerSecond / 10;
			
			if (timeoutCounter < 2)
			{
				if (pause)
				{
					if (_onTxPrepareCallback)
					{
						_onTxPrepareCallback();
					}
					_api.CancelTx();
					_api.CancelRx();
					_api.TxStart(0);
					timeoutCounter = tickCountPer100Millecond;
					pause = false;
				}
				else
				{
					_api.CancelTx();
					if (_onTxCompleteCallback)
					{
						_onTxCompleteCallback();
					}
					timeoutCounter = 3 * tickCountPerSecond;
					pause = true;
				}
			}
			else
			{
				timeoutCounter--;
			}
		}
		
		static void EnqueuePacketCallback(void *callbackParam, int dst, unsigned char *data, unsigned int dataCount, unsigned int deliveryTimeout, int &packetId, bool &result)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Modem *im = (Modem *)callbackParam;
			AddressType dstsList[] = {dst};
			result = im->EnqueueOutputData(dstsList, 1, data, dataCount,deliveryTimeout, packetId);
		}
	};
}
}

#endif