///////////////////////////////////////////////////////////////////////////////
//	Тест чувствительности
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TESTS_TESTSENSITIVITY_H
#define MODEM_TESTS_TESTSENSITIVITY_H

#include "../../Utils/CallbackWrapper.h"
#include "Test.h"
#include "../Protocol/PacketFormat.h"

namespace Rblib
{
namespace Modem
{
namespace Tests
{
	class TestSensitivity : public Test
	{
	public:
		enum CommandCode
		{
			CommandStop = 0,
			CommandStart = 1
		};
		
		enum State
		{
			StateWaitStart,
			StateWaitStop,
		};
		
		typedef CallbackWrapper<> GoToListeningModeCallbackType;
		typedef CallbackWrapper<> GoToNormalModeCallbackType;

#pragma pack(push, 1)		
		struct OutputDataFormat
		{
			unsigned char AttenuatorStep;
			float PowerLevel;
			unsigned int NumberOfRepeats;
			unsigned int ReceivedCount;
			int RssiMin;
			int RssiAvg;
			int RssiMax;
			
			OutputDataFormat()
			{
				AttenuatorStep = 0;
				PowerLevel = 0;
				NumberOfRepeats = 0;
				ReceivedCount = 0;
				RssiMin = - 127;
				RssiAvg = - 127;
				RssiMax = - 127;
			}
		};
#pragma pack (pop)
	public:
		GoToListeningModeCallbackType GoToListeningModeCallback;
		GoToNormalModeCallbackType GoToNormalModeCallback;
	protected:
		bool _run;
		State _state;
		int _testCounter;
		float _rssiMin;
		float _rssiAvg;
		float _rssiMax;
	public:
		TestSensitivity(int id): Test(id)
		{
			Reset();
		}
		
		virtual ~TestSensitivity()
		{
		}
		
		virtual TestTypeCode GetType()
		{
			return TestTypeCodeSensitivity;
		}
		
		virtual void Reset()
		{
			_run = false;
			_state = StateWaitStart;
			_testCounter = 0;
			_rssiMin = -127.0;
			_rssiAvg = -127.0;
			_rssiMax = -127.0;
		}
		
		// тест завершен, можно уничтожать
		virtual bool Complete()
		{
			// уничтожается только при перезагрузке устройства
			return false;
		}
		
		virtual TestCommandResultCode TestCommand(unsigned char *testData, int testDataCount)
		{
			TestCommandResultCode res =  TestCommandResultCodeUnknown;
			
			if (testData == 0 || testDataCount < 1)
			{
				return TestCommandResultCodeInvalidArgument;
			}
			
			CommandCode command = (CommandCode)testData[0];
			
			switch (command)
			{
			case CommandStart:
				{
					if (!_run)
					{
						GoToListeningModeCallback.Call();
						_state = StateWaitStart;
						_run = true;
					}
					res =  TestCommandResultCodeNoError;
				}
				break;
			case CommandStop:
				{
					if (_run)
					{
						GoToNormalModeCallback.Call();
						_run = false;
					}
					res =  TestCommandResultCodeNoError;
				}
				break;
			default:
				{
					res =  TestCommandResultCodeInvalidArgument;
				}
				break;
			}
			
			return res;
		}
		
		virtual int PopData(unsigned char *&bufferPtr)
		{
			return 0;
		}
		
		virtual bool OnRxData(int selfAddress, int sourceAddress, unsigned char *data, int dataCount, int rssi)
		{
			if (!_run)
			{
				return false;
			}
			
			if (data == 0 || dataCount < sizeof(Protocol::TestPacketDataFormat))
			{
				return false;
			}
			
			Protocol::TestPacketDataFormat *testData = (Protocol::TestPacketDataFormat *)data;
			
			if (testData->TestType != GetType())
			{
				return false;
			}
			
			if (data == 0 || dataCount < sizeof(Protocol::TestPacketSensitivityDataFormat))
			{
				return false;
			}
			
			Protocol::TestPacketSensitivityDataFormat *testSensytivityData = (Protocol::TestPacketSensitivityDataFormat *)data;
			
			switch(_state)
			{
			case StateWaitStart:
				{
					if (testSensytivityData->PacketType == Protocol::TestPacketSensitivityDataFormat::PacketTypeStart)
					{
						_testCounter = 0;
						_state = StateWaitStop;
					}
				}
				break;
			case StateWaitStop:
				{
					if (testSensytivityData->PacketType == Protocol::TestPacketSensitivityDataFormat::PacketTypeStart)
					{
						_testCounter = 0;
					}
					
					if (testSensytivityData->PacketType == Protocol::TestPacketSensitivityDataFormat::PacketTypeWork)
					{
						if (_testCounter == 0)
						{
							_rssiMin = rssi;
							_rssiAvg = rssi;
							_rssiMax = rssi;
						}
						else
						{
							if (rssi < _rssiMin)
							{
								_rssiMin = rssi;
							}
							
							if (rssi > _rssiMax)
							{
								_rssiMax = rssi;
							}
							
							_rssiAvg += rssi;
							_rssiAvg /= 2.0;
						}
						
						_testCounter++;
						
						if (_testCounter > testSensytivityData->NumberOfRepeats)
						{
							_state = StateWaitStart;
						}
					}
					
					if (testSensytivityData->PacketType == Protocol::TestPacketSensitivityDataFormat::PacketTypeStop)
					{
						OutputDataFormat outputData;
						outputData.AttenuatorStep = testSensytivityData->AttenuatorStep;
						outputData.PowerLevel = testSensytivityData->PowerLevel;
						outputData.NumberOfRepeats = testSensytivityData->NumberOfRepeats;
						outputData.ReceivedCount = _testCounter;
						outputData.RssiMin = (int)_rssiMin;
						outputData.RssiAvg = (int)_rssiAvg;
						outputData.RssiMax = (int)_rssiMax;
						
						PushTestData(&outputData, sizeof(outputData));
						
						_state = StateWaitStart;
					}
				}
				break;
			}
			
			return true;
		}
	};
}
}
}

#endif