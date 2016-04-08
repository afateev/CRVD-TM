#ifndef DRIVER_DS2480B_SCENARIOMATCHROM_H
#define DRIVER_DS2480B_SCENARIOMATCHROM_H

#include "../Utils/CallbackWrapper.h"
#include "Commands.h"
#include "Scenario.h"

namespace Rblib
{
namespace Driver
{
namespace Ds2480bImplementation
{
	class ScenarioMatchRom : public Scenario
	{
	public:
		CallbackWrapper<unsigned char *, int> OnResultReaded;
	public:
		enum State
		{
			StateIdle,
			StateReset,
			StateWaitResetResponse,
			StateSendCmd,
			StateWaitCmdResponse,
			StateDone,
			StateError
		};
	protected:
		State _state;
		unsigned char _buffer[128];
		int _writeCount;
		unsigned char _cmdCode;
	public:
		ScenarioMatchRom()
		{
			
		}
		
		virtual void Reset()
		{
			Scenario::Reset();
			_state = StateIdle;
		}
		
		virtual void Start(unsigned char *romId, unsigned char cmd, unsigned char *data, unsigned char dataCount)
		{
			Scenario::Start();
			if (_state != StateIdle)
			{
				return;
			}
			
			_cmdCode = cmd;
			
			_writeCount = 0;
			_buffer[_writeCount] = CommandSetDataMode;
			_writeCount++;
			_buffer[_writeCount] = 0x55;	// Match ROM command
			_writeCount++;
			
			_writeCount += Stuff(romId, &_buffer[_writeCount], 8);
			_writeCount += Stuff(&_cmdCode, &_buffer[_writeCount], 1);
			_writeCount += Stuff(data, &_buffer[_writeCount], dataCount);
			
			_buffer[_writeCount] = CommandSetCommandMode;
			_writeCount++;
			
			_state = StateReset;
		}
		
		virtual void Tick(unsigned int tickFrequency)
		{
			Scenario::Tick(tickFrequency);
			
			switch(_state)
			{
			case StateReset:
				{
					_state = StateWaitResetResponse;
					WaitInput(1);
					// 0.5 сек таймаут ожидания ответа
					StartTimeout(tickFrequency * 0.5);
					
					CommandReset cmd;
					Write(&cmd, sizeof(cmd));
				}
				break;
			case StateWaitResetResponse:
				{
					if (InputDone())
					{
						CommandResetResponse response(_inputBuffer[0]);
						if (response.Valid())
						{
							if (response.Presence == CommandResetResponse::PresenceCodePresence)
							{
								_state = StateSendCmd;
							}
							else
							{
								_state = StateDone;
							}
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
			case StateSendCmd:
				{
					_state = StateWaitCmdResponse;
					WaitInput(_writeCount - 2);
					// 1 сек таймаут ожидания ответа
					StartTimeout(tickFrequency * 1);
					
					WaitWrite();
					Write((void *)_buffer, _writeCount);
				}
				break;
			case StateWaitCmdResponse:
				{
					if (WriteComplete() && InputDone())
					{
						bool result = true;
						
						for (int i = 1; i <= 9; i++)
						{
							result &= _inputBuffer[i] == _buffer[i + 1];
						}
						
						bool allFF = true;
						for (int i = 10; i < _inputCount; i++)
						{
							if (_inputBuffer[i] != 0xFF)
							{
								allFF = false;
								break;
							}
						}
						
						result &= !allFF;
						
						if (result)
						{
							OnResultReaded(&_inputBuffer[10], _inputCount - 10);
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