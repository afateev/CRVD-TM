///////////////////////////////////////////////////////////////////////////////
//	Базовый класс тестов
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TESTS_TEST_H
#define MODEM_TESTS_TEST_H

#include "../../Utils/CallbackWrapper.h"
#include "TestTypeCode.h"

namespace Rblib
{
namespace Modem
{
namespace Tests
{
	class Test
	{
	public:
		typedef CallbackWrapper<unsigned char, int, unsigned char *, int> TestDataOutputType;
		typedef CallbackWrapper<int, unsigned char, int, unsigned char *, int> SendTestDataToNodeCallabckType;
	public:
		TestDataOutputType TestDataOutput;
		SendTestDataToNodeCallabckType SendTestDataToNodeCallabck;
	protected:
		int _id;
	public:
		Test(int id)
		{
			_id = id;
			Reset();
		}
		
		virtual ~Test()
		{
		}
		
		virtual TestTypeCode GetType()
		{
			return TestTypeCodeUnknown;
		}
		
		virtual int GetId()
		{
			return _id;
		}
		
		virtual bool TypeAndIdValid(TestTypeCode testType, int testId)
		{
			return GetType() == testType && GetId() == testId;
		}
		
		virtual void Reset()
		{

		}
		
		// тест завершен, можно уничтожать
		virtual bool Complete()
		{
			return true;
		}
		
		virtual TestCommandResultCode TestCommand(unsigned char *testData, int testDataCount)
		{
			return TestCommandResultCodeUnknown;
		}
		
		virtual void PushTestData(void *data, int dataCount)
		{
			TestDataOutput.Call(GetType(), GetId(), (unsigned char *)data, dataCount);
		}
		
		virtual void SendTestDataToNode(int nodeId, void *data, int dataCount)
		{
			SendTestDataToNodeCallabck.Call(nodeId, GetType(), GetId(), (unsigned char *)data, dataCount);
		}
		
		virtual int PopData(unsigned char *&bufferPtr)
		{
			return 0;
		}
		
		virtual bool OnRxData(int selfAddress, int sourceAddress, unsigned char *data, int dataCount, int rssi)
		{
			return false;
		}
		
		virtual void OnDeliveryComplete(int packetId, int node, bool result)
		{
		}
	};
}
}
}

#endif