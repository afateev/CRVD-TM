#ifndef MODEM_TESTS_DATAQUEUE_H
#define MODEM_TESTS_DATAQUEUE_H

#include "DataItem.h"

namespace Rblib
{
namespace Modem
{
namespace Tests
{
	class DataQueue
	{
	public:
		static const unsigned int MaxPacketsCount = 10;
		static const unsigned int MaxDataCount = 128;
		typedef DataItem ItemType;
	protected:
		queue_static<ItemType, MaxPacketsCount> _items;
		
		struct DataItem
		{
			unsigned char Data[MaxDataCount];
			
			DataItem(unsigned char *data, int dataCount)
			{
				int count = dataCount;
				if (count > MaxDataCount)
				{
					count = MaxDataCount;
				}
				
				for (int i = 0; i < count; i++)
				{
					Data[i] = data[i];
				}
			}
		};
		
		queue_static<DataItem, MaxPacketsCount> _itemsData;
		bool _lock;
	public:
		DataQueue()
		{
			_lock = false;
		}
		
		bool Push(ItemType item, void *data)
		{
			if (_lock)
			{
				return false;
			}
			
			if (data == 0)
			{
				return false;
			}
			
			if(item.DataCount > MaxDataCount)
			{
				return false;
			}
			
			_lock = true;
			
			bool res = true;
			
			if (_items.size() >= MaxPacketsCount)
			{
				res = false;
				_lock = false;
				ItemType tmp = Pop(0, 0);
				_lock = true;
			}
			
			_items.push(item);
			_itemsData.push(DataItem((unsigned char *)data, item.DataCount));
			
			_lock = false;
			
			return res;
		}
		
		ItemType Pop(void *buffer, int bufferSize)
		{
			while (_lock)
			{
				;
			}
			
			_lock = true;
			
			ItemType tmp;
			if (_items.empty())
			{
				return tmp;
			}
			tmp = _items.front();
			_items.pop();
			
			unsigned char *dst = (unsigned char *)buffer;
			
			for(unsigned int i = 0; i < tmp.DataCount; i++)
			{
				if (dst != 0 && i < bufferSize)
				{
					dst[i] = _itemsData.front().Data[i];
				}
			}
			
			_itemsData.pop();
			
			_lock = false;
			
			return tmp;
		}
		
		bool Empty()
		{
			return _items.empty();
		}
	};
}
}
}

#endif