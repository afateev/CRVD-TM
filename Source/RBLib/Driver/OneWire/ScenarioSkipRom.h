#ifndef DRIVER_ONEWIRE_SCENARIOSKIPROM_H
#define DRIVER_ONEWIRE_SCENARIOSKIPROM_H

#include "../Utils/CallbackWrapper.h"
#include "Scenario.h"

namespace Rblib
{
namespace Driver
{
namespace OneWireImplementation
{
	class ScenarioSkipRom : public Scenario
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
			StateWaitRead,
			StateWaitWrite,
			StateDone,
			StateError
		};
	protected:
		State _state;
		int _readCount;
		unsigned char _cmdCode;
		bool _write;
	public:
		ScenarioSkipRom()
		{
			
		}
		
		virtual void Reset()
		{
			Scenario::Reset();
			_write = false;
			_state = StateIdle;
		}
		
		virtual void Start(unsigned char cmd, unsigned char readCount)
		{
			Scenario::Start(cmd, readCount);
			if (_state != StateIdle)
			{
				return;
			}
			
			_cmdCode = cmd;
			_write = false;
			_outputCount = 0;
			_outputBuffer[_outputCount] = 0xCC;	// Skip ROM command
			_outputCount++;
			_outputCount += Copy(&_cmdCode, &_outputBuffer[_outputCount], 1);
			_readCount = readCount;
			
			_state = StateReset;
		}
		
		virtual void Start(unsigned char cmd, unsigned char *data, unsigned char dataCount)
		{
			Scenario::Start(cmd, data, dataCount);
			if (_state != StateIdle)
			{
				return;
			}
			
			_cmdCode = cmd;
			_write = true;
			
			_outputCount = 0;
			_outputCount += Copy(&_cmdCode, &_outputBuffer[_outputCount], 1);
			_outputCount += Copy(data, &_outputBuffer[_outputCount], dataCount);
			
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
					
					BusReset();
				}
				break;
			case StateWaitResetResponse:
				{
					if (Timeout())
					{
						_state = StateError;
					}
				}
				break;
			case StateSendCmd:
				{
					if (_write)
					{
						_state = StateWaitWrite;
					}
					else
					{
						_state = StateWaitRead;
						WaitInput(_readCount);
						// 1 сек таймаут ожидания ответа
						StartTimeout(tickFrequency * 1.5);
						
						WaitWrite();
						Write();
					}
				}
				break;
			case StateWaitRead:
				{
					if (WriteComplete() && InputDone())
					{
						OnResultReaded(_inputBuffer, _inputCount);
						_state = StateDone;
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
		
		virtual void OnBusResetComplete(bool presence)
		{
			Scenario::OnBusResetComplete(presence);
			
			if (presence)
			{
				_state = StateSendCmd;
			}
			else
			{
				_state = StateDone;
			}
		}
	};
}
}
}

#endif