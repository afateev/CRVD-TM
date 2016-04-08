///////////////////////////////////////////////////////////////////////////////
//	Информация о цикле сети
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TIMESLOTCHAININFO_H
#define MODEM_TIMESLOTCHAININFO_H

#include "../../Utils/vector_static.h"

namespace Rblib
{
namespace Modem
{
	struct TimeSlotChainInfo
	{
		int MainSlotCount;
		int AddtitionalSlotCount;
		int MainTxSlot;
		int SyncPacketPeriod;
		int SyncTimeout;
		vector_static<int, 255> AdditionalSlotList;
		
		TimeSlotChainInfo()
		{
			Reset();
		}
		
		int TotalSlotCount()
		{
			return MainSlotCount + AddtitionalSlotCount;
		}
		
		void Reset()
		{
			MainSlotCount = 0;
			AddtitionalSlotCount = 0;
			MainTxSlot = -1;
			SyncPacketPeriod = 1;
			SyncTimeout = 1;
			AdditionalSlotList.clear();
		}
	};
}
}

#endif