///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Беспроводной модем
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_TEXTLOG_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_TEXTLOG_IMPLEMENTER_H

#include "FunctionData.h"
#include "../../../../Utils/CallbackWrapper.h"
#include "../../../../Utils/CallbackListWrapper.h"

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
	template
		<
			int maxStrLen				// максимальная длина строки
		>
	class Implementer
	{
	public:
		static const int MaxStrLen = maxStrLen;
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
	protected:
		unsigned char _functionData[MaxStrLen + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
	public:
		void Out(char * str, unsigned int strLen)
		{
			if (str == 0 || strLen < 1)
			{
				return;
			}
			
			if (strLen > MaxStrLen)
			{
				strLen = MaxStrLen;
			}
			
			TextLog::DataType textLog;
			textLog.Data.Str = str;
			textLog.Data.StrLen = strLen;
							
			int functionDataLen = textLog.ToByteArray(_functionData, sizeof(_functionData));
			FromFunctionCallback.Call(Device::FunctionCodeTextLog, 0, _functionData, functionDataLen);
		}
		
		void Out(char *str)
		{
			if (str == 0)
			{
				return;
			}
			
			int i = 0;
			for(i = 0; i < MaxStrLen; i++)
			{
				if (str[i] == 0)
				{
					break;
				}
			}
			
			Out(str, i);
		}
	};
}
}
}
}
}

#endif