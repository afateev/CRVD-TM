////////////////////////////////////////////////////////////////////////////////
//	Сигнал "Дискретный вход"
//
//	Fateev A.I.		28/03/2014
////////////////////////////////////////////////////////////////////////////////

#ifndef SIGNALDISCRETEINPUT_H
#define SIGNALDISCRETEINPUT_H

template
	<
		class Port,
		int pin,
		bool activeLevel
	>
class SignalDiscreteInput
{
public:
	static void Init()
	{
		Reset();
		Port::SetInputPin(pin);
	}
	
	static void Reset()
	{
	}
	
	static bool Get()
	{
		return Port::Read(pin) ? activeLevel : !activeLevel;
	}
};

#endif
