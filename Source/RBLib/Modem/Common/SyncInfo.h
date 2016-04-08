///////////////////////////////////////////////////////////////////////////////
//	Информация о синхронизации
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_SYNCINFO_H
#define MODEM_SYNCINFO_H

namespace Rblib
{
namespace Modem
{
	template<class AddressType>
	struct SyncInfo
	{
		AddressType SourceAddress;
		float TxTime;
		unsigned short TimeSlot;
		
		SyncInfo()
		{
			SourceAddress = 0;
			TxTime = 0;
			TimeSlot = 0;
		}
	};
}
}

#endif