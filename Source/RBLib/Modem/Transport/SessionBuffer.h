///////////////////////////////////////////////////////////////////////////////
//	Буфер данных для сессии
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TRANSPORT_SESSIONBUFFER_H
#define MODEM_TRANSPORT_SESSIONBUFFER_H

#include "../../Utils/queue_static.h"

namespace Rblib
{
namespace Modem
{
namespace Transport
{
	// Кольцевой буфер с произвольным доступом
	template
		<
			class T,
			int Capacity
		>
	class SessionBufferContainer: public queue_static<T, Capacity>
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
	};
	
	// буфер использованных билетов
	template
		<
			typename ticketType,
			int maxCount
		>
	class TransportUsedTycketsBuffer
	{
	public:
		typedef ticketType TicketType;
		static const int MaxCount = maxCount;
		typedef SessionBufferContainer<TicketType, MaxCount> ContainerType;
	protected:
		ContainerType _buffer;
	public:
		void Push(TicketType ticket)
		{
			if (_buffer.size() >= MaxCount - 1)
			{
				_buffer.pop();
			}
			
			_buffer.push(ticket);
		}
		
		bool Present(TicketType ticket)
		{
			for (int i = 0; i < _buffer.size(); i++)
			{
				TicketType tmp = _buffer.get(i);
				if (tmp == ticket)
				{
					return true;
				}
			}
			
			return false;
		}
	};
	
	// Буфер исходящих данных для транспорта
	template
		<
			int capacity
		>
	class TransportOutputBuffer
	{
	public:
		static const int Capacity = capacity;
		typedef SessionBufferContainer<unsigned char, Capacity> ContainerType;
		typedef unsigned short DataCountType;
	protected:
		ContainerType _buffer;
		int _dataChunksCount;
	public:
		TransportOutputBuffer()
		{
			Reset();
		}
		
		// Сброс
		void Reset()
		{
			_buffer.clear();
			_dataChunksCount = 0;
		}
		
		// Свободное место в байтах
		int FreeSpace()
		{
			return Capacity - _buffer.size();
		}
		
		// Можно ли поместить в буфер указанный объем данных
		bool IsFits(DataCountType dataCount)
		{
			return FreeSpace() - sizeof(DataCountType) > dataCount;
		}
		
		bool Push(void *data, DataCountType dataCount)
		{
			if (data == 0 || dataCount < 1 || !IsFits(dataCount))
			{
				return false;
			}
			
			PushData(&dataCount, sizeof(dataCount));
			PushData(data, dataCount);
			
			_dataChunksCount++;
			
			return true;
		}
		
		int Pop(void *buffer, int bufferSize)
		{
			if (buffer == 0 || bufferSize < 1 || _dataChunksCount < 1)
			{
				return 0;
			}
			
			DataCountType dataCount = 0;
			PopData(&dataCount, sizeof(dataCount));
			
			int returned = dataCount;
			if (returned > bufferSize)
			{
				returned = bufferSize;
			}
			
			// извлекаем сколько можно поместить в буфер
			PopData(buffer, returned);
			// остальное выкидываем
			PopData(0, dataCount - returned);
			
			_dataChunksCount--;
			
			return returned;
		}
		
		int Count()
		{
			return _dataChunksCount;
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
	};
	
	// Буфер данных для сессии
	template
		<
			int MaxSessionPacketSize
		>
	class SessionBuffer
	{
	public:
		static const unsigned int MaxDataSize = MaxSessionPacketSize;
	protected:
		bool _used;
		int _dataId;
		unsigned char _buffer[MaxDataSize];
		unsigned int _dataCount;
	public:
		SessionBuffer()
		{
			Reset();
		}
		
		void Reset()
		{
			_used = false;
			_dataId = -1;
			_dataCount = 0;
		}
		
		void ResetIfContains(int dataId)
		{
			if (dataId == _dataId)
			{
				Reset();
			}
		}
		
		bool Empty()
		{
			return !_used;
		}
		
		unsigned int DataCount()
		{
			if (!Empty())
			{
				return _dataCount;
			}
			
			return 0;
		}
		
		bool Set(int dataId, void *data, unsigned int dataCount)
		{
			if (data == 0 || dataCount < 1 || dataCount > MaxDataSize)
			{
				return false;
			}
			
			if (_used)
			{
				if (dataId == _dataId && dataCount == dataCount)
				{
					return true;
				}
			}
			
			for (int i = 0; i < dataCount; i++)
			{
				_buffer[i] = ((unsigned char *)data)[i];
			}
			
			_dataCount = dataCount;
			_dataId = dataId;
			_used = true;
			
			return true;
		}
		
		int Get(void *buffer, int bufferSize, int *dataId = 0)
		{
			if (buffer == 0 || bufferSize < _dataCount)
			{
				return 0;
			}
			
			if (!_used)
			{
				return 0;
			}
			
			int dataCount = _dataCount;
			
			for (int i = 0; i < dataCount; i++)
			{
				((unsigned char *)buffer)[i] = _buffer[i];
			}
			
			if (dataId != 0)
			{
				*dataId = _dataId;
			}
			
			return dataCount;
		}
	};
}
}
}

#endif