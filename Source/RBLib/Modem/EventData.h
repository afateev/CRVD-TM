#ifndef MODEM_EVENTDATA_H
#define MODEM_EVENTDATA_H

namespace Rblib
{
namespace Modem
{
#pragma pack(push, 1)
	template
		<
			int deliveryHistoryMaxSize
		>
	struct EventData
	{
		static const int StateHistoryMaxSize = 8;
		static const int DeliveryHistoryMaxSize = deliveryHistoryMaxSize;
		
		struct DeliveryHistoryItem
		{
			unsigned char PacketId;
			unsigned char NodeId;
			unsigned char DeliveryResult;
		public:
			DeliveryHistoryItem()
			{
				Reset();
			}
			
			void Reset()
			{
				PacketId = -1;
				NodeId = -1;
				DeliveryResult = -1;
			}
		};
	protected:
		unsigned char _eventDataId;
	public:
		unsigned char Event										: 1;
		unsigned char SelfActivityTableChanged					: 1;
		unsigned char SelfActivityTableRegularDistribution		: 1;
		unsigned char RemoteNodeActivityTableReceived			: 1;
		unsigned char RemoteNodeActivityTableChanged			: 1;
		unsigned char RouteTableChanged							: 1;
		unsigned char DataReceived								: 1;
		unsigned char DebugDataRefilled							: 1;
		unsigned char TestDataPresent							: 1;
		unsigned char StateHistorySize;
		unsigned char StateHistory[StateHistoryMaxSize];
		unsigned char DeliveryHistorySize;
		DeliveryHistoryItem DeliveryHistory[DeliveryHistoryMaxSize];
		unsigned char RemoteNodeActivityTableReceivedList[32];
		unsigned char RemoteNodeActivityTableChangedList[32];
	public:
		EventData()
		{
			_eventDataId = 0;
			Reset();
		}
		
		void Reset()
		{
			// прикаждом сбросе счЄтчик увеличиваетс€, что позвол€ет отличить экземпл€ры структур
			// т.е. оперделить не устрела ли информаци€
			_eventDataId++;
			
			Event = 0;
			SelfActivityTableChanged = 0;
			SelfActivityTableRegularDistribution = 0;
			RemoteNodeActivityTableReceived = 0;
			RemoteNodeActivityTableChanged = 0;
			RouteTableChanged = 0;
			DataReceived = 0;
			DebugDataRefilled = 0;
			TestDataPresent = 0;
			StateHistorySize = 0;
			DeliveryHistorySize = 0;
			
			for (int i = 0; i < StateHistoryMaxSize; i++)
			{
				StateHistory[i] = 0;
			}
			
			for (int i = 0; i < 32; i++)
			{
				RemoteNodeActivityTableReceivedList[i] = 0;
				RemoteNodeActivityTableChangedList[i] = 0;
			}
		}
		
		unsigned char EventDataId()
		{
			return _eventDataId;
		}
		
		void SetSelfActivityTableChanged()
		{
			SelfActivityTableChanged = 1;
			Event = 1;
		}
		
		void SetSelfActivityTableRegularDistribution()
		{
			SelfActivityTableRegularDistribution = 1;
			Event = 1;
		}
		
		void SetRouteTableChanged()
		{
			RouteTableChanged = 1;
			Event = 1;
		}
		
		void SetDataReceived()
		{
			DataReceived = 1;
			Event = 1;
		}
		
		void SetDebugDataRefilled()
		{
			DebugDataRefilled = 1;
			Event = 1;
		}
		
		void SetTestDataPresent()
		{
			TestDataPresent = 1;
			Event = 1;
		}
		
		void SetRemoteNodeActivityTableReceived(int id)
		{
			if (id >= 32 * 8)
			{
				return;
			}
			
			int byte = id / 8;
			int bit = id % 8;
			RemoteNodeActivityTableReceivedList[byte] |= (1 << bit);
			
			RemoteNodeActivityTableReceived = 1;
			Event = 1;
		}
		
		bool GetRemoteNodeActivityTableReceived(int id)
		{
			if (id >= 32 * 8)
			{
				return false;
			}
			
			int byte = id / 8;
			int bit = id % 8;
			return (RemoteNodeActivityTableReceivedList[byte] >> bit) & 1 == 1;
		}
		
		void SetRemoteNodeActivityTableChanged(int id)
		{
			if (id >= 32 * 8)
			{
				return;
			}
			
			int byte = id / 8;
			int bit = id % 8;
			RemoteNodeActivityTableChangedList[byte] |= (1 << bit);
			
			RemoteNodeActivityTableChanged = 1;
			Event = 1;
		}
		
		bool GetRemoteNodeActivityTableChanged(int id)
		{
			if (id >= 32 * 8)
			{
				return false;
			}
			
			int byte = id / 8;
			int bit = id % 8;
			return (RemoteNodeActivityTableChangedList[byte] >> bit) & 1 == 1;
		}
		
		void AddStateToHistory(unsigned char state)
		{
			if (StateHistorySize >= StateHistoryMaxSize)
			{
				StateHistorySize = StateHistoryMaxSize;
				
				for (int i = 0; i < StateHistorySize - 1; i++)
				{
					StateHistory[i] = StateHistory[i + 1];
				}
			}
			
			if (StateHistorySize < StateHistoryMaxSize)
			{
				StateHistorySize++;
			}
			
			StateHistory[StateHistorySize - 1] = state;
			
			Event = 1;
		}
		
		unsigned char GetStateFromHistory(int index)
		{
			if (index < 0 || index >= StateHistorySize)
			{
				return 0;
			}
			
			return StateHistory[index];
		}
		
		void SetDeliveryComplete(unsigned char packetId, unsigned char nodeId, unsigned char deliveryResult)
		{
			if (DeliveryHistorySize >= DeliveryHistoryMaxSize)
			{
				DeliveryHistorySize = DeliveryHistoryMaxSize;
				
				for (int i = 0; i < DeliveryHistorySize - 1; i++)
				{
					DeliveryHistory[i] = DeliveryHistory[i + 1];
				}
			}
			
			if (DeliveryHistorySize < DeliveryHistoryMaxSize)
			{
				DeliveryHistorySize++;
			}
			
			DeliveryHistory[DeliveryHistorySize - 1].PacketId = packetId;
			DeliveryHistory[DeliveryHistorySize - 1].NodeId = nodeId;
			DeliveryHistory[DeliveryHistorySize - 1].DeliveryResult = deliveryResult;
			
			Event = 1;
		}
		
		bool GetDeliveryHistory(int index, unsigned char &packetId, unsigned char &nodeId, unsigned char &deliveryResult)
		{
			if (index < 0 || index >= DeliveryHistorySize)
			{
				return false;
			}
			
			packetId = DeliveryHistory[index].PacketId;
			nodeId = DeliveryHistory[index].NodeId;
			deliveryResult = DeliveryHistory[index].DeliveryResult;
			
			return true;
		}
	};
#pragma pack (pop)
}
}

#endif