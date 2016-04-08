#ifndef DRIVER_ONEWIRE_SCENARIO_H
#define DRIVER_ONEWIRE_SCENARIO_H

#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Driver
{
namespace OneWireImplementation
{
	class Scenario
	{
	public:
		CallbackWrapper<> BusReset;
		CallbackWrapper<unsigned char> BusWrite;
		CallbackWrapper<> BusRead;
	protected:
		unsigned int _timoutCounter;
		unsigned char _outputBuffer[128];
		unsigned int _outputCount;
		unsigned int _outputPos;
		unsigned char _inputBuffer[128];
		unsigned int _inputCount;
		unsigned int _inputWaitCount;
		bool _write;
	public:
		virtual void Reset()
		{
			_timoutCounter = 0;
			_outputCount = 0;
			_outputPos = 0;
			_inputCount = 0;
			_inputWaitCount = 0;
			_write = false;
		}
		
		virtual void Start()
		{
		}
		
		virtual void Start(unsigned char cmd, unsigned char readCount)
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
		
		void Write()
		{
			_outputPos = 0;
			if (_outputPos >= _outputCount)
			{
				OnWriteComplete();
			}
			else
			{
				BusWrite(_outputBuffer[_outputPos]);
			}
		}
		
		void WaitWrite()
		{
			_write = true;
		}
		
		virtual void OnWriteComplete()
		{
			_write = false;
			
			if (_inputWaitCount > 0)
			{
				BusRead();
			}
		}
		
		virtual bool WriteComplete()
		{
			return !_write;
		}
		
		virtual void OnBusReadComplete(unsigned char data)
		{
			if (_inputWaitCount > 0)
			{
				_inputBuffer[_inputCount] = data;
				_inputCount++;
				_inputWaitCount--;
				if (_inputWaitCount > 0)
				{
					BusRead();
				}
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
		
		virtual void OnBusResetComplete(bool presence)
		{
			
		}
		
		virtual void OnBusWriteComplete()
		{
			_outputPos++;
			if (_outputPos >= _outputCount)
			{
				OnWriteComplete();
			}
			else
			{
				BusWrite(_outputBuffer[_outputPos]);
			}
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
		
		static unsigned int Copy(unsigned char *src, unsigned char *dst, unsigned int count)
		{
			if (dst == 0)
			{
				return 0;
			}
			
			for (unsigned int i = 0; i < count; i++)
			{
				dst[i] = src[i];
			}
			
			return count;
		}
	};
}
}
}

#endif