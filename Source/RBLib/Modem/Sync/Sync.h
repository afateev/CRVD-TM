///////////////////////////////////////////////////////////////////////////////
//	Класс синхронизации
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_SYNC_SYNC_H
#define MODEM_SYNC_SYNC_H

#include "../../Utils/CallbackWrapper.h"
#include "../Common/SyncInfo.h"
#include "../Common/TimeSlotChainInfo.h"
#include "TimeSlot.h"
#include "TimeSlotChain.h"
#include <math.h>
#include <stdio.h>

namespace Rblib
{
namespace Modem
{
namespace Sync
{
	template
		<
			class TickSource,
			int slotCountPerSecond,
			int syncMaxSlotCountPerCycle, 	// максимальное кол-во окон в цикле
			int maxAdditionalSlotCount,		// максимальное количество дополнительных окон
			int debugDataCodeStart,			// стартовый номер кодов отладочной информации
			bool snifferMode = false,		// режим, в котором дальше прослушивания эфира не уходим
			bool useExternalSync = false	// использовать внешнюю синхронизацию
		>
	class Sync
	{
	public:
		enum State
		{
			StateDisabled = 0,
			StateStarting,
			StateMeasureSyncTxTime,
			StateMeasureBiggestPacketTxTime,
			StateListening,
			StateSynchronizing,
			StateSync,
			StateError
		};
		
		enum DebugDataCode
		{
			DebugDataCodeSyncInfo	= debugDataCodeStart
		};
		
		enum ResetReasoneCode
		{
			ResetReasoneErrorWhileMeasureSyncTxTime = 0,
			ResetReasoneErrorWhileMeasureBiggestPacketTxTime = 1
		};

#pragma pack(push, 1)
		struct DebugDataStructSyncInfo
		{
			unsigned char SyncPacketReceivedCount;
			unsigned char SyncPacketUsedCountForTimeCorrection;
			float TimeCorrectionValue;
			int TimeSlotCorrectionValue;
			bool SyncFlag;
			
			DebugDataStructSyncInfo()
			{
				SyncPacketReceivedCount = 0;
				SyncPacketUsedCountForTimeCorrection = 0;
				TimeCorrectionValue = 0;
				TimeSlotCorrectionValue = 0;
				SyncFlag = false;
			}
		};
#pragma pack (pop)
		
		struct SyncCorrectionInfo
		{
			int Source;
			float TimeCorrection;
			bool TimeCorrectionValid;
			int SlotCorrection;
			
			SyncCorrectionInfo()
			{
				Source = -1;
				TimeCorrection = 0;
				TimeCorrectionValid = true;
				SlotCorrection = 0;
			}
		};
		
		typedef unsigned char AddressType;
		typedef TimeSlot<TickSource, slotCountPerSecond, useExternalSync> TimeSlotType;
		typedef TimeSlotChainInfo TimeSlotChainInfoType;
		typedef SyncInfo<AddressType> SyncInfoType;
		typedef CallbackWrapper<int, bool, int, bool &> TxTimeSlotChainCallbackType;
		typedef CallbackWrapper<int> TimeSlotChainCallbackType;
		typedef CallbackWrapper<bool, bool> RxOnOffCallbackType;
		typedef CallbackWrapper<> TxSyncEquivalentCallbackType;
		typedef CallbackWrapper<> TxBiggestPacketEquivalentCallbackType;
		typedef CallbackWrapper<> TxSyncRequestCallbackType;
		typedef CallbackWrapper<float, int> TxSyncCallbackType;
		typedef CallbackWrapper<State> StateChangedCallbackType;
		typedef CallbackWrapper<int, void *, int> DebugDataCallbackType;
		typedef Rblib::vector_static<AddressType, syncMaxSlotCountPerCycle> SyncRequestListType;
		typedef Rblib::vector_static<SyncCorrectionInfo, syncMaxSlotCountPerCycle> SyncCorrectionListType;
		typedef CallbackWrapper<int> ResetCallbackType;
		typedef CallbackWrapper<bool &> AllowEnergySavingListeningCallbackType;
		
