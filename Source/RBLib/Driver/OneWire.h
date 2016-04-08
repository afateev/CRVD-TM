#ifndef DRIVER_ONEWIRE_H
#define DRIVER_ONEWIRE_H

#include "../Utils/CallbackWrapper.h"
#include "OneWire/Scenario.h"
#include "OneWire/ScenarioSkipRom.h"

int getCrc(int oneByte, int crc){

int i = oneByte ^ crc;
crc = 0;
if((i & 0x01)>0) crc ^= 0x5e;
if((i & 0x02)>0) crc ^= 0xbc;
if((i & 0x04)>0) crc ^= 0x61;
if((i & 0x08)>0) crc ^= 0xc2;
if((i & 0x10)>0) crc ^= 0x9d;
if((i & 0x20)>0) crc ^= 0x23;
if((i & 0x40)>0) crc ^= 0x46;
if((i & 0x80)>0) crc ^= 0x8c;
return crc;

}

unsigned char CalcCrc(unsigned char *data, int count)
{
	unsigned char crc = 0;
	for(int i = 0; i <count; i++)
	{
		crc = getCrc(data[i], crc);
	}
	
	return crc;
}

namespace Rblib
{
namespace Driver
{
	template
		<
			class InterfaceType
		>
	class OneWireProtocol
	{
	public:
		typedef InterfaceType Interface;
		
		enum State
		{
			StateIdle,
			StateReset,
			StateWrite,
			StateRead,
		};
	public:	
		CallbackWrapper<bool> OnBusResetComplete;
		CallbackWrapper<> OnBusWriteComplete;
		CallbackWrapper<unsigned char> OnBusReadComplete;
	protected:
		State _state;
		int _counter;
		bool _resetPending;
		bool _resetTestResult;
		bool _resetPresenceResult;
		bool _writePending;
		unsigned char _writeValue;
		bool _readPending;
		unsigned char _readValue;
		int _bitCounter;
		bool _readBit1;
		bool _readBit2;
	public:
		OneWireProtocol()
		{
			_state = StateIdle;
			_resetPending = false;
			_writePending = false;
			_writeValue = 0;
		}
		
		void Tick(unsigned int tickFrequency)
		{
			switch(_state)
			{
			case StateIdle:
				{
					if (_resetPending)
					{
						_resetPending = false;
						_counter = 0;
						_state = StateReset;
						break;
					}
					
					if (_writePending)
					{
						_writePending = false;
						_counter = 0;
						_bitCounter = 0;
						_state = StateWrite;
						break;
					}
					
					if (_readPending)
					{
						_readPending = false;
						_counter = 0;
						_bitCounter = 0;
						_readValue = 0;
						_state = StateRead;
						break;
					}
				}
				break;
			case StateReset:
				{
					if (_counter == 0)
					{
						Interface::PullDown();
					}
					
					if (_counter == 51)
					{
						Interface::Release();
					}
					
					if (_counter == 52)
					{
						_resetTestResult = Interface::Read();
					}
					
					if (_counter == 58)
					{
						_resetPresenceResult = Interface::Read() == 0;
					}
					
					if (_counter == 110)
					{
						_state = StateIdle;
						OnBusResetComplete(_resetPresenceResult);
					}
					
					_counter++;
				}
				break;
			case StateWrite:
				{
					if (_counter == 0)
					{
						Interface::PullDown();
						bool bit = _writeValue >> _bitCounter & 1;
						if (bit == 1)
						{
							Interface::Release();
						}
						_bitCounter++;
					}
					
					
					if (_counter == 6)
					{
						Interface::Release();
					}
					
					if (_counter == 7)
					{
						_counter = -1;
						if (_bitCounter >= 8)
						{
							_state = StateIdle;
							OnBusWriteComplete();
						}
					}
					
					_counter++;
				}
				break;
			case StateRead:
				{
					if (_counter == 0)
					{
						Interface::PullDown();
						for (int i = 0; i < 10; i++)
						{
							Interface::Release();
						}
						
						_readBit1 = Interface::Read() == 1;
						_readBit2 = Interface::Read() == 1;
						if (_readBit1 && _readBit2)
						{
							_readValue |= (1 << _bitCounter);
						}
						_bitCounter++;
					}

					if (_counter == 7)
					{
						_counter = -1;
						if (_bitCounter >= 8)
						{
							_state = StateIdle;
							OnBusReadComplete(_readValue);
						}
					}
					
					_counter++;
				}
				break;
			
			}
			
			
		}
		
		void BusReset()
		{
			_resetPending = true;
		}
		
