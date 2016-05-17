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
			return "��������� �������";
		case EventDriveStop:
			return "��������� ��������";
		case EventEnergizingOn:
			return "����������� ��������";
		case EventEnergizingOff:
			return "����������� ���������";
		case EventForcingOn:
			return "���������� ��������";
		case EventForcingOff:
			return "���������� ���������";
		case EventOmvOn:
			return "��� ��������";
		case EventOmvOff:
			return "��� ���������";
		case EventCosControl:
			return "������� ����� �� cosF";
		case EventRotorCurrentControl:
			return "������� ����� �� I�";
		case EventReactCurrentControl:
			return "������� ����� �� I�����";
		case EventReactPowerControl:
			return "������� ����� �� Q�����";
		case EventRControl:
			return "������ ���������� ���";
		case EventHeatRot:
			return "������ ������";
		case EventCoolRot:
			return "���������� ������";
		case EventAsynch:
			return "����������� ���";
		case EventCheckoutStart:
			return "����������� ��������";
		case EventCheckoutStop:
			return "����������� ���������";
		case EventRunMain:
			return "� ������ ��������";
		case EventRunReserve:
			return "� ������ ���������";
		case EventRunEmergency:
			return "� ������ ���������";
		case EventProgramStart:
			return "����������� �������";
			
		case EventProtAsynch:
			return "����������� ���";
		case EventProtLostEnergizin:
			return "������ �����������";
		case EventProtWrongMode:
			return "����������� �����";
		case EventProtLongStart:
			return "������������ ����";
		case EventProtKZ:
			return "�������� ���������";
		case EventProtLowUst:
			return "������ U��";
		case EventProtLowIst:
			return "������ I��";
		case EventProtStator:
			return "��������� ������";
		case EventStopButton:
			return "������ ����";
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
					return "����������� ���";
				case 1:
					return "������ �����������";
				case 2:
					return "����������� �����";
				case 3:
					return "������������ ����";
				case 4:
					return "�������� ���������";
				case 5:
					return "������ ���������� �������";
				case 6:
					return "������ ��� �������";
				case 7:
					return "��������� ������";
				case 8:
					return "������ '����'";
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
		if (!_seekFile(0, 2, pos)) // � �����
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
		if (!_seekFile(0, 2, pos)) // � �����
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
			
			// � �����
			if (!_writeFile(0, 2, (unsigned char *)&data, sizeof(data)))
				return;
			// ����������, ����� �������
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