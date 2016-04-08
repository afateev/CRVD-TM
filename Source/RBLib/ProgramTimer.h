#ifndef PROGRAMTIMER_H
#define PROGRAMTIMER_H

#include "EventDispetcher.h"


namespace Rblib
{
	class ProgramTimer
	{
		Event _event;
		bool _directCall;
		float _freq;		
		EventCode _code;
		
		long _handlerId;

		unsigned long _interval;
		unsigned int _repeat;
		bool _infinite;	
		unsigned long _count;
		bool _start;				// Был старт
		bool _init;					// Была инициализация
	public:
		// Конструктор по умолчанию
		ProgramTimer() : 
			_event(EventEmty),
			_directCall(false), _freq(0), _code(EventEmty),
			_start(false), _init(false)
		{
		}
		// Коструктор с инициализацией
		// event - входное cобытие,
		// directCall - прямой вызов(отложенный вызов),
		// freq - частота входный событий,
		// outCode - код выходного события
		ProgramTimer(Event event, bool directCall, float freq, EventCode outCode) :
			_event(EventEmty),
			_start(false), _init(true)
		{
			Init(event, directCall, freq, outCode);
		}
		
		// Инициализация
		void Init (Event event, bool directCall, float freq, EventCode outCode)
		{
			// Если таймер тикает, нельзя инициализировать
			if (_start)
				return;
			_event = event;
			_directCall = directCall;
			_freq = freq;
			_code = outCode;
			_init = true;
		}
		
		//обработчик
		void operator()(const Event &e)
		{
			if (!_init)
				return;
			if (!_start)
				return;
			if( (_event.Code == e.Code) &&
				(_event.Param1 == e.Param1) &&
				(_event.Param2 == e.Param2) &&
				(_event.Param3 == e.Param3) )
			{
				QuantFire();
			}
		}
		
		//квантование
		void QuantFire()
		{			
			if(_count != 0)
			{
				_count--;
				return;
			}
			
			_count = _interval;

			if(_infinite == false)
			{
				if(_repeat == 0)
				{
					if(_directCall == false)
						Stop();
					return;
				}
				else
					_repeat--;
			}
			
			Event e(_code);
			Singletone<EventDispetcher>::Instance().PushEvent(e);			
		}
		
		//запустить остановленный таймер
		//delay - время задержки (мс)
		//interval - интервал между событиями (мс)
		//repeat - количество событий, если repeat = 0, то неограниченное количество 
		bool Start(float delay, float interval, unsigned int repeat)
		{
			if (!_init)
				return false;
			if(_start)
				return false;
			
			_count = (unsigned long)((float)delay * _freq / 1000);
			_interval = (unsigned long)((float)interval * _freq / 1000);
			if(_interval > 0)
				_interval--;
			if(repeat != 0)
			{
				_infinite = false;
				_repeat = repeat;
			}
			else
				_infinite = true;
			
			_handlerId = Singletone<EventDispetcher>::Instance().AddHandler(_event.Code, EventDispetcher::EventFunctor(*this), _directCall);

			_start = true;
			return true;
		}
		
		//остановить запущенный таймер
		void Stop()
		{
			if (!_init)
				return;
			if(_start)
			{
				Singletone<EventDispetcher>::Instance().ClearEventHandler(_handlerId);
				_start = false;
			}
		}

		//проверить, был ли запущен таймер
		bool IsStarted()
		{
			return _start;
		}
	};
}

#endif //PROGRAMTIMER_H