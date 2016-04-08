///////////////////////////////////////////////////////////////////////////////
//	Парсер пактов
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_PROTOCOL_PARSER_H
#define MODEM_PROTOCOL_PARSER_H

#include "../../Utils/CallbackWrapper.h"
#include "../Common/SyncInfo.h"
#include "PacketFormat.h"

namespace Rblib
{
namespace Modem
{
namespace Protocol
{
	struct ActivityTableInfo
	{
		AddressType Source;
		AddressType Owner;
		unsigned char RelayMode;
		float Ticket;
		unsigned char ItemsCount;
		AddressType *Nodes;
		unsigned char *Rates;
		
		ActivityTableInfo(AddressType source, AddressType owner, unsigned char relayMode, float ticket, unsigned char itemsCount, AddressType *nodes, unsigned char *rates)
		{
			Source = source;
			Owner = owner;
			RelayMode = relayMode;
			Ticket = ticket;
			ItemsCount = itemsCount;
			Nodes = nodes;
			Rates = rates;
		}
	};
	
	template
		<
			int MaxPacketSize
		>
	class Parser
	{
	public:
		typedef AddressType AddressType;
		typedef SyncInfo<AddressType> SyncInfoType;
		typedef PacketActivityTable<63> PacketActivityTable;
		typedef PacketTransportSession<MaxPacketSize> PacketTransportSession;
		typedef PacketBeaconData<MaxPacketSize> PacketBeaconData;
		typedef PacketTestData<MaxPacketSize> PacketTestData;
		
		typedef CallbackWrapper<AddressType> OnSyncRequestCallbackType;
		typedef CallbackWrapper<NetConfig::NetConfigStruct> OnNetConfigCallbackType;
		typedef CallbackWrapper<SyncInfoType &> OnSyncInfoCallbackType;
		typedef CallbackWrapper<AddressType, unsigned int, bool> OnNetTimeCallbackType;
		typedef CallbackWrapper<ActivityTableInfo &> OnActivityTableCallbackType;
		typedef CallbackWrapper<AddressType, unsigned char *, int> OnTransportSessionCallbackType;
		typedef CallbackWrapper<AddressType, unsigned char, AddressType, unsigned char *, int, unsigned char *, int> OnBeaconDataCallbackType;
		typedef CallbackWrapper<AddressType, unsigned char *, int, int> OnTestDataCallbackType;
	public:
		OnSyncRequestCallbackType OnSyncRequestCallback;
		OnNetConfigCallbackType OnNetConfigCallback;
		OnSyncInfoCallbackType OnSyncInfoCallback;
		OnNetTimeCallbackType OnNetTimeCallback;
		OnActivityTableCallbackType OnActivityTableCallback;
		OnTransportSessionCallbackType OnTransportSessionCallback;
		OnBeaconDataCallbackType OnBeaconDataCallback;
		OnTestDataCallbackType OnTestDataCallback;
	public:
		// Обработать порцию данных
		void ProcessData(unsigned selfNetId, bool netIdFilterEnabled, void *data, unsigned int dataCount, int rssi)
		{
			if (dataCount >= sizeof(PacketHeader))
			{
				PacketHeader *hdr = (PacketHeader *)data;
				
				// фильтрация пакетов по идентификатору сети
				if (netIdFilterEnabled)
				{
#ifdef RBLIB_MODEM_INCLUDE_NETID_TO_PACKET_HEADER
					if (hdr->NetId != selfNetId)
					{
						return;
					}
#endif
				}
				
				switch (hdr->Cmd)
				{
				case CommandSyncRequest:
					{
						PacketSyncRequest *pkt = (PacketSyncRequest *)data;
						if (dataCount < sizeof(PacketSyncRequest))
						{
							break;
						}
						
						OnSyncRequestCallback.Call(pkt->Hdr.SourceAddress);
					}
					break;
				case CommandSync:
					{
						PacketSync *pkt = (PacketSync *)data;
						if (dataCount < sizeof(PacketSync))
						{
							break;
						}
						
						SyncInfoType syncInfo;
						syncInfo.SourceAddress = pkt->Hdr.SourceAddress;
						syncInfo.TxTime = pkt->TxTime;
						syncInfo.TimeSlot = pkt->TimeSlot;
						
						OnNetConfigCallback.Call(pkt->NetConfig);
						OnSyncInfoCallback.Call(syncInfo);
						OnNetTimeCallback.Call(pkt->Hdr.SourceAddress, pkt->NetTime, pkt->NetTimeValid);
					}
					break;
				case CommandActivityTable:
					{
						PacketActivityTable *pkt = (PacketActivityTable *)data;
						if (dataCount < sizeof(PacketActivityTable))
						{
							break;
						}
						ActivityTableInfo info(pkt->Hdr.SourceAddress, pkt->Owner, pkt->RelayMode, pkt->Ticket, pkt->ItemsCount, pkt->Nodes, pkt->Rates);
						OnActivityTableCallback.Call(info);
					}
					break;
				case CommandTransportSession:
					{
						PacketTransportSession *pkt = (PacketTransportSession *)data;
						if (dataCount < PacketTransportSession::GetMinPacketSize())
						{
							break;
						}
						
						if (pkt->DataCount > 0 && pkt->DataCount <= (dataCount - PacketTransportSession::GetMinPacketSize()))
						{
							int i = 0;
							while (i < pkt->DataCount)
							{
								unsigned short *partSize = (unsigned short *)&pkt->Data[i];
								i += 2;
								unsigned char *partData = &pkt->Data[i];
								i += *partSize;
								
								if (*partSize < 1)
								{
									break;
								}
								
								OnTransportSessionCallback.Call(pkt->Hdr.SourceAddress, partData, *partSize);
							}
						}
					}
					break;
				case CommandBeaconData:
					{
						PacketBeaconData *pkt = (PacketBeaconData *)data;
						if (dataCount < PacketBeaconData::GetMinPacketSize())
						{
							break;
						}
						
						if (pkt->DestinationCount > 0 && pkt->DestinationCount > (dataCount - PacketBeaconData::GetMinPacketSize()))
						{
							break;
						}
						
						if (pkt->DataCount > 0 && pkt->DataCount > (dataCount - pkt->DestinationCount - PacketBeaconData::GetMinPacketSize()))
						{
							break;
						}
						
						OnBeaconDataCallback.Call(pkt->Hdr.SourceAddress, pkt->PacketId, pkt->InitialAddress, pkt->GetDestinationPtr(), pkt->DestinationCount, pkt->GetDataPtr(), pkt->DataCount);
					}
					break;
				case CommandTestData:
					{
						PacketTestData *pkt = (PacketTestData *)data;
						if (dataCount < PacketTestData::GetMinPacketSize())
						{
							break;
						}
						
						if (pkt->DataCount > 0 && pkt->DataCount <= (dataCount - PacketTestData::GetMinPacketSize()))
						{
							OnTestDataCallback.Call(pkt->Hdr.SourceAddress, pkt->Data, pkt->DataCount, rssi);
						}
					}
					break;
				}
			}
		}
	};
}
}
}

#endif