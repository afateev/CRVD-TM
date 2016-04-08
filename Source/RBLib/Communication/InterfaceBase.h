///////////////////////////////////////////////////////////////////////////////
//	Базовый класс интерфейса
//	Интерфейс связывает физический интерфейс с парсером пакетов
//	и обработчиком команд
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERFACEBASE_H
#define INTERFACEBASE_H

#include "..\Protocol\PacketQueue.h"
#include "..\Protocol\InternalCommands.h"
#include "..\Protocol\InternalCommands.h"

namespace Rblib
{
namespace Communication
{

/// \brief Интерфейс интерфейса :)
class InterfaceBase
{
public:
	/// \brief Получить уникальный идентификатор интерфейса.
	virtual unsigned char GetId(void) = 0;
	/// \brief Прочитать физический интерфейс, выделить пакеты и положить их в очередь.
	virtual void ReadInterface(void) = 0;
	/// \brief Обработать пакет.
	virtual void ProcessPacket(PacketQueueInfo* packetInfo) = 0;
	/// \brief Обработать исходящий пакет (отправить в физический интерфейс).
	virtual void ProcessOutputPacket(PacketQueueInfo* packetInfo) = 0;
	/// \brief Отправить пакет.
	virtual void SendPacket(unsigned char *data, unsigned int length) = 0;
	/// \brief Построить команду.
	virtual unsigned char *BuildCommand(Protocol::CmdInfo *cmd, unsigned int *packetSize) = 0;
	/// \brief Отправить команду.
	virtual void SendCommand(Protocol::CmdInfo *cmd) = 0;
};

}
}

#endif