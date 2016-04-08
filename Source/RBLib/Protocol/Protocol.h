///////////////////////////////////////////////////////////////////////////////
//	Список поддерживаемых библиотекой протоколов
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "../NullType.h"
// Реализация протокола G1
#include "G1/Implementer.h"
// Реализация протокола NMEA
#include "NMEA/Implementer.h"

namespace Rblib
{
namespace Protocol
{
	// Cписок протоколов
	enum ProtocolType
	{
		ProtocolNone = 0,
		ProtocolFirTwig2,
		ProtocolFirTwig3,
		ProtocolWake,
		ProtocolBigWake,
		ProtocolNmea
	};

	template <ProtocolType p,
		class t,
		class u>
	struct ProtocolTypelist
	{
		static const ProtocolType protocol = p;
		typedef t Head;
		typedef u Tail;
	};

	template
	<
		ProtocolType p1 = ProtocolNone, typename t1 = NullType,
		ProtocolType p2 = ProtocolNone, typename t2 = NullType,
		ProtocolType p3 = ProtocolNone, typename t3 = NullType,
		ProtocolType p4 = ProtocolNone, typename t4 = NullType,
		ProtocolType p5 = ProtocolNone, typename t5 = NullType
	> 
	struct MakeProtocolTypelist
	{
	private:
		typedef typename MakeProtocolTypelist
		<
			p2, t2,
			p3, t3,
			p4, t4,
			p5, t5
		>
		::Result TailResult;
	public:
		typedef ProtocolTypelist<p1, t1, TailResult> Result;
	};

	template<>
	struct MakeProtocolTypelist<>
	{
		typedef NullType Result;
	};

	template <class TList, ProtocolType ppp> struct ProtocolSelector;

	template <ProtocolType p, class Head, class Tail>
	struct ProtocolSelector<ProtocolTypelist<p, Head, Tail>, p>
	{
		typedef Head Result;
	};

	template <ProtocolType p, class Head, class Tail, ProtocolType requiredProt>
	struct ProtocolSelector<ProtocolTypelist<p, Head, Tail>, requiredProt>
	{
		typedef typename ProtocolSelector<Tail, requiredProt>::Result Result;
	};
}
}

#endif