		static const bool UseExternalSync = useExternalSync;
	public:
		AllowEnergySavingListeningCallbackType AllowEnergySavingListeningCallback;
	protected:
		AddressType _selfAddress;
		TimeSlotType _timeSlot;
		TimeSlotChain<maxAdditionalSlotCount> _timeSlotChain;
		TimeSlotChainInfoType _timeSlotChainParams;
		TxTimeSlotChainCallbackType _onTxTimeSlotCallback;
		TimeSlotChainCallbackType _onRxTimeSlotCallback;
		TimeSlotChainCallbackType _onEndTimeChainCallback;
		State _state;
		bool _useSlowDown;
		RxOnOffCallbackType _rxOnOffCallback;
		TxSyncEquivalentCallbackType _txSyncEquivalentCallback;
		TxBiggestPacketEquivalentCallbackType _txBiggestPacketEquivalentCallback;
		TxSyncRequestCallbackType _txSyncRequestCallback;
		TxSyncCallbackType _txSyncCallback;
		StateChangedCallbackType _onStateChangedCallback;
		DebugDataCallbackType _debugDataCallback;
		SyncRequestListType _syncRequestList;
		SyncCorrectionListType _syncCorrectionList;
		bool _infiniteRx;
		int _syncRequestCountDown;
		int _syncPacketCountDown;
		int _syncTimeoutCountDown;
		float _txSyncTime;
		float _txSyncTimeLastMeasured;
		int _measureRepeatCount;
		int _txSyncTimeMeasureCount;
		static const int TxSyncTimeMeasureMaxCount = 5;
		float _txBiggestPacketTime;
		float _txBiggestPacketTimeLastMeasured;
		bool _waitTxComplete;
		bool _timeSlotTxOutOfBounds;
		float _maxAllowableTxTimeDeviation;
		bool _imFirstMaster;
		volatile float _nextTimeCorrection; 
		volatile int _nextSlotCorrection;
		static const int SlotCorrectionHistoryMaxSize = 4;	// должно быть чётным
		int _slotCorrectionHistory[SlotCorrectionHistoryMaxSize];
		int _slotCorrectionHistorySize;
		float _lastFixTime;
		int _lastFixSlotNumber;
		bool _lastFixSlotIsSpecial;
		ResetCallbackType _resetCallback;
		bool _infiniteListening;
		int _syncLostCounter;
		int _rxSyncCounter[syncMaxSlotCountPerCycle];
		int _rxSyncErrorCounter[syncMaxSlotCountPerCycle];
		unsigned int _syncPeriodCounter;
	public:
		Sync()
		{
			Reset();
		}
		
		void Reset()
		{
			_selfAddress = 0;
			_state = StateDisabled;
			_useSlowDown = false;
			_infiniteRx = false;
			_syncRequestCountDown = 0;
			_syncPacketCountDown = 0;
			_syncTimeoutCountDown = 0;
			_txSyncTime = 0;
			_measureRepeatCount = 0;
			_txBiggestPacketTime = 0;
			_txSyncTimeMeasureCount = 0;
			_waitTxComplete = false;
			_timeSlotTxOutOfBounds = false;
			_imFirstMaster = false;
			_nextTimeCorrection = 0;
			_maxAllowableTxTimeDeviation = 0;
			_lastFixTime = 0;
			_lastFixSlotNumber = 0;
			_lastFixSlotIsSpecial = false;
			_infiniteListening = false;
			_syncLostCounter = 0;
			ResetRxSyncCounters();
			_syncPeriodCounter = 0;
		}
		
		// Инициализация
		// maxError - максимальная допустимая рассинхронизация, %
		// maxAllowableTxTimeDeviation - максимальная допустимая разница во времени передачи синхропакета, %
		// selfAddress - наш адрес
		bool Init(float maxError, float maxAllowableTxTimeDeviation, AddressType selfAddress)
		{
			Reset();
			
			bool res = true;
			
			res &= _timeSlot.Init(maxError);
			if (res)
			{
				_timeSlot.SetOnTimeSlotCallback(OnTimeSlotStatic, this);
			}
			
			if (res)
			{
				_timeSlotChain.Reset();
				_timeSlotChain.ResetAdditionalTimeSlotList();
				_timeSlotChain.SetOnTxTimeSlotCallback(OnTxTimeSlotStatic, this);
				_timeSlotChain.SetOnRxTimeSlotCallback(OnRxTimeSlotStatic, this);
				_timeSlotChain.SetOnEndTimeChainCallback(OnEndTimeChainStatic, this);
				_timeSlotChain.OnEndTimeChainSpecialSlotCallback.Set(OnEndTimeChainSpecialSlotStatic, this);
				
				_maxAllowableTxTimeDeviation = maxAllowableTxTimeDeviation;
			}
			
			return res;
		}
		
		void SetOnTxTimeSlotCallback(TxTimeSlotChainCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onTxTimeSlotCallback.Set(callback, callbackParam);
		}
		
		void SetOnRxTimeSlotCallback(TimeSlotChainCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onRxTimeSlotCallback.Set(callback, callbackParam);
		}
		
		void SetOnEndTimeChainCallback(TimeSlotChainCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onEndTimeChainCallback.Set(callback, callbackParam);
		}
		
		void SetRxOnOffCallback(RxOnOffCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_rxOnOffCallback.Set(callback, callbackParam);
		}
		
		void SetTxSyncEquivalentCallback(TxSyncEquivalentCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_txSyncEquivalentCallback.Set(callback, callbackParam);
		}
		
		void SetTxBiggestPacketEquivalentCallback(TxBiggestPacketEquivalentCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_txBiggestPacketEquivalentCallback.Set(callback, callbackParam);
		}
		
		void SetTxSyncRequestCallback(TxSyncRequestCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_txSyncRequestCallback.Set(callback, callbackParam);
		}
		
		void SetTxSyncCallback(TxSyncCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_txSyncCallback.Set(callback, callbackParam);
		}
		
		void SetOnStateChangedCallback(typename StateChangedCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onStateChangedCallback.Set(callback, callbackParam);
		}
		
		void SetDebugDataCallback(typename DebugDataCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_debugDataCallback.Set(callback, callbackParam);
		}
		void SetResetCallback(typename ResetCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_resetCallback.Set(callback, callbackParam);
		}
		
		void SetInfinitListening(bool value)
		{
			_infiniteListening = value;
		}
		
