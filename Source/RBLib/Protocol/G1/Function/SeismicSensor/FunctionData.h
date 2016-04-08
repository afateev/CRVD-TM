///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Сейсмический датчик
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_SEISMICSENSOR_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_SEISMICSENSOR_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"
#include "ChannelInfoTypeCode.h"
#include "ParamCode.h"
#include "AlarmParamCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace SeismicSensor
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeGetSeismicInfoOutputMode			= 1,
		MessageCodeSeismicInfoOutputMode			= 2,
		MessageCodeSetSeismicInfoOutputMode			= 3,
		MessageCodeSetSeismicInfoOutputModeResult	= 4,
		MessageCodeSeismicInfo						= 5,
		MessageCodeGetParams 						= 6,
		MessageCodeParams							= 7,
		MessageCodeSetParams						= 8,
		MessageCodeSetParamsResult 					= 9,
		MessageCodeAlarm 							= 10,
		MessageCodeParamsChanged					= 11,
		MessageCodeSignalLevel						= 12,
		MessageCodeAutotuningStart					= 13,
		MessageCodeAutotuningStartResult			= 14,
		MessageCodeAutotuningComplete				= 15,
		MessageCodeFrequentAlarm					= 16,
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
	
	// Выходной формат данных функции Сейсмический датчик
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeSeismicSensor, MessageFormat>
	{
	};
	
	// Входной формат данных функции Сейсмический датчик
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeSeismicSensor, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeSeismicSensor, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	class MessageSeismicInfoOutputMode
	{
	public:
		bool Enabled;
		unsigned char ChannelCount;
		ChannelInfoTypeCode *ChannelInfoType;
	public:
		MessageSeismicInfoOutputMode()
		{
			Enabled = false;
			ChannelCount = 0;
			ChannelInfoType = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(ChannelCount))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Enabled;
			dstIndex++;
			
			if (ChannelCount > 0 && ChannelInfoType != 0)
			{
				dstPtr[dstIndex] = ChannelCount;
				dstIndex++;
				
				for (int i = 0; i < ChannelCount; i ++)
				{
					dstPtr[dstIndex] = ChannelInfoType[i];
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
	
	class MessageSetSeismicInfoOutputMode
	{
	public:
		bool Enabled;
		unsigned char ChannelCount;
		ChannelInfoTypeCode *ChannelInfoType;
	protected:
		bool _valid;
	public:
		MessageSetSeismicInfoOutputMode(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 2)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Enabled = src[0];
			ChannelCount = src[1];
			
			if (dataCount < ChannelCount + sizeof(Enabled) + sizeof(ChannelCount))
			{
				return;
			}
			
			ChannelInfoType = (ChannelInfoTypeCode *)&src[2];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageSetSeismicInfoOutputModeResult
	{
	public:
		enum ErrorCode
		{
			ErrorCodeOk							= 0,
			ErrorCodeInvalidChannelInfoCode		= 1,
			ErrorCodeTooManyChannelCount		= 2,
			ErrorCodeInvalidDataFormat			= 3,
		};
		ErrorCode Error;
	public:
		MessageSetSeismicInfoOutputModeResult()
		{
			Error = ErrorCodeOk;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Error))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Error;
			dstIndex++;

			return dstIndex;
		}
	};
	
	class MessageSeismicInfo
	{
	public:
		unsigned char ChannelCount;
		
		struct ChannelDataStruct
		{
			unsigned int FirstItemXValue;
			unsigned int XDelta;
			ChannelInfoTypeCode InfoType;
			unsigned short ItemsCount;
			unsigned short *YValues;
			
#ifdef _WINDOWS
			static const unsigned int FixedContentSize = sizeof(unsigned int) + sizeof(unsigned int) + sizeof(ChannelInfoTypeCode) + sizeof(unsigned short);
#else
			static const unsigned int FixedContentSize = sizeof(FirstItemXValue) + sizeof(XDelta) + sizeof(InfoType) + sizeof(ItemsCount);
#endif
			
			ChannelDataStruct()
			{
				FirstItemXValue = 0;
				XDelta = 0;
				InfoType = ChannelInfoTypeCodeNoData;
				ItemsCount = 0;
				YValues = 0;
			}
		};
		
		ChannelDataStruct *ChannelData;
	public:
		MessageSeismicInfo()
		{
			ChannelCount = 0;
			ChannelData = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (ChannelData == 0)
			{
				return 0;
			}
			
			int estimatedDataSize = sizeof(ChannelCount);
			
			for (int i = 0; i < ChannelCount; i++)
			{
				estimatedDataSize += ChannelDataStruct::FixedContentSize;
				estimatedDataSize += sizeof(unsigned short) * ChannelData[i].ItemsCount;
			}
			
			if (dst == 0 || dstSize < estimatedDataSize)
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (ChannelCount > 0)
			{
				dstPtr[dstIndex] = ChannelCount;
				dstIndex++;
				
				for (int i = 0; i < ChannelCount; i ++)
				{
					dstPtr[dstIndex] = ChannelData[i].FirstItemXValue;
					dstIndex++;
					dstPtr[dstIndex] = ChannelData[i].FirstItemXValue >> 8;
					dstIndex++;
					dstPtr[dstIndex] = ChannelData[i].FirstItemXValue >> 16;
					dstIndex++;
					dstPtr[dstIndex] = ChannelData[i].FirstItemXValue >> 24;
					dstIndex++;
					
					dstPtr[dstIndex] = ChannelData[i].XDelta;
					dstIndex++;
					dstPtr[dstIndex] = ChannelData[i].XDelta >> 8;
					dstIndex++;
					dstPtr[dstIndex] = ChannelData[i].XDelta >> 16;
					dstIndex++;
					dstPtr[dstIndex] = ChannelData[i].XDelta >> 24;
					dstIndex++;
					
					dstPtr[dstIndex] = ChannelData[i].InfoType;
					dstIndex++;
					
					dstPtr[dstIndex] = ChannelData[i].ItemsCount;
					dstIndex++;
					dstPtr[dstIndex] = ChannelData[i].ItemsCount >> 8;
					dstIndex++;
					
					for (int j = 0; j < ChannelData[i].ItemsCount; j++)
					{
						unsigned short yValue = 0;
						if (ChannelData[i].YValues != 0)
						{
							yValue = ChannelData[i].YValues[j];
						}
						
						dstPtr[dstIndex] = yValue;
						dstIndex++;
						dstPtr[dstIndex] = yValue >> 8;
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
	
	class MessageAlarm
	{
	public:
		typedef float ParamValueType;
	public:
		unsigned char Count;
		AlarmParamCode *Ids;
		ParamValueType *Values;
	public:
		MessageAlarm()
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
	
	class MessageSignalLevel
	{
	public:
		float SignalLevel;
	public:
		MessageSignalLevel()
		{
			SignalLevel = 0.0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(SignalLevel))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			for (int j = 0; j < sizeof(SignalLevel); j++)
			{
				dstPtr[dstIndex] = ((unsigned char *)&SignalLevel)[j];
				dstIndex++;
			}

			return dstIndex;
		}
	};
	
	class MessageFrequentAlarm
	{
	public:
		typedef float ParamValueType;
	public:
		unsigned char Count;
		AlarmParamCode *Ids;
		ParamValueType *Values;
	public:
		MessageFrequentAlarm()
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
}
}
}
}
}

#endif