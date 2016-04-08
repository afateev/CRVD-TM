///////////////////////////////////////////////////////////////////////////////
//	����� ������ ������� ���������� ������� � ����������� �� ���������
//	������ ��� ����� ������ � �������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETHEADERFILTER_PORTMODEM_H
#define PACKETHEADERFILTER_PORTMODEM_H

#include "Protocol.h"
#include "FirTwig2\PacketHeaderFilterPortModem.h"
#include "FirTwig3\PacketHeaderFilterPortModem.h"

namespace Rblib
{
namespace Protocol
{
	// �������� �������� / ������������ ���
	typedef MakeProtocolTypelist<
			ProtocolFirTwig2, FirTwig2::PacketHeaderFilterPortModem,
			ProtocolFirTwig3, FirTwig3::PacketHeaderFilterPortModem
		>::Result PacketHeaderFilterPortModemTypes;
	
	// �������� �������
	template <ProtocolType p>
	class PacketHeaderFilterPortModem: public ProtocolSelector<PacketHeaderFilterPortModemTypes, p>::Result
	{
	};
}
}

#endif