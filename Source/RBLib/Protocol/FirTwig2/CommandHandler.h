///////////////////////////////////////////////////////////////////////////////
//	Базовый обработчик команд
//	Протокол FirTwig2
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDHANDLERBASE_FIRTWIG2_H
#define COMMANDHANDLERBASE_FIRTWIG2_H

#include "Parser.h"
#include "PacketFormat.h"
#include "CommandConverter.h"
#include "..\..\Communication\InterfaceBase.h"
#include "..\InternalCommands.h"
#include "..\CommandHandlerBase.h"
#include "RequestParams.h"


namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{

/// \brief Обработчик команд, поступающих в формате FirTwig2.
/// \details Осуществляет разбор пакета и конвертацию команд во внутренний формат
///	с учётом заданной системы команд (CommandSet).
///	Затем уже сконвертированная команда передаётся в CommandHandlerBase 
///	для выполнения обработки по умолчанию и запуска внешних пользовательских обработчиков.
/// \tparam _commandSet Система команд (CommandSet) на основе которой будет осуществляться 
///	конвертация команд из внешнего представления (FirTwig2) во внутреннее.
template<DevCommandSet _commandSet>
class CommandHandler : public CommandHandlerBase
{

public:

	/// Обработака команды.
	/// \details Здесь осуществляется разбор пакета, конвертация команды
	/// и запуск обработчик базового класса _CommandHandlerBase::ProcessCommand.
	inline void PreProcessCommand(unsigned char* data, unsigned char length, Communication::InterfaceBase *outputInterface)
	{
		// инициализация парсера
		Parser parser;
		parser.SelectPacket(data, length);
		parser.SelectCommandSet(_commandSet);

		// разбор пакета
		Parser::PacketSummaryInfo summaryInfo;
		parser.GetSummaryInfo(summaryInfo);

		// пакеты, перенаправленные ведущим узлом, не нужно обрабатывать
		if (summaryInfo.NetAddress != -1 && (summaryInfo.NetAddress & 0x40))
			return;

		// конвертация команды
		CommandConverter::CmdData cmdData;
		CmdInternal cmdInternal = CommandConverter::Convert(summaryInfo, _commandSet, cmdData);

    RequestParams params(summaryInfo);
        
		// обработка команды
		CmdInfo cmd(cmdInternal
					, cmdData.Buffer
					, cmdData.Length
					, params);

		ProcessCommand(&cmd, outputInterface);
	}

};

}
}
}

#endif
