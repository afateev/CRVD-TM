///////////////////////////////////////////////////////////////////////////////
//	Кольцевой буфер
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TRANSPORT_RINGBUFFER_H
#define MODEM_TRANSPORT_RINGBUFFER_H

#include "../../Utils/queue_static.h"

namespace Rblib
{
namespace Modem
{
namespace Transport
{
	// Контейнер кольцевой буфера с произвольным доступом
	template
		<
			class T,
			int Capacity
		>
	class RingBufferContainer: public queue_static<T, Capacity>
	{
	public:
		typedef queue_static<T, Capacity> Base;
	public:
		// возращает ссылку на указанный элемент очереди
		T& get(unsigned int i)
		{
			if(Base::_size == 0 || i >= Base::_size)
			{
				return *((T*)&Base::_buf[0]);
			}
			
			unsigned int temp = Base::_tail + 1 + i;
			if(temp >= Capacity)
			{
				temp -= Capacity;
			}
			
			return Base::_buf[temp];
		}
		
		// заменить значение указанного элемента очереди
		void set(unsigned int i, T& value)
		{
			if(Base::_size == 0 || i >= Base::_size)
			{
				return;
			}
			
			unsigned int temp = Base::_tail + 1 + i;
			if(temp >= Capacity)
			{
				temp -= Capacity;
			}
			
			Base::_buf[temp] = value;
		}
	};
	
	// Кольцевой буфер
	template
		<
			class HeaderType,
			int capacity
		>
	class RingBuffer
	{
	public:
		static const int Capacity = capacity;
		typedef RingBufferContainer<unsigned char, Capacity> ContainerType;
		typedef unsigned short DataCountType;
	protected:
		ContainerType _buffer;
		int _itemsCount;
		volatile bool _nowPush;
		volatile bool _nowPop;
	public:
		RingBuffer()
		{
			Reset();
			_nowPush = false;
			_nowPop = false;
		}
		
		// Сброс
		void Reset()
		{
			_buffer.clear();
			_itemsCount = 0;
		}
		
		// Свободное место в байтах
		int FreeSpace()
		{
			return Capacity - _buffer.size();
		}
		
		// Можно ли поместить в буфер указанный объем данных
		bool IsFits(DataCountType dataCount)
		{
			return FreeSpace() - sizeof(HeaderType) - sizeof(DataCountType) > dataCount;
		}
		
		bool IsFits(void **dataChunk, DataCountType *dataChunkSize, DataCountType DataChunkCount)
		{
			int necessary = 0;
			
			for (int i = 0; i < DataChunkCount; i++)
			{
				void *chunk = dataChunk[i];
				DataCountType chunkSize = dataChunkSize[i];
				if (chunk == 0 || chunkSize < 1)
				{
					return false;
				}
				necessary += sizeof(DataCountType) + chunkSize;
			}
			
			return IsFits(necessary);
		}
		
		// Пусто
		bool Empty()
		{
			return _itemsCount < 1;
		}
		
		int ItemsCount()
		{
			return _itemsCount;
		}
		
		bool Push(HeaderType *header, void **dataChunk, DataCountType *dataChunkSize, DataCountType dataChunkCount, bool waitOnBusy = false)
		{
			if (dataChunk == 0 || dataChunkSize == 0 || dataChunkCount < 1 || !IsFits(dataChunk, dataChunkSize, dataChunkCount))
			{
				return false;
			}
			
			// идёт извлечение, надо подождать
			while(_nowPop)
			{
				// если не можем ждать, выходим с ошибкой
				if (!waitOnBusy)
				{
					return 0;
				}
			}
			
			// идёт пополнение, надо подождать
			while(_nowPush)
			{
				// если не можем ждать, выходим с ошибкой
				if (!waitOnBusy)
				{
					return 0;
				}
			}
			
			_nowPush = true;
			
			PushData(header, sizeof(HeaderType));
			PushData(&dataChunkCount, sizeof(DataCountType));
			for (int i = 0; i < dataChunkCount; i++)
			{
				void *chunk = dataChunk[i];
				DataCountType chunkSize = dataChunkSize[i];
				
				PushData(&chunkSize, sizeof(DataCountType));
				PushData(chunk, chunkSize);
			}
			
			_itemsCount++;
			
			_nowPush = false;
			
			return true;
		}
		