		void Start(AddressType selfAddress, TimeSlotChainInfoType timeSlotChainParams)
		{
			_selfAddress = selfAddress;
			_timeSlotChainParams = timeSlotChainParams;
			SetState(StateStarting);
			
			ResetSyncPacketCountDown();
			ResetSyncTimeoutCountDown();
			ResetRxSyncCounters();
			_txSyncTime = 0;
			_measureRepeatCount = 0;
			_txBiggestPacketTime = 0;
			_txSyncTimeMeasureCount = 0;
			_timeSlotChain.Reset();
			_timeSlotChain.ResetAdditionalTimeSlotList();
			
			for (int i = 0; i < _timeSlotChainParams.AdditionalSlotList.size(); i++)
			{
				_timeSlotChain.AddTxSlot(_timeSlotChainParams.AdditionalSlotList[i]);
			}
			
			_timeSlot.Enable();
		}
		
		void Stop()
		{
			SetState(StateDisabled);
			_timeSlot.Disable();
			_timeSlotChainParams.Reset();
		}
		
		// Обработчик тактовых сигналов
		void OnTick()
		{
			_timeSlot.OnTick();
		}
		
		// Зафиксировать момент времени, по которому идёт синхронизация
		void FixTime()
		{
			_lastFixTime = _timeSlot.GetCurTime();
			_lastFixSlotNumber = _timeSlotChain.GetCurSlotAbsolute();
			_lastFixSlotIsSpecial = _timeSlotChain.IsSpecialSlot();
		}
		
		// Обработка окончания приема
		void OnRxComplete()
		{
			if (_infiniteRx)
			{
				_rxOnOffCallback.Call(true, false);
			}
		}
		
		// Обработка окончания передачи
		void OnTxComplete()
		{
			TickSource::SyncTxClear();
			_txSyncTimeLastMeasured = _lastFixTime;
			_txBiggestPacketTimeLastMeasured = _lastFixTime;
			_waitTxComplete = false;
			
			if (_infiniteRx)
			{
				_rxOnOffCallback.Call(true, false);
			}
		}
		
		// Обработчик входящих запросов синхронизации
		void OnSyncRequestReceived(AddressType sourceAddress)
		{
			_syncRequestList.push_back(sourceAddress);
		}
		
		// Обработчик входящей информации о синхронизации
		void OnSyncInfoReceived(SyncInfoType &syncInfo)
		{
			float rxTime = _lastFixTime;
			
			SyncCorrectionInfo syncCorrectionInfo;
			syncCorrectionInfo.Source = syncInfo.SourceAddress;
			
			// вычисляем разницу времени передачи синхропакета между своим временем и временем того узла чей синхропакет поймали
			float txTimeDelta = syncInfo.TxTime - _txSyncTime;
			
			float timeDelta = rxTime + txTimeDelta;
			timeDelta -= syncInfo.TxTime;
			timeDelta -= 0.010; // поправка на разность определения времени окончания передачи и окончания приема
			syncCorrectionInfo.TimeCorrection =  timeDelta;
			syncCorrectionInfo.TimeCorrectionValid = !_lastFixSlotIsSpecial;
			// в случае использования внешней синхронизации на отправителе синхропакета, время будет нулевым
			syncCorrectionInfo.TimeCorrectionValid &= UseExternalSync || syncInfo.TxTime > 0.0;
			
			if (syncCorrectionInfo.Source < syncMaxSlotCountPerCycle)
			{
				_rxSyncCounter[syncCorrectionInfo.Source] = 0;
			}
			
			if (_lastFixSlotIsSpecial)
			{
				if (_rxSyncErrorCounter[syncCorrectionInfo.Source] < 0xFFFF)
				{
					_rxSyncErrorCounter[syncCorrectionInfo.Source]++;
				}
			}	
			
			// синхропакеты должны идти одновременно от всех узлов в одном цикле
			if (_state == StateSync && _syncPacketCountDown > 1)
			{
				syncCorrectionInfo.TimeCorrectionValid = false;
			}
			
			// в случае использования внешней синхронизации на приемнике
			if (UseExternalSync)
			{
				// мы не должны выполнять коррекцию времени, поэтому коррекцию считаем нулевой
				syncCorrectionInfo.TimeCorrection = 0;
				
				if (syncCorrectionInfo.TimeCorrectionValid)
				{
					// сообщим модулю внешней синхронизации о том, что в текущем окне был принят синхропакет
					if (UseSyncInfoFromSource(_state == StateSync, syncCorrectionInfo.Source))
					{
						bool use = true;
						typename SyncCorrectionListType::iterator i;
						for (i = _syncCorrectionList.begin(); i != _syncCorrectionList.end(); i++)
						{
#ifdef RADIODOZOR
							//уже был больший номер
							if (i->Source > syncCorrectionInfo.Source)
#else
							//уже был меньший номер
							if (i->Source < syncCorrectionInfo.Source)
#endif
							{
								// не надо выдавать сигнал коррекции, т.к. синхронизируемя по одному ведущему узло и он уже был
								use = false;
								break;
							}
						}

						if (use)
						{
							TickSource::PulseSyncRx();
						}
					}
				}
			}
			
			// допустимая разница это % от нашего времени передачи
			float maxAllowableTxTimeDeviation = _maxAllowableTxTimeDeviation / 100.0 * _txSyncTime;
			// если разница между нами и другим узлом большая,
			// то либо косячное время у нас, либо у него
			// но синхронизироваться в любом случае нельзя, т.к. ничего хорошего не выйдет: слишком большая разница времени передачи одинаковых пакетов
			if (Abs(txTimeDelta) > maxAllowableTxTimeDeviation)
			{
				// разница во времени учитывается при вычислении timeDelta,
				// разница возможно из-за разной частоты ядра,
				// пока разрешим синхронизироваться по таким "разным" источникам
				//syncCorrectionInfo.TimeCorrectionValid = false;
			}
			
			syncCorrectionInfo.SlotCorrection = syncInfo.TimeSlot - _lastFixSlotNumber;
			// Костыль: при этих условиях возникало зависание коррекции окна на -1 (пока не понятно почему), это чтобы вывести из этого состояния, т.е. куда нибудь скорректироваться
			
			if (syncCorrectionInfo.TimeCorrection >= 0.05 && syncCorrectionInfo.TimeCorrection < 0.15 && _lastFixSlotNumber == 1)
			{
				syncCorrectionInfo.TimeCorrection -= 0.1;
			}
				
			_syncCorrectionList.push_back(syncCorrectionInfo);
		}
		
