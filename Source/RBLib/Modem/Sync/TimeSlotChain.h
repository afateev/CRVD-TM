#ifndef MODEM_SYNC_TIMESLOTCHAIN_H
#define MODEM_SYNC_TIMESLOTCHAIN_H

#include "../../Utils/CallbackWrapper.h"
#include "../../Utils/vector_static.h"

namespace Rblib
{
namespace Modem
{
namespace Sync
{
	template
		<
			int AdditionalSlotMaxCount
		>
	class TimeSlotChain
	{
	public:
		typedef CallbackWrapper<int> CallbackType;
		typedef CallbackWrapper<int, bool, int> TxCallbackType;
		typedef vector_static<int, AdditionalSlotMaxCount> AdditionalSlotListType;
	public:
		CallbackType OnEndTimeChainSpecialSlotCallback;
	protected:
		TxCallbackType _onTxTimeSlotCallback;
		CallbackType _onRxTimeSlotCallback;
		CallbackType _onEndTimeChainCallback;
		int _txSlot;
		int _currentSlot;
		int _mainSlotCount;
		int _additionalSlotCount;
		int _currentDataSlot;
		int _dataSlotCount;
		int _dataSlotCounter;
		bool _dataPhase;
		int _decrement;
		AdditionalSlotListType _additionalSlotList;
		bool _specialSlot;
		static const unsigned int SpecialSlotCount = 8;
		static const int DataSlotScale = 4;
	public:
		TimeSlotChain()
		{
			Reset();
			ResetAdditionalTimeSlotList();
		}
		
		void Reset()
		{
			_txSlot = -1;
			_currentSlot = 0;
			_mainSlotCount = 0;
			_additionalSlotCount = 0;
			_currentDataSlot = 0;
			_dataSlotCount = 0;
			_dataSlotCounter = 0;
			_dataPhase = false;
			_decrement = 0;
		}
		
		void SetOnTxTimeSlotCallback(TxCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onTxTimeSlotCallback.Set(callback, callbackParam);
		}
		
		void SetOnRxTimeSlotCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onRxTimeSlotCallback.Set(callback, callbackParam);
		}
		
		void SetOnEndTimeChainCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onEndTimeChainCallback.Set(callback, callbackParam);
		}
		
		void OnTimeSlot()
		{
			_decrement = 0;
			
			if (_currentSlot < _mainSlotCount)
			{
				_specialSlot = false;
				if (_currentSlot == _txSlot)
				{
					_onTxTimeSlotCallback.Call(_currentSlot, true, 0);
				}
				else
				{
					_onRxTimeSlotCallback.Call(_currentSlot);
				}
				
				_currentSlot++;
				_decrement = 1;
				_dataSlotCount = _additionalSlotCount;
				_dataPhase = _dataSlotCount > 0;
				_currentDataSlot = 0;
				_dataSlotCounter = 0;
			}
			else
			{
				if (_dataPhase)
				{
					if (_dataSlotCounter == 0)
					{
						if (IsAdditionalTxSlot(_currentDataSlot))
						{
							_onTxTimeSlotCallback.Call(_currentDataSlot, false, _dataSlotCounter);
						}
					}
					
					if (_dataSlotCounter == 1)
					{
						if (IsAdditionalTxSlot(_currentDataSlot))
						{
							_onTxTimeSlotCallback.Call(_currentDataSlot, false, _dataSlotCounter);
						}
						else
						{
							_onRxTimeSlotCallback.Call(_mainSlotCount + _currentDataSlot);
						}
					}
					
					if (_dataSlotCounter >= DataSlotScale)
					{
						// слоты данных в которых надо включать прием / передачу
						if (_currentDataSlot < _dataSlotCount - 1)
						{
							_currentDataSlot++;
							_dataSlotCounter = 0;
						}
						else
						// завершаем фазу передачи данных
						{
							_dataPhase = false;
						}
					}
					else
					{
						_dataSlotCounter++;
					}
				}
				else
				{
					if (_currentSlot == _mainSlotCount)
					{
						_specialSlot = true;
						_onEndTimeChainCallback.Call(_currentSlot);
						
						_currentSlot++;
						_decrement = 1;
					}
					else
					{
						if (_currentSlot == _mainSlotCount + SpecialSlotCount)
						{
							OnEndTimeChainSpecialSlotCallback(_currentSlot);
							_currentSlot = 0;
							_decrement = -(_mainSlotCount + SpecialSlotCount);
						}
						else
						{
							_currentSlot++;
							_decrement = 1;
						}
						
						if (_currentSlot > _mainSlotCount + SpecialSlotCount)
						{
							_currentSlot = 0; // TODO теоретически такая ситуация не должна возникать, выставить диагностический флаг
						}
					}
				}
			}
		}
		
		void SetCount(int mainCount, int additionalCount)
		{
			_mainSlotCount = mainCount;
			_additionalSlotCount = additionalCount;
		}
		
		int GetTxSlot()
		{
			return _txSlot;
		}
		
		void SetTxSlot(int slot)
		{
			_txSlot = slot;
		}
		
		void SetCurSlot(int slot)
		{
			_currentSlot = slot;
		}
		
		int GetCurSlot()
		{
			return _currentSlot - _decrement;
		}
		
		int GetCurSlotAbsolute()
		{
			int res = _currentSlot;
			
			if (_currentSlot >= _mainSlotCount)
			{
				res = _mainSlotCount + _currentDataSlot * DataSlotScale + _dataSlotCounter;
			}

			return res - _decrement;
		}
		
		int GetMainSlotCount()
		{
			return _mainSlotCount;
		}
		
		int GetTotalSlotCount()
		{
			return 1 + _additionalSlotList.size();
		}
		
		void AddTxSlot(int slot)
		{
			_additionalSlotList.push_back(slot);
		}
		
		void ResetAdditionalTimeSlotList()
		{
			_additionalSlotList.clear();
		}
		
		bool IsAdditionalTxSlot(int slot)
		{
			typename AdditionalSlotListType::iterator i;
			for (i = _additionalSlotList.begin(); i != _additionalSlotList.end(); i++)
			{
				if (slot == *i)
				{
					return true;
				}
			}
			
			return false;
		}
		
		bool IsSpecialSlot()
		{
			return _specialSlot;
		}
		
		float GetCyclesCountPerSecond()
		{
			float cycleTime = 0.0025 * (float)_mainSlotCount;
			cycleTime += 0.0025 * (float)(DataSlotScale + 1) * _additionalSlotCount;
			cycleTime += 0.0025 * (float)(SpecialSlotCount + 2);
			
			float res = 1.0 / cycleTime;
			return res;
		}
	};
}
}
}

#endif