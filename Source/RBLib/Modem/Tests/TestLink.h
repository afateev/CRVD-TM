///////////////////////////////////////////////////////////////////////////////
//	Тест связи
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TESTS_TESTLINK_H
#define MODEM_TESTS_TESTLINK_H

#include "../../Utils/CallbackWrapper.h"
#include "Test.h"

namespace Rblib
{
namespace Modem
{
namespace Tests
{
namespace TestLink
{
	enum ResultCode
	{
		ResultCodeUnknown					= 0,
		ResultCodeDone						= 1,
		ResultCodeReceiverNotRespond		= 2,
		ResultCodeInvalidResultReceived		= 3,
	};
	
#pragma pack(push, 1)
	struct TestCommandStruct
	{
		unsigned char ReceiverId;
		unsigned short TransmitCount;
		unsigned short PayloadCount;
		unsigned char DeliveryResultRequest;
		unsigned char DeliveryResultAddress;
	};
	
	enum PacketType
	{
		PacketTypeUnknown		= 0,
		PacketTypeStart			= 1,
		PacketTypeStartAck		= 2,
		PacketTypeTest			= 3,
		PacketTypeGetResult		= 4,
		PacketTypeResult		= 5,
	};
	
	struct PacketStructHeader
	{
		TestTypeCode TestType;
		int TestId;
		unsigned char Dst;
		unsigned char PacketType;
	public:
		PacketStructHeader(int testId, unsigned char dst, unsigned char packetType)
		{
			TestType = TestTypeCodeLink;
			TestId = testId;
			Dst = dst;
			PacketType = packetType;
		}
	};
	
	struct PacketStructStart
	{
		PacketStructHeader Header;
	public:
		PacketStructStart(int testId, unsigned char dst) : Header(testId, dst, PacketTypeStart)
		{
		}
	};
	
	struct PacketStructStartAck
	{
		PacketStructHeader Header;
	public:
		PacketStructStartAck(int testId, unsigned char dst) : Header(testId, dst, PacketTypeStartAck)
		{
		}
	};
	
	struct PacketStructTest
	{
		PacketStructHeader Header;
	public:
		PacketStructTest(int testId, unsigned char dst) : Header(testId, dst, PacketTypeTest)
		{
		}
	};
	
	struct PacketStructGetResult
	{
		PacketStructHeader Header;
	public:
		PacketStructGetResult(int testId, unsigned char dst) : Header(testId, dst, PacketTypeGetResult)
		{
		}
	};
	
	struct ResultStruct
	{
		unsigned short ReceivedCount;
		int RssiMin;
		int RssiAvg;
		int RssiMax;
	public:
		ResultStruct()
		{
			ReceivedCount = 0;
			RssiMin = -127;
			RssiAvg = -127;
			RssiMax = -127;
		}
	};
	
	struct PacketStructResult
	{
		PacketStructHeader Header;
		ResultStruct Result;
	public:
		PacketStructResult(int testId, unsigned char dst) : Header(testId, dst, PacketTypeResult)
		{
		}
	};
	
	struct OutputDataFormat
	{
		unsigned char ResultCode;
		unsigned char ReceiverId;
		unsigned short TranmitCount;
		unsigned short ReceivedCount;
		int RssiMin;
		int RssiAvg;
		int RssiMax;
			
		OutputDataFormat()
		{
			ResultCode = 0;
			ReceiverId = -1;
			TranmitCount = 0;
			ReceivedCount = 0;
			RssiMin = - 127;
			RssiAvg = - 127;
			RssiMax = - 127;
		}
	};
#pragma pack(pop)		
}
	
	class TestLinkMaster : public Test
	{
	public:
		enum State
		{
			StateStart,
			StateSendStart,
			StateWaitStartAck,
			StateSendTestPacket,
			StateGetResult,
			StateWaitResult,
			StateComplete,
			StateDone
		};
		
		static const int OutputBufferSize = 64;
		static const int WaitMaxCount = 5;
		static const int RetransmitMaxCount = 10;
	protected:
		unsigned char _receiverId;
		unsigned short _transmitCount;
		unsigned short _payloadCount;
		unsigned char _deliveryResultRequest;
		unsigned char _deliveryResultAddress;
		State _state;
		int _waitCounter;
		int _retransmitCounter;
		int _transmitCounter;
		TestLink::ResultCode _resultCode;
		TestLink::ResultStruct _result;
		unsigned char _outputBuffer[OutputBufferSize];
	public:
		TestLinkMaster(int id): Test(id)
		{
			Reset();
		}
		
		virtual ~TestLinkMaster()
		{
		}
		
		virtual TestTypeCode GetType()
		{
			return TestTypeCodeLink;
		}
		
