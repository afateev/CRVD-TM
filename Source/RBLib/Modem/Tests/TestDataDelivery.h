///////////////////////////////////////////////////////////////////////////////
//	Тест доставки данных
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TESTS_TESTDATADELIVERY_H
#define MODEM_TESTS_TESTDATADELIVERY_H

#include "../../Utils/CallbackWrapper.h"
#include "../../Utils/vector_static.h"
#include "Test.h"

namespace Rblib
{
namespace Modem
{
namespace Tests
{
	class TestDataDelivery : public Test
	{
	public:
		enum CommandCode
		{
			CommandStop = 0,
			CommandStart = 1
		};
		
		static const int MaxParallelPacketsCount = 16;
		static const int MaxPacketSize = 1024;
		typedef Rblib::vector_static<int, MaxParallelPacketsCount + 1> WaitDeliveryListType;
		
		typedef CallbackWrapper<int, unsigned char *, unsigned int, unsigned int, int &, bool &> EnqueuePacketCallbackType;
	public:
		EnqueuePacketCallbackType EnqueuePacketCallback;
	protected:
		bool _run;
		WaitDeliveryListType _waitDeliveryList;
		unsigned int _counter;
		int _dstAddress;
		unsigned short _packetSize;
		int _threadsCount;
		int _deliveryTimeout;
	public:
		TestDataDelivery(int id): Test(id)
		{
			Reset();
		}
		
		virtual ~TestDataDelivery()
		{
		}
		
		virtual TestTypeCode GetType()
		{
			return TestTypeCodeDataDelivery;
		}
		
		virtual void Reset()
		{
			_run = false;
			_waitDeliveryList.clear();
			_counter = 0;
			_dstAddress = -1;
			_packetSize = 0;
			_threadsCount = 0;
			_deliveryTimeout = 0;
		}
		
		// тест завершен, можно уничтожать
		virtual bool Complete()
		{
			return !_run;
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
						if (testDataCount < 6)
						{
							res =  TestCommandResultCodeInvalidArgument;
							break;
						}
						{
							_dstAddress = testData[1];
							_packetSize = testData[3];
							_packetSize <<= 8;
							_packetSize |= testData[2];
							_threadsCount = testData[4];
							_deliveryTimeout = testData[5];
							
							if (_packetSize < 1 || _packetSize > MaxPacketSize)
							{
								res =  TestCommandResultCodeInvalidArgument;
								break;
							}
							
							if (_threadsCount < 1 || _threadsCount > MaxParallelPacketsCount)
							{
								res =  TestCommandResultCodeInvalidArgument;
								break;
							}
							
							if (_deliveryTimeout < 1)
							{
								res =  TestCommandResultCodeInvalidArgument;
								break;
							}
							
							_run = true;
						}
					}
					res =  TestCommandResultCodeNoError;
				}
				break;
			case CommandStop:
				{
					if (_run)
					{
						Reset();
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
			if (!_run)
			{
				return false;
			}
			
			if (_waitDeliveryList.size() < _threadsCount)
			{
				SendPacket();
			}
			
			return 0;
		}
		
		virtual bool OnRxData(int selfAddress, int sourceAddress, unsigned char *data, int dataCount, int rssi)
		{
			return false;
		}
		
		virtual void OnDeliveryComplete(int packetId, int node, bool result)
		{
			if (!_run)
			{
				return;
			}
			
			for (int i = 0; i < _waitDeliveryList.size(); i++)
			{
				if (_waitDeliveryList[i] == packetId)
				{
					_waitDeliveryList.erase(&_waitDeliveryList[i]);
					i = 0;
				}
			}
		}
	protected:
		void SendPacket()
		{
			unsigned char test[] = {0xFF, 0xFF, 0xFF, 0xFF, _counter & 0xFF, (_counter >> 8) & 0xFF, (_counter >> 16) & 0xFF, (_counter >> 24) & 0xFF};
			int packetId = -1;
			bool res = false;
			EnqueuePacketCallback.Call(_dstAddress, test, _packetSize, _deliveryTimeout, packetId, res);
			
			if (res)
			{
				_counter++;
				_waitDeliveryList.push_back(packetId);
			}
		}
	};
}
}
}

#endif