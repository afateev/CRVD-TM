///////////////////////////////////////////////////////////////////////////////
//	Класс выбора фильтра заголовков пакетов в зависимости от протокола
//	Фильтр по умолчанию
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETHEADERFILTER_DEFAULT_H
#define PACKETHEADERFILTER_DEFAULT_H

#include "Protocol.h"
#include "FirTwig2\PacketHeaderFilterDefault.h"
#include "FirTwig3\PacketHeaderFilterDefault.h"

namespace Rblib
{
namespace Protocol
{
	// Селектор протокол / пространство имён
	typedef MakeProtocolTypelist<
			ProtocolFirTwig2, FirTwig2::PacketHeaderFilterDefault,
			ProtocolFirTwig3, FirTwig3::PacketHeaderFilterDefault
		>::Result PacketHeaderFilterDefaultTypes;
	
	// Селектор фильтра
	template <ProtocolType p>
	class PacketHeaderFilterDefault: public ProtocolSelector<PacketHeaderFilterDefaultTypes, p>::Result
	{
	};
}
}

#endif
