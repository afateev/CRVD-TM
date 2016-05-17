////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		27/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef CRVD_EVENTS_H
#define CRVD_EVENTS_H

//#include "MegaLib\MegaLib.h"
#include "../RbLib/Rblib.h"

enum EventCode
{
	EventEmpty 			= 0,
	EventDriveStart 	= 1,
	EventDriveStop 		= 2,
	EventEnergizingOn	= 3,
	EventEnergizingOff	= 4,
	EventForcingOn		= 5,
	EventForcingOff		= 6,
	EventOmvOn			= 7,
	EventOmvOff			= 8,
	EventCosControl		= 9,
	EventRotorCurrentControl	= 10,
	EventRControl		= 11,
	EventHeatRot		= 12,
	EventCoolRot		= 13,
	EventAsynch			= 14,
	EventCheckoutStart	= 15,
	EventCheckoutStop	= 16,
	EventRunMain		= 17,
	EventRunReserve		= 18,
	EventRunEmergency	= 19,
	EventProgramStart	= 20,
	EventReactCurrentControl	= 21,
	EventReactPowerControl		= 22,
	//EventSetDown		= ,
	//EventSetUp		= ,
	
	EventProtAsynch			= 50,
	EventProtLostEnergizin	= 51,
	EventProtWrongMode		= 52,
	EventProtLongStart		= 53,
	EventProtKZ				= 54,
	EventProtLowUst			= 55,
	EventProtLowIst			= 56,
	EventProtStator			= 57,
	EventStopButton			= 58
};

class Event
{
public:
	struct EventData
	{
		time_t _dt;
		EventCode _code;
		int _param;
		
		EventData() : _dt(0), _code(EventEmpty), _param(-1)
		{}
		EventData(time_t dt, EventCode code) : _dt(dt), _code(code), _param(-1)
		{}
		EventData(time_t dt, EventCode code, int param) : _dt(dt), _code(code), _param(param)
		{}
	};
protected:
	EventData _data;
	bool _noLog;
public:
	Event() : _data(), _noLog(0)
	{}
	Event(time_t dt, EventCode code) : _data(dt, code), _noLog(0)
	{}
	Event(time_t dt, EventCode code, int param) : _data(dt, code, param), _noLog(0)
	{}
	
	Event(EventData data) : _data(data), _noLog(0)
	{}
	
	time_t GetDt()
	{
		return _data._dt;
	}
	
	void SetDt(time_t dt)
	{
		_data._dt = dt;
	}
	
	EventCode GetCode()
	{
		return _data._code;
	}
	
	char * GetText()
	{
		switch(_data._code)
		{
		case EventDriveStart:
			return "Двигатель включен";
		case EventDriveStop:
			return "Двигатель отключен";
		case EventEnergizingOn:
			return "Возбуждение включено";
		case EventEnergizingOff:
			return "Возбуждение отключено";
		case EventForcingOn:
			return "Форсировка включена";
		case EventForcingOff:
			return "Форсировка отключена";
		case EventOmvOn:
			return "ОМВ включено";
		case EventOmvOff:
			return "ОМВ отключено";
		case EventCosControl:
			return "Включен канал по cosF";
		case EventRotorCurrentControl:
			return "Включен канал по Iр";
		case EventReactCurrentControl:
			return "Включен канал по Iреакт";
		case EventReactPowerControl:
			return "Включен канал по Qреакт";
		case EventRControl:
			return "Ручное управление вкл";
		case EventHeatRot:
			return "Нагрев ротора";
		case EventCoolRot:
			return "Охлаждение ротора";
		case EventAsynch:
			return "Асинхронный ход";
		case EventCheckoutStart:
			return "Опробование включено";
		case EventCheckoutStop:
			return "Опробование отключено";
		case EventRunMain:
			return "В работе основной";
		case EventRunReserve:
			return "В работе резервный";
		case EventRunEmergency:
			return "В работе аварийный";
		case EventProgramStart:
			return "Возбудитель включен";
			
		case EventProtAsynch:
			return "АСИНХРОННЫЙ ХОД";
		case EventProtLostEnergizin:
			return "ПОТЕРЯ ВОЗБУЖДЕНИЯ";
		case EventProtWrongMode:
			return "НЕРАСЧЕТНЫЙ РЕЖИМ";
		case EventProtLongStart:
			return "ЗАТЯНУВШИЙСЯ ПУСК";
		case EventProtKZ:
			return "КОРОТКОЕ ЗАМЫКАНИЕ";
		case EventProtLowUst:
			return "НИЗКОЕ Uст";
		case EventProtLowIst:
			return "НИЗКИЙ Iст";
		case EventProtStator:
			return "СТАТОРНЫЕ ЗАЩИТЫ";
		case EventStopButton:
			return "КНОПКА СТОП";
		}
		return 0;
	}
	char * GetParamText()
	{
		switch(_data._code)
		{
		case EventDriveStop:
			{
				switch(_data._param)
				{
				case 0:
					return "Асинхронный ход";
				case 1:
					return "Потеря возбуждения";
				case 2:
					return "Нерасчетный режим";
				case 3:
					return "Затянувшийся пуск";
				case 4:
					return "Короткое замыкание";
				case 5:
					return "Низкое напряжение статора";
				case 6:
					return "Низкий ток статора";
				case 7:
					return "Статорные защиты";
				case 8:
					return "Кнопка 'Стоп'";
				};
				
				return 0;
			}
		}
		return 0;
	}
	
	EventData GetData()
	{
		return _data;
	}
	
