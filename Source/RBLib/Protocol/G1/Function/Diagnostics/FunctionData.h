///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Диагностика
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_DIAGNOSTICS_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_DIAGNOSTICS_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"
#include "DiagnosticCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Diagnostics
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeGetState							= 1,
		MessageCodeState							= 2,
		MessageCodeGetSavedState					= 3,
		MessageCodeSavedState						= 4,
		MessageCodeResetSavedState					= 5,
		MessageCodeResetSavedStateResult			= 6,
		MessageCodeDeviceStateChanged				= 7,
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
	
	// Выходной формат данных функции
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeDiagnostics, MessageFormat>
	{
	};
	
	// Входной формат данных функции
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeDiagnostics, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeDiagnostics, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	class MessageGetState
	{
	public:
		unsigned char Count;
		DiagnosticCode *Codes;
	protected:
		bool _valid;
	public:
		MessageGetState(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 1)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Count = src[0];
			
			if (dataCount < Count * sizeof(DiagnosticCode) + sizeof(Count))
			{
				return;
			}
			
			Codes = (DiagnosticCode *)&src[1];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	typedef int DiagnosticValueType;
	
	class MessageState
	{
	public:
		unsigned char Count;
		DiagnosticCode *Codes;
		DiagnosticValueType *Values;
	public:
		MessageState()
		{
			Count = 0;
			Codes = 0;
			Values = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Count) * (sizeof(DiagnosticCode) + sizeof(DiagnosticValueType)))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (Count > 0 && Codes != 0 && Values != 0)
			{
				dstPtr[dstIndex] = Count;
				dstIndex++;
				
				for (int i = 0; i < Count; i ++)
				{
					dstPtr[dstIndex] = Codes[i];
					dstIndex++;
				}
				
				for (int i = 0; i < Count; i ++)
				{
					DiagnosticValueType value = Values[i];
					
					for (int j = 0; j < sizeof(DiagnosticValueType); j++)
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
	
	class MessageGetSavedState
	{
	public:
		unsigned char Count;
		DiagnosticCode *Codes;
	protected:
		bool _valid;
	public:
		MessageGetSavedState(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 1)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Count = src[0];
			
			if (dataCount < Count * sizeof(DiagnosticCode) + sizeof(Count))
			{
				return;
			}
			
			Codes = (DiagnosticCode *)&src[1];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageSavedState
	{
	public:
		unsigned char Count;
		DiagnosticCode *Codes;
		DiagnosticValueType *Values;
	public:
		MessageSavedState()
		{
			Count = 0;
			Codes = 0;
			Values = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Count) * (sizeof(DiagnosticCode) + sizeof(DiagnosticValueType)))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (Count > 0 && Codes != 0 && Values != 0)
			{
				dstPtr[dstIndex] = Count;
				dstIndex++;
				
				for (int i = 0; i < Count; i ++)
				{
					dstPtr[dstIndex] = Codes[i];
					dstIndex++;
				}
				
				for (int i = 0; i < Count; i ++)
				{
					DiagnosticValueType value = Values[i];
					
					for (int j = 0; j < sizeof(DiagnosticValueType); j++)
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
}
}
}
}
}

#endif