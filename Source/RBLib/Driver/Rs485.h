#ifndef DRIVER_RS485_H
#define DRIVER_RS485_H

#include "../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Driver
{
	template
		<
			class InterfaceType
		>
	class Rs485
	{
	public:
		typedef InterfaceType Interface;
		static const unsigned int BufferSize = 256;
		
		enum State
		{
			StateNotInited,
			StateRead,
			StateWrite
		};
	public:
		CallbackWrapper<unsigned char *, int> OnReadComplete;
	protected:
		State _state;
		unsigned char _inputBuffer[BufferSize];
		unsigned int _inputCount;
		unsigned int _timeoutCounter;
		float _timoutTime; // время в сек
	public:
		Rs485()
		{
			_state = StateNotInited;
		}
		
		void Init(float timoutTime)
		{
			if (_state != StateNotInited)
			{
				return;
			}
			
			_timoutTime = timoutTime;
			Interface::OnWriteCompleteCallback.Set(OnWriteCompleteCallback, this);
			Interface::OnReadCallback.Set(OnReadCallback, this);
			
			SetState(StateRead);
		}
		
		void Tick(unsigned int tickFrequency)
		{
			switch(_state)
			{
			case StateRead:
				{
					unsigned int maxCount = tickFrequency * _timoutTime;
					if (_timeoutCounter >= maxCount)
					{
						if (_inputCount > 0)
						{
							OnReadComplete(_inputBuffer, _inputCount);
							_inputCount = 0;
						}
					}
					else
					{
						_timeoutCounter++;
					}
				}
				break;
			}
		}
		
		void Write(void *data, unsigned int len)
		{
			SetState(StateWrite);
			Interface::Write((unsigned char *)data, len);
		}
	protected:
		void SetState(State newState)
		{
			_state = newState;
			
			switch(_state)
			{
			case StateRead:
				{
					_inputCount = 0;
					Interface::WriteEnable(false);
				}
				break;
			case StateWrite:
				{
					Interface::WriteEnable(true);
				}
				break;
			}
		}
		
		void OnWriteComplete()
		{
			switch(_state)
			{
			case StateWrite:
				{
					SetState(StateRead);
				}
				break;
			}
		}
		
		static void OnWriteCompleteCallback(void *callbackParam)
		{
			Rs485 *im = (Rs485 *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnWriteComplete();
		}
		
		void OnRead(unsigned char data)
		{
			_timeoutCounter = 0;
			if (_inputCount < BufferSize)
			{
				_inputBuffer[_inputCount] = data;
				_inputCount++;
			}
		}
		
		static void OnReadCallback(void *callbackParam, unsigned char data)
		{
			Rs485 *im = (Rs485 *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnRead(data);
		}
	};
}
}

#endif