		virtual void Reset()
		{
			_resultCode = TestLink::ResultCodeUnknown;
			_waitCounter = 0;
			_transmitCounter = 0;
			_state = StateStart;
		}
		
		// тест завершен, можно уничтожать
		virtual bool Complete()
		{
			return _state == StateDone;
		}
		
		virtual TestCommandResultCode TestCommand(unsigned char *testData, int testDataCount)
		{
			TestCommandResultCode res =  TestCommandResultCodeUnknown;
			
			if (testData == 0 || testDataCount < sizeof(TestLink::TestCommandStruct))
			{
				return TestCommandResultCodeInvalidArgument;
			}
			
			TestLink::TestCommandStruct *commandData = (TestLink::TestCommandStruct *)testData;
			_receiverId = commandData->ReceiverId;
			_transmitCount = commandData->TransmitCount;
			_payloadCount = commandData->PayloadCount;
			_deliveryResultRequest = commandData->DeliveryResultRequest;
			_deliveryResultAddress = commandData->DeliveryResultAddress;
			
			_state = StateStart;
			res =  TestCommandResultCodeNoError;
			
			return res;
		}
		
		virtual int PopData(unsigned char *&bufferPtr)
		{
			int len = 0;
			
			switch(_state)
			{
			case StateStart:
				{
					_retransmitCounter = 0;
					_transmitCounter = 0;
					_state = StateSendStart;
				}
				break;
			case StateSendStart:
				{
					TestLink::PacketStructStart pkt(GetId(), _receiverId);
					len = sizeof(pkt);
					memcpy(_outputBuffer, &pkt, len);
					_waitCounter = 0;
					_state = StateWaitStartAck;
				}
				break;
			case StateWaitStartAck:
				{
					if (_waitCounter < WaitMaxCount)
					{
						_waitCounter++;
					}
					else
					{
						_waitCounter = 0;
						if (_retransmitCounter < RetransmitMaxCount)
						{
							_retransmitCounter++;
							_state = StateSendStart;
						}
						else
						{
							_resultCode = TestLink::ResultCodeReceiverNotRespond;
							_state = StateComplete;
						}
					}
				}
				break;
			case StateSendTestPacket:
				{
					if (_transmitCounter < _transmitCount)
					{
						_transmitCounter++;
						TestLink::PacketStructTest pkt(GetId(), _receiverId);
						// добавляем мусор, для добора длины пакета, которую попросил пользователь
						len = sizeof(pkt) + _payloadCount;
						// но не более длины выходного буфера
						if (len > OutputBufferSize)
						{
							len = OutputBufferSize;
						}
						memcpy(_outputBuffer, &pkt, len);
					}
					else
					{
						_waitCounter = 0;
						_retransmitCounter = 0;
						_state = StateGetResult;
					}
				}
				break;
			case StateGetResult:
				{
					TestLink::PacketStructGetResult pkt(GetId(), _receiverId);
					len = sizeof(pkt);
					memcpy(_outputBuffer, &pkt, len);
					_waitCounter = 0;
					_state = StateWaitResult;
				}
				break;
			case StateWaitResult:
				{
					if (_waitCounter < WaitMaxCount)
					{
						_waitCounter++;
					}
					else
					{
						_waitCounter = 0;
						if (_retransmitCounter < RetransmitMaxCount)
						{
							_retransmitCounter++;
							_state = StateGetResult;
						}
						else
						{
							_resultCode = TestLink::ResultCodeReceiverNotRespond;
							_state = StateComplete;
						}
					}
				}
				break;
			case StateComplete:
				{
					TestLink::OutputDataFormat outputData;
					outputData.ResultCode = _resultCode;
					outputData.ReceiverId = _receiverId;
					outputData.TranmitCount = _transmitCounter;
					outputData.ReceivedCount = _result.ReceivedCount;
					outputData.RssiMin = _result.RssiMin;
					outputData.RssiAvg = _result.RssiAvg;
					outputData.RssiMax = _result.RssiMax;
							
					PushTestData(&outputData, sizeof(outputData));
					
					if (_deliveryResultRequest)
					{
						SendTestDataToNode(_deliveryResultAddress, &outputData, sizeof(outputData));
					}
					
					_state = StateDone;
				}
				break;
			}
			
			bufferPtr = _outputBuffer;
			return len;
		}
		
