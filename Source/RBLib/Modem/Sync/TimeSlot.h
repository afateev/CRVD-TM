///////////////////////////////////////////////////////////////////////////////
//	����� ������������� ���� ����� ����
//	��������� ���������� ��������
//	��������� �������� ������������ ����, �������������� �
//	��������� �������� ������ ������� ������ ����
//	�������� ���������� � ������ ������� ������ ����
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_SYNC_TIMESLOT_H
#define MODEM_SYNC_TIMESLOT_H

#include "../../Utils/CallbackWrapper.h"

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
			bool externalSync = false
		>
	class TimeSlot
	{
	public:
		typedef CallbackWrapper<> OnTimeSlotCallbackType;
	protected:
		float _maxError;
		float _correction;
		OnTimeSlotCallbackType _onTimeSlotCallback;
	public:
		// ����������� �� ���������
		TimeSlot()
		{
			_maxError = 0.0;
			_correction = 0;
		}
		
		// �������������
		// ������������ ���������� ����������������, %
		bool Init(float maxError)
		{
			_onTimeSlotCallback.Reset();

			_maxError = maxError;
			
			if (!SettingsValid())
			{
				return false;
			}
			
			return true;
		}
		
		// ���������/���������� ������ ����
		void Enable(bool enable = true)
		{
			if (enable)
			{
				if (!SettingsValid())
				{
					return;
				}
				
				TickSource::SetFrequency(SlotCountPerSecond(), 1.0 / 3.0);
				TickSource::Enable();
			}
			else
			{
				TickSource::SetFrequency(SlotCountPerSecond(), 1.0 / 3.0);
				TickSource::Disable();
			}
		}
		
		// ���������� ������ ����
		void Disable()
		{
			Enable(false);
		}
		
		// ������ ���������� ����������� ������� ������� ������ ����
		void SetOnTimeSlotCallback(OnTimeSlotCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onTimeSlotCallback.Set(callback, callbackParam);
		}
		
		// ���������� �������� ��������� ����
		// ��������� ����������� 1 ��� ��� ��������� ����������� �������
		void SetCorrection(float correction)
		{
			_correction = correction;
		}
		
		// ���������� �������� ��������
		void OnTick()
		{
			float frequency = (float)SlotCountPerSecond();
			
			if (_correction != 0)
			{
				float slotTime = 1.0 / (float)SlotCountPerSecond();
				
				if (_correction <= -1.0 || _correction >= 1.0)
				{
					_correction -= (int)_correction;
				}
				
				float correctedSlotTime = slotTime + slotTime * _correction;

				frequency = 1.0 / correctedSlotTime;
				
				_correction = 0;
			}
			
			TickSource::SetFrequency(frequency, 1.0 / 3.0);
			_onTimeSlotCallback.Call();
		}
		
		// �������� ����� ������ ����
		// ������������ �������� � ��������� �� 0 �� 1
		// �������� 0 - ������������� ������ ����
		// �������� 1 - ������������� ����� ����
		float GetCurTime()
		{
			return TickSource::GetTime();
		}
		
		// ���������� ���� � �������
		int SlotCountPerSecond()
		{
			return slotCountPerSecond;
		}
		
		// ���������� ���������������� � ���������
		float MaxErrorPercent()
		{
			return _maxError;
		}
	protected:
		// �������� ��������
		bool SettingsValid()
		{
			// ������� ��������� ������������ ������� �� ����� ���� ������ ���-�� ����
			if (TickSource::GetTimerSourceFrequency() < SlotCountPerSecond())
			{
				return false;
			}
			
			// ������ ���� ���� �� ���� ���� � �������
			if (SlotCountPerSecond() < 1)
			{
				return false;
			}
			
			// ������ ���� ������ �������� ���������� ����������������
			if (_maxError < 0.01)
			{
				return false;
			}
			
			return true;
		}
	};
	
	template
		<
			class TickSource,
			int slotCountPerSecond
		>
	class TimeSlot<TickSource, slotCountPerSecond, true>
	{
	public:
		typedef CallbackWrapper<> OnTimeSlotCallbackType;
	protected:
		float _maxError;
		float _correction;
		OnTimeSlotCallbackType _onTimeSlotCallback;
	public:
		// ����������� �� ���������
		TimeSlot()
		{
			_maxError = 0.0;
			_correction = 0;
		}
		
		// �������������
		// ������������ ���������� ����������������, %
		bool Init(float maxError)
		{
			_onTimeSlotCallback.Reset();

			return true;
		}
		
		// ���������/���������� ������ ����
		void Enable(bool enable = true)
		{
			if (enable)
			{
				TickSource::Enable();
			}
			else
			{
				TickSource::Disable();
			}
		}
		
		// ���������� ������ ����
		void Disable()
		{
			Enable(false);
		}
		
		// ������ ���������� ����������� ������� ������� ������ ����
		void SetOnTimeSlotCallback(OnTimeSlotCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onTimeSlotCallback.Set(callback, callbackParam);
		}
		
		// ���������� �������� ��������� ����
		// ��������� ����������� 1 ��� ��� ��������� ����������� �������
		void SetCorrection(float correction)
		{
			TickSource::PulseCorrection();
		}
		
		// ���������� �������� ��������
		void OnTick()
		{
			_onTimeSlotCallback.Call();
		}
		
		// �������� ����� ������ ����
		// ������������ �������� � ��������� �� 0 �� 1
		// �������� 0 - ������������� ������ ����
		// �������� 1 - ������������� ����� ����
		float GetCurTime()
		{
			return 0.0;
		}
		
		// ���������� ���� � �������
		int SlotCountPerSecond()
		{
			return slotCountPerSecond;
		}
		
		// ���������� ���������������� � ���������
		float MaxErrorPercent()
		{
			return _maxError;
		}
	};
}
}
}

#endif