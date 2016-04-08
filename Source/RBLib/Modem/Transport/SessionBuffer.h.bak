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
	
	// Буфер данных для сессии (старый унивесальный вариант)
	template
		<
			int capacity,
			typename dataIdType = unsigned char
		>
	class SessionBufferOld
	{
	public:
		static const int Capacity = capacity;
		typedef SessionBufferContainer<unsigned char, Capacity> ContainerType;
		typedef dataIdType DataIdType;
		typedef unsigned short DataCountType;
	protected:
		ContainerType _buffer;
		int _dataChunksCount;
	public:
		SessionBufferOld()
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
			return FreeSpace() - sizeof(DataIdType) - sizeof(DataCountType) > dataCount;
		}
		
		// Пусто
		bool Empty()
		{
			return _dataChunksCount < 1;
		}
		
		int Count()
		{
			return _dataChunksCount;
		}
		
		bool Push(DataIdType dataId, void *data, DataCountType dataCount)
		{
			if (data == 0 || dataCount < 1 || !IsFits(dataCount))
			{
				return false;
			}
			
			PushData(&dataId, sizeof(dataId));
			PushData(&dataCount, sizeof(dataCount));
			PushData(data, dataCount);
			
			_dataChunksCount++;
			
			return true;
		}
		
		DataIdType FrontChunkId()
		{
			if (_dataChunksCount < 1)
			{
				return DataIdType();
			}
			
			DataIdType dataId;
			FrontData(&dataId, sizeof(dataId));
			
			return dataId;
		}
		
		int FrontChunkSize()
		{
			if (_dataChunksCount < 1)
			{
				return 0;
			}
			
			DataIdType dataId;
			DataCountType dataCount = 0;
			FrontData(&dataId, sizeof(dataId), 0);
			FrontData(&dataCount, sizeof(dataCount), sizeof(dataId));
			
			return dataCount;
		}
		
		int Pop(void *buffer, int bufferSize, DataIdType *poppedId = 0)
		{
			if (buffer == 0 || bufferSize < 1 || _dataChunksCount < 1)
			{
				return 0;
			}
			
			DataIdType dataId;
			DataCountType dataCount = 0;
			PopData(&dataId, sizeof(dataId));
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
			
			if (poppedId != 0)
			{
				*poppedId = dataId;
			}
			
			return returned;
		}
		
		int Pop(void *buffer, int bufferSize, bool (functor)(DataIdType left, DataIdType right), DataIdType right)
		{
			if (buffer == 0 || bufferSize < 1 || functor == 0 || _dataChunksCount < 1)
			{
				return 0;
			}
			
			int returned = 0;
			
			for (int i = 0; i < _dataChunksCount; i++)
			{
				DataIdType id = FrontChunkId();
				int dataCount = FrontChunkSize();
				returned = dataCount;
				if (returned > bufferSize)
				{
					returned = bufferSize;
				}
				if (functor(id, right))
				{
					PopData(0, sizeof(DataIdType));
					PopData(0, sizeof(DataCountType));
					// извлекаем сколько можно поместить в буфер
					PopData(buffer, returned);
					// остальное выкидываем
					PopData(0, dataCount - returned);
					_dataChunksCount--;
					break;
				}
				else
				{
					RewindToNextChunk();
				}
			}
			
			return returned;
		}
		
		int PopPart(void *buffer, int bufferSize, DataIdType *poppedId = 0)
		{
			if (buffer == 0 || bufferSize < 1 || _dataChunksCount < 1)
			{
				return 0;
			}
			
			int rollOver = _buffer.size();
			
			DataIdType dataId = 0;
			DataCountType dataCount = 0;
			PopData(&dataId, sizeof(dataId));
			PopData(&dataCount, sizeof(dataCount));
			
			rollOver -= dataCount + sizeof(dataId) + sizeof(dataCount);
			
			int returned = dataCount;
			if (returned > bufferSize)
			{
				returned = bufferSize;
			}
			
			// извлекаем сколько можно поместить в буфер
			PopData(buffer, returned);
			
			// если извлекли не всё (что-то осталось от куска)
			if (returned < dataCount)
			{
				dataCount -= returned;
				// засовываем обратно идетнификатор и длину данных
				PushData(&dataId, sizeof(dataId));
				PushData(&dataCount, sizeof(dataCount));
				// перемещаем из головы в хвост неиспользованные данне
				RollOver(dataCount);
				// перематываем очередь так, чтобы текущий кусок данных был опять в начале
				RollOver(rollOver);
			}
			else
			{
				_dataChunksCount--;
			}
			
			return returned;
		}
	
		bool IdPresent(DataIdType dataId)
		{
			bool res = false;
			
			for (int i = 0; i < _dataChunksCount; i++)
			{
				DataIdType id = FrontChunkId();
				if (id == dataId)
				{
					res = true;
				}
				RewindToNextChunk();
			}
			
			return res;
		}
	
		void Remove(DataIdType dataId)
		{
			for (int i = 0; i < _dataChunksCount; i++)
			{
				DataIdType id = FrontChunkId();
				int dataCount = FrontChunkSize();
				if (id == dataId)
				{
					PopData(0, sizeof(DataIdType));
					PopData(0, sizeof(DataCountType));
					PopData(0, dataCount);
					_dataChunksCount--;
					i--;
				}
				else
				{
					RewindToNextChunk();
				}
			}
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
		
		void FrontData(void *buffer, DataCountType dataCount, unsigned int offset = 0)
		{
			unsigned char *dst = (unsigned char *)buffer;
			
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
		
		void RewindToNextChunk()
		{
			if (_dataChunksCount < 1)
			{
				return;
			}
			int dataCount = FrontChunkSize();
			int rollOverCount = sizeof(DataIdType) + sizeof(DataCountType) + dataCount;
			RollOver(rollOverCount);
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
		typedef unsigned char DataCountType;
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
	
	// Буфер исходящих данных для сессии
	template
		<
			int MaxSessionPacketSize,
			int MinDatapacketSize,
			typename dataIdType = unsigned char
		>
	class SessionOutputBuffer
	{
	public:
		typedef dataIdType DataIdType;
		static const unsigned int MaxDataSize = MaxSessionPacketSize;
	protected:
		static const unsigned int MaxPartsCount = 1024 / MinDatapacketSize + 2;
#pragma pack(push, 1)
		class Part
		{
		public:
			bool Used;
			DataIdType Id;
			unsigned char DataCount;
			unsigned char Data[MaxDataSize];
		public:			
			void Reset()
			{
				Used = false;
			}
			
			void Set(DataIdType id, unsigned char *data, unsigned int dataCount)
			{
				Id = id;
				DataCount = dataCount;
				if (data != 0)
				{
					for (int i = 0; i < dataCount; i++)
					{
						Data[i] = data[i];
					}
				}
			}
			
			int GetData(unsigned char *buffer)
			{
				for (int i = 0; i < DataCount; i++)
				{
					buffer[i] = Data[i];
				}
				
				return DataCount;
			}
		};
#pragma pack (pop)
		//static const unsigned int PartSize = sizeof(Part);
		typedef unsigned short DataCountType;
		typedef Part *PartPtr;
		unsigned char _buffer[MaxPartsCount * sizeof(Part)];
		unsigned char _usedCount;
		
	public:
		SessionOutputBuffer()
		{
			Reset();
		}
		
		// Сброс
		void Reset()
		{
			_usedCount = 0;
			for (int i = 0; i < MaxPartsCount; i++)
			{
				PartPtr ptr = GetPtr(i);
				if (ptr != 0)
				{
					ptr->Reset();
				}
			}
		}
		
		int Count()
		{
			return _usedCount;
		}
		
		// Пусто
		bool Empty()
		{
			return Count() < 1;
		}
		
		bool Push(DataIdType dataId, void *data, DataCountType dataCount)
		{
			if (data == 0 || dataCount < 1 || dataCount > MaxDataSize)
			{
				return false;
			}
			
			if (_usedCount >= MaxPartsCount)
			{
				return false;
			}
			
			PartPtr part = GetNextNotUsed();
			if (part == 0)
			{
				return false;
			}
			
			part->Set(dataId, (unsigned char *)data, dataCount);
			part->Used = true;
			
			_usedCount++;
			return true;
		}
		
		int Pop1(void *buffer, int bufferSize, DataIdType *poppedId = 0)
		{
			if (buffer == 0 || bufferSize < MaxDataSize)
			{
				return 0;
			}
			
			if (_usedCount < 1)
			{
				return 0;
			}
			
			int dataCount = 0;
			
			PartPtr part = GetNextUsed();
			if(part != 0)
			{
				dataCount = part->GetData((unsigned char *)buffer);
			
				if (poppedId != 0)
				{
					*poppedId = part->Id;
				}
				
				part->Used = false;
				
				_usedCount--;
			}
			
			return dataCount;
		}
		
		int GetNext(void *buffer, int bufferSize, DataIdType *dataId = 0)
		{
			if (buffer == 0 || bufferSize < MaxDataSize)
			{
				return 0;
			}
			
			if (_usedCount < 1)
			{
				return 0;
			}
			
			int dataCount = 0;
			
			PartPtr part = GetNextUsed();
			if(part != 0)
			{
				dataCount = part->GetData((unsigned char *)buffer);
			
				if (dataId != 0)
				{
					*dataId = part->Id;
				}
			}
			
			return dataCount;
		}
		
		bool Remove(DataIdType dataId)
		{
			for (int i = 0; i < MaxPartsCount; i++)
			{
				PartPtr part = GetPtr(i);
				if (part != 0)
				{
					if (part->Used && part->Id == dataId)
					{
						part->Used = false;
						_usedCount--;
						return true;
					}
				}
			}
			
			return false;
		}
	protected:
		PartPtr GetPtr(unsigned int i)
		{
			if (i >= MaxPartsCount)
			{
				return 0;
			}
			
			return (PartPtr)&_buffer[i * sizeof(Part)];
		}
		
		void ForeachPart(void (functor)(PartPtr))
		{
			if (functor == 0)
			{
				return;
			}
			
			for (int i = 0; i < MaxPartsCount; i++)
			{
				PartPtr ptr = GetPtr(i);
				functor(ptr);
			}
		}
		
		static void Reset(PartPtr part)
		{
			if (part == 0)
			{
				return;
			}
			
			part->Reset();
		}
		
		PartPtr GetFirstNotUsed()
		{
			for (int i = 0; i < MaxPartsCount; i++)
			{
				PartPtr ptr = GetPtr(i);
				if (ptr != 0)
				{
					if (!ptr->Used)
					{
						return ptr;
					}
				}
			}
			
			return 0;
		}
		
		PartPtr GetNextNotUsed()
		{
			static int last = 0;
			
			for (int i = 0; i < MaxPartsCount; i++)
			{
				last++;
				if (last >= MaxPartsCount)
				{
					last = 0;
				}
				PartPtr ptr = GetPtr(last);
				if (ptr != 0)
				{
					if (!ptr->Used)
					{
						return ptr;
					}
				}
			}
			
			return 0;
		}
		
		PartPtr GetNextUsed()
		{
			static int last = MaxPartsCount;
			
			for (int i = 0; i < MaxPartsCount; i++)
			{
				last++;
				if (last >= MaxPartsCount)
				{
					last = 0;
				}
				PartPtr ptr = GetPtr(last);
				if (ptr != 0)
				{
					if (ptr->Used)
					{
						return ptr;
					}
				}
			}
			
			return 0;
		}
	};
	
	// Буфер входящих данных для сессии
	template
		<
			int MaxSessionPacketSize,
			int MinDatapacketSize,
			typename dataIdType = unsigned char
		>
	class SessionInputBuffer
	{
	public:
		typedef dataIdType DataIdType;
		static const unsigned int MaxDataSize = MaxSessionPacketSize;
	protected:
		static const unsigned int MaxPartsCount = 1024 / MinDatapacketSize + 2;
#pragma pack(push, 1)
		class Part
		{
		public:
			bool Used;
			DataIdType Id;
			unsigned char DataCount;
			unsigned char Data[MaxDataSize];
		public:			
			void Reset()
			{
				Used = false;
			}
			
			void Set(DataIdType id, unsigned char *data, unsigned int dataCount)
			{
				Id = id;
				DataCount = dataCount;
				if (data != 0)
				{
					for (int i = 0; i < dataCount; i++)
					{
						Data[i] = data[i];
					}
				}
			}
			
			int GetData(unsigned char *buffer)
			{
				for (int i = 0; i < DataCount; i++)
				{
					buffer[i] = Data[i];
				}
				
				return DataCount;
			}
		};
#pragma pack (pop)
		//static const unsigned int PartSize = sizeof(Part);
		typedef unsigned short DataCountType;
		typedef Part *PartPtr;
		unsigned char _buffer[MaxPartsCount * sizeof(Part)];
		unsigned char _usedCount;
		
	public:
		SessionInputBuffer()
		{
			Reset();
		}
		
		// Сброс
		void Reset()
		{
			_usedCount = 0;
			for (int i = 0; i < MaxPartsCount; i++)
			{
				PartPtr ptr = GetPtr(i);
				if (ptr != 0)
				{
					ptr->Reset();
				}
			}
		}
		
		int Count()
		{
			return _usedCount;
		}
		
		bool Push(DataIdType dataId, unsigned char partNumber, void *data, DataCountType dataCount)
		{
			if (data == 0 || dataCount < 1 || dataCount > MaxDataSize || partNumber >= MaxPartsCount)
			{
				return false;
			}
			
			if (_usedCount >= MaxPartsCount)
			{
				return false;
			}
			
			PartPtr part = GetPtr(partNumber);
			if (part == 0)
			{
				return false;
			}
			
			if (!part->Used)
			{
				part->Set(dataId, (unsigned char *)data, dataCount);
				part->Used = true;
				_usedCount++;
			}
			
			return true;
		}
		
		int GetData(void *buffer, int bufferSize)
		{
			if (buffer == 0)
			{
				return 0;
			}
			
			int dstCount = 0;
			unsigned char *dst = (unsigned char *)buffer;
			
			for (int i = 0; i < MaxPartsCount; i++)
			{
				PartPtr part = GetPtr(i);
				if (part != 0)
				{
					if (part->Used)
					{
						if (dstCount + part->DataCount > bufferSize)
						{
							return dstCount;
						}
						
						dstCount += part->GetData(&dst[dstCount]);
					}
				}
			}
			
			return dstCount;
		}
		
		int GetPartDataPtr(unsigned char partNumber, unsigned char *&partData)
		{
			PartPtr part = GetPtr(partNumber);
			if (part == 0)
			{
				return 0;
			}
			
			partData = part->Data;
			return part->DataCount;
		}
		
		void Remove(unsigned char partNumber)
		{
			if (_usedCount < 1)
			{
				return;
			}
			
			PartPtr part = GetPtr(partNumber);
			if (part == 0)
			{
				return;
			}
			
			if (part->Used)
			{
				part->Used = false;
				_usedCount--;
			}
		}
	protected:
		PartPtr GetPtr(unsigned int partNumber)
		{
			if (partNumber >= MaxPartsCount)
			{
				return 0;
			}
			
			return (PartPtr)&_buffer[partNumber * sizeof(Part)];
		}
		
		void ForeachPart(void (functor)(PartPtr))
		{
			if (functor == 0)
			{
				return;
			}
			
			for (int i = 0; i < MaxPartsCount; i++)
			{
				PartPtr ptr = GetPtr(i);
				functor(ptr);
			}
		}
		
		static void Reset(PartPtr part)
		{
			if (part == 0)
			{
				return;
			}
			
			part->Reset();
		}
		
		PartPtr GetFirstNotUsed()
		{
			for (int i = 0; i < MaxPartsCount; i++)
			{
				PartPtr ptr = GetPtr(i);
				if (ptr != 0)
				{
					if (!ptr->Used)
					{
						return ptr;
					}
				}
			}
			
			return 0;
		}
		
		PartPtr GetNextNotUsed()
		{
			static int last = 0;
			
			for (int i = 0; i < MaxPartsCount; i++)
			{
				last++;
				if (last >= MaxPartsCount)
				{
					last = 0;
				}
				PartPtr ptr = GetPtr(last);
				if (ptr != 0)
				{
					if (!ptr->Used)
					{
						return ptr;
					}
				}
			}
			
			return 0;
		}
		
		PartPtr GetNextUsed()
		{
			static int last = MaxPartsCount;
			
			for (int i = 0; i < MaxPartsCount; i++)
			{
				last++;
				if (last >= MaxPartsCount)
				{
					last = 0;
				}
				PartPtr ptr = GetPtr(last);
				if (ptr != 0)
				{
					if (ptr->Used)
					{
						return ptr;
					}
				}
			}
			
			return 0;
		}
	};
}
}
}

#endif