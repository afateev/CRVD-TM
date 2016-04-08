///////////////////////////////////////////////////////////////////////////////
//	��������� ������������ ������ ���������� "�����"
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_LIBCONFIG_BASECONFIG_H
#define MODEM_LIBCONFIG_BASECONFIG_H

namespace Rblib
{
namespace Modem
{
namespace LibConfig
{
	class BaseConfig
	{
	public:
		static const int ApiMaxPacketSize = 63;
		static const int MaxNodeCount = 255;				// ������������ ���������� ����� ����
		
		static const int SyncSlotCountPerSecond = 400;
		static const int SyncMaxSlotCountPerCycle = 512;
		static const int SyncMaxAdditionalSlotCount = SyncMaxSlotCountPerCycle - MaxNodeCount; // �� ������ ��� SyncMaxSlotCountPerCycle
		
		static const int RadioTimeout = 5; // �������� � ��������
		
		static const bool SnifferMode = false;
		static const int DeliveryHistoryMaxCount = 8;
		
		static const int MaxSessionsCount = 4;
		static const int MaxActiveOutputSessionsCount = 1;
		
		static const bool UseExternalSync = false;
	};
}
}
}

#endif