///////////////////////////////////////////////////////////////////////////////
//	Класс глобальных переменных
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include "Utils\Singletone.h"

namespace Rblib
{

// Обертка для статического счетчика перезагрузок
// он не должен инициализироваться при перезагрузке
template<class CounterType>
class WatchdogResetCounter
{
protected:
	// счетчик перезагрузок по сторожевому таймеру
	RBLIB_NO_INIT static CounterType _watchdogResetCount;
};

// Класс глобальных переменных
class GlobalVars :
	public WatchdogResetCounter<unsigned long>
{
protected:	
	// переменные в памяти
	struct RuntimeVars
	{
		unsigned long UpTime;	// время жизни приложения
		// Конструктор по умолчанию
		RuntimeVars()
		{
			UpTime = 0;
		}
	};
	
	// Экземпляры переменных в памяти
	RuntimeVars _runtimeVars;

	// Друг синглетон
	friend class Singletone<EventDispetcher>;

public:
	// Сбросить счетчик перезагрузок по Watchdog
	void ClearWatchdogResetCount()
	{
		_watchdogResetCount = 0;
	}

	// Увеличить счетчик перезагрузок по Watchdog
	void IncWatchdogResetCount()
	{
		_watchdogResetCount++;
	}

	// Получить счетчик перезагрузок по Watchdog
	unsigned long GetWatchdogResetCount()
	{
		return _watchdogResetCount;
	}

	// Сбросить счетчик времени жизни
	void ClearUpTimeCounter()
	{
		_runtimeVars.UpTime = 0;
	}

	// Увеличить счетчик времени жизни
	void IncUpTimeCounter()
	{
		_runtimeVars.UpTime++;
	}

	// Получить счетчик времени жизни
	unsigned long GetUpTimeCounter()
	{
		return _runtimeVars.UpTime;
	}
};

// Инстанцирование статических переменных
template<class CounterType>
CounterType WatchdogResetCounter<CounterType>::_watchdogResetCount;

}

#endif