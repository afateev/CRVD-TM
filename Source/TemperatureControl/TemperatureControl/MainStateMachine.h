#ifndef MAINSTATEMACHNE_H
#define MAINSTATEMACHNE_H

class MainStateMachine
{
public:
	static const int MaxDevCnt = 6;
	static const int ChannelCnt = 4;
	static const int LeartTemperatureTreshold = 50;
	
	enum State
	{
		StateStoped,
		StateClear,
		StateIdle,
		StateLearnStart,
		StateLearnSignalEnable,
		StateLearnSignalDelay,
		StateLearnDevCountRequest,
		StateLearnDevCountWaitResponse,
		StateLearnPlayCount,
		StateLearnMeasureTemperature,
		StateLearnWaitMeasureTemperature,
		StateLearnTemperatureRequest,
		StateLearnTemperatureWaitResponse,
		StateLearnStoreDevice,
		StateLearnEnd,
		StateNextChannel,
		StateMeasureTemperature,
		StateWaitMeasureTemperature,
		StateTemperatureRequest,
		StateTemperatureWaitResponse,
		StateError,
	};
public:
	Rblib::CallbackWrapper<bool, int> LedOnOff;
	Rblib::CallbackWrapper<bool> BeepOnOff;
	Rblib::CallbackWrapper<int> SelectChannel;
	Rblib::CallbackWrapper<> SearchRom;
	Rblib::CallbackWrapper<> MeasureTemperature;
	Rblib::CallbackWrapper<unsigned char *> ReadTemperature;
	Rblib::CallbackWrapper<unsigned int> ClearDevList;
	Rblib::CallbackWrapper<unsigned int, unsigned int, unsigned int, unsigned char *> StoreDevId;
	Rblib::CallbackWrapper<unsigned int, unsigned int &> GetDevCount;
	Rblib::CallbackWrapper<unsigned int, unsigned int, unsigned char *> GetDevId;
	Rblib::CallbackWrapper<unsigned int, unsigned int, int> StoreTempt;
protected:
	State _state;
	bool _learnPending;
	int _learnPendingChannel;
	bool _learnCancelPending;
	int _learnCancelPendingChannel;
	int _learnChannel;
	int _delayCounter;
	int _devCount;
	unsigned char _devices[MaxDevCnt][8];
	unsigned char _errorCount[ChannelCnt][MaxDevCnt];
	int _temperature[MaxDevCnt];
	int _curDev;
	int _devStorePos;
	int _curChannel;
public:
	MainStateMachine()
	{
		Reset();
	}
	
	void Reset()
	{
		_state = StateStoped;
		_learnPendingChannel = 0;
		_curChannel = 1;
		for (int i = 0; i < ChannelCnt; i++)
		for (int j = 0; j < MaxDevCnt; j++)
		{
			_errorCount[i][j] = 0;
		}
	}
	
	void Start()
	{
		if (_state != StateStoped)
		{
			return;
		}
		
		_state = StateClear;
	}
	
	void OnButtonClick(int channel)
	{
		_learnCancelPending = true;
		_learnCancelPendingChannel = channel;
	}
	
	void OnButtonLongClick(int channel)
	{
		_learnPending = true;
		_learnPendingChannel = channel;
	}
	
