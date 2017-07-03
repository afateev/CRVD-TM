////////////////////////////////////////////////////////////////////////////////
//	Сигнал "Периодические импульсы"
//
//	Fateev A.I.		28/03/2014
////////////////////////////////////////////////////////////////////////////////

#ifndef SIGNALPULSE_H
#define SIGNALPULSE_H

template
	<
		class Port,
		int pin,
		bool activeLevel
	>
class SignalPulse
{
public:
	static unsigned long _period;
	static unsigned long _countdown;
	static bool _active;
public:
	static void Init(unsigned long val)
	{
		Port::SetOutputPin(pin);
		On(false);
		SetTime(val);
		Reset();
	}
	
	static void Reset()
	{
		_countdown = _period;
		_active = false;
	}
	
	static void SetTime(unsigned long val)
	{
		_period = val;
		if (_period < 2)
		{
			_period = 2;
		}
		
		Reset();
	}
	
	static unsigned long GetTime()
	{
		return _period;
	}
	
	static void Set(bool active)
	{
		if (_active != active)
		{
			_active = active;
			_countdown = _period - 1; // первый импуль через заданный интервал
		}
	}
	
	static void OnOneMs()
	{
		if (_active)
		{
			if (_countdown > 0)
			{
				On(false);
				_countdown--;
			}
			else
			{
				On(true);
				_countdown = _period - 1;
			}
		}
		else
		{
			On(false);
		}
	}
protected:
	static void On(bool on)
	{
		if (on)
		{
			if (activeLevel)
			{
				Port::SetBit(pin);
			}
			else
			{
				Port::ClearBit(pin);
			}
		}
		else
		{
			if (activeLevel)
			{
				Port::ClearBit(pin);
			}
			else
			{
				Port::SetBit(pin);
			}
		}
	}
};

template <class Port, int pin, bool activeLevel>
unsigned long SignalPulse<Port, pin, activeLevel>::_period = 0;

template <class Port, int pin, bool activeLevel>
unsigned long SignalPulse<Port, pin, activeLevel>::_countdown = 0;

template <class Port, int pin, bool activeLevel>
bool SignalPulse<Port, pin, activeLevel>::_active = false;

#endif