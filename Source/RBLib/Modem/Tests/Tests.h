///////////////////////////////////////////////////////////////////////////////
//	Класс тестов
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TESTS_TESTS_H
#define MODEM_TESTS_TESTS_H

#include "../../Utils/CallbackWrapper.h"
#include "../../Utils/vector_static.h"
#include "TestTypeCode.h"
#include "TestCommandResultCode.h"
#include "Test.h"
#include "TestSensitivity.h"
#include "TestLink.h"
#include "TestOutputPower.h"
#include "TestDataDelivery.h"

namespace Rblib
{
namespace Modem
{
namespace Tests
{
	class Tests
	{
	public:
		typedef Test *TestListItemType;
		static const int MaxTestCount = 10;
		typedef vector_static<TestListItemType, MaxTestCount + 1> TestListType;
		typedef CallbackWrapper<unsigned char, int, unsigned char *, int> OnTestDataCallbackType;
		typedef CallbackWrapper<int, unsigned char, int, unsigned char *, int> OnSendTestDataToNodeCallbackType;
		typedef CallbackWrapper<> GoToListeningModeCallbackType;
		typedef CallbackWrapper<> GoToPowerTestModeCallbackType;
		typedef CallbackWrapper<> GoToNormalModeCallbackType;
		typedef CallbackWrapper<int, unsigned char *, unsigned int, unsigned int, int &, bool &> EnqueuePacketCallbackType;
	public:
		OnTestDataCallbackType OnTestDataCallback;
		OnSendTestDataToNodeCallbackType OnSendTestDataToNodeCallback;
		GoToListeningModeCallbackType GoToListeningModeCallback;
		GoToPowerTestModeCallbackType GoToPowerTestModeCallback;
		GoToNormalModeCallbackType GoToNormalModeCallback;
		EnqueuePacketCallbackType EnqueuePacketCallback;
	protected:
		TestListType _testList;
		int _popDataNextItem;
	public:
		Tests()
		{
			Reset();
		}
		
		void Reset()
		{
			for (int i = 0; i < _testList.size(); i++)
			{
				if (_testList[i])
				{
					_testList[i]->Reset();
					delete _testList[i];
				}
			}
			
			_testList.clear();
			_popDataNextItem = 0;
		}
		
		TestCommandResultCode TestCommand(unsigned char testType, int testId, unsigned char *testData, int testDataCount)
		{
			TestCommandResultCode resultCode = TestCommandResultCodeUnknown;
			Test *found = 0;
			
			for (int i = 0; i < _testList.size(); i++)
			{
				if (_testList[i])
				{
					if (_testList[i]->TypeAndIdValid((TestTypeCode)testType, testId))
					{
						found = _testList[i];
						break;
					}
				}
			}
			
			if (!found)
			{
				found = CreateTest((TestTypeCode)testType, testId, resultCode);
			}
			
			if (found)
			{
				resultCode = found->TestCommand(testData, testDataCount);
			}
			
			return resultCode;
		}
		
		int PopData(unsigned char *&bufferPtr)
		{
			for (int i = 0; i < _testList.size(); i++)
			{
				if (_testList[i]->Complete())
				{
					_testList[i]->Reset();
					delete _testList[i];
					_testList.erase(&_testList[i]);
					i = 0;
				}
			}
			
			if (_testList.size() < 1)
			{
				return 0;
			}
			
			if (_popDataNextItem >= _testList.size())
			{
				_popDataNextItem = 0;
			}
			
			int res = 0;
			
			if (_testList[_popDataNextItem])
			{
				res = _testList[_popDataNextItem]->PopData(bufferPtr);
			}
			
			_popDataNextItem++;
			
			return res;
		}
		
		// Обработчик входящих данных
		void OnRxData(int selfAddress, int sourceAddress, unsigned char *data, int dataCount, int rssi)
		{
			if (data == 0 || dataCount < sizeof(Protocol::TestPacketDataFormat))
			{
				return;
			}
			
			bool processed = false;
			
			for (int i = 0; i < _testList.size(); i++)
			{
				if (_testList[i])
				{
					processed |= _testList[i]->OnRxData(selfAddress, sourceAddress, data, dataCount, rssi);
				}
			}
			
			if (!processed)
			{
				Test *newTest = CreateTestOnRxData(selfAddress, sourceAddress, data, dataCount);
				if (newTest)
				{
					newTest->OnRxData(selfAddress, sourceAddress, data, dataCount, rssi);
				}
			}
		}
		