	void Tick(unsigned int tickFrequency)
	{
		switch(_state)
		{
		case StateClear:
			{
				LedOnOff(false, 0);
				BeepOnOff(false);
				_learnPending = false;
				_learnCancelPending = false;
				_curDev = 0;
				_state = StateIdle;
			}
			break;
		case StateIdle:
			{
				if (_learnPending)
				{
					_learnPending = false;
					_learnCancelPending = false;
					_learnChannel = _learnPendingChannel;
					_devStorePos = 0;
					_state = StateLearnStart;
					_curChannel = 1; // после обучения с первого канала начнем
					break;
				}
				
				if (_curChannel > ChannelCnt)
				{
					_curChannel = 1;
				}
				_state = StateNextChannel;
			}
			break;
		case StateLearnStart:
			{
				_state = StateLearnSignalEnable;
			}
			break;
		case StateLearnSignalEnable:
			{
				LedOnOff(true, _learnChannel);
				BeepOnOff(true);
				_state = StateLearnSignalDelay;
				_delayCounter = 0;
			}
			break;
		case StateLearnSignalDelay:
			{
				_delayCounter++;
				if (_delayCounter >= tickFrequency * 2)
				{
					BeepOnOff(false);
					_state = StateLearnDevCountRequest;
				}
			}
			break;
		case StateLearnDevCountRequest:
			{
				if (ProcessLearnCancelRequest())
				{
					break;
				}

				_devCount = 0;
				SelectChannel(_learnChannel);
				SearchRom();
				_state = StateLearnDevCountWaitResponse;
				_delayCounter = 0;
			}
			break;
		case StateLearnDevCountWaitResponse:
			{
				if (ProcessLearnCancelRequest())
				{
					break;
				}
				
				_delayCounter++;
				if (_delayCounter >= tickFrequency * 2)
				{
					_delayCounter = 0;
					_state = StateLearnPlayCount;
				}
			}
			break;
		case StateLearnPlayCount:
			{
				if (_devCount < 1)
				{
					_delayCounter = 0;
					BeepOnOff(true);
					_state = StateError;
				}
				else
				{
					if (_delayCounter > tickFrequency * _devCount)
					{
						BeepOnOff(false);
						_delayCounter = 0;
						_state = StateLearnMeasureTemperature;
					}
					else
					{
						_delayCounter++;
						if (_delayCounter % (tickFrequency) < tickFrequency / 2)
						{
							BeepOnOff(true);
						}
						else
						{
							BeepOnOff(false);
						}
					}
				}
			}
			break;
		case StateLearnMeasureTemperature:
			{
				MeasureTemperature();
				_delayCounter = 0;
				_state = StateLearnWaitMeasureTemperature;
			}
			break;
		case StateLearnWaitMeasureTemperature:
			{
				if (ProcessLearnCancelRequest())
				{
					break;
				}
				
				_delayCounter++;
				if (_delayCounter > tickFrequency * 1)
				{
					_curDev = 0;
					_state = StateLearnTemperatureRequest;
				}
			}
			break;
		case StateLearnTemperatureRequest:
			{
				if (ProcessLearnCancelRequest())
				{
					break;
				}
				
				if (_devCount < 1)
				{
					_state = StateLearnEnd;
					break;
				}
				
				_delayCounter = 0;
				_state = StateLearnTemperatureWaitResponse;
				ReadTemperature(_devices[_curDev]);
			}
			break;
		case StateLearnTemperatureWaitResponse:
			{
				_delayCounter++;
				if (_delayCounter > tickFrequency * 1)
				{
					LearnNextDev();
				}
			}
			break;
		case StateLearnStoreDevice:
			{
				if (_delayCounter > tickFrequency * _devStorePos / 2)
				{
					BeepOnOff(false);
					_delayCounter = 0;
					_state = StateLearnMeasureTemperature;
				}
				else
				{
					_delayCounter++;
					if (_delayCounter % (tickFrequency / 2) < tickFrequency / 4)
					{
						BeepOnOff(true);
					}
					else
					{
						BeepOnOff(false);
					}
				}
			}
			break;
		case StateLearnEnd:
			{
				LedOnOff(false, 0);
				_state = StateClear;
			}
			break;
		case StateNextChannel:
			{
				SelectChannel(_curChannel);
				unsigned int devCount = 0;
				GetDevCount(_curChannel, devCount);
				
				if (devCount < 1)
				{
					_curChannel++;
					_state = StateIdle;
				}
				else
				{
					_curDev = 0;
					_state = StateMeasureTemperature;
				}
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
					_curDev = 0;
					_state = StateTemperatureRequest;
				}
			}
			break;
		case StateTemperatureRequest:
			{
				unsigned char devId[8];
				GetDevId(_curChannel, _curDev + 1, devId);
				_delayCounter = 0;
				_state = StateTemperatureWaitResponse;
				ReadTemperature(devId);
			}
			break;
		case StateTemperatureWaitResponse:
			{
				_delayCounter++;
				if (_delayCounter > tickFrequency * 1)
				{
					if (_errorCount[_curChannel - 1][_curDev] < 10)
					{
						_errorCount[_curChannel - 1][_curDev]++;
					}
					else
					{
						StoreTempt(_curChannel, _curDev + 1, 0x7FFF);
					}
					NextDev();
				}
			}
			break;
		case StateError:
			{
				_delayCounter++;
				if (_delayCounter >= tickFrequency * 5)
				{
					_state = StateClear;
				}
			}
			break;
		}
	}
	
	void OnDeviceFound(unsigned char *deviceId)
	{
		if (_state != StateLearnDevCountWaitResponse)
		{
			return;
		}
		
		if (_devCount >= MaxDevCnt)
		{
			return;
		}
		
		for (int i = 0; i < 8; i++)
		{
			_devices[_devCount][i] = deviceId[i];
		}
		// если ни разу не измеряли, выставляем очень большую, чтобы отловить переход с низкой на высокую
		_temperature[_devCount] = 10000;
		_devCount++;
	}
	
	bool TemperatureValid(int t)
	{
		return t >= -55 && t <= +125;
	}
	
	void OnTemperatureReaded(int t, bool dataValid)
	{
		switch(_state)
		{
		case StateLearnTemperatureWaitResponse:
			{
				if (TemperatureValid(t))
				{
					// превышение порога с одновременным повышением температуры
					if (t >= LeartTemperatureTreshold && t > _temperature[_curDev])
					{
						DequeueDevice();
					}
					else
					{
						_temperature[_curDev] = t;
						LearnNextDev();
					}
				}
				else
				{
					LearnNextDev();
				}
			}
			break;
		case StateTemperatureWaitResponse:
			{
				_errorCount[_curChannel - 1][_curDev] = 0;
				if (dataValid && TemperatureValid(t))
				{
					StoreTempt(_curChannel, _curDev + 1, t);
				}
				NextDev();
			}
			break;
		}
	}
