////////////////////////////////////////////////////////////////////////////////
//	Сигнал "Дискретный выход"
//
//	Fateev A.I.		28/03/2014
////////////////////////////////////////////////////////////////////////////////

#ifndef SIGNALDISCRETEOUTPUT_H
#define SIGNALDISCRETEOUTPUT_H

template
	<
		class Port,
		int pin,
		bool activeLevel
	>
class SignalDiscreteOutput
{
protected:
	static bool _active;
public:
	static void Init()
	{
		Reset();
		Port::SetOutputPin(pin);
		On(false);
	}
	
	static void Reset()
	{
		_active = false;
	}
	
	static void Set(bool active)
	{
		_active = active;
	}
	
	static void Update()
	{
		On(_active);
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
bool SignalDiscreteOutput<Port, pin, activeLevel>::_active = false;

#endif
