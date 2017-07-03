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
public:
	static void Init(float ref)
	{
		Reset();
		_ref = ref;
	}
	
	static void Reset()
	{
		_value = 0;
	}
	
	static void Update()
	{
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
		//_value = Adc::DoConversion(ch);
		
		if (MaxValue > 0)
		{
			_value /= MaxValue;
		}
		
		_value *= _ref;
	}
	
	static float Get()
	{
		return _value;
	}
};

template <class Adc, typename ChennelValueType, ChennelValueType ch, unsigned short MaxValue>
float SignalAnalogInput<Adc, ChennelValueType, ch, MaxValue>::_value = 0;

template <class Adc, typename ChennelValueType, ChennelValueType ch, unsigned short MaxValue>
float SignalAnalogInput<Adc, ChennelValueType, ch, MaxValue>::_ref = 0;

#endif
