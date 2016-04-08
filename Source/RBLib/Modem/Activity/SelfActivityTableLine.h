///////////////////////////////////////////////////////////////////////////////
//	������ ������� ���������� ����� ���� �������� ��������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_ACTIVITY_SELFACTIVITYTABLELINE_H
#define MODEM_ACTIVITY_SELFACTIVITYTABLELINE_H

#include "../../Utils/CallbackWrapper.h"
#include "../../Utils/map_static.h"
#include "SelfActivityTableItem.h"

namespace Rblib
{
namespace Modem
{
namespace Activity
{
	template
		<
			typename AddressType,			// ��� ������������� ����
			unsigned int TableLineMaxLen	// ������������ ������ ������ ������� ���������� ����
		>
	class SelfActivityTableLine
	{
	public:
		typedef SelfActivityTableItem ItemType;
		typedef ItemType TableType[TableLineMaxLen];
		typedef CallbackWrapper<int> NodeOfflineCallbackType;
	protected:
		TableType _table;	// ������ ������� ���������� ����
	public:
		NodeOfflineCallbackType NodeOfflineCallback;
	public:
		// ����������� �� ���������
		SelfActivityTableLine()
		{
		}
		
		// �������� ���������
		void Reset()
		{
			for (int i = 0; i < TableLineMaxLen; i++)
			{
				_table[i].Reset();
			}
		}
		
		// ������������� ���������� ����
		void OnActivityDetected(AddressType nodeId, unsigned int normalActivityInterval)
		{
			if (nodeId >= TableLineMaxLen || normalActivityInterval < 1)
			{
				return;
			}
			
			_table[nodeId].OnActivityDetected(normalActivityInterval);
		}
		
		// ��������� �������� ��������� ��������
		void IncrementCheckIntervalCounters()
		{
			for (int i = 0; i < TableLineMaxLen; i++)
			{
				_table[i].IncrementCheckIntervalCounter();
			}
		}
		
		// ��������� ����������
		bool CheckActivity(unsigned int normalActivityInterval, unsigned int checkPeriod)
		{
			if (normalActivityInterval < 1 || checkPeriod < 1)
			{
				return false;
			}
			
			bool changed = false;
			
			for (int i = 0; i < TableLineMaxLen; i++)
			{
				bool nodeRateChanged = _table[i].CheckActivity(normalActivityInterval, checkPeriod);
				
				if (nodeRateChanged && _table[i].GetRate() < 1)
				{
					NodeOfflineCallback.Call(i);
				}
				
				changed |= nodeRateChanged;
			}
			
			return changed;
		}
		
		// ����������� ������
		bool OnShutDown()
		{
			bool changed = false;
			
			for (int i = 0; i < TableLineMaxLen; i++)
			{
				// ������� �� ��� � ��� ���� ����� �� ����������
				if (_table[i].GetRate() > 0)
				{
					NodeOfflineCallback.Call(i);
					// � ��� �� ����������, ����� ���� ��������� � ��� ��� ����������� ������� ���������� (������� ��� ���� �� ������, � ����� ������)
					changed = true;
				}
			}
			
			return changed;
		}
		
		// �������� ������� ������ �������
		int GetSize()
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
				rates[j] = _table[i].GetRate();
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
			
			return  _table[node].GetRate();
		}
	};
}
}
}

#endif