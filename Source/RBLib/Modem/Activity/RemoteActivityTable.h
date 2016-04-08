///////////////////////////////////////////////////////////////////////////////
//	Таблица активности удаленных узлов сети
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_ACTIVITY_REMOTEACTIVITYTABLE_H
#define MODEM_ACTIVITY_REMOTEACTIVITYTABLE_H

#include "RemoteActivityTableLine.h"
#include "../../Utils/map_static.h"

namespace Rblib
{
namespace Modem
{
namespace Activity
{
#pragma pack(push, 1)

	template
		<
			typename AddressType,
			unsigned int tableLineSize,
			unsigned int tableSize
		>
	class RemoteActivityTable
	{
	public:
		static const unsigned int TableLineSize = tableLineSize;
		static const unsigned int TableSize = tableSize;
		typedef RemoteActivityTableLine<TableLineSize> LineType;
		typedef LineType TableType[TableSize];
		typedef CallbackWrapper<int> RemoteTableCallbackType;
	protected:
		TableType _table;
		bool _present[TableSize];
	public:
		RemoteTableCallbackType RemoteActivityTableChangedCallback;
	public:
		RemoteActivityTable()
		{
			Reset();
		}
		
		void Reset()
		{
			for (int i = 0; i < TableSize; i++)
			{
				_table[i].Reset();
				_present[i] = false;
			}
		}
		
		void UpdateLine(AddressType nodeId, unsigned char relayMode, AddressType *nodes, unsigned char *rates, unsigned int itemsCount)
		{
			if (nodeId >= TableSize || nodes == 0 || rates == 0 || itemsCount < 1)
			{
				return;
			}
			
			bool changed = false;
			
			_present[nodeId] = true;
			
			_table[nodeId].RelayMode = relayMode;
			
			for (unsigned int j = 0; j < itemsCount; j ++)
			{
				AddressType node = nodes[j];
				unsigned char rate = rates[j];
				if (node < TableLineSize)
				{
					if (_table[nodeId].Items[node].Rate != rate)
					{
						changed = true;
					}
					
					_table[nodeId].Items[node].Rate = rate;
				}
			}
			
			_table[nodeId].Counter = 0;
			
			if (changed)
			{
				RemoteActivityTableChangedCallback.Call(nodeId);
			}
		}
		
		void CheckUpdateTimeout(unsigned int threshold)
		{
			for (int i = 0; i < TableSize; i++)
			{
				if (_present[i])
				{
					if (_table[i].Counter > threshold)
					{
						_present[i] = false;
						_table[i].Reset();
						RemoteActivityTableChangedCallback.Call(i);
					}
					else
					{
						_table[i].Counter++;
					}
				}
			}
		}
		
		// Прекращение работы
		bool OnShutDown()
		{
			for (int i = 0; i < TableSize; i++)
			{
				_table[i].Reset();
				if (_present[i])
				{
					RemoteActivityTableChangedCallback.Call(i);
				}
				_present[i] = false;
				
			}
			
			return true;
		}
		
		bool TablePresent(AddressType nodeId)
		{
			if (nodeId >= TableSize)
			{
				return false;
			}
			
			return _present[nodeId];
		}
		
		int GetSize(AddressType nodeId)
		{
			if (nodeId >= TableSize)
			{
				return 0;
			}
			
			int res = 0;
			
			if (_present[nodeId])
			{
				res = _table[nodeId].Size();
			}
			
			return res;
		}
		
		int GetTable(AddressType nodeId, AddressType *nodes, unsigned char *rates, unsigned int buffersSize)
		{
			if (nodeId >= TableSize || nodes == 0 || rates == 0 || buffersSize < 1)
			{
				return 0;
			}
			
			int res = 0;
			
			if (_present[nodeId])
			{
				res = _table[nodeId].GetTable(nodes, rates, buffersSize);
			}
			
			return res;
		}
		
		unsigned char GetTableValue(AddressType remoteNodeId, AddressType node)
		{
			if (remoteNodeId >= TableSize)
			{
				return 0;
			}
			
			if (_present[remoteNodeId])
			{
				return _table[remoteNodeId].GetTableValue(node);
			}

			return 0;
		}
		
		unsigned char GetRelayMode(AddressType remoteNodeId)
		{
			if (remoteNodeId >= TableSize)
			{
				return 0;
			}
			
			if (_present[remoteNodeId])
			{
				return _table[remoteNodeId].RelayMode;
			}

			return 0;
		}
	};
	
#pragma pack (pop)
}
}
}

#endif