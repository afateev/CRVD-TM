///////////////////////////////////////////////////////////////////////////////
//	Флаги состояний модема
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_STATE_FLAGS_H
#define MODEM_STATE_FLAGS_H

namespace Rblib
{
namespace Modem
{
namespace State
{
	struct FlagsStruct
	{
		enum SyncStateType
		{
			SyncStateUnknown = 0,
			SyncStateSearchigSyncSources,
			SyncStateStateSynchronizing,
			SyncStateSync,
			SyncStateError
		};
		
		bool ApiInited;
		bool PowerOn;
		bool ApiTestOk;
		bool ConfigApplied;
		bool NodeConfigLoaded;
		bool SyncInited;
		bool SyncRunning;
		SyncStateType SyncState;
		bool NetConfigInvalid;
		bool ActivityProcessingRunning;
		bool ActivityTablesCollected;
		bool RoutingRunning;
		bool RoutesReady;
		bool TransportRunning;
		bool GoToListeningMode;
		bool GoToPowerTestMode;
		bool ApiError;
		
		// Конструктор по умолчанию
		FlagsStruct()
		{
			Reset();
		}
		
		// Сброс всех флагов
		void Reset()
		{
			ApiInited = false;
			PowerOn = false;
			ApiTestOk = false;
			ConfigApplied = false;
			NodeConfigLoaded = false;
			SyncInited = false;
			SyncRunning = false;
			SyncState = SyncStateUnknown;
			NetConfigInvalid = false;
			ActivityProcessingRunning = false;
			ActivityTablesCollected = false;
			RoutingRunning = false;
			RoutesReady = false;
			TransportRunning = false;
			GoToListeningMode = false;
			GoToPowerTestMode = false;
			ApiError = false;
		}
	};
}
}
}

#endif