		int Get(int start, HeaderType *header, void **dataChunk, DataCountType **dataChunkSize, DataCountType *dataChunkCount)
		{
			if (_itemsCount < 1)
			{
				return start;
			}
			
			int i = start;
			i += Get(header, sizeof(HeaderType), i);
			DataCountType chunkCount = 0;
			i += Get(&chunkCount, sizeof(DataCountType), i);
			for (int j = 0; j < chunkCount; j++)
			{
				DataCountType dataCnt = 0;
				i += Get(&dataCnt, sizeof(DataCountType), i);
				
				void *dst = 0;
				if (dataChunk != 0)
				{
					dst = dataChunk[j];
				}
				i += Get(dst, dataCnt, i);
				
				if (dataChunkSize != 0)
				{
					if (dataChunkSize[j] != 0)
					{
						*dataChunkSize[j] = dataCnt;
					}
				}
			}
			
			if (dataChunkCount != 0)
			{
				*dataChunkCount = chunkCount;
			}
			
			return i;
		}
		
		void UpdateFrontHeader(HeaderType *header)
		{
			if (_itemsCount < 1)
			{
				return;
			}
			
			Set(header, sizeof(HeaderType), 0);
		}
		
		void Pop(HeaderType *header, void **dataChunk, DataCountType **dataChunkSize, DataCountType *dataChunkCount)
		{
			if (_itemsCount < 1)
			{
				return;
			}
			
			// идёт запихивание, нельзя извлекать
			if (_nowPush)
			{
				return;
			}
			
			_nowPop = true;
			
			PopData(header, sizeof(HeaderType));
			DataCountType chunkCount = 0;
			PopData(&chunkCount, sizeof(DataCountType));
			for (int j = 0; j < chunkCount; j++)
			{
				DataCountType dataCnt = 0;
				PopData(&dataCnt, sizeof(DataCountType));
				
				void *dst = 0;
				if (dataChunk != 0)
				{
					dst = dataChunk[j];
				}
				PopData(dst, dataCnt);
				
				if (dataChunkSize != 0)
				{
					if (dataChunkSize[j] != 0)
					{
						*dataChunkSize[j] = dataCnt;
					}
				}
			}
			
			if (dataChunkCount != 0)
			{
				*dataChunkCount = chunkCount;
			}
			
			_itemsCount--;
			
			_nowPop = false;
		}
		
		void RoolOverToNextChunk()
		{
			int frontChunkSize = Get(0, 0, 0, 0, 0);
			RollOver(frontChunkSize);
		}
		
		bool NowPush()
		{
			return _nowPush;
		}
	protected:
		void PushData(void *data, DataCountType dataCount)
		{
			unsigned char *src = (unsigned char *)data;
			
			for (int i = 0; i < dataCount; i++)
			{
				_buffer.push(src[i]);
			}
		}
		
		int Get(void *buffer, DataCountType dataCount, unsigned int offset = 0)
		{
			unsigned char *dst = (unsigned char *)buffer;
			
			if (buffer != 0 && dataCount > 0)
			{
				for (int i = 0; i < dataCount; i++)
				{
					if(_buffer.size() < i)
					{
						break;
						
					}
					
					if (dst != 0)
					{
						dst[i] = _buffer.get(i + offset);
					}
				}
			}
			
			return dataCount;
		}
		
		void Set(void *buffer, DataCountType dataCount, unsigned int offset = 0)
		{
			unsigned char *src = (unsigned char *)buffer;
			
			if (buffer != 0 && dataCount > 0)
			{
				for (int i = 0; i < dataCount; i++)
				{
					if(_buffer.size() < i)
					{
						break;
						
					}
					
					if (src != 0)
					{
						_buffer.set(i + offset, src[i]);
					}
				}
			}
			
			return;
		}
		
		void PopData(void *buffer, DataCountType dataCount)
		{
			unsigned char *dst = (unsigned char *)buffer;
			
			for (int i = 0; i < dataCount; i++)
			{
				if(_buffer.empty())
				{
					break;
				}
				
				if (dst != 0)
				{
					dst[i] = _buffer.front();
				}
				_buffer.pop();
			}
		}
		void RollOver(int count)
		{
			unsigned char tmp;
			for (int i = 0; i < count; i++)
			{
				tmp = _buffer.front();
				_buffer.pop();
				_buffer.push(tmp);
			}
		}
	};
}
}
}

#endif