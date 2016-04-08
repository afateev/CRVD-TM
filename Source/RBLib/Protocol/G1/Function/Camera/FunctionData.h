///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Камера
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_CAMERA_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_CAMERA_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"
#include "CaptureStartResultCode.h"
#include "ParamCode.h"
#include "CaptureFormatCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Camera
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeStartCapture						= 1,
		MessageCodeStartCaptureResult				= 2,
		MessageCodeCaptureFinished					= 3,
		MessageCodeGetAmbientLightLevel				= 4,
		MessageCodeAmbientLightLevel				= 5,
		MessageCodeGetBacklightPowerLevel			= 6,
		MessageCodeBacklightPowerLevel				= 7,
		MessageCodeGetParams 						= 8,
		MessageCodeParams							= 9,
		MessageCodeSetParams						= 10,
		MessageCodeSetParamsResult 					= 11,
		MessageCodeParamsChanged					= 12,
		MessageCodeSelectPhotoInfo					= 13,
		MessageCodeSelectPhotoInfoResult			= 14,
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
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeCamera, MessageFormat>
	{
	};
	
	// Входной формат данных функции
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeCamera, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeCamera, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	class MessageStartCapture
	{
	public:
		CaptureFormatCode CaptureFormat;	// формат съемки
	protected:
		bool _valid;
	public:
		MessageStartCapture()
		{
			CaptureFormat = CaptureFormatUnknown;
		}
	
		MessageStartCapture(void *data, int dataCount)
		{
			_valid = false;
			
			// если параметры не указаны, то применяем формат по умолчанию
			if (data == 0 || dataCount < 1)
			{
				CaptureFormat = CaptureFormatDefault;
				_valid = true;
			}
			else
			{
				if (dataCount >= sizeof(CaptureFormat))
				{
					unsigned char *src = (unsigned char *)data;
					CaptureFormat = (CaptureFormatCode)(*(unsigned char *)&src[0]);
					_valid = true;
				}
			}
		}
		
		bool Valid()
		{
			return _valid;
		}
		
	};
	
	class MessageStartCaptureResult
	{
	public:
		CaptureStartResultCode Result;
	public:
		MessageStartCaptureResult()
		{
			Result = CaptureStartResultUnknown;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Result))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Result;
			dstIndex++;
			
			return dstIndex;
		}
	};
	
	class MessageCaptureFunished
	{
	public:
		unsigned int Id;
		unsigned int Size;
	public:
		MessageCaptureFunished()
		{
			Id = 0;
			Size = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Id) + sizeof(Size))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Id;
			dstIndex++;
			dstPtr[dstIndex] = (Id >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Id >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Id >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = Size;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 24) & 0xFF;
			dstIndex++;
			
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

	// запрос на извлечение информации из БД фотографий
	class MessageSelectPhotoInfo
	{
	public:
		unsigned long TimeFrom; // левая граница временного интервала
		unsigned long TimeTo; // правая граница временного интервала
	protected:
		bool _valid;
	public:

		MessageSelectPhotoInfo(void *data, int dataCount)
		: TimeFrom(0) // по умолчанию левая граница ставится на минимум
		, TimeTo(-1UL) // по умолчанию правая граница ставится на максимум
		{
			_valid = false;

			if (data == 0 || dataCount < 4)
			{
				return;
			}

			TimeFrom = *((unsigned long*)data);
			_valid = true;

			if (dataCount < 8)
				return;

			TimeTo = *((unsigned long*)data + 4);
		}

		inline bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageSelectPhotoInfoResult
	{
	public:
		unsigned long Length;
	public:
		MessageSelectPhotoInfoResult(unsigned long length)
		: Length(length)
		{
		}

		inline int ToByteArray(void *dst, int dstSize)
		{
			*((unsigned long*)dst) = Length;
			return 4;
		}
	};

}
}
}
}
}

#endif
