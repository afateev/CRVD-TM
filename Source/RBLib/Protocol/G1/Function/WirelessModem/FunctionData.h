///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Беспроводной модем
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_WIRELESSMODEM_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_WIRELESSMODEM_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"
#include "ConfigParamCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace WirelessModem
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeGetState							= 1,
		MessageCodeState							= 2,
		MessageCodeStateChanged						= 3,
		MessageCodeGetSelfActivityTable				= 4,
		MessageCodeSelfActivityTable				= 5,
		MessageCodeSelfActivityTableChanged 		= 6,
		MessageCodeGetRemoteNodeActivityTable		= 7,
		MessageCodeRemoteNodeActivityTable			= 8,
		MessageCodeRemoteNodeActivityTableReceived 	= 9,
		MessageCodeRemoteNodeActivityTableChanged 	= 10,
		MessageCodeGetRouteTable					= 11,
		MessageCodeRouteTable						= 12,
		MessageCodeRouteTableChanged 				= 13,
		MessageCodeDataEnqueue 						= 14,
		MessageCodeDataEnqueueResult 				= 15,
		MessageCodeDataDeliveryResult 				= 16,
		MessageCodeDataReceived 					= 17,
		MessageCodeDebugData	 					= 18,
		MessageCodeGetConfigParams					= 19,
		MessageCodeConfigParams						= 20,
		MessageCodeSetConfigParams					= 21,
		MessageCodeSetConfigParamsResult			= 22,
		MessageCodeApplyConfigParams				= 23,
		MessageCodeApplyConfigParamsAck				= 24,
		MessageCodeEnableDebugDataOutput			= 25,
		MessageCodeTestCommand						= 26,
		MessageCodeTestCommandResult				= 27,
		MessageCodeTestData							= 28,
	};
	
	class MessageFormat
	{
	public:
		MessageCode Code;
		unsigned short DataLen;
		void *Data;
	protected:
		bool _valid;
	public:
		MessageFormat()
		{
			Code = MessageCodeInvalid;
			DataLen = 0;
			Data = 0;
			_valid = true;
		}
		
		int Size()
		{
			return sizeof(Code) + sizeof(DataLen) + sizeof(unsigned char) * DataLen;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < (3 + DataLen))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			unsigned char *dataPtr = (unsigned char *)Data;
			
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Code;
			dstIndex++;
			
			if (DataLen > 0 && Data != 0)
			{
				dstPtr[dstIndex] = DataLen;
				dstIndex++;
				dstPtr[dstIndex] = (DataLen >> 8) & 0xFF;
				dstIndex++;
				
				for (int i = 0; i < DataLen; i ++)
				{
					dstPtr[dstIndex] = dataPtr[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
		
		void FromByteArray(unsigned char *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 3)
			{
				return;
			}
			
			Code = (MessageCode) data[0];
			DataLen = data[1];
			DataLen += data[2] << 8;
			
			if (dataCount < Size())
			{
				return;
			}
			
			Data = &data[3];
			
			_valid = true;
		}
	};
	
	// Выходной формат данных функции Модем
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeWirelessModem, MessageFormat>
	{
	};
	
	// Входной формат данных функции Модем
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeWirelessModem, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeWirelessModem, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	class MessageSelfActivityTable
	{
	public:
		unsigned char TableSize;
		unsigned char *Nodes;
		unsigned char *Rates;
	public:
		MessageSelfActivityTable()
		{
			TableSize = 0;
			Nodes = 0;
			Rates = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(TableSize))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (TableSize > 0 && Nodes != 0 && Rates != 0)
			{
				dstPtr[dstIndex] = TableSize;
				dstIndex++;
				
				for (int i = 0; i < TableSize; i ++)
				{
					if (dstIndex + 2 * sizeof(unsigned char) >= dstSize)
					{
						break;
					}
					
					dstPtr[dstIndex] = Nodes[i];
					dstIndex++;
					dstPtr[dstIndex] = Rates[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	class MessageRemoteNodeActivityTable
	{
	public:
		unsigned char RemoteNodeId;
		unsigned char TableSize;
		unsigned char *Nodes;
		unsigned char *Rates;
	public:
		MessageRemoteNodeActivityTable()
		{
			RemoteNodeId = 0;
			TableSize = 0;
			Nodes = 0;
			Rates = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < (sizeof(RemoteNodeId) + sizeof(TableSize)))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = RemoteNodeId;
			dstIndex++;
			
			if (TableSize > 0 && Nodes != 0 && Rates != 0)
			{
				dstPtr[dstIndex] = TableSize;
				dstIndex++;
				
				for (int i = 0; i < TableSize; i ++)
				{
					dstPtr[dstIndex] = Nodes[i];
					dstIndex++;
					dstPtr[dstIndex] = Rates[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	class MessageRouteTable
	{
	public:
		unsigned char TableSize;
		unsigned char *ToNodes;
		unsigned char *FromNodes;
	public:
		MessageRouteTable()
		{
			TableSize = 0;
			ToNodes = 0;
			FromNodes = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(TableSize))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (TableSize > 0 && ToNodes != 0 && FromNodes != 0)
			{
				dstPtr[dstIndex] = TableSize;
				dstIndex++;
				
				for (int i = 0; i < TableSize; i ++)
				{
					if (dstIndex + 2 * sizeof(unsigned char) >= dstSize)
					{
						break;
					}
					
					dstPtr[dstIndex] = ToNodes[i];
					dstIndex++;
					dstPtr[dstIndex] = FromNodes[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	class MessageDataEnqueue
	{
	public:
		unsigned char DataId;
		unsigned int Timeout;
		unsigned char DstCount;
		unsigned short DataCount;
		unsigned char *DstList;
		unsigned char *Data;
	protected:
		bool _valid;
	public:
		MessageDataEnqueue(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 8)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			DataId = src[0];
			Timeout = src[1];
			Timeout += (src[2] << 8);
			Timeout += (src[3] << 16);
			Timeout += (src[4] << 24);
			DstCount = src[5];
			DataCount = src[6];
			DataCount += (src[7] << 8);
			
			if (dataCount < DstCount + DataCount + sizeof(DataId) + sizeof(DataCount))
			{
				return;
			}
			
			DstList = &src[8];
			Data = &src[8 + DstCount];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageDataEnqueueResult
	{
	public:
		enum EqueueResult
		{
			EqueueResultUnknown				= 0,
			EqueueResultEnqueued			= 1,
			EqueueResultNoSpaceInQueue		= 2,
		};
	public:
		unsigned char DataId;
		EqueueResult Result;
		unsigned char PacketId;
	public:
		MessageDataEnqueueResult()
		{
			DataId = 0;
			Result = EqueueResultUnknown;
			PacketId = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			unsigned char *dstPtr = (unsigned char *)dst;
			
			int dstIndex = 0;
			dstPtr[dstIndex] = DataId;
			dstIndex++;
			dstPtr[dstIndex] = Result;
			dstIndex++;
			dstPtr[dstIndex] = PacketId;
			dstIndex++;
			
			return dstIndex;
		}
	};
	
	class MessageDataDeliveryResult
	{
	public:
		enum DeliveryResult
		{
			DeliveryResultUnknown				= 0,
			DeliveryResultDelivered				= 1,
			DeliveryResultImpossible			= 2,
			DeliveryResultError					= 3
		};
	public:
		unsigned char PacketId;
		unsigned char Node;
		DeliveryResult Result;
	public:
		MessageDataDeliveryResult()
		{
			PacketId = 0;
			Node = 0;
			Result = DeliveryResultUnknown;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			unsigned char *dstPtr = (unsigned char *)dst;
			
			int dstIndex = 0;
			dstPtr[dstIndex] = PacketId;
			dstIndex++;
			dstPtr[dstIndex] = Node;
			dstIndex++;
			dstPtr[dstIndex] = Result;
			dstIndex++;
			
			return dstIndex;
		}
	};
	
	class MessageDataReceived
	{
	public:
		unsigned char SourceAddress;
		unsigned short DataCount;
		unsigned char *Data;
	public:
		MessageDataReceived()
		{
			SourceAddress = 0;
			DataCount = 0;
			Data = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < (sizeof(SourceAddress) + sizeof(DataCount)))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (DataCount > 0 && Data != 0)
			{
				dstPtr[dstIndex] = SourceAddress;
				dstIndex++;
				dstPtr[dstIndex] = DataCount;
				dstIndex++;
				dstPtr[dstIndex] = (DataCount >> 8);
				dstIndex++;
				
				for (int i = 0; i < DataCount; i ++)
				{
					if (dstIndex + sizeof(unsigned char) >= dstSize)
					{
						break;
					}
					
					dstPtr[dstIndex] = Data[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	class MessageDebugData
	{
	public:
		unsigned short Code;
		unsigned short DataCount;
		unsigned char *Data;
	public:
		MessageDebugData()
		{
			Code = 0;
			DataCount = 0;
			Data = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < (sizeof(Code) + sizeof(DataCount)))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (DataCount > 0 && Data != 0)
			{
				dstPtr[dstIndex] = Code;
				dstIndex++;
				dstPtr[dstIndex] = (Code >> 8);
				dstIndex++;
				dstPtr[dstIndex] = DataCount;
				dstIndex++;
				dstPtr[dstIndex] = (DataCount >> 8);
				dstIndex++;
				
				for (int i = 0; i < DataCount; i ++)
				{
					if (dstIndex + sizeof(unsigned char) >= dstSize)
					{
						break;
					}
					
					dstPtr[dstIndex] = Data[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	class MessageGetConfigParams
	{
	public:
		unsigned char Count;
		ConfigParamCode *Ids;
	protected:
		bool _valid;
	public:
		MessageGetConfigParams(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 1)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Count = src[0];
			
			if (dataCount < Count * sizeof(ConfigParamCode) + sizeof(Count))
			{
				return;
			}
			
			Ids = (ConfigParamCode *)&src[1];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	typedef unsigned int ConfigParamValueType;
	
	class MessageConfigParams
	{
	public:
		unsigned char Count;
		ConfigParamCode *Ids;
		ConfigParamValueType *Values;
	public:
		MessageConfigParams()
		{
			Count = 0;
			Ids = 0;
			Values = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Count) * (sizeof(ConfigParamCode) + sizeof(ConfigParamValueType)))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (Count > 0 && Ids != 0 && Values != 0)
			{
				dstPtr[dstIndex] = Count;
				dstIndex++;
				
				for (int i = 0; i < Count; i ++)
				{
					dstPtr[dstIndex] = Ids[i];
					dstIndex++;
				}
				
				for (int i = 0; i < Count; i ++)
				{
					unsigned int value = Values[i];
					
					for (int j = 0; j < sizeof(ConfigParamValueType); j++)
					{
						dstPtr[dstIndex] = value >> (8 * j);
						dstIndex++;
					}
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	class MessageSetConfigParams
	{
	public:
		unsigned char Count;
		ConfigParamCode *Ids;
		ConfigParamValueType *Values;
	protected:
		bool _valid;
	public:
		MessageSetConfigParams(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 1)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Count = src[0];
			
			if (dataCount < Count * (sizeof(ConfigParamCode) + sizeof(ConfigParamValueType)) + sizeof(Count))
			{
				return;
			}
			
			Ids = (ConfigParamCode *)&src[1];
			Values = (ConfigParamValueType *)&src[1 + Count * sizeof(ConfigParamCode)];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageSetConfigParamsResult
	{
	public:
		unsigned char Count;
		ConfigParamCode *Ids;
		bool *Results;
	public:
		MessageSetConfigParamsResult()
		{
			Count = 0;
			Ids = 0;
			Results = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Count) * (sizeof(ConfigParamCode) + sizeof(bool)))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (Count > 0 && Ids != 0 && Results != 0)
			{
				dstPtr[dstIndex] = Count;
				dstIndex++;
				
				for (int i = 0; i < Count; i ++)
				{
					dstPtr[dstIndex] = Ids[i];
					dstIndex++;
				}
				
				for (int i = 0; i < Count; i ++)
				{
					dstPtr[dstIndex] = Results[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	class MessageTestCommand
	{
	public:
		unsigned char TestType;
		int TestId;
		unsigned short DataCount;
		unsigned char *Data;
	protected:
		bool _valid;
	public:
		MessageTestCommand(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 8)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			TestType = src[0];
			TestId = src[1];
			TestId += (src[2] << 8);
			TestId += (src[3] << 16);
			TestId += (src[4] << 24);
			DataCount = src[5];
			DataCount += (src[6] << 8);
			
			if (dataCount < DataCount + sizeof(TestType) + sizeof(TestId) + sizeof(DataCount))
			{
				return;
			}
			
			Data = &src[7];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageTestCommandResult
	{
	public:
		unsigned char TestType;
		int TestId;
		unsigned char Result;
	public:
		MessageTestCommandResult()
		{
			TestType = 0;
			TestId = -1;
			Result = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			unsigned char *dstPtr = (unsigned char *)dst;
			
			int dstIndex = 0;
			dstPtr[dstIndex] = TestType;
			dstIndex++;
			dstPtr[dstIndex] = TestId;
			dstIndex++;
			dstPtr[dstIndex] = (TestId >> 8);
			dstIndex++;
			dstPtr[dstIndex] = (TestId >> 16);
			dstIndex++;
			dstPtr[dstIndex] = (TestId >> 24);
			dstIndex++;
			dstPtr[dstIndex] = Result;
			dstIndex++;
			
			return dstIndex;
		}
	};
	
	class MessageTestData
	{
	public:
		unsigned char TestType;
		int TestId;
		unsigned short DataCount;
		unsigned char *Data;
	public:
		MessageTestData()
		{
			TestType = 0;
			TestId = -1;
			DataCount = 0;
			Data = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < (sizeof(TestType) + sizeof(TestId) + sizeof(DataCount)))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (DataCount > 0 && Data != 0)
			{
				dstPtr[dstIndex] = TestType;
				dstIndex++;
				dstPtr[dstIndex] = TestId;
				dstIndex++;
				dstPtr[dstIndex] = (TestId >> 8);
				dstIndex++;
				dstPtr[dstIndex] = (TestId >> 16);
				dstIndex++;
				dstPtr[dstIndex] = (TestId >> 24);
				dstIndex++;
				dstPtr[dstIndex] = DataCount;
				dstIndex++;
				dstPtr[dstIndex] = (DataCount >> 8);
				dstIndex++;
				
				for (int i = 0; i < DataCount; i ++)
				{
					if (dstIndex + sizeof(unsigned char) >= dstSize)
					{
						break;
					}
					
					dstPtr[dstIndex] = Data[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
}
}
}
}
}

#endif