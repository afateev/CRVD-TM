///////////////////////////////////////////////////////////////////////////////
//	��������� �����
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
	// ��������� ��������� ������ � ������������ ��������
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
		// ��������� ������ �� ��������� ������� �������
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
		
		// �������� �������� ���������� �������� �������
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
	
	// ��������� �����
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
		
		// �����
		void Reset()
		{
			_buffer.clear();
			_itemsCount = 0;
		}
		
		// ��������� ����� � ������
		int FreeSpace()
		{
			return Capacity - _buffer.size();
		}
		
		// ����� �� ��������� � ����� ��������� ����� ������
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
		
		// �����
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
			
			// ��� ����������, ���� ���������
			while(_nowPop)
			{
				// ���� �� ����� �����, ������� � �������
				if (!waitOnBusy)
				{
					return 0;
				}
			}
			
			// ��� ����������, ���� ���������
			while(_nowPush)
			{
				// ���� �� ����� �����, ������� � �������
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
			
			// ��� �����������, ������ ���������
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