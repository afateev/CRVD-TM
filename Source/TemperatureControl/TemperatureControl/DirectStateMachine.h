#ifndef DIRECTSTATEMACHNE_H
#define DIRECTSTATEMACHNE_H

class DirectStateMachine
{
public:
	enum State
	{
		StateStoped,
		StateClear,
		StateIdle,
		StateMeasureTemperature,
		StateWaitMeasureTemperature,
		StateTemperatureRequest,
		StateTemperatureWaitResponse
	};
public:
	Rblib::CallbackWrapper<> MeasureTemperature;
	Rblib::CallbackWrapper<> ReadTemperature;
	Rblib::CallbackWrapper<int> StoreTemp;
	Rblib::CallbackWrapper<int &> GetTemp;
	Rblib::CallbackWrapper<int &> GetHeatThreshold;
	Rblib::CallbackWrapper<int &> GetCoolThreshold;
	Rblib::CallbackWrapper<bool> HeatOnOff;
	Rblib::CallbackWrapper<bool> CoolOnOff;
	static const int ErrorThreshold = 20;
	static const int HeatCoolThresholdDelta = 10;
protected:
	State _state;
	int _delayCounter;
	int _errorCounter;
	bool _heat;
	bool _cool;
public:
	DirectStateMachine()
	{
		Reset();
	}
	
	void Reset()
	{
		_state = StateStoped;
		_errorCounter = 0;
		_heat = false;
		_cool = false;
		HeatOnOff(false);
		CoolOnOff(false);
	}
	
	void Start()
	{
		if (_state != StateStoped)
		{
			return;
		}
		
		_state = StateClear;
	}
	
	void Tick(unsigned int tickFrequency)
	{
		switch(_state)
		{
		case StateClear:
			{
				_state = StateIdle;
			}
			break;
		case StateIdle:
			{
				_state = StateMeasureTemperature;
			}
			break;
		case StateMeasureTemperature:
			{
				MeasureTemperature();
				_delayCounter = 0;
				_state = StateWaitMeasureTemperature;
			}
			break;
		case StateWaitMeasureTemperature:
			{
				_delayCounter++;
				if (_delayCounter > tickFrequency * 1)
				{
					_state = StateTemperatureRequest;
				}
			}
			break;
		case StateTemperatureRequest:
			{
				_delayCounter = 0;
				_state = StateTemperatureWaitResponse;
				ReadTemperature();
			}
			break;
		case StateTemperatureWaitResponse:
			{
				_delayCounter++;
				if (_delayCounter > tickFrequency * 1)
				{
					if (_errorCounter < ErrorThreshold)
					{
						_errorCounter++;
					}
					_state = StateClear;
				}
			}
			break;
		}
		
		if (_errorCounter >= ErrorThreshold)
		{
			StoreTemp(0x7FFF);
			HeatOnOff(false);
			CoolOnOff(false);
		}
	}
	
	void OnTemperatureReaded(int t, bool dataValid)
	{
		switch(_state)
		{
		case StateTemperatureWaitResponse:
			{
				if (dataValid && TemperatureValid(t))
				{
					StoreTemp(t);
					
					GetTemp(t);
					
					int heatThreshold = 0;
					GetHeatThreshold(heatThreshold);
					
					int coolThreshold = 0;
					GetCoolThreshold(coolThreshold);
					
					if (t <= heatThreshold)
					{
						HeatOnOff(true);
					}
					
					if (t >= heatThreshold + HeatCoolThresholdDelta)
					{
						HeatOnOff(false);
					}
					
					if (t >= coolThreshold)
					{
						CoolOnOff(true);
					}
					
					if (t <= coolThreshold - HeatCoolThresholdDelta)
					{
						CoolOnOff(false);
					}
				}
				_errorCounter = 0;
				_state = StateClear;
			}
			break;
		}
	}
	
	bool TemperatureValid(int t)
	{
		return t >= -55 && t <= +125;
	}
protected:
};

#endif