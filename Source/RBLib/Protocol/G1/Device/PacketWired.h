///////////////////////////////////////////////////////////////////////////////
//	Формат пакета общий для всех устройств
//	Реализация для асинхронных проводных каналов
//	Основана на пакете WAKE
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_DEVICE_PACKETWIRED_H
#define PROTOCOL_G1_DEVICE_PACKETWIRED_H

#include "../../BigWake/Packet.h"
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
	class PacketWired : public BigWake::Packet<InternalBufferLen>
	{
	public:
		typedef BigWake::Packet<InternalBufferLen> Base;
	public:
		PacketWired()
		{
		}
		
		// Конструктор из потока байт (байты должны быть со стаффингом)
		PacketWired(void *data, unsigned int dataLen) : Base(data, dataLen)
		{
		}
		
		// Конструктор по полю команды и полю данных
		PacketWired(CommandCode command, void *data, unsigned int dataLen) : Base(command, data, dataLen)
		{
		}
		
		// Конструктор по полю команды и полю данных
		void Set(CommandCode command, void *data, unsigned int dataLen)
		{
			Base::Set(command, data, dataLen);
		}
		
		CommandCode Command()
		{
			return (CommandCode) Base::Command();
		}
	};
	
#pragma pack (pop)
}
}
}
}

#endif