		// обработчик результата доставки данных
		void OnDeliveryComplete(int packetId, int node, bool result)
		{
			for (int i = 0; i < _testList.size(); i++)
			{
				if (_testList[i])
				{
					_testList[i]->OnDeliveryComplete(packetId, node, result);
				}
			}
		}
	protected:
		TestListItemType CreateTest(TestTypeCode type, int id, TestCommandResultCode &resultCode)
		{
			resultCode = TestCommandResultCodeUnknown;
			
			if (_testList.size() >= MaxTestCount)
			{
				resultCode = TestCommandResultCodeMaxTestsCountExceeded;
				return 0;
			}
			
			TestListItemType item = 0;
			
			switch (type)
			{
			case TestTypeCodeSensitivity:
				{
					TestSensitivity *test = new(nothrow) TestSensitivity(id);
					test->GoToListeningModeCallback.Set(GoToListeningMode, this);
					test->GoToNormalModeCallback.Set(GoToNormalMode, this);
					item = test;
				}
				break;
			case TestTypeCodeLink:
				{
					item = new(nothrow) TestLinkMaster(id);
				}
				break;
			case TestTypeCodeOutputPower:
				{
					TestOutputPower *test = new(nothrow) TestOutputPower(id);
					test->GoToPowerTestModeCallback.Set(GoToPowerTestMode, this);
					test->GoToNormalModeCallback.Set(GoToNormalMode, this);
					item = test;
				}
				break;
			case TestTypeCodeDataDelivery:
				{
					TestDataDelivery *test = new(nothrow) TestDataDelivery(id);
					test->EnqueuePacketCallback.Set(EnqueuePacket, this);
					item = test;
				}
				break;
			default:
				resultCode = TestCommandResultCodeInvalidTestType;
				break;
			}
			
			if (!item)
			{
				resultCode = TestCommandResultCodeNoMoreMemory;
			}
			else
			{
				item->TestDataOutput.Set(TestDataOutputCallback, this);
				item->SendTestDataToNodeCallabck.Set(SendTestDataToNodeCallabck, this);
				_testList.push_back(item);
				resultCode = TestCommandResultCodeNoError;
			}
			
			return item;
		}
		
		TestListItemType CreateTestOnRxData(int selfAddress, int sourceAddress, unsigned char *data, int dataCount)
		{
			if (data == 0 || dataCount < sizeof(Protocol::TestPacketDataFormat))
			{
				return 0;
			}
			
			Protocol::TestPacketDataFormat *testData = (Protocol::TestPacketDataFormat *)data;
			TestTypeCode testType = (TestTypeCode)testData->TestType;
			int testId = testData->TestId;
			
			TestListItemType item = 0;
			
			switch(testType)
			{
			case TestTypeCodeLink:
				{
					if (TestLinkSlave::MustCreate(selfAddress, sourceAddress, data, dataCount))
					{
						item = new(nothrow) TestLinkSlave(testId);
					}
				}
				break;
			}
			
			if (item)
			{
				item->TestDataOutput.Set(TestDataOutputCallback, this);
				item->SendTestDataToNodeCallabck.Set(SendTestDataToNodeCallabck, this);
				_testList.push_back(item);
			}
			
			return item;
		}
		
		static void TestDataOutputCallback(void *callbackParam, unsigned char testType, int testId, unsigned char *data, int dataCount)
		{
			Tests *im = (Tests *)callbackParam;
			
			if (!im)
			{
				return;
			}
			
			im->OnTestDataCallback.Call(testType, testId, data, dataCount);
		}
		
		static void SendTestDataToNodeCallabck(void *callbackParam, int nodeId, unsigned char testType, int testId, unsigned char *data, int dataCount)
		{
			Tests *im = (Tests *)callbackParam;
			
			if (!im)
			{
				return;
			}
			
			im->OnSendTestDataToNodeCallback.Call(nodeId, testType, testId, data, dataCount);
		}
		
		static void GoToListeningMode(void *callbackParam)
		{
			Tests *im = (Tests *)callbackParam;
			
			if (!im)
			{
				return;
			}
			
			im->GoToListeningModeCallback.Call();
		}
		
		static void GoToPowerTestMode(void *callbackParam)
		{
			Tests *im = (Tests *)callbackParam;
			
			if (!im)
			{
				return;
			}
			
			im->GoToPowerTestModeCallback.Call();
		}
		
		static void GoToNormalMode(void *callbackParam)
		{
			Tests *im = (Tests *)callbackParam;
			
			if (!im)
			{
				return;
			}
			
			im->GoToNormalModeCallback.Call();
		}
		
		static void EnqueuePacket(void *callbackParam, int dst, unsigned char *data, unsigned int dataCount, unsigned int deliveryTimeout, int &packetId, bool &result)
		{
			Tests *im = (Tests *)callbackParam;
			
			if (!im)
			{
				return;
			}
			
			im->EnqueuePacketCallback.Call(dst, data, dataCount, deliveryTimeout, packetId, result);
		}
	};
}
}
}

#endif