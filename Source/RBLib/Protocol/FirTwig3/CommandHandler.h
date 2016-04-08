///////////////////////////////////////////////////////////////////////////////
//	Базовый обработчик команд
//	Протокол FirTwig3
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDHANDLERBASE_FIRTWIG3_H
#define COMMANDHANDLERBASE_FIRTWIG3_H

#include "Parser.h"
#include "..\..\Communication\InterfaceBase.h"
#include "..\InternalCommands.h"
#include "..\CommandHandlerBase.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig3
{

class CommandHandler : public CommandHandlerBase
{
public:

	// Обработать команду (предварительно)
	static inline void PreProcessCommand(unsigned char* data, unsigned char length, Communication::InterfaceBase *outputInterface)
	{
	}
  
};

}
}
}

#endif