		bool IsFirstMaster()
		{
			return _imFirstMaster;
		}
		
		int GetCurrentTimeSlot()
		{
			return _timeSlotChain.GetCurSlot();
		}
		
		int GetTimeSlotCount()
		{
			return _timeSlotChain.GetMainSlotCount();
		}
		
		int GetTotalSlotCount()
		{
			return _timeSlotChain.GetTotalSlotCount();
		}
		
		float GetCyclesCountPerSecond()
		{
			return _timeSlotChain.GetCyclesCountPerSecond();
		}
	protected:
		static float Abs(float val)
		{
			if (val < 0)
			{
				return -val;
			}
			
			return val;
		}
		
		static void OnTimeSlotStatic(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Sync *im = (Sync *)callbackParam;
			
			// если ждём завершения передачи, а уже настало другое окно
			if (im->_waitTxComplete)
			{
				// выставлем флаг ошибки
				im->_timeSlotTxOutOfBounds = true;
			}
			
			im->_timeSlotChain.OnTimeSlot();
			
			if (im->_timeSlotChain.IsSpecialSlot())
			{
				if (im->_timeSlotChain.GetCurSlot() == im->_timeSlotChain.GetMainSlotCount() + 1)
				{
					if (im->_nextTimeCorrection != 0)
					{
						im->_timeSlot.SetCorrection(im->_nextTimeCorrection);
						im->_nextTimeCorrection = 0;
					}
				}
			}
			else
			{
				if (im->_nextSlotCorrection != 0)
				{
					im->_timeSlotChain.SetCurSlot(im->_nextSlotCorrection + 1);
					im->_nextSlotCorrection = 0;
				}
			}
		}
		
		static void OnTxTimeSlotStatic(void *callbackParam, int slot, bool mainSlot, int dataSlotCounter)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Sync *im = (Sync *)callbackParam;
			im->OnTxTimeSlot(slot, mainSlot, dataSlotCounter);
		}
		
		static void OnRxTimeSlotStatic(void *callbackParam, int slot)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Sync *im = (Sync *)callbackParam;
			im->OnRxTimeSlot(slot);
		}
		
		static void OnEndTimeChainStatic(void *callbackParam, int count)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Sync *im = (Sync *)callbackParam;
			im->OnEndTimeChain(count);
		}
		