		virtual bool OnRxData(int selfAddress, int sourceAddress, unsigned char *data, int dataCount, int rssi)
		{
			if (data == 0 || dataCount < sizeof(TestLink::PacketStructHeader))
			{
				return false;
			}
			
			TestLink::PacketStructHeader *hdr = (TestLink::PacketStructHeader *)data;
			
			if (!TypeAndIdValid(hdr->TestType, hdr->TestId))
			{
				return false;
			}
			
			if (hdr->Dst != selfAddress || sourceAddress != _receiverId)
			{
				return false;
			}
			
			switch (hdr->PacketType)
			{
			case TestLink::PacketTypeStartAck:
				{
					if (_state == StateWaitStartAck)
					{
						_state = StateSendTestPacket;
					}
				}
				break;
			case TestLink::PacketTypeResult:
				{
					if (_state == StateWaitResult)
					{
						if (dataCount < sizeof(TestLink::PacketStructResult))
						{
							_resultCode = TestLink::ResultCodeInvalidResultReceived;
						}
						else
						{
							TestLink::PacketStructResult *resultPacket = (TestLink::PacketStructResult *)data;
							_result = resultPacket->Result;
							_resultCode = TestLink::ResultCodeDone;
						}
						
						_state = StateComplete;
					}
				}
				break;
			}
			
			return true;
		}
	};
	
	class TestLinkSlave : public Test
	{
	public:
		static const int OutputBufferSize = 64;
		static const int CompleteMaxCount = 10;
	protected:
		unsigned char _masterId;
		bool _sendStartAck;
		bool _sendResult;
		bool _complete;
		int _testCounter;
		float _rssiMin;
		float _rssiAvg;
		float _rssiMax;
		unsigned char _outputBuffer[OutputBufferSize];
		int _completeCounter;
	public:
		TestLinkSlave(int id): Test(id)
		{
			Reset();
		}
		
		virtual ~TestLinkSlave()
		{
		}
		
		virtual TestTypeCode GetType()
		{
			return TestTypeCodeLink;
		}
		
		virtual void Reset()
		{
			_masterId = -1;
			_sendStartAck = false;
			_sendResult = false;
			_complete = false;
			_completeCounter = 0;
			_testCounter = 0;
			_rssiMin = -127.0;
			_rssiAvg = -127.0;
			_rssiMax = -127.0;
		}
		
		// тест завершен, можно уничтожать
		virtual bool Complete()
		{
			return _completeCounter >= CompleteMaxCount;
		}
		
		virtual int PopData(unsigned char *&bufferPtr)
		{
			int len = 0;
			
			if (_sendStartAck)
			{
				_sendStartAck = false;
				
				TestLink::PacketStructStartAck pkt(GetId(), _masterId);
				len = sizeof(pkt);
				memcpy(_outputBuffer, &pkt, len);
			}
			
			if (_sendResult)
			{
				_sendResult = false;
				
				TestLink::PacketStructResult pkt(GetId(), _masterId);
				pkt.Result.ReceivedCount = _testCounter;
				pkt.Result.RssiMin = (int)_rssiMin;
				pkt.Result.RssiAvg = (int)_rssiAvg;
				pkt.Result.RssiMax = (int)_rssiMax;

				len = sizeof(pkt);
				memcpy(_outputBuffer, &pkt, len);
				
				_complete = true;
				_completeCounter = 0;
			}
			
			if (_complete)
			{
				if (_completeCounter < CompleteMaxCount)
				{
					_completeCounter++;
				}
			}
			
			bufferPtr = _outputBuffer;
			return len;
		}
		
		virtual bool OnRxData(int selfAddress, int sourceAddress, unsigned char *data, int dataCount, int rssi)
		{
			if (data == 0 || dataCount < sizeof(TestLink::PacketStructHeader))
			{
				return false;
			}
			
			TestLink::PacketStructHeader *hdr = (TestLink::PacketStructHeader *)data;
			
			if (!TypeAndIdValid(hdr->TestType, hdr->TestId))
			{
				return false;
			}
			
			if (hdr->Dst != selfAddress)
			{
				return false;
			}
			
			switch (hdr->PacketType)
			{
			case TestLink::PacketTypeStart:
				{
					_masterId = sourceAddress;
					_sendStartAck = true;
					_testCounter = 0;
				}
				break;
			case TestLink::PacketTypeTest:
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
				}
				break;
			case TestLink::PacketTypeGetResult:
				{
					_masterId = sourceAddress;
					_sendResult = true;
				}
				break;
			}
			
			return true;
		}
		
		static bool MustCreate(int selfAddress, int sourceAddress, unsigned char *data, int dataCount)
		{
			if (data == 0 || dataCount < sizeof(TestLink::PacketStructHeader))
			{
				return false;
			}
			
			TestLink::PacketStructHeader *hdr = (TestLink::PacketStructHeader *)data;
			
			if (hdr->TestType != TestTypeCodeLink)
			{
				return false;
			}
			
			if (hdr->Dst != selfAddress)
			{
				return false;
			}
			
			if (hdr->PacketType != TestLink::PacketTypeStart)
			{
				return false;
			}

			return true;
		}
	};
}
}
}

#endif