///////////////////////////////////////////////////////////////////////////////
//	Формат пакета общий для всех устройств
//	Реализация для беспроводных каналов
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_DEVICE_PACKETWIRELESS_H
#define PROTOCOL_G1_DEVICE_PACKETWIRELESS_H

#include "CommandCode.h"
#include "CommandParams.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Device
{
#pragma pack(push, 1)

	template
		<
			int InternalBufferLen = 255
		>
	class PacketWireless
	{
	public:
		static const int BufferSize = InternalBufferLen;
	protected:
		unsigned char _buffer[BufferSize];
		bool _valid;
	public:
		PacketWireless()
		{
		}
		
		// Конструктор из потока байт
		PacketWireless(void *data, unsigned int dataLen)
		{
			FromByteArray(data, dataLen);
		}
		
		// Конструктор из потока байт
		void FromByteArray(void *data, unsigned int dataLen)
		{
			_valid = false;
			
			if (data == 0 || dataLen < 3)
			{
				return;
			}
			
			int copyCount = dataLen;
			if (copyCount > BufferSize)
			{
				copyCount = BufferSize;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			for (int i = 0; i < copyCount; i++)
			{
				_buffer[i] = src[i];
			}
			
			if (dataLen - 3 < DataLen())
			{
				return;
			}
			
			_valid = true;
		}
		
		// Конструктор по полю команды и полю данных
		PacketWireless(CommandCode command, void *data, unsigned int dataLen)
		{
			Set(command, data, dataLen);
		}
		
		// Конструктор по полю команды и полю данных
		void Set(CommandCode command, void *data, unsigned int dataLen)
		{
			_valid = true;
			_buffer[0] = command;
			_buffer[1] = dataLen;
			_buffer[2] = dataLen >> 8;
			
			int copyCount = dataLen;
			if (copyCount > BufferSize - 3)
			{
				copyCount = BufferSize - 3;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			for (int i = 0; i < copyCount; i++)
			{
				_buffer[i + 3] = src[i];
			}
		}
		
		unsigned char *GetBufferPtr()
		{
			return _buffer;
		}
		
		// Получить представление пакета в виде массива байт
		int ToByteArray(void *dst, int dstSize)
		{
			if (!Valid())
			{
				return 0;
			}
			
			int copyCount = DataLen() + 3;
			if (copyCount > dstSize)
			{
				copyCount = dstSize;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			
			for (int i = 0; i < copyCount; i++)
			{
				dstPtr[i] = _buffer[i];
			}
			
			return copyCount;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		CommandCode Command()
		{
			return (CommandCode) _buffer[0];
		}
		
		// Длина данных
		unsigned int DataLen()
		{
			unsigned int res = _buffer[2];
			res <<= 8;
			res |= _buffer[1];
			
			return res;
		}
		
		// Содержимое поля данных
		unsigned char * Data()
		{
			return &_buffer[3];
		}
	};
	
#pragma pack (pop)
}
}
}
}

#endif