		static void OnEndTimeChainSpecialSlotStatic(void *callbackParam, int count)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Sync *im = (Sync *)callbackParam;
			im->OnEndTimeChainSpecialSlot(count);
		}
		
		void OnTxTimeSlot(int slot, bool mainSlot, int dataSlotCounter)
		{
			if (mainSlot)
			{
				if (_infiniteRx)
				{
					_rxOnOffCallback.Call(false, false);
				}
				
				if (_syncPacketCountDown <= 1)
				{
					_txSyncCallback.Call(_txSyncTime, slot);
					return;
				}
			}

			if (_state == StateSync)
			{
				if (_infiniteRx && dataSlotCounter < 1)
				{
					_rxOnOffCallback.Call(false, false);
				}
				
				bool txOccured = false;
				_onTxTimeSlotCallback.Call(slot, mainSlot, dataSlotCounter, txOccured);
				
				if (_infiniteRx && !mainSlot && dataSlotCounter < 1 && !txOccured)
				{
					_rxOnOffCallback.Call(true, false);
				}
			}
		}
		
		void OnRxTimeSlot(int slot)
		{
			switch(_state)
			{
			case StateSync:
				{
					_rxOnOffCallback.Call(true, !_infiniteRx);
				}
				break;
			}
			
			_onRxTimeSlotCallback.Call(slot);
		}
		
		void OnEndTimeChain(int count)
		{
			switch(_state)
			{
			case StateStarting:
				{
					_timeSlotChain.Reset();
					_syncRequestList.clear();
					_syncCorrectionList.clear();
					_slotCorrectionHistorySize = 0;
					_rxOnOffCallback.Call(true, false);
					// повторно можно не измерять, только после старта измеряем
					if ((_txSyncTime > 0 && _txBiggestPacketTime > 0) || _txSyncTimeMeasureCount > 0)
					{
						if (UseExternalSync)
						{
							TickSource::SoftReset();
						}
						
						SetState(StateListening);
					}
					else
					{
						_measureRepeatCount = 0;
						SetState(StateMeasureSyncTxTime);
					}
				}
				break;
			case StateMeasureSyncTxTime:
				{
					bool complete = false;
					
					if (UseExternalSync)
					{
						
						if (_timeSlotTxOutOfBounds)
						{
							_txSyncTimeMeasureCount=0;
							SetState(StateError);
							break;
						}
						
						complete = _txSyncTimeMeasureCount >= TxSyncTimeMeasureMaxCount;
						
						if (complete)
						{
							SetState(StateListening);
						}
						else
						{
							_txSyncTimeMeasureCount++;
							_waitTxComplete = true;
							_timeSlotTxOutOfBounds = false;
							TickSource::SyncTxSet();
							_txSyncEquivalentCallback.Call();
						}
					}
					else
					{
						if (_timeSlotTxOutOfBounds)
						{
							_txSyncTimeLastMeasured = 0;
							SetState(StateError);
							break;
						}
						
						if (_txSyncTimeLastMeasured > 0)
						{
							if (_txSyncTime > 0 )
							{
								float delta = fabs(_txSyncTime - _txSyncTimeLastMeasured);
								
								_txSyncTime += _txSyncTimeLastMeasured;
								_txSyncTime /= 2;
								
								complete = delta / _txSyncTime * 100.0 < _timeSlot.MaxErrorPercent();
							}
							else
							{
								_txSyncTime = _txSyncTimeLastMeasured;
							}
						}
						
						if (complete)
						{
							_measureRepeatCount = 0;
							SetState(StateMeasureBiggestPacketTxTime);
						}
						else
						{
							// делаем не более 16 попыток, потом перезагружаемся
							if (_measureRepeatCount > 16)
							{
								_resetCallback.Call(ResetReasoneErrorWhileMeasureSyncTxTime);
								// если не сработал сброс выставится хотя бы ошибка
								SetState(StateError);
								break;
							}
							_measureRepeatCount++;
							
							_txSyncTimeLastMeasured = 0;
							_waitTxComplete = true;
							_timeSlotTxOutOfBounds = false;
							_txSyncEquivalentCallback.Call();
						}
					}
				}
				break;
			case StateMeasureBiggestPacketTxTime:
				{
					bool complete = false;
					
					if (_timeSlotTxOutOfBounds)
					{
						_txBiggestPacketTimeLastMeasured = 0;
						SetState(StateError);
						break;
					}
					
					if (_txBiggestPacketTimeLastMeasured > 0)
					{
						if (_txBiggestPacketTime > 0 )
						{
							float delta = fabs(_txBiggestPacketTime - _txBiggestPacketTimeLastMeasured);
							
							_txBiggestPacketTime += _txBiggestPacketTimeLastMeasured;
							_txBiggestPacketTime /= 2;
							
							complete = delta / _txBiggestPacketTime * 100.0 < _timeSlot.MaxErrorPercent();
						}
						else
						{
							_txBiggestPacketTime = _txBiggestPacketTimeLastMeasured;
						}
					}
					
					if (complete)
					{
						SetState(StateListening);
					}
					else
					{
						// делаем не более 16 попыток, потом перезагружаемся
						if (_measureRepeatCount > 16)
						{
							_resetCallback.Call(ResetReasoneErrorWhileMeasureBiggestPacketTxTime);
							// если не сработал сброс выставится хотя бы ошибка
							SetState(StateError);
							break;
						}
						_measureRepeatCount++;
						
						_txBiggestPacketTimeLastMeasured = 0;
						_waitTxComplete = true;
						_timeSlotTxOutOfBounds = false;
						_txBiggestPacketEquivalentCallback.Call();
					}
				}
				break;
			case StateListening:
				{
					_infiniteRx = true;
					_rxOnOffCallback.Call(true, false);
					
					if (snifferMode)
					{
						break;
					}
					
					if (_infiniteListening)
					{
						break;
					}
					
					// были синхропакеты
					if (!_syncCorrectionList.empty())
					{
						// значит в сети есть синхроизированные узлы
						// нам ничего другого не остается, кроме как стать ведомым
						SetState(StateSynchronizing);
					}
					else
					{
						// были запросы синхронизации
						if (!_syncRequestList.empty())
						{
#ifdef RADIODOZOR
							// ищем максимальные адрес
							AddressType max = _syncRequestList.front();
							typename SyncRequestListType::iterator i;
							for (i = _syncRequestList.begin(); i != _syncRequestList.end(); i++)
							{
								if (*i > max)
								{
									max = *i;
								}
							}

							// если у нас больше, значит у нас выше приоритет
							if (_selfAddress > max)
#else
							// ищем минимальный адрес
							AddressType min = 0xFF;
							typename SyncRequestListType::iterator i;
							for (i = _syncRequestList.begin(); i != _syncRequestList.end(); i++)
							{
								if (*i < min)
								{
									min = *i;
								}
							}
							
							// если у нас меньше, значит у нас выше приоритет
							if (_selfAddress < min)
#endif
							{
								// становимся ведущим
								_imFirstMaster = true;
								SetState(StateSync);
							}
							else
							{
								// иначе ведомым
								SetState(StateSynchronizing);
							}
						}
						// ничего не было
						else
						{
							// счётчик отправки запросов ещё не кончился
							if (_syncRequestCountDown > 0)
							{
								_syncRequestCountDown--;
							}
							else
							{
								_syncRequestCountDown = _timeSlotChainParams.TotalSlotCount() * _timeSlotChainParams.SyncPacketPeriod / (2 + _timeSlotChainParams.MainTxSlot);
								// пока выключим прием
								_rxOnOffCallback.Call(false, false);
								// после запроса будем ждать пакеты до конца цикла
								_infiniteRx = true;
								// отправим запрос
								_txSyncRequestCallback.Call();
								
								if (UseExternalSync)
								{
									// подвиним начало окна на случайный интервал, вдруг попадаем в тот момент, когда в эфире единственный передатчик, а мы его как раз не слушаем
									_timeSlot.Disable();
									_timeSlot.Enable();
								}
								else
								{
									// подвиним начало окна на интервал зависящий от номера слота, вдруг попадаем в тот момент, когда в эфире единственный передатчик, а мы его как раз не слушаем
									_timeSlot.SetCorrection(-0.1 * (float)_timeSlotChainParams.MainTxSlot);
								}
							}
						}
					}
				}
				break;
			case StateSynchronizing:
				{
					_infiniteRx = false;
					
					// была информация о синхронизации
					if (IsSyncInfoReceived())
					{
						float timeCorrection = 0.0;
						int slotCorrection = 0;
						
						// если не синхронно
						if (!IsSync(&timeCorrection, &slotCorrection, false))
						{
							// слушаем ещё
							_infiniteRx = true;
							_rxOnOffCallback.Call(true, false);
						}
						else
						{
							// синхронизировались
							// включать прием будем только в нужных окнах
							_rxOnOffCallback.Call(false, false);
							SetState(StateSync);
						}
						
						// Выявление ситуации когда скачем туда-суда между соседними окнами с коррекцией времени на одно и то же число
						if (_slotCorrectionHistorySize < SlotCorrectionHistoryMaxSize)
						{
							_slotCorrectionHistory[_slotCorrectionHistorySize] = slotCorrection;
							_slotCorrectionHistorySize++;
						}
						else
						{
							for (int i = 1; i < SlotCorrectionHistoryMaxSize; i++)
							{
								_slotCorrectionHistory[i - 1] = _slotCorrectionHistory[i];
							}
							_slotCorrectionHistory[_slotCorrectionHistorySize - 1] = slotCorrection;
							
							int stepLeftCount = 0;
							int stepZeroCount = 0;
							int summ = 0;
							for (int i = 0; i < SlotCorrectionHistoryMaxSize; i++)
							{
								stepLeftCount += _slotCorrectionHistory[i] < 0 ? 1 : 0;
								stepZeroCount += _slotCorrectionHistory[i] == 0 ? 1 : 0;
								
								summ += _slotCorrectionHistory[i];
							}
							
							// ситуация возникла
							if (stepLeftCount == SlotCorrectionHistoryMaxSize / 2 && stepLeftCount == stepZeroCount && summ < 0)
							{
								// пытаемся выйти из ситуации
								timeCorrection += 0.1;
							}
						}
						
						// корректируем
						_nextTimeCorrection = timeCorrection;
						_nextSlotCorrection = slotCorrection;
						
						ResetSyncTimeoutCountDown();
					}
					// ничего не было
					else
					{
						if (_syncTimeoutCountDown > 0)
						{
							_syncTimeoutCountDown--;
						}
						
						if (_syncTimeoutCountDown % _timeSlotChainParams.SyncPacketPeriod == 0)
						{
							DebugDataStructSyncInfo debugData;
							debugData.SyncPacketReceivedCount = _syncCorrectionList.size();
							debugData.SyncFlag = false;
							_debugDataCallback.Call(DebugDataCodeSyncInfo, &debugData, sizeof(debugData));
						}
						
						if (_syncTimeoutCountDown < 1)
						{
							// не услышали источников синхронизации
							// отправим запрос
							_txSyncRequestCallback.Call();
							// начинаем всё сначала
							SetState(StateStarting);
						}
						else
						{
							// слушаем ещё
							_infiniteRx = true;
							_rxOnOffCallback.Call(true, false);
						}
					}
				}
				break;
			case StateSync:
				{
					_infiniteRx = false;
					_rxOnOffCallback.Call(false, false);
					
					if (_syncPacketCountDown > 1)
					{
						_syncPacketCountDown--;
					}
					else
					{
						ResetSyncPacketCountDown();
						bool syncError = false;
						_syncPacketCountDown +=	GetSyncPeriodCorrection(syncError);
						_syncPeriodCounter++;
						/* TODO
						if (syncError)
						{
							// есть старший источник синхрониззации не совпадающий с текущим старшим, начинаем всё сначала
							SetState(StateStarting);
						}
						*/
					}
					
					IncrementRxSyncCounters();
					
					// была информация о синхронизации
					if (IsSyncInfoReceived())
					{
						_imFirstMaster = false;
						
						float timeCorrection = 0.0;
						int slotCorrection = 0;
						
						// если не синхронно
						if (!IsSync(&timeCorrection, &slotCorrection, true))
						{
							// потеря синхронизации, начинаем всё сначала
							//SetState(StateStarting);
						}
						else
						{
							ResetSyncTimeoutCountDown();
						}
						
						// корректируем время чуть-чуть в пределах небольшой рассинхронизации
						_nextTimeCorrection = timeCorrection;
						_nextSlotCorrection = slotCorrection;
						//_timeSlotChain.SetCurSlot(slotCorrection);
					}
					else
					{
						if (_syncTimeoutCountDown > 0)
						{
							_syncTimeoutCountDown--;
						}
						
						if (_syncPacketCountDown % _timeSlotChainParams.SyncPacketPeriod == 0)
						{
							DebugDataStructSyncInfo debugData;
							debugData.SyncPacketReceivedCount = _syncCorrectionList.size();
							debugData.SyncFlag = _syncTimeoutCountDown > 0;
							_debugDataCallback.Call(DebugDataCodeSyncInfo, &debugData, sizeof(debugData));
						}
						
						if (_syncTimeoutCountDown < 1)
						{
							// потеря источников синхронизации, начинаем всё сначала
							SetState(StateStarting);
						}
					}
				}
				break;
			}
			
			_syncRequestList.clear();
			_syncCorrectionList.clear();
			
			_onEndTimeChainCallback.Call(count);
			
			switch(_state)
			{
			case StateSync:
				{
					bool allowEnergySavingListening = false;
					AllowEnergySavingListeningCallback(allowEnergySavingListening);
					
					_infiniteRx = !allowEnergySavingListening;
				
					if (_infiniteRx)
					{
						_rxOnOffCallback.Call(true, false);
					}
				}
				break;
			}
		}
		
		void OnEndTimeChainSpecialSlot(int count)
		{
			/*
			switch(_state)
			{
			case StateSync:
				{
					_infiniteRx = true; //TODO _syncPeriodCounter % (_timeSlotChainParams.SyncPacketPeriod - 1) == 0;
				
					if (_infiniteRx)
					{
						_rxOnOffCallback.Call(true, false);
					}
				}
				break;
			}*/
		}
		
		void SetState(State newState)
		{
			switch (newState)
			{
			case StateStarting:
				{
					_timeSlotTxOutOfBounds = false;
				}
				break;
			case StateMeasureSyncTxTime:
				{
					_timeSlotChain.SetCount(2, 0);
					_txSyncTimeLastMeasured = 0;
					_txSyncTimeMeasureCount = 0;
				}
				break;
			case StateMeasureBiggestPacketTxTime:
				{
					_timeSlotChain.SetCount(2, 0);
					_txBiggestPacketTimeLastMeasured = 0;
				}
				break;
			case StateListening:
				{
					_useSlowDown = true;
					_timeSlotChain.SetCount(2 + _timeSlotChainParams.MainTxSlot, 0);
				}
				break;
			case StateSynchronizing:
				{
					_timeSlotChain.SetCount(_timeSlotChainParams.MainSlotCount, _timeSlotChainParams.AddtitionalSlotCount);
					// нормальный таймаут
					ResetSyncTimeoutCountDown();
				}
				break;
			case StateSync:
				{
					_timeSlotChain.SetCount(_timeSlotChainParams.MainSlotCount, _timeSlotChainParams.AddtitionalSlotCount);
					_timeSlotChain.SetTxSlot(_timeSlotChainParams.MainTxSlot);
					ResetSyncPacketCountDown();
					ResetSyncTimeoutCountDown(_imFirstMaster ? 3 : 1);
					ResetRxSyncCounters();
					_syncPeriodCounter = 0;
				}
				break;
			}
			
			if (_state != newState)
			{
				_state = newState;
				_onStateChangedCallback.Call(_state);
			}
		}
		
		void ResetSyncPacketCountDown()
		{
			_syncPacketCountDown = _timeSlotChainParams.SyncPacketPeriod;
		}
		
		void ResetSyncTimeoutCountDown(unsigned int mul = 1)
		{
			_syncTimeoutCountDown = _timeSlotChainParams.SyncPacketPeriod * _timeSlotChainParams.SyncTimeout * mul;
		}
		
		bool IsSyncInfoReceived()
		{
			return !_syncCorrectionList.empty();
		}
		
		bool UseSyncInfoFromSource(bool considerPriority, int sourceAddress)
		{
			bool use = true;
			
			if (considerPriority)
			{
#ifdef RADIODOZOR
				use = sourceAddress > _selfAddress;
#else
				use = sourceAddress < _selfAddress;
#endif
			}
			
			return use;
		}
		
		bool IsSync(float *timeCorrection, int *slotCorrection, bool considerPriority)
		{
			bool res = _state == StateSync;
			
			DebugDataStructSyncInfo debugData;
			debugData.SyncPacketReceivedCount = _syncCorrectionList.size();
			
			if (UseExternalSync)
			{
				if (!TickSource::IsSyncOk())
				{
					_syncLostCounter++;
				}
				else
				{
					_syncLostCounter = 0;
				}
			}
			
			if (IsSyncInfoReceived())
			{
				bool enyOneTimeUsed = false;
				bool enyOneSlotUsed = false;
				float timeCorrectionAvg = 0;
				float timeCorrectionThreshold = _timeSlot.MaxErrorPercent() / 100.0;
				bool slotIncorrect = false;
				int slotCorrectionMaster = 0;
				int slotCorrectionMasterValue = 0;
				
				typename SyncCorrectionListType::iterator i;
				for (i = _syncCorrectionList.begin(); i != _syncCorrectionList.end(); i++)
				{
					bool use = UseSyncInfoFromSource(considerPriority, i->Source);
					
					if (use)
					{
						if (i->TimeCorrectionValid)
						{
							if (!enyOneTimeUsed)
							{
								timeCorrectionAvg = i->TimeCorrection;
								enyOneTimeUsed = true;
							}
							else
							{
								timeCorrectionAvg += i->TimeCorrection;
								timeCorrectionAvg /= 2.0;
							}
							
							if (UseExternalSync)
							{
								timeCorrectionAvg = 1.0;
							}
							
							debugData.SyncPacketUsedCountForTimeCorrection++;
						}
						
						if (!enyOneSlotUsed)
						{
							slotCorrectionMaster = i->Source;
							slotCorrectionMasterValue = i->SlotCorrection;
							enyOneSlotUsed = true;
						}
						else
						{
#ifdef RADIODOZOR
							if (i->Source > slotCorrectionMaster)
#else
							if (i->Source < slotCorrectionMaster)
#endif
							{
								slotCorrectionMaster = i->Source;
								slotCorrectionMasterValue = i->SlotCorrection;
							}
						}
						
						slotIncorrect |= slotCorrectionMasterValue != 0;
					}
				}
				
				if (enyOneTimeUsed || enyOneSlotUsed)
				{
					if (enyOneTimeUsed)
					{
						if (_state == StateSync)
						{
							if (!UseExternalSync)
							{
								res &= timeCorrectionAvg < 2.0 * timeCorrectionThreshold;
							}
							else
							{
								res &= _syncLostCounter < _timeSlotChainParams.SyncPacketPeriod * _timeSlotChainParams.SyncTimeout;
							}
								
							res &= !slotIncorrect;
						}
						else
						{
							if (!UseExternalSync)
							{
								res |= timeCorrectionAvg < timeCorrectionThreshold;
							}
							else
							{
								res |= TickSource::IsSyncOk();
							}
							
							// окно корректируем, только когда синхронность в норме, иначе можем попасть на мёртвую зону при скачкообразной коррекции окна
							if (!res)
							{
								//slotCorrection = 0;
							}
							res &= !slotIncorrect; 
						}
						
						if (timeCorrection)
						{
							if (timeCorrectionAvg > 0)
							{
								*timeCorrection = timeCorrectionAvg;
							}
							else
							{
								*timeCorrection = timeCorrectionAvg;
							}
							
							debugData.TimeCorrectionValue = *timeCorrection;
						}
					}
					
					if (enyOneSlotUsed)
					{
						if (slotCorrection)
						{
							*slotCorrection = slotCorrectionMasterValue;
							debugData.TimeSlotCorrectionValue = *slotCorrection;
						}
					}
				}
			}
			
			debugData.SyncFlag = res;
			_debugDataCallback.Call(DebugDataCodeSyncInfo, &debugData, sizeof(debugData));
			
			return res;
		}
		
		void ResetRxSyncCounters()
		{
			for (int i = 0; i < syncMaxSlotCountPerCycle; i++)
			{
				_rxSyncCounter[i] = 0xFFFF;
				_rxSyncErrorCounter[i] = 0;
			}
		}
		
		void IncrementRxSyncCounters()
		{
			for (int i = 0; i < syncMaxSlotCountPerCycle; i++)
			{
				if (_rxSyncCounter[i] < _timeSlotChainParams.SyncPacketPeriod * _timeSlotChainParams.SyncTimeout)
				{
					_rxSyncCounter[i]++;
				}
			}
		}
		
		int GetCurrentSyncMaster()
		{
			int master = -1;

			for (int i = 0; i < syncMaxSlotCountPerCycle; i++)
			{
				if (_rxSyncCounter[i] >= 0 && _rxSyncCounter[i] < _timeSlotChainParams.SyncPacketPeriod * _timeSlotChainParams.SyncTimeout)
				{
					if (master < 0)
					{
						master = i;
					}
					else
					{
#ifdef RADIODOZOR
						if (i > master)
						{
							master = i;
						}
#else
						if (i < master)
						{
							master = i;
						}
#endif
					}
				}
			}
			
			return master;
		}
		
		int GetSyncPeriodCorrection(bool &syncError)
		{
			int master = GetCurrentSyncMaster();
			int correction = 0;
			
			if (master >= 0)
			{
				int delta = _rxSyncCounter[master] % _timeSlotChainParams.SyncPacketPeriod;
				if (delta > 0)
				{
					delta -= _timeSlotChainParams.SyncPacketPeriod;
					correction = -delta;
				}
				else
				{
					if (_rxSyncErrorCounter[master] > 0)
					{
						syncError = true;
					}
				}
			}
			
			for (int i = 0; i < syncMaxSlotCountPerCycle; i++)
			{
				_rxSyncErrorCounter[i] = 0;
			}
			
			return correction;
		}
	};
}
}
}

#endif