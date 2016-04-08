///////////////////////////////////////////////////////////////////////////////
//	Класс выбора парсера в зависимости от протокола
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PARSER_H
#define PARSER_H

#include "Protocol.h"
#include "FirTwig2\Parser.h"
#include "FirTwig3\Parser.h"

namespace Rblib
{
namespace Protocol
{
	// Селектор протокол / пространство имён
	typedef MakeProtocolTypelist<
			ProtocolFirTwig2, FirTwig2::Parser,
			ProtocolFirTwig3, FirTwig3::Parser
		>::Result ParserTypes;
	
	// Селектор парсера
	template <ProtocolType p>
	class Parser: public ProtocolSelector<ParserTypes, p>::Result
	{
	};
}
}

#endif