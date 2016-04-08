///////////////////////////////////////////////////////////////////////////////
//	Элемент строки таблицы активности узлов сети слышимых напрямую
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_ACTIVITY_SELFACTIVITYTABLEITEM_H
#define MODEM_ACTIVITY_SELFACTIVITYTABLEITEM_H

namespace Rblib
{
namespace Modem
{
namespace Activity
{
#pragma pack(push, 1)

	class SelfActivityTableItem
	{
	protected:	
		unsigned char _activityInCurrentIntervalCount;		// сколько пришло пакетов в текущем интервале проверки
		unsigned short _checkIntervalCount;					// счётчик цинклов сети в текущем интервале проверки
		unsigned char _currentRate;							// текущий рейтинг связи (_activityInCurrentIntervalCount / _chectPeriod) * 100
	public:
		// Констуктор
		SelfActivityTableItem()
		{
			Reset();
		}
		
		// Сброс
		void Reset()
		{
			_checkIntervalCount = 0;
			_currentRate = 0;
			ResetActivityInCurrentIntervalCount();
			ResetCurrentRate();
		}
		
		// Обнаружена активность
		void OnActivityDetected(unsigned int normalActivityInterval)
		{
			if (normalActivityInterval < 1)
			{
				return;
			}
			
			// желательно чтобы счётчик проверки тикал кратно нормальному интервалу активности,
			// чтобы не было погрешности измерения +- один интервал активности
			unsigned char offset = _checkIntervalCount % normalActivityInterval;
			if (offset > 0)
			{
				_checkIntervalCount += offset;
				
				// если отсчет уже идёт, но не было принято ни одного синхропакета, то отсчёт начнём с этого момента
				// для того чтобы тикать одновременно с удалённым узлом
				if (_activityInCurrentIntervalCount < 1)
				{
					_checkIntervalCount = 0;
				}
			}
			
			IncrementActivityInCurrentIntervalCount();
		}
		
		// Сбросить счётчик активности в текущем интервале
		void ResetActivityInCurrentIntervalCount()
		{
			_activityInCurrentIntervalCount = 0;
		}
		
		// Сбросить счётчик интервала проверки
		void ResetIntervalCount()
		{
			_checkIntervalCount = 0;
		}
		
		// Сбросить текущий рейтинг
		void ResetCurrentRate()
		{
			_currentRate = 0;
		}
		
		// Увеличить счётчик интервала проверки
		void IncrementCheckIntervalCounter()
		{
			if (_checkIntervalCount < (256 * sizeof(_checkIntervalCount) - 1))
			{
				_checkIntervalCount++;
			}
		}
		
		// Увеличить счётчик активности в текущем интервале
		void IncrementActivityInCurrentIntervalCount()
		{
			if (_activityInCurrentIntervalCount < 255)
			{
				_activityInCurrentIntervalCount++;
			}
		}
		
		// Проверить состояние
		// вернёт true если произощли изменения в текущем рейтинге
		bool CheckActivity(unsigned int normalActivityInterval, unsigned int checkPeriod)
		{
			if (normalActivityInterval < 1 || checkPeriod < 1)
			{
				return false;
			}
			
			bool changed = false;
			if (_checkIntervalCount >= (normalActivityInterval * checkPeriod))
			{
				unsigned char normalActivityCount = checkPeriod;
				float res = _activityInCurrentIntervalCount;
				res /= (float)normalActivityCount;
				res *= 100.0;
				
				unsigned char result = (unsigned char)res;
				changed = _currentRate != result;
				_currentRate = result;
					
				ResetActivityInCurrentIntervalCount();
				ResetIntervalCount();
			}
			
			return changed;
		}
		
		// Получить текущий рейтинг
		unsigned char GetRate()
		{
			return _currentRate;
		}
	};
	
#pragma pack (pop)
}
}
}

#endif