///////////////////////////////////////////////////////////////////////////////
//	—трока таблицы активности удаленного узла сети
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_ACTIVITY_REMOTEACTIVITYTABLELINE_H
#define MODEM_ACTIVITY_REMOTEACTIVITYTABLELINE_H

#include "RemoteActivityTableItem.h"

namespace Rblib
{
namespace Modem
{
namespace Activity
{
#pragma pack(push, 1)

	template
		<
			unsigned int tableLineMaxLen
		>
	class RemoteActivityTableLine
	{
	public:
		static const unsigned int TableLineMaxLen = tableLineMaxLen;
	public:
		unsigned char RelayMode;
		RemoteActivityTableItem Items[TableLineMaxLen];
		unsigned int Counter;
	public:
		RemoteActivityTableLine()
		{
			Reset();
		}
		
		void Reset()
		{
			RelayMode = 0;
			
			for (int i = 0; i < TableLineMaxLen; i++)
			{
				Items[i].Reset();
			}
			
			Counter = 0;
		}
		
		int Size()
		{
			return TableLineMaxLen;
		}
		
		int GetTable(unsigned char *nodes, unsigned char *rates, unsigned int buffersSize)
		{
			if (nodes == 0 || rates == 0 || buffersSize < 1)
			{
				return 0;
			}
			
			int j = 0;
			
			for (int i = 0; i < TableLineMaxLen; i++)
			{
				nodes[j] = i;
				rates[j] = Items[i].Rate;
				j++;
				if (j >= buffersSize)
				{
					break;
				}
			}
			
			return j;
		}
		
		unsigned char GetTableValue(unsigned char node)
		{
			if (node >= TableLineMaxLen)
			{
				return 0;
			}
			
			return Items[node].Rate;
		}
	};
	
#pragma pack (pop)
}
}
}

#endif