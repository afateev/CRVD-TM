#ifndef DRIVER_DS2480B_SCENARIO_H
#define DRIVER_DS2480B_SCENARIO_H

#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Driver
{
namespace Ds2480bImplementation
{
	class Scenario
	{
	public:
		CallbackWrapper<void *, unsigned int> Write;
	protected:
		unsigned int _timoutCounter;
		unsigned char _inputBuffer[128];
		unsigned int _inputCount;
		unsigned int _inputWaitCount;
		bool _write;
	public:
		virtual void Reset()
		{
			_timoutCounter = 0;
			_inputCount = 0;
			_inputWaitCount = 0;
			_write = false;
		}
		
		virtual void Start()
		{
		}
		
		virtual void Start(unsigned char cmd, unsigned char *data, unsigned char dataCount)
		{
		}
		
		virtual void Start(unsigned char *romId, unsigned char cmd, unsigned char *data, unsigned char dataCount)
		{
		}
		
		virtual void Tick(unsigned int tickFrequency)
		{
			if (_timoutCounter > 0)
			{
				_timoutCounter--;
			}
		}
		
		void WaitWrite()
		{
			_write = true;
		}
		
		virtual void OnWriteComplete()
		{
			_write = false;
		}
		
		virtual bool WriteComplete()
		{
			return !_write;
		}
		
		virtual void OnRx(unsigned char data)
		{
			if (_inputWaitCount > 0)
			{
				_inputBuffer[_inputCount] = data;
				_inputCount++;
				_inputWaitCount--;
			}
		}
		
		virtual void StartTimeout(unsigned int tickCount)
		{
			_timoutCounter = tickCount;
		}
		
		virtual bool Timeout()
		{
			return _timoutCounter < 1;
		}
		
		virtual void WaitInput(unsigned int inputCount)
		{
			_inputCount = 0;
			_inputWaitCount = inputCount;
		}
		
		virtual bool InputDone()
		{
			return _inputWaitCount < 1;
		}
		
		virtual bool Done(bool &result)
		{
			result = false;
			return true;
		}
		
		static unsigned int Stuff(unsigned char *src, unsigned char *dst, unsigned int count)
		{
			if (dst == 0)
			{
				return 0;
			}
			
			unsigned int j = 0;
			for (unsigned int i = 0; i < count; i++)
			{
				dst[j] = src[i];
				j++;
				// спец код удваивается, чтобы он воспринимался как данные а не как команда
				if (src[i] == CommandSetCommandMode)
				{
					dst[j] = src[i];
					j++;
					
				}
			}
			
			return j;
		}
	};
}
}
}

#endif