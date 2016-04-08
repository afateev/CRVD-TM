#ifndef MODEM_DEBUGDATAQUEUE_QUEUE_H
#define MODEM_DEBUGDATAQUEUE_QUEUE_H

#include "Item.h"

namespace Rblib
{
namespace Modem
{
namespace DebugDataQueue
{
	template
		<
			class CodeType,
			unsigned int maxPacketsCount,
			unsigned int dataBufferSize
		>
	class Queue
	{
	public:
		static const unsigned int MaxPacketsCount = maxPacketsCount;
		static const unsigned int DataBufferSize = dataBufferSize;
		typedef Item<CodeType> ItemType;
	protected:
		queue_static<ItemType, MaxPacketsCount> _items;
		queue_static<unsigned char, DataBufferSize> _itemsData;
	public:
		bool Push(ItemType item, void *data)
		{
			if (data == 0)
			{
				return false;
			}
			
			if(item.DataCount > DataBufferSize - _itemsData.size())
			{
				return false;
			}
			
			bool res = true;
			
			if (_items.size() >= MaxPacketsCount)
			{
				res = false;
				ItemType tmp = Pop(0, 0);
			}
			
			_items.push(item);
			unsigned char *src = (unsigned char *)data;
			for(unsigned int i = 0; i < item.DataCount; i++)
			{
				_itemsData.push(src[i]);
			}
			
			return res;
		}
		
		ItemType Front()
		{
			ItemType tmp;
			if (_items.empty())
			{
				return tmp;
			}
			tmp = _items.front();
			
			return tmp;
		}
		
		ItemType Pop(void *buffer, int bufferSize)
		{
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
					dst[i] = _itemsData.front();
				}
				_itemsData.pop();
			}
			
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