	void SetNoLog(bool value = true)
	{
		_noLog = value;
	}
	
	bool IsNoLog()
	{
		return _noLog;
	}
};

template<unsigned int MaxCount, unsigned int CacheSize>
class EventLog
{
protected:
	typedef bool FileCallback(long int offset, int origin, unsigned char *data, unsigned int count);
	typedef bool FileSeekCallback(long int offset, int origin, unsigned long &pos);
	typedef Rblib::queue_static<Event, MaxCount> QueueType;
	
	static QueueType _events;
public:
	static Event::EventData _eventsCache[CacheSize];
	static unsigned long _lastEventInCache;
	static unsigned long _eventsCountInCache;
	static unsigned long _newEventsCount;
protected:
	//static unsigned int _count;
	static FileCallback *_readFile;
	static FileCallback *_writeFile;
	static FileSeekCallback *_seekFile;
	static Event _lastEvent;
public:
	static void Init(FileCallback *read, FileCallback *write, FileSeekCallback *seek)
	{
		_readFile = read;
		_writeFile = write;
		_seekFile = seek;
	}
	
	static void Push(Event event)
	{
		if (_events.size() > MaxCount - 2)
		{
			return;
		}
			
		if (!event.IsNoLog() && event.GetCode() != EventEmpty)
			_events.push(event);
		
		switch (event.GetCode())
		{
		case EventDriveStart:
		case EventDriveStop:
		case EventCheckoutStart:
		case EventCheckoutStop:
			_lastEvent = event;
			break;
		}
	}
	
	static void Push(time_t dt, EventCode code)
	{
		Push(Event(dt, code));
	}
	
	static void Push(time_t dt, EventCode code, int param)
	{
		Push(Event(dt, code, param));
	}
	
	static unsigned int GetCount()
	{
		return _events.size();
	}
	
	static Event GetLastEvent()
	{
		return _lastEvent;
	}
	
	static void GetLastEventDescription(time_t *dt, char **eventText, char **eventParamText)
	{
		Event lastEvent = GetLastEvent();
		
		if (dt)
		{
			*dt = lastEvent.GetDt();
		}
		if (eventText)
		{
			*eventText = lastEvent.GetText();
		}
		if (eventParamText)
		{
			*eventParamText = lastEvent.GetParamText();
		}
	}
	
	static unsigned long GetEventsCount()
	{
		if (!_seekFile)
			return 0;
		if (!_writeFile)
			return 0;
		
		if (!_readFile)
			return 0;
		
		unsigned long pos;
		if (!_seekFile(0, 2, pos)) // в конец
			return 0;
		return pos / sizeof(Event::EventData);
	}
	
	static void Cache(unsigned long lastEventNumber)
	{
		if (!_seekFile)
			return;
		if (!_writeFile)
			return;
		
		if (!_readFile)
			return;
		
		unsigned long pos;
		if (!_seekFile(0, 2, pos)) // в конец
			return;
		
		unsigned long count = pos / sizeof(Event::EventData);
		
		long last = lastEventNumber;
		if (last > count)
			last = count;
		long first = last;
		first -= CacheSize - 1;
		if (first < 1)
			first = 1;
		
		_lastEventInCache = last;
		_eventsCountInCache = last - first + 1;
		
		if (!_readFile((first - 1) * sizeof(Event::EventData), 0, (unsigned char *)_eventsCache, _eventsCountInCache * sizeof(Event::EventData)))
			_eventsCountInCache = 0;
	}
	
	static void Run()
	{
		if (!_seekFile)
			return;
		if (!_writeFile)
			return;
		
		if (!_readFile)
			return;
		
		if (!_events.empty())
		{
			Event e = _events.front();
			Event::EventData data = e.GetData();
			
			// в конец
			if (!_writeFile(0, 2, (unsigned char *)&data, sizeof(data)))
				return;
			// записалось, можно удалить
			_events.pop();
			_newEventsCount++;
		}
	}
};

template<unsigned int MaxCount, unsigned int CacheSize>
EventLog<MaxCount, CacheSize>::QueueType EventLog<MaxCount, CacheSize>::_events;

template<unsigned int MaxCount, unsigned int CacheSize>
Event EventLog<MaxCount, CacheSize>::_lastEvent;

template<unsigned int MaxCount, unsigned int CacheSize>
Event::EventData EventLog<MaxCount, CacheSize>::_eventsCache[CacheSize];

template<unsigned int MaxCount, unsigned int CacheSize>
unsigned long EventLog<MaxCount, CacheSize>::_lastEventInCache = 0;

template<unsigned int MaxCount, unsigned int CacheSize>
unsigned long EventLog<MaxCount, CacheSize>::_eventsCountInCache = 0;

template<unsigned int MaxCount, unsigned int CacheSize>
unsigned long EventLog<MaxCount, CacheSize>::_newEventsCount = 0;

template<unsigned int MaxCount, unsigned int CacheSize>
EventLog<MaxCount, CacheSize>::FileCallback *EventLog<MaxCount, CacheSize>::_readFile = 0;

template<unsigned int MaxCount, unsigned int CacheSize>
EventLog<MaxCount, CacheSize>::FileCallback *EventLog<MaxCount, CacheSize>::_writeFile = 0;

template<unsigned int MaxCount, unsigned int CacheSize>
EventLog<MaxCount, CacheSize>::FileSeekCallback *EventLog<MaxCount, CacheSize>::_seekFile = 0;

typedef EventLog<32, 13> Events;

#endif