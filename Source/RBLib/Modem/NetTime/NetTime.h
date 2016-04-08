///////////////////////////////////////////////////////////////////////////////
//	 ласс обслуживани€ сетевого времени
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_NETTIME_H
#define MODEM_NETTIME_H

#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Modem
{
namespace NetTime
{
	class NetTime
	{
	public:
		typedef float NetTimeType;
		typedef unsigned char AddressType;
		typedef CallbackWrapper<int &, int &> GetCurrentTimeSlotAndCountCallbackType;
	protected:
		NetTimeType _currentTime;
		bool _timeValid;
		bool _setByReceivedTime;
		GetCurrentTimeSlotAndCountCallbackType _getCurrentTimeSlotAndCountCallback;
		int _receivedCount;
		NetTimeType _lastReceivedTime;
		AddressType _masterAddress;
	public:
		//  онструткор по умолчанию
		NetTime()
		{
			_currentTime = 0;
			_timeValid = false;
			Reset();
		}
		
		// —брос сетевого времени
		void Reset()
		{
			_currentTime = 0;
			SetTimeValid(false);
			_setByReceivedTime = false;
			_receivedCount = 0;
			_lastReceivedTime = 0.0;
		}
		
		// «адать текущее сетевое врем€ и начать отсчЄт
		void SetCurTime(NetTimeType curTime)
		{
			_currentTime = curTime;
			SetTimeValid(true);
		}
		
		// «адать текущее сетевое врем€ на основе пришедшего от других узлов и начать отсчЄт
		void SetCurTimeByReceivedTime()
		{
			// в текущем цикле сети что-то получали
			if (_receivedCount > 0)
			{
				_currentTime = _lastReceivedTime;
				SetTimeValid(true);
			}
			else
			{
				// подождем следующих циклов
				_setByReceivedTime = true;
				// а пока будем считать неизвестным
				SetTimeValid(false);
			}
		}
		
		// ќбработчик сетевого времени, полученного от другого узла
		void OnNetTimeReceived(AddressType source, NetTimeType netTime)
		{
			// синхронизации времени по узлу с наименьшим номером
			if (_receivedCount < 1)
			{
				_masterAddress = source;
				_lastReceivedTime = netTime;
			}
			else
			{
#ifdef RADIODOZOR
				if (source > _masterAddress)
#else
				if (source < _masterAddress)
#endif
				{
					_masterAddress = source;
					_lastReceivedTime = netTime;
				}
			}
		
			_receivedCount++;
		}
		
		// ќбработчик конца цикла сети
		void OnEndTimeChain()
		{
			// требуетс€ установить на основе пришедшего времени
			if (_setByReceivedTime)
			{
				// в текущем цикле сети что-то получали
				if (_receivedCount > 0)
				{
					_currentTime = _lastReceivedTime;
					SetTimeValid(true);
					// установли
					_setByReceivedTime = false;
				}
			}
			
			// в текущем цикле сети что-то получали, надо проверить совпадение времени
			if (_timeValid && _receivedCount > 0)
			{
				// если есть расхождени€
				if (_currentTime != _lastReceivedTime)
				{
					// врем€ нельз€ считать правильным
					SetTimeValid(false);
					// установим в следующий раз
					_setByReceivedTime = true;
				}
			}
			
			if (_timeValid)
			{
				_currentTime += 1.0;
			}
			
			// забудем что что-то получали
			_receivedCount = 0;
		}
		
		bool TimeValid()
		{
			return _timeValid;
		}
		
		// ѕолучить текущее сетевое врем€
		NetTimeType GetTime(bool * timeValid = 0)
		{
			NetTimeType res = _currentTime;
			int currnetTimeSlot = 0;
			int timeSlotCount = 0;
			_getCurrentTimeSlotAndCountCallback.Call(currnetTimeSlot, timeSlotCount);
			
			if (timeSlotCount > 0)
			{
				float fractionalPart = currnetTimeSlot;
				fractionalPart /= (float)timeSlotCount;
				
				res += fractionalPart;
			}
			
			if (timeValid)
			{
				*timeValid = _timeValid;
			}
			
			return res;
		}
		
		void SetGetCurrentTimeSlotAndCountCallback(GetCurrentTimeSlotAndCountCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_getCurrentTimeSlotAndCountCallback.Set(callback, callbackParam);
		}
	protected:
		void SetTimeValid(bool timeValid)
		{
			_timeValid = timeValid;
		}
	};
}
}
}

#endif