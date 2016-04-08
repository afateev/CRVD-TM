
#ifndef _EVENT_FILTER_H_
#define _EVENT_FILTER_H_

#include "Utils/Singletone.h"
#include "EventDispetcher.h"

namespace Rblib
{

template<bool NeedEvents>
class EventFilter
{
public:

    EventFilter(unsigned long id = 0)
    : _sourceFreq(0)
    , _blockEvents(false)
    , _eventsDelay(0)
    , _sourceCounter(0)
    , _sourceMax(0)
    , _id(id)
    {
    }

    EventFilter(Event sourceEvent, float sourceFreq, float eventsDelay, unsigned long id = 0)
    : _sourceFreq(sourceFreq)
    , _sourceEvent(sourceEvent)
    , _blockEvents(false)
    , _eventsDelay(eventsDelay)
    , _sourceCounter(0)
    , _sourceMax((unsigned long)(eventsDelay * sourceFreq / 1000.0)) // количество тактирующих событий за время задержки
    , _id(id)
    {
        Singletone<Rblib::EventDispetcher>::Instance().AddHandler(sourceEvent.Code,
                                                                  EventDispetcher::EventFunctor(*this),
                                                                  true);
    }

    void Init(Event sourceEvent, float sourceFreq, float eventsDelay)
    {
        Reset();

        _sourceFreq = sourceFreq;
        _sourceEvent = sourceEvent;
        _sourceMax = (unsigned long)(eventsDelay * sourceFreq / 1000.0);

        Singletone<Rblib::EventDispetcher>::Instance().AddHandler(sourceEvent.Code,
                                                                  EventDispetcher::EventFunctor(*this),
                                                                  true);
    }

    void Reset()
    {
        EnableEvents();
        StopDelayMeasure();
    }

    // Проверить и заблокировать на заданный при инициализации интервал, если не заблокировано
    bool Check()
    {
        if (EventsBlocked())
            return false;

        BlockEvents();
        StartDelayMeasure();

        return true;
    }

    // Заблокировать на заданный при инициализации интервал
    void Block()
    {
        BlockEvents();
        StartDelayMeasure();
    }

    // Заблокировать на заданный праметром интервал
    void Block(float delay)
    {
        BlockEvents();
        StartDelayMeasure((unsigned long)(delay * _sourceFreq / 1000.0));
    }

public:

    void operator()(const Event &e)
    {
        if (e != _sourceEvent)
            return;

        if (CheckDelayIsUp())
        {
            EnableEvents(); 
            if (NeedEvents)
            {
                SendEvent();
            }
        }
    }

private:
    
    inline void BlockEvents()
    {
        _blockEvents = true;
    }

    inline void EnableEvents()
    {
        _blockEvents = false;
    }

    inline bool EventsBlocked()
    {
        return _blockEvents;
    }

    inline void StartDelayMeasure()
    {
        _sourceCounter = _sourceMax;
    }

    // заблокировать на нестандартное число тиков
    inline void StartDelayMeasure(unsigned long counterMax)
    {
        _sourceCounter = counterMax;
    }

    inline void StopDelayMeasure()
    {
        _sourceCounter = 0;
    }

    inline bool CheckDelayIsUp()
    {
        if (!_sourceCounter)
            return false;

        _sourceCounter--;

        if (_sourceCounter)
            return false;

        return true;
    }

    inline void SendEvent()
    {
        Rblib::Event e(Rblib::EventFilterEvent, _id, _id >> 8, _id >> 16, _id >> 24);
        Rblib::Singletone<Rblib::EventDispetcher>::Instance().PushEvent(e);
    }

private:

    float _sourceFreq;
  
    Event _sourceEvent;
    float _eventsDelay;
    unsigned long _sourceCounter;
    unsigned long _sourceMax;

    bool _blockEvents;

    unsigned long _id;
};

}

#endif // _EVENT_FILTER_H_

