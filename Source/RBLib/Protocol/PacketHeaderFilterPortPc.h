///////////////////////////////////////////////////////////////////////////////
//	����� ������ ������� ���������� ������� � ����������� �� ���������
//	������ ��� ����� ������ � ��
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
	// �������� �������� / ������������ ���
	typedef MakeProtocolTypelist<
			ProtocolFirTwig2, FirTwig2::PacketHeaderFilterPortPc,
			ProtocolFirTwig3, FirTwig3::PacketHeaderFilterPortPc
		>::Result PacketHeaderFilterPortPcTypes;
	
	// �������� �������
	template <ProtocolType p>
	class PacketHeaderFilterPortPc: public ProtocolSelector<PacketHeaderFilterPortPcTypes, p>::Result
	{
	};
}
}

#endif