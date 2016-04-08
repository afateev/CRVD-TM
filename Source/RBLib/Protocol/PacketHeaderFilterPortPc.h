///////////////////////////////////////////////////////////////////////////////
//	Класс выбора фильтра заголовков пакетов в зависимости от протокола
//	Фильтр для порта обмена с ПК
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETHEADERFILTER_PORTPC_H
#define PACKETHEADERFILTER_PORTPC_H

#include "Protocol.h"
#include "FirTwig2\PacketHeaderFilterPortPc.h"
#include "FirTwig3\PacketHeaderFilterPortPc.h"

namespace Rblib
{
namespace Protocol
{
	// Селектор протокол / пространство имён
	typedef MakeProtocolTypelist<
			ProtocolFirTwig2, FirTwig2::PacketHeaderFilterPortPc,
			ProtocolFirTwig3, FirTwig3::PacketHeaderFilterPortPc
		>::Result PacketHeaderFilterPortPcTypes;
	
	// Селектор фильтра
	template <ProtocolType p>
	class PacketHeaderFilterPortPc: public ProtocolSelector<PacketHeaderFilterPortPcTypes, p>::Result
	{
	};
}
}

#endif