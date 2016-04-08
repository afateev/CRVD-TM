///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Обрывной датчик
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace WirebreakSensor
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeGetWireState						= 1,
		MessageCodeWireState						= 2,
		MessageCodeWireStateChanged					= 3,
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
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeWirebreakSensor, MessageFormat>
	{
	};
	
	// Входной формат данных функции Модем
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeWirebreakSensor, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeWirebreakSensor, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	class MessageWireState
	{
	public:
		unsigned char Count;
		bool *State;
	public:
		MessageWireState()
		{
			Count = 0;
			State = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Count) * sizeof(bool))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (Count > 0 && State != 0)
			{
				dstPtr[dstIndex] = Count;
				dstIndex++;
				
				for (int i = 0; i < Count; i ++)
				{
					dstPtr[dstIndex] = State[i];
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