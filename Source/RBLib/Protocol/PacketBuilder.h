///////////////////////////////////////////////////////////////////////////////
//	Класс выбора строителя пакетов в зависимости от протокола
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETBUILDER_H
#define PACKETBUILDER_H

#include "Protocol.h"
#include "FirTwig2\PacketBuilder.h"
#include "FirTwig3\PacketBuilder.h"

namespace Rblib
{
namespace Protocol
{
	// Селектор протокол / пространство имён
	typedef MakeProtocolTypelist<
			ProtocolFirTwig2, FirTwig2::PacketBuilder,
			ProtocolFirTwig3, FirTwig3::PacketBuilder
		>::Result PacketBuilderTypes;
	
	// Селектор парсера
	template <ProtocolType p>
	class PacketBuilder: public ProtocolSelector<PacketBuilderTypes, p>::Result
	{
	};
}
}

#endif