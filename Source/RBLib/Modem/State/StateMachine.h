///////////////////////////////////////////////////////////////////////////////
//	Автомат состояний модема
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_STATE_STATEMACHINE_H
#define MODEM_STATE_STATEMACHINE_H

#include "State.h"
#include "Flags.h"
#include "LockReasoneCode.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Modem
{
namespace State
{
	class StateMachine
	{
	public:
		typedef CallbackWrapper<> CallbackType;
		typedef CallbackWrapper<State> StateChangedCallbackType;
		typedef CallbackWrapper<int> ResetCallbackType;
		static const unsigned int RestartOnLockTimeout = 1;
	public:
		FlagsStruct Flags;
		
		enum ResetReasoneCode
		{
			ResetReasoneConfigChanged = 0
		};
	public:
		CallbackType StartInfinitListeningCallback;
		CallbackType StartCarrierTransmitCallback;
	public:
		CallbackType CommonInitCallback;
	protected:
		State _state;
		CallbackType _powerUpCallback;
		CallbackType _powerDownCallback;
		CallbackType _nodeConfigLoadCallback;
		CallbackType _startSyncCallback;
		CallbackType _stopSyncCallback;
		CallbackType _startActivityProcessingCallback;
		CallbackType _stopActivityProcessingCallback;
		StateChangedCallbackType _onStateChangedCallback;
		CallbackType _startRoutingCallback;
		CallbackType _stopRoutingCallback;
		CallbackType _startTransportCallback;
		CallbackType _stopTransportCallback;
		ResetCallbackType _resetCallback;
		unsigned int _lockElapsedTimeCount;
		LockReasoneCode _lockReasoneCode;
	public:
		// Конструктор по умолчанию
		StateMachine()
		{
			Reset();
		}
		
		// Включить модем
		void Enable()
		{
			switch (_state)
			{
			case StateDisabled:
				{
					SetState(StateInitializing);
					_lockReasoneCode = LockReasoneUnknown;
					
					CommonInitCallback();
					if (!Flags.ApiInited)
					{
						Lock(LockReasoneTransiverInitError);
						break;
					}
					
					if (!Flags.SyncInited)
					{
						Lock(LockReasoneSyncInitError);
						break;
					}
					
					_powerUpCallback.Call();
					if (!Flags.PowerOn)
					{
						Lock(LockReasonePowerUpError);
						break;
					}
					
					if (!Flags.ApiTestOk)
					{
						Lock(LockReasoneTransiverTestError);
						break;
					}
					
					if (!Flags.ConfigApplied)
					{
						Lock(LockReasoneTransiverConfigError);
						break;
					}
					
					_nodeConfigLoadCallback.Call();
					if (!Flags.NodeConfigLoaded)
					{
						Lock(LockReasoneNetConfigError);
						break;
					}
					
					_startSyncCallback.Call();
					if (!Flags.SyncRunning)
					{
						Lock(LockReasoneSyncStartError);
						break;
					}
				}
				break;
			}
		}
		
		// Отключить модем
		void Disable()
		{
			_stopTransportCallback.Call();
			_stopRoutingCallback.Call();
			_stopActivityProcessingCallback.Call();
			_stopSyncCallback.Call();
			_powerDownCallback.Call();
			
			SetState(StateDisabled);
		}
		
		// Должна вызываться в конце цикла сети
		void OnEndTimeSlotChain()
		{
			// попалась несовпадающая конфигурация, вырубаемся
			if (Flags.NetConfigInvalid)
			{
				Flags.NetConfigInvalid = false;
				//Lock();
				//return;
			}
			
			if (Flags.ApiError)
			{
				Lock(LockReasoneTransiverError);
				return;
			}
			
			switch (Flags.SyncState)
			{
			case FlagsStruct::SyncStateUnknown:
				{
				}
				break;
			case FlagsStruct::SyncStateSearchigSyncSources:
				{
					switch (_state)
					{
					case StateInitializing:
					case StateSynchronizing:
						{
							SetState(StateSearchigSyncSources);
						}
						break;
					case StateSync:
					case StatePrepareLinkTable:
					case StatePrepareRouteTable:
					case StateReady:
						{
							_stopTransportCallback.Call();
							_stopRoutingCallback.Call();
							_stopActivityProcessingCallback.Call();
							SetState(StateSearchigSyncSources);
						}
						break;
					}
				}
				break;
			case FlagsStruct::SyncStateStateSynchronizing:
				{
					switch (_state)
					{
					case StateSearchigSyncSources:
						{
							SetState(StateSynchronizing);
						}
						break;
					}
				}
				break;
			case FlagsStruct::SyncStateSync:
				{
					switch (_state)
					{
					case StateSearchigSyncSources:
					case StateSynchronizing:
						{
							SetState(StateSync);
						}
						break;
					}
				}
				break;
			case FlagsStruct::SyncStateError:
				{
					Lock(LockReasoneSyncError);
				}
				break;
			}
			
			if (Flags.GoToListeningMode && _state != StateInfiniteListening)
			{
				switch (Flags.SyncState)
				{
				case FlagsStruct::SyncStateSearchigSyncSources:
				case FlagsStruct::SyncStateStateSynchronizing:
				case FlagsStruct::SyncStateSync:
					{
						_stopTransportCallback.Call();
						_stopRoutingCallback.Call();
						_stopActivityProcessingCallback.Call();
						
						StartInfinitListeningCallback.Call();
						
						SetState(StateInfiniteListening);
					}
					break;
				}
			}
			
			if (Flags.GoToPowerTestMode && _state != StateCarrierTransmit)
			{
				switch (Flags.SyncState)
				{
				case FlagsStruct::SyncStateSearchigSyncSources:
				case FlagsStruct::SyncStateStateSynchronizing:
				case FlagsStruct::SyncStateSync:
					{
						Disable();
						
						StartCarrierTransmitCallback.Call();
						
						if (!Flags.ConfigApplied)
						{
							Lock(LockReasoneTransiverConfigError);
							break;
						}
						
						SetState(StateCarrierTransmit);
					}
					break;
				}
			}
			
			switch (_state)
			{
			case StateSync:
				{
					_startActivityProcessingCallback.Call();
					if (!Flags.ActivityProcessingRunning)
					{
						Lock(LockReasoneActivityTableError);
						break;
					}
					SetState(StatePrepareLinkTable);
				}
				break;
			case StatePrepareLinkTable:
				{
					if (Flags.ActivityTablesCollected)
					{
						_startRoutingCallback.Call();
						if (!Flags.RoutingRunning)
						{
							Lock(LockReasoneRouteTableError);
							break;
						}
						SetState(StatePrepareRouteTable);
					}
				}
				break;
			case StatePrepareRouteTable:
				{
					if (Flags.RoutesReady)
					{
						_startTransportCallback.Call();
						if (!Flags.TransportRunning)
						{
							Lock(LockReasoneTransportError);
							break;
						}
						SetState(StateReady);
					}
				}
				break;
			}
		}
		
		void SetPowerUpCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_powerUpCallback.Set(callback, callbackParam);
		}
		
		void SetPowerDownCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_powerDownCallback.Set(callback, callbackParam);
		}
		
		void SetNodeConfigLoadCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_nodeConfigLoadCallback.Set(callback, callbackParam);
		}
		
		void SetStartSyncCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_startSyncCallback.Set(callback, callbackParam);
		}
		
		void SetStopSyncCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_stopSyncCallback.Set(callback, callbackParam);
		}
		
		void SetStartActivityProcessingCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_startActivityProcessingCallback.Set(callback, callbackParam);
		}
		
		void SetStopActivityProcessingCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_stopActivityProcessingCallback.Set(callback, callbackParam);
		}
		
		void SetStartRoutingCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_startRoutingCallback.Set(callback, callbackParam);
		}
		
		void SetStopRoutingCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_stopRoutingCallback.Set(callback, callbackParam);
		}
		
		void SetStartTransportCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_startTransportCallback.Set(callback, callbackParam);
		}
		
		void SetStopTransportCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_stopTransportCallback.Set(callback, callbackParam);
		}
		
		void SetOnStateChangedCallback(StateChangedCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onStateChangedCallback.Set(callback, callbackParam);
		}
		
		void SetResetCallback(ResetCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_resetCallback.Set(callback, callbackParam);
		}
		
		bool IsSync()
		{
			return Flags.SyncState == FlagsStruct::SyncStateSync;
		}
		
		State GetState()
		{
			return _state;
		}
		
		LockReasoneCode GetLockReasone()
		{
			return _lockReasoneCode;
		}
		
		void ApplyConfigParams()
		{
			Restart();
		}
		
		void Restart()
		{
			Disable();
			_resetCallback.Call(ResetReasoneConfigChanged);
		}
		
		void ServiceTick(int tickFrequency)
		{
			if (_state == StateLocked)
			{
				if (_lockElapsedTimeCount >= RestartOnLockTimeout * tickFrequency)
				{
					Disable();
					Reset();
					Enable();
				}
				else
				{
					_lockElapsedTimeCount++;
				}
			}
			else
			{
				_lockElapsedTimeCount = 0;
			}
		}
	protected:
		void Reset()
		{
			_state = StateDisabled;
			_lockReasoneCode = LockReasoneUnknown;
			Flags.Reset();
			_lockElapsedTimeCount = 0;
		}
		
		void SetState(State _newState)
		{
			_state = _newState;
			
			_onStateChangedCallback.Call(_state);
		}
		
		void Lock(LockReasoneCode reasoneCode)
		{
			if (_state == StateLocked)
			{
				return;
			}
			
			_lockReasoneCode = reasoneCode;
			
			_stopTransportCallback.Call();
			_stopRoutingCallback.Call();
			_stopActivityProcessingCallback.Call();
			_stopSyncCallback.Call();
			_powerDownCallback.Call();
			
			_lockElapsedTimeCount = 0;
			SetState(StateLocked);
		}
	};
}
}
}

#endif