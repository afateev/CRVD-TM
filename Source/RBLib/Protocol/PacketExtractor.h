///////////////////////////////////////////////////////////////////////////////
//	 ласс выбора экстрактора пакетов в зависимости от протокола
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETEXTRACTOR_H
#define PACKETEXTRACTOR_H

#include "Protocol.h"
#include "FirTwig2\PacketExtractor.h"
#include "FirTwig3\PacketExtractor.h"
#include "Wake\PacketExtractor.h"
#include "BigWake\PacketExtractor.h"

namespace Rblib
{
namespace Protocol
{
	// Ёкстракторы, использующие указатель на функцию дл€ возврата выделенного пакета.
	typedef MakeProtocolTypelist<
			ProtocolFirTwig2, FirTwig2::PacketExtractor<>,
			ProtocolFirTwig3, FirTwig3::PacketExtractor<>,
			ProtocolWake, Wake::PacketExtractor<>,
			ProtocolBigWake, BigWake::PacketExtractor<>
		>::Result PacketExtractorTypes;

	// Ёкстракторы, использующие обобщЄнный функтор дл€ возврата выделенного пакета.
	typedef MakeProtocolTypelist<
		ProtocolFirTwig2, FirTwig2::PacketExtractor<true>,
		ProtocolFirTwig3, FirTwig3::PacketExtractor<true>,
		ProtocolWake, Wake::PacketExtractor<true>
	>::Result PacketExtractorTypesFunctorCallback;

	// —електор экстрактора
	template <ProtocolType p, bool functorCallback = false>
	class PacketExtractor;

	template <ProtocolType p>
	class PacketExtractor<p, false>: public ProtocolSelector<PacketExtractorTypes, p>::Result
	{
	};

	template <ProtocolType p>
	class PacketExtractor<p, true>: public ProtocolSelector<PacketExtractorTypesFunctorCallback, p>::Result
	{
	};
}
}

#endif