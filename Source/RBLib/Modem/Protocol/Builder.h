///////////////////////////////////////////////////////////////////////////////
//	√енератор пакетов
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_PROTOCOL_BUILDER_H
#define MODEM_PROTOCOL_BUILDER_H

#include "../Common/SyncInfo.h"
#include "PacketFormat.h"

namespace Rblib
{
namespace Modem
{
namespace Protocol
{
	template
		<
			int maxPacketSize
		>
	class Builder
	{
	public:
		static const unsigned int MaxPacketSize = maxPacketSize;
		typedef int (MakeTransportSessionCallback) (void *, unsigned char *, int);
		typedef PacketActivityTable<63> PacketActivityTable;
		typedef PacketTransportSession<MaxPacketSize> PacketTransportSession;
		typedef PacketBeaconData<MaxPacketSize> PacketBeaconData;
		typedef PacketTestData<MaxPacketSize> PacketTestData;
		static const unsigned int MaxSessionPacketSize = PacketTransportSession::MaxDataSize;
	public:
		unsigned char PacketData[MaxPacketSize];
		int PacketLen;
	public:
		//  онструктор по умолчанию
		Builder()
		{
			PacketLen = 0;
		}
		
		// Ёквивалент синхропакета дл€ измерени€ времени передачи синхропакета
		bool MakeSyncEquivalent()
		{
			PacketLen = sizeof(PacketSync);
			for (int i = 0; i < PacketLen; i++)
			{
				PacketData[i] = CommandInvalid;
			}
			return true;
		}
		
		// Ёквивалент самого большого пакета дл€ измерени€ времени передачи самого большого пакета
		bool MakeBiggestPacketEquivalent(int packetLen = MaxPacketSize)
		{
			PacketLen = packetLen;
			for (int i = 0; i < PacketLen; i++)
			{
				PacketData[i] = CommandInvalid;
			}
			return true;
		}
		
		// «апрос синхронизации
		bool MakeSyncRequest(int sourceAddress, unsigned int netId)
		{
			PacketSyncRequest pkt(sourceAddress, netId);
			PacketLen = sizeof(pkt);
			memcpy(PacketData, &pkt, PacketLen);
			return true;
		}
		
		// —инхропакет
		bool MakeSync(int sourceAddress, unsigned int netId, float txSyncTime, int slot, NetConfig::NetConfigStruct netConfig, unsigned int netTime, bool netTimeValid)
		{
			PacketSync pkt(sourceAddress, netId, txSyncTime, slot, netConfig, netTime, netTimeValid);
			PacketLen = sizeof(pkt);
			memcpy(PacketData, &pkt, PacketLen);
			return true;
		}
		
		bool MakeActivityTable(int sourceAddress, unsigned int netId, float **ticket, unsigned char **owner, unsigned char **relayMode, unsigned char **itemsCount, unsigned char **nodes, unsigned char **rates, unsigned int *maxItemsCount)
		{
			PacketActivityTable pktBaseData(sourceAddress, netId);
			PacketLen = sizeof(pktBaseData);
			memcpy(PacketData, &pktBaseData, PacketLen);
			
			PacketActivityTable *pkt = (PacketActivityTable *)PacketData;
			
			if (ticket != 0)
			{
				*ticket = &pkt->Ticket;
			}
			
			if (owner != 0)
			{
				*owner = &pkt->Owner;
			}
			
			if (relayMode != 0)
			{
				*relayMode = &pkt->RelayMode;
			}
			
			if (itemsCount != 0)
			{
				*itemsCount = &pkt->ItemsCount;
			}
			
			if (nodes != 0)
			{
				*nodes = pkt->Nodes;
			}
			
			if (rates != 0)
			{
				*rates = pkt->Rates;
			}
			
			if (maxItemsCount != 0)
			{
				*maxItemsCount = PacketActivityTable::MaxItemsCount;
			}
			
			
			return true;
		}
		
		bool MakeTransportSession(int sourceAddress, unsigned int netId, void *callbackObject, MakeTransportSessionCallback *callback, bool limitedPacketSize = false, int packetSizeLimit = 0)
		{
			PacketTransportSession pkt(sourceAddress, netId);
			int maxDataSize = pkt.GetMaxDataSize();
			if (limitedPacketSize)
			{
				if (maxDataSize > packetSizeLimit)
				{
					maxDataSize = packetSizeLimit;
				}
			}
			
			if (callback != 0)
			{
				int filled = 0;
				while (filled < maxDataSize)
				{
					unsigned short *partSize = (unsigned short *)&pkt.Data[filled];
					*partSize = 0;
					unsigned char *partBuffer = &pkt.Data[filled + 2];
					int dataCount = callback(callbackObject, partBuffer, maxDataSize - filled - 2);
					
					// больше нет данных
					if (dataCount < 1)
					{
						break;
					}
					
					// данные уже не влезут в один пакет
					if (dataCount + filled > maxDataSize)
					{
						break;
					}
					
					if (dataCount > maxDataSize)
					{
						dataCount = maxDataSize;
					}
					
					*partSize = dataCount;
					
					filled += 2;		// поле с размером части
					filled += *partSize;// поле с содержимым части
				}
				
				pkt.DataCount = filled;
			}
			
			PacketLen = pkt.GetRealPacketSize();
			memcpy(PacketData, &pkt, PacketLen);
			
			return pkt.DataCount > 0;
		}
		
		bool MakeBeaconData(int sourceAddress, unsigned int netId, int packetId, int initialAddress, unsigned char *destinationList, int destonationCount, unsigned char *data, int dataCount)
		{
			PacketBeaconData pkt(sourceAddress, netId, packetId, initialAddress, destinationList, destonationCount, data, dataCount);
			
			PacketLen = pkt.GetRealPacketSize();
			memcpy(PacketData, &pkt, PacketLen);
			
			// пакет считаетс€ построенным, когда в нЄм содержатс€ полезные данные
			return pkt.DataCount > 0;
		}
		
		bool MakeTestPacket(int sourceAddress, unsigned int netId, unsigned char *data, int dataCount)
		{
			PacketTestData pkt(sourceAddress, netId, data, dataCount);
			
			PacketLen = pkt.GetRealPacketSize();
			memcpy(PacketData, &pkt, PacketLen);
			
			// пакет считаетс€ построенным, когда в нЄм содержатс€ полезные данные
			return pkt.DataCount > 0;
		}
	};
}
}
}

#endif