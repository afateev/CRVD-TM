///////////////////////////////////////////////////////////////////////////////
//	����� ������������ �������� �������
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
		// ����������� �� ���������
		NetTime()
		{
			_currentTime = 0;
			_timeValid = false;
			Reset();
		}
		
		// ����� �������� �������
		void Reset()
		{
			_currentTime = 0;
			SetTimeValid(false);
			_setByReceivedTime = false;
			_receivedCount = 0;
			_lastReceivedTime = 0.0;
		}
		
		// ������ ������� ������� ����� � ������ ������
		void SetCurTime(NetTimeType curTime)
		{
			_currentTime = curTime;
			SetTimeValid(true);
		}
		
		// ������ ������� ������� ����� �� ������ ���������� �� ������ ����� � ������ ������
		void SetCurTimeByReceivedTime()
		{
			// � ������� ����� ���� ���-�� ��������
			if (_receivedCount > 0)
			{
				_currentTime = _lastReceivedTime;
				SetTimeValid(true);
			}
			else
			{
				// �������� ��������� ������
				_setByReceivedTime = true;
				// � ���� ����� ������� �����������
				SetTimeValid(false);
			}
		}
		
		// ���������� �������� �������, ����������� �� ������� ����
		void OnNetTimeReceived(AddressType source, NetTimeType netTime)
		{
			// ������������� ������� �� ���� � ���������� �������
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
		
		// ���������� ����� ����� ����
		void OnEndTimeChain()
		{
			// ��������� ���������� �� ������ ���������� �������
			if (_setByReceivedTime)
			{
				// � ������� ����� ���� ���-�� ��������
				if (_receivedCount > 0)
				{
					_currentTime = _lastReceivedTime;
					SetTimeValid(true);
					// ���������
					_setByReceivedTime = false;
				}
			}
			
			// � ������� ����� ���� ���-�� ��������, ���� ��������� ���������� �������
			if (_timeValid && _receivedCount > 0)
			{
				// ���� ���� �����������
				if (_currentTime != _lastReceivedTime)
				{
					// ����� ������ ������� ����������
					SetTimeValid(false);
					// ��������� � ��������� ���
					_setByReceivedTime = true;
				}
			}
			
			if (_timeValid)
			{
				_currentTime += 1.0;
			}
			
			// ������� ��� ���-�� ��������
			_receivedCount = 0;
		}
		
		bool TimeValid()
		{
			return _timeValid;
		}
		
		// �������� ������� ������� �����
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