///////////////////////////////////////////////////////////////////////////////
//	Тест выходной мощности
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TESTS_TESTOUTPUTPOWER_H
#define MODEM_TESTS_TESTOUTPUTPOWER_H

#include "../../Utils/CallbackWrapper.h"
#include "Test.h"

namespace Rblib
{
namespace Modem
{
namespace Tests
{
	class TestOutputPower : public Test
	{
	public:
		enum CommandCode
		{
			CommandStop = 0,
			CommandStart = 1
		};
		
		typedef CallbackWrapper<> GoToPowerTestModeCallbackType;
		typedef CallbackWrapper<> GoToNormalModeCallbackType;
	public:
		GoToPowerTestModeCallbackType GoToPowerTestModeCallback;
		GoToNormalModeCallbackType GoToNormalModeCallback;
	protected:
		bool _run;
	public:
		TestOutputPower(int id): Test(id)
		{
			Reset();
		}
		
		virtual ~TestOutputPower()
		{
		}
		
		virtual TestTypeCode GetType()
		{
			return TestTypeCodeOutputPower;
		}
		
		virtual void Reset()
		{
			_run = false;
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
						GoToPowerTestModeCallback.Call();
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
	};
}
}
}

#endif