		static void BusReset(void *callbackParam)
		{
			OneWireProtocol *im = (OneWireProtocol *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->BusReset();
		}
		
		void BusWrite(unsigned char val)
		{
			_writeValue = val;
			_writePending = true;
		}
		
		static void BusWrite(void *callbackParam, unsigned char val)
		{
			OneWireProtocol *im = (OneWireProtocol *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->BusWrite(val);
		}
		
		void BusRead()
		{
			_readPending = true;
		}
		
		static void BusRead(void *callbackParam)
		{
			OneWireProtocol *im = (OneWireProtocol *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->BusRead();
		}
	};
	
	template
		<
			class OneWireProtocolType
		>
	class OneWire
	{
	public:
		enum State
		{
			StateNotInited,
			StateIdle,
			StateSkipRom,
		};
	protected:
		State _state;
		OneWireProtocolType *_oneWireProtocol;
		OneWireImplementation::ScenarioSkipRom _skipRom;
		
		OneWireImplementation::Scenario *_currentScenario;
		
		bool _detectPending;
		bool _detected;
		bool _searchRomPending;
		bool _skipRomPending;
		unsigned char _skipRomCmd;
		unsigned char _skipRomData[128];
		unsigned int _skipRomDataCount;
		bool _skipRomWrite;
		bool _matchRomPending;
		unsigned char _matchRomId[8];
		unsigned char _matchRomCmd;
		unsigned char _matchRomData[128];
		unsigned int _matchRomDataCount;
	public:
		CallbackWrapper<unsigned char *> OnDeviceFound;
		CallbackWrapper<unsigned char *, int> OnSkipRomResultReaded;
		CallbackWrapper<unsigned char *, int> OnMatchRomResultReaded;
	public:
		OneWire()
		{
			_oneWireProtocol = 0;
			_state = StateNotInited;
			_currentScenario = 0;
			_detectPending = false;
			_detected = false;
			_searchRomPending = false;
			_skipRomPending = false;
			_skipRomWrite = false;
			_matchRomPending = false;
		}
		
		void Init(OneWireProtocolType *oneWireProtocol)
		{
			if (0 == oneWireProtocol)
			{
				return;
			}
			
			if (_state != StateNotInited)
			{
				return;
			}
			
			_oneWireProtocol = oneWireProtocol;
			
			_oneWireProtocol->OnBusResetComplete.Set(OnBusResetCompleteCallback, this);
			_oneWireProtocol->OnBusWriteComplete.Set(OnBusWriteCompleteCallback, this);
			_oneWireProtocol->OnBusReadComplete.Set(OnBusReadCompleteCallback, this);
			
			_skipRom.BusReset.Set(OneWireProtocolType::BusReset, _oneWireProtocol);
			_skipRom.BusWrite.Set(OneWireProtocolType::BusWrite, _oneWireProtocol);
			_skipRom.BusRead.Set(OneWireProtocolType::BusRead, _oneWireProtocol);
			_skipRom.OnResultReaded.Set(OnSkipRomResultReadedCallback, this);
			
			SetState(StateIdle);
		}
		
		void Tick(unsigned int tickFrequency)
		{
			if (_currentScenario)
			{
				_currentScenario->Tick(tickFrequency);
			}
			
			switch(_state)
			{
			case StateIdle:
				{
					if (_skipRomPending)
					{
						SetState(StateSkipRom);
						_skipRomPending = false;
						_currentScenario = &_skipRom;
						if (_currentScenario)
						{
							_currentScenario->Reset();
							if (_skipRomWrite)
							{
								_currentScenario->Start(_skipRomCmd, _skipRomData, _skipRomDataCount);
							}
							else
							{
								_currentScenario->Start(_skipRomCmd, _skipRomDataCount);
							}
						}
						break;
					}
				}
				break;
			case StateSkipRom:
				{
					bool result = false;
					if (_currentScenario)
					{
						if (_currentScenario->Done(result))
						{
							SetState(StateIdle);
						}
					}
					else
					{
						SetState(StateIdle);
					}
				}
				break;
			}
		}
		
		void SkipRom(unsigned char cmd, unsigned char readCount)
		{
			_skipRomCmd = cmd;
			_skipRomWrite = false;
			_skipRomDataCount = readCount;
			
			if (_skipRomDataCount > sizeof(_skipRomData))
			{
				_skipRomDataCount = sizeof(_skipRomData);
			}
			
			_skipRomPending = true;
		}
		
		void SkipRom(unsigned char cmd, unsigned char *data, unsigned char dataCount)
		{
			_skipRomCmd = cmd;
			_skipRomWrite = true;
			
			_skipRomDataCount = dataCount;
			if (_skipRomDataCount > sizeof(_skipRomData))
			{
				_skipRomDataCount = sizeof(_skipRomData);
			}
			
			for (int i = 0; i < _skipRomDataCount; i++)
			{
				_skipRomData[i] = data[i];
			}
			
			_skipRomPending = true;
		}
	protected:
		void SetState(State newState)
		{
			_state = newState;
		}
		
		void Write(void *data, unsigned int len)
		{
			_oneWireProtocol->BusWrite((unsigned char *)data, len);
		}
		
		void OnBusResetComplete(bool presence)
		{
			if (_currentScenario)
			{
				_currentScenario->OnBusResetComplete(presence);
			}
		}
		
		static void OnBusResetCompleteCallback(void *callbackParam, bool presence)
		{
			OneWire *im = (OneWire *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnBusResetComplete(presence);
		}
		
		void OnBusWriteComplete()
		{
			if (_currentScenario)
			{
				_currentScenario->OnBusWriteComplete();
			}
		}
		
		static void OnBusWriteCompleteCallback(void *callbackParam)
		{
			OneWire *im = (OneWire *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnBusWriteComplete();
		}
		
		void OnBusReadComplete(unsigned char data)
		{
			if (_currentScenario)
			{
				_currentScenario->OnBusReadComplete(data);
			}
		}
		
		static void OnBusReadCompleteCallback(void *callbackParam, unsigned char data)
		{
			OneWire *im = (OneWire *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnBusReadComplete(data);
		}
		
		static void OnSkipRomResultReadedCallback(void *callbackParam, unsigned char *data, int count)
		{
			OneWire *im = (OneWire *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnSkipRomResultReaded(data, count);
		}
	};
}
}

#endif