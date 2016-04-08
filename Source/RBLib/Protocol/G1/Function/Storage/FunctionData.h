///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Хранилище
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_STORAGE_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_STORAGE_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"
#include "ResultCode.h"
#include "BitFlagsHolder.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Storage
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeReadFile							= 1,
		MessageCodeReadFileResult					= 2,
		MessageCodePushFile							= 3,
		MessageCodePushFileResult					= 4,
		MessageCodePushFileDone						= 5, // отправлены все части
		MessageCodeCleanFilePhotoDB					= 6, // устарела, НЕ ПОДДЕРЖИВАЕТСЯ, коды остались зарезервированы для совместимости с предыдущими версиями
		MessageCodeCleanFilePhotoDBResult			= 7, // устарела, НЕ ПОДДЕРЖИВАЕТСЯ, коды остались зарезервированы для совместимости с предыдущими версиями
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
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeStorage, MessageFormat>
	{
	};
	
	// Входной формат данных функции
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeStorage, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeStorage, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	class MessageReadFile
	{
	public:
		unsigned int FileId;
		unsigned int StartOffset;
		unsigned int Size;
	protected:
		bool _valid;
	public:
		MessageReadFile()
		{
			FileId = -1;
			StartOffset = -1;
			Size = 0;
		}
		
		MessageReadFile(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(FileId) + sizeof(StartOffset) + sizeof(Size))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			FileId = *(unsigned int *)&src[0];
			StartOffset = *(unsigned int *)&src[sizeof(FileId)];
			Size = *(unsigned int *)&src[sizeof(FileId) + sizeof(StartOffset)];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(FileId) + sizeof(StartOffset) + sizeof(Size))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = FileId;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = StartOffset;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 24) & 0xFF;
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

	class MessageReadFileResult
	{
	public:
		ResultCode Result;
		unsigned int FileId;
		unsigned int StartOffset;
		unsigned int Size;
		unsigned char *Data;
	public:
		MessageReadFileResult()
		{
			Result = ResultUnknown;
			FileId = -1;
			StartOffset = -1;
			Size = 0;
			Data = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Result) + sizeof(FileId) + sizeof(StartOffset) + sizeof(Size) + Size || (Data == 0 && Size > 0))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Result;
			dstIndex++;
			
			dstPtr[dstIndex] = FileId;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = StartOffset;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (StartOffset >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = Size;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (Size >> 24) & 0xFF;
			dstIndex++;
			
			for (int i = 0; i < Size; i++)
			{
				dstPtr[dstIndex] = Data[i];
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	class MessagePushFile
	{
	public:
		unsigned int PushAddress;
		unsigned int FileId;
		unsigned int PartSize;
		BitFlagsHolder RequestedParts;
	protected:
		bool _valid;
	public:
		MessagePushFile()
		{
			PushAddress = -1;
			FileId = -1;
			PartSize = 0;
		}
		
		MessagePushFile(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(PushAddress) + sizeof(FileId) + sizeof(PartSize))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			PushAddress = *(unsigned int *)&src[0];
			FileId = *(unsigned int *)&src[sizeof(PushAddress)];
			PartSize = *(unsigned int *)&src[sizeof(PushAddress) + sizeof(FileId)];
			
			unsigned char filedsLen = sizeof(PushAddress) + sizeof(FileId) + sizeof(PartSize);

			RequestedParts.Deserialize(src + filedsLen, dataCount - filedsLen);

			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(PushAddress) + sizeof(FileId) + sizeof(PartSize) + RequestedParts.SerializedSize())
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = PushAddress;
			dstIndex++;
			dstPtr[dstIndex] = (PushAddress >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PushAddress >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PushAddress >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = FileId;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = PartSize;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 24) & 0xFF;
			dstIndex++;
			
			dstIndex += RequestedParts.Serialize(dstPtr + dstIndex);

			return dstIndex;
		}
	};
	
	class MessagePushFileResult
	{
	public:
		ResultCode Result;
		unsigned int FileId;
		unsigned int FileSize;
		unsigned int PartSize;
		unsigned int PartsCount;
	public:
		MessagePushFileResult()
		{
			Result = ResultUnknown;
			FileId = -1;
			FileSize = 0;
			PartSize = 0;
			PartsCount = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Result) + sizeof(FileId) + sizeof(FileSize) + sizeof(PartSize) + sizeof(PartsCount))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Result;
			dstIndex++;
			
			dstPtr[dstIndex] = FileId;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FileId >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = FileSize;
			dstIndex++;
			dstPtr[dstIndex] = (FileSize >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FileSize >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (FileSize >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = PartSize;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartSize >> 24) & 0xFF;
			dstIndex++;
			
			dstPtr[dstIndex] = PartsCount;
			dstIndex++;
			dstPtr[dstIndex] = (PartsCount >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartsCount >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (PartsCount >> 24) & 0xFF;
			dstIndex++;

			return dstIndex;
		}
	};

}
}
}
}
}

#endif