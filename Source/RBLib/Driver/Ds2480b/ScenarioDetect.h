#ifndef DRIVER_DS2480B_SCENARIODETECT_H
#define DRIVER_DS2480B_SCENARIODETECT_H

#include "../../Utils/CallbackWrapper.h"
#include "Commands.h"
#include "Scenario.h"

namespace Rblib
{
namespace Driver
{
namespace Ds2480bImplementation
{
	class ScenarioDetect : public Scenario
	{
	public:
		CallbackWrapper<> SendBreak;
	public:
		enum State
		{
			StateIdle,
			StateSendBreak,
			StateDelay1,
			StateWriteTimingByte,
			StateDelay2,
			StateWriteConfig,
			StateWaitResponse,
			StateDone,
			StateError
		};
	protected:
		State _state;
	public:
		ScenarioDetect()
		{
			
		}
		
		virtual void Reset()
		{
			Scenario::Reset();
			_state = StateIdle;
		}
		
		virtual void Start()
		{
			Scenario::Start();
			if (_state != StateIdle)
			{
				return;
			}
			
			_state = StateSendBreak;
		}
		
		virtual void Tick(unsigned int tickFrequency)
		{
			Scenario::Tick(tickFrequency);
			
			switch(_state)
			{
			case StateSendBreak:
				{
					_state = StateDelay1;
					// 2 ms + 1ms на отправку BREAK
					StartTimeout(tickFrequency * 0.003);
					SendBreak();
				}
				break;
			case StateDelay1:
				{
					if (Timeout())
					{
						_state = StateWriteTimingByte;
					}
				}
				break;
			case StateWriteTimingByte:
				{
					_state = StateDelay2;
					// 2 ms + 1ms на запись 1 байта
					StartTimeout(tickFrequency * 0.003);
					unsigned char timingByte = 0xC1;
					Write(&timingByte, sizeof(timingByte));
				}
				break;
			case StateDelay2:
				{
					if (Timeout())
					{
						_state = StateWriteConfig;
					}
				}
				break;
			case StateWriteConfig:
				{
					_state = StateWaitResponse;
					WaitInput(5);
					// 0.5 сек таймаут ожидания ответа
					StartTimeout(tickFrequency * 0.5);
					
					static const unsigned char config[] = {0x17, 0x45, 0x5B, 0x0F, 0x91};
					Write((void *)&config, sizeof(config));
				}
				break;
			case StateWaitResponse:
				{
					if (InputDone())
					{
						bool result = true;
						result &= _inputBuffer[0] == 0x16;
						result &= _inputBuffer[1] == 0x44;
						result &= _inputBuffer[2] == 0x5A;
						result &= _inputBuffer[3] == 0x00;
						
						if (result)
						{
							_state = StateDone;
						}
						else
						{
							_state = StateError;
						}
					}
					else
					{
						if (Timeout())
						{
							_state = StateError;
						}
					}
				}
				break;
			}
		}
		
		virtual bool Done(bool &result)
		{
			result = _state == StateDone;
			return _state == StateDone || _state == StateError;
		}
	};
}
}
}

#endif