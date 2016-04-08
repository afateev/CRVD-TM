///////////////////////////////////////////////////////////////////////////////
//	—трока таблицы активности узлов сети слышимых напр€мую
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
			typename AddressType,			// тип идентификатор узла
			unsigned int TableLineMaxLen	// максимальный размер строки таблицы активности узла
		>
	class SelfActivityTableLine
	{
	public:
		typedef SelfActivityTableItem ItemType;
		typedef ItemType TableType[TableLineMaxLen];
		typedef CallbackWrapper<int> NodeOfflineCallbackType;
	protected:
		TableType _table;	// строка таблицы активности узла
	public:
		NodeOfflineCallbackType NodeOfflineCallback;
	public:
		//  онструктор по умолчанию
		SelfActivityTableLine()
		{
		}
		
		// —бросить состо€ние
		void Reset()
		{
			for (int i = 0; i < TableLineMaxLen; i++)
			{
				_table[i].Reset();
			}
		}
		
		// «афиксирована активность узла
		void OnActivityDetected(AddressType nodeId, unsigned int normalActivityInterval)
		{
			if (nodeId >= TableLineMaxLen || normalActivityInterval < 1)
			{
				return;
			}
			
			_table[nodeId].OnActivityDetected(normalActivityInterval);
		}
		
		// ”величить счЄтчики интервала проверки
		void IncrementCheckIntervalCounters()
		{
			for (int i = 0; i < TableLineMaxLen; i++)
			{
				_table[i].IncrementCheckIntervalCounter();
			}
		}
		
		// ѕроверить активность
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
		
		// ѕрекращение работы
		bool OnShutDown()
		{
			bool changed = false;
			
			for (int i = 0; i < TableLineMaxLen; i++)
			{
				// сообщим от тех с кем была св€зь но потер€лась
				if (_table[i].GetRate() > 0)
				{
					NodeOfflineCallback.Call(i);
					// с кем то потер€лась, значи надо уведометь о том что собственна€ таблица изменилась (сказать что была не пуста€, а стала пуста€)
					changed = true;
				}
			}
			
			return changed;
		}
		
		// ѕолучить текущий размер таблицы
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