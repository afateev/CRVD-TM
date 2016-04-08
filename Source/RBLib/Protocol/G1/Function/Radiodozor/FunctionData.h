///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Радиодозор
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_RADIODOZOR_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_RADIODOZOR_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"
#include "ModeCode.h"
#include "ParamCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Radiodozor
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeGetMode							= 1,
		MessageCodeMode								= 2,
		MessageCodeSetMode							= 3,
		MessageCodeSetModeResult					= 4,
		MessageCodeModeChanged						= 5,
		MessageCodeGetParams 						= 6,
		MessageCodeParams							= 7,
		MessageCodeSetParams						= 8,
		MessageCodeSetParamsResult 					= 9,
		MessageCodeParamsChanged					= 10,
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
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeRadiodozor, MessageFormat>
	{
	};
	
	// Входной формат данных функции Модем
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeRadiodozor, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeRadiodozor, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	class MessageGetParams
	{
	public:
		unsigned char Count;
		ParamCode *Ids;
	protected:
		bool _valid;
	public:
		MessageGetParams(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 1)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Count = src[0];
			
			if (dataCount < Count * sizeof(ParamCode) + sizeof(Count))
			{
				return;
			}
			
			Ids = (ParamCode *)&src[1];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	typedef float ParamValueType;
	
	class MessageParams
	{
	public:
		unsigned char Count;
		ParamCode *Ids;
		ParamValueType *Values;
	public:
		MessageParams()
		{
			Count = 0;
			Ids = 0;
			Values = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Count) * (sizeof(ParamCode) + sizeof(ParamValueType)))
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
					ParamValueType value = Values[i];
					
					for (int j = 0; j < sizeof(ParamValueType); j++)
					{
						dstPtr[dstIndex] = ((unsigned char *)&value)[j];
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
	
	class MessageSetParams
	{
	public:
		unsigned char Count;
		ParamCode *Ids;
		ParamValueType *Values;
	protected:
		bool _valid;
	public:
		MessageSetParams(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 1)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Count = src[0];
			
			if (dataCount < Count * (sizeof(ParamCode) + sizeof(ParamValueType)) + sizeof(Count))
			{
				return;
			}
			
			Ids = (ParamCode *)&src[1];
			Values = (ParamValueType *)&src[1 + Count * sizeof(ParamCode)];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageSetParamsResult
	{
	public:
		unsigned char Count;
		ParamCode *Ids;
		bool *Results;
	public:
		MessageSetParamsResult()
		{
			Count = 0;
			Ids = 0;
			Results = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Count) * (sizeof(ParamCode) + sizeof(bool)))
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
}
}
}
}
}

#endif