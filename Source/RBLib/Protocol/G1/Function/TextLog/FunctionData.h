///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Текстовый лог
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_TEXTLOG_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_TEXTLOG_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"


namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace TextLog
{
	// Формат передачи тектового лога
	class DataFormat
	{
	public:
		char *Str;
		unsigned char StrLen;
	public:	
		DataFormat()
		{
			Str = 0;
			StrLen = 0;
		}
		
		int Size()
		{
			return sizeof(StrLen) + sizeof(char) * StrLen;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			unsigned char *dstPtr = (unsigned char *)dst;
			
			int dstIndex = 0;
			dstPtr[dstIndex] = StrLen;
			dstIndex++;
			
			for (int i = 0; i < StrLen; i ++)
			{
				if (dstIndex + sizeof(unsigned char) >= dstSize)
				{
					break;
				}
				
				dstPtr[dstIndex] = Str[i];
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	// Выходной формат данных функции Текстовый лог
	class DataType : public Device::CommandFromFunctionParams<Device::FunctionCodeTextLog, DataFormat >
	{
	};
}
}
}
}
}

#endif