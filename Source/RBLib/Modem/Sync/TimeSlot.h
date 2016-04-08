///////////////////////////////////////////////////////////////////////////////
//	Класс обслуживающий окно цикла сети
//	Управляет аппаратным таймером
//	Позволяет задавать длительность окна, корректировать её
//	Позволяет измерить момент времени внутри окна
//	Вызывает обработчик в момент времени начала окна
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
		// Конструктор по умолчанию
		TimeSlot()
		{
			_maxError = 0.0;
			_correction = 0;
		}
		
		// Инициализация
		// максимальная допустимая рассинхронизация, %
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
		
		// Запустить/остановить отсчёт окон
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
		
		// Остановить отсчёт окон
		void Disable()
		{
			Enable(false);
		}
		
		// Задать обработчик наступления момента времени начала окна
		void SetOnTimeSlotCallback(OnTimeSlotCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onTimeSlotCallback.Set(callback, callbackParam);
		}
		
		// Установить величину коррекции окна
		// Коррекция выполняется 1 раз при очередном переполении таймера
		void SetCorrection(float correction)
		{
			_correction = correction;
		}
		
		// Обработчик тактовых сигналов
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
		
		// Получить время внутри окна
		// возвращается значение в диапазоне от 0 до 1
		// значение 0 - соответствует началу окна
		// значение 1 - соответствует концу окна
		float GetCurTime()
		{
			return TickSource::GetTime();
		}
		
		// Количество окон в секунду
		int SlotCountPerSecond()
		{
			return slotCountPerSecond;
		}
		
		// Допустимая рассинхронизация в процентах
		float MaxErrorPercent()
		{
			return _maxError;
		}
	protected:
		// Проверка настроек
		bool SettingsValid()
		{
			// частота источника тактирования таймера не может быть меньше кол-ва окон
			if (TickSource::GetTimerSourceFrequency() < SlotCountPerSecond())
			{
				return false;
			}
			
			// должно быть хотя бы одно окно в секунду
			if (SlotCountPerSecond() < 1)
			{
				return false;
			}
			
			// должна быть задана разумная допустимая рассинхронизация
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
		// Конструктор по умолчанию
		TimeSlot()
		{
			_maxError = 0.0;
			_correction = 0;
		}
		
		// Инициализация
		// максимальная допустимая рассинхронизация, %
		bool Init(float maxError)
		{
			_onTimeSlotCallback.Reset();

			return true;
		}
		
		// Запустить/остановить отсчёт окон
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
		
		// Остановить отсчёт окон
		void Disable()
		{
			Enable(false);
		}
		
		// Задать обработчик наступления момента времени начала окна
		void SetOnTimeSlotCallback(OnTimeSlotCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onTimeSlotCallback.Set(callback, callbackParam);
		}
		
		// Установить величину коррекции окна
		// Коррекция выполняется 1 раз при очередном переполении таймера
		void SetCorrection(float correction)
		{
			TickSource::PulseCorrection();
		}
		
		// Обработчик тактовых сигналов
		void OnTick()
		{
			_onTimeSlotCallback.Call();
		}
		
		// Получить время внутри окна
		// возвращается значение в диапазоне от 0 до 1
		// значение 0 - соответствует началу окна
		// значение 1 - соответствует концу окна
		float GetCurTime()
		{
			return 0.0;
		}
		
		// Количество окон в секунду
		int SlotCountPerSecond()
		{
			return slotCountPerSecond;
		}
		
		// Допустимая рассинхронизация в процентах
		float MaxErrorPercent()
		{
			return _maxError;
		}
	};
}
}
}

#endif