protected:
	bool ProcessLearnCancelRequest()
	{
		bool res = false;
		if (_learnCancelPending)
		{
			_learnCancelPending = false;
			if (_learnChannel == _learnCancelPendingChannel)
			{
				_state = StateLearnEnd;
				res = true;
			}
		}
		
		return res;
	}
	
	void LearnNextDev()
	{
		_curDev++;
		if (_curDev >= _devCount)
		{
			_curDev = 0;
			_state = StateLearnMeasureTemperature;
		}
		else
		{
			_state = StateLearnTemperatureRequest;
		}
	}
	
	void DequeueDevice()
	{
		if (_devCount < 1)
		{
			return;
		}
		
		StoreCureDev();
		
		for (int i = _curDev; i < _devCount - 1; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				_devices[i][j] = _devices[i + 1][j];
			}
			_temperature[i] = _temperature[i + 1];
		}
		
		_devCount--;
		
		_delayCounter = 0;
		_state = StateLearnStoreDevice;
	}
	
	void StoreCureDev()
	{
		if (_devStorePos < 1)
		{
			ClearDevList(_learnChannel);
		}
		_devStorePos++;
		StoreDevId(_learnChannel, _devStorePos, _devStorePos, _devices[_curDev]);
	}
	
	void NextDev()
	{
		unsigned int devCount = 0;
		GetDevCount(_curChannel, devCount);
		_curDev++;
		if (_curDev >= devCount)
		{
			_curChannel++;
			_state = StateIdle;
		}
		else
		{
			_state = StateTemperatureRequest;
		}
	}
};

#endif