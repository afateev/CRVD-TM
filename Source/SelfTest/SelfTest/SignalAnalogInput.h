////////////////////////////////////////////////////////////////////////////////
//	Сигнал "Аналоговый вход"
//
//	Fateev A.I.		28/03/2014
////////////////////////////////////////////////////////////////////////////////

#ifndef SIGNALANALOGINPUT_H
#define SIGNALANALOGINPUT_H

template
	<
		class Adc,
		typename ChennelValueType,
		ChennelValueType ch,
		unsigned short MaxValue
	>
class SignalAnalogInput
{
protected:
	static float _value;
	static float _ref;
	volatile static bool _lock;
	static float _output;
public:
	static void Init(float ref)
	{
		Reset();
		_ref = ref;
	}
	
	static void Reset()
	{
		_value = 0;
		_lock = false;
		_output = 0;
	}
	
	static void Update()
	{
		/*
		// двойное измерение, что исключить "дрыганье"
		float val1 = Adc::DoConversion(ch);
		bool ok = false;
		while (!ok)
		{
			float val2 = Adc::DoConversion(ch);
			float delta = val1 - val2;
			if (delta < 0)
			{
				delta = -delta;
			}
			
			ok = delta < 2;
			val1 = val2;
		}
		
		_value = val1;
		
		// а вообще по-нормальному надо так
		*/
		
		float val = Adc::DoConversion(ch);
		
		if (MaxValue > 0)
		{
			val /= MaxValue;
		}
		
		val *= _ref;
		
		while(_lock)
		{
			;
		}
		
		_lock = true;
		_value = val;
		_lock = false;
	}
	
	static float Get()
	{
		if (!_lock)
		{
			_lock = true;
			_output = _value;
			_lock = false;
		}
		
		return _output;
	}
};

template <class Adc, typename ChennelValueType, ChennelValueType ch, unsigned short MaxValue>
float SignalAnalogInput<Adc, ChennelValueType, ch, MaxValue>::_value = 0;

template <class Adc, typename ChennelValueType, ChennelValueType ch, unsigned short MaxValue>
float SignalAnalogInput<Adc, ChennelValueType, ch, MaxValue>::_ref = 0;

template <class Adc, typename ChennelValueType, ChennelValueType ch, unsigned short MaxValue>
bool SignalAnalogInput<Adc, ChennelValueType, ch, MaxValue>::_lock = false;

template <class Adc, typename ChennelValueType, ChennelValueType ch, unsigned short MaxValue>
float SignalAnalogInput<Adc, ChennelValueType, ch, MaxValue>::_output = 0;

#endif
