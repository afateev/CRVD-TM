///////////////////////////////////////////////////////////////////////////////
//	����� ������ ������� ���������� ������� � ����������� �� ���������
//	������ �� ���������
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
	// �������� �������� / ������������ ���
	typedef MakeProtocolTypelist<
			ProtocolFirTwig2, FirTwig2::PacketHeaderFilterDefault,
			ProtocolFirTwig3, FirTwig3::PacketHeaderFilterDefault
		>::Result PacketHeaderFilterDefaultTypes;
	
	// �������� �������
	template <ProtocolType p>
	class PacketHeaderFilterDefault: public ProtocolSelector<PacketHeaderFilterDefaultTypes, p>::Result
	{
	};
}
}

#endif
