///////////////////////////////////////////////////////////////////////////////
//	‘орматы данных примен€емые в сесси€х
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TRANSPORT_SESSIONDATA_H
#define MODEM_TRANSPORT_SESSIONDATA_H

namespace Rblib
{
namespace Modem
{
namespace Transport
{
	enum SessionDataType
	{
		SessionDataTypeUnknown										= 0,
		SessionDataTypeFastSingleDestination						= 1,
		SessionDataTypeFastSingleDestinationWithRouting				= 2,
		SessionDataTypeFastMultyDestination							= 3,
		SessionDataTypeAck											= 8,
		SessionDataTypeNotifyIntermediateDelivery					= 10,
	};
	
	template
		<
			typename TicketType						// тип идентификатора сессии
		>
	class SessionDataBase
	{
	public:
		SessionDataType DataType;
		unsigned char PacketId;
		TicketType Ticket;
	protected:
		bool _valid;
		static const int _baseContentSize = sizeof(DataType) + sizeof(PacketId) + sizeof(Ticket);
	public:
		static const unsigned int RequiredDataCount = _baseContentSize;
	private:
		SessionDataBase()
		{
		}
	public:
		SessionDataBase(SessionDataType dataType, unsigned char packetId, TicketType ticket)
		{
			DataType = dataType;
			PacketId = packetId;
			Ticket = ticket;
			_valid = false;
		}
		
		SessionDataBase(unsigned char *data, int dataCount)
		{
			_valid = false;
			int i = 0;
			bool outOfBounds = false;
			i += SafeCopy(data, dataCount, i, &DataType, sizeof(DataType), 0, sizeof(DataType), outOfBounds);
			i += SafeCopy(data, dataCount, i, &PacketId, sizeof(PacketId), 0, sizeof(PacketId), outOfBounds);
			i += SafeCopy(data, dataCount, i, &Ticket, sizeof(Ticket), 0, sizeof(Ticket), outOfBounds);
			
			_valid = _baseContentSize == i && !outOfBounds;
		}
		
		int ToByteArray(unsigned char *buffer, int bufferSize, bool &outOfBounds)
		{
			if (!_valid)
			{
				return 0;
			}
			
			int i = 0;
			
			i += SafeCopy(&DataType, sizeof(DataType), 0, buffer, bufferSize, i, sizeof(DataType), outOfBounds);
			i += SafeCopy(&PacketId, sizeof(PacketId), 0, buffer, bufferSize, i, sizeof(PacketId), outOfBounds);
			i += SafeCopy(&Ticket, sizeof(Ticket), 0, buffer, bufferSize, i, sizeof(Ticket), outOfBounds);
			
			return i;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		static int SafeCopy(void *src, int srcSize, int srcOffset, void *dst, int dstSize, int dstOffset, int count, bool &outOfBounds)
		{
			int copied = 0;
			
			if (src == 0 || dst == 0)
			{
				return copied;
			}
			
			unsigned char *srcPtr = (unsigned char *)src;
			unsigned char *dstPtr = (unsigned char *)dst;
			
			int srcIndex = srcOffset;
			int dstIndex = dstOffset;
			
			for (int i = 0; i < count && srcIndex < srcSize && dstIndex < dstSize; i++, srcIndex++, dstIndex++)
			{
				dstPtr[dstIndex] = srcPtr[srcIndex];
				copied++;
			}
			
			outOfBounds |= srcOffset + count > srcSize || dstOffset + count > dstSize || copied != count;
			
			return copied;
		}
	};
	
	template
		<
			typename AddressType,						// тип идентификатора узла,
			typename TicketType,						// тип идентификатора сессии
			int MaxPacketSize							// максимальный размер пакета сессии
		>
	class SessionDataFastSingleDestination : public SessionDataBase<TicketType>
	{
	public:
		typedef SessionDataBase<TicketType> Base;
		static const SessionDataType RequiredDataType = SessionDataTypeFastSingleDestination;
	public:
		AddressType DestinationAddress;
		unsigned short DataCount;
		unsigned char *Data;
	protected:
		static const int _fixedContentSize = Base::_baseContentSize + sizeof(DestinationAddress) + sizeof(DataCount);
	public:
		// ћаксимальный размер полезных данных в этом пакете
		static const int MaxDataSize = MaxPacketSize - _fixedContentSize;
	public:
		SessionDataFastSingleDestination(unsigned char packetId, TicketType ticket, AddressType dst, unsigned char *data, int dataCount) : Base(RequiredDataType, packetId, ticket)
		{
			DestinationAddress = dst;
			DataCount = dataCount;
			Data = data;
			Base::_valid = Data != 0 && DataCount > 0;
		}
		
		SessionDataFastSingleDestination(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
			if (!Base::_valid)
			{
				return;
			}
			
			int i = Base::_baseContentSize;
			bool outOfBounds = false;
			i += Base::SafeCopy(data, dataCount, i, &DestinationAddress, sizeof(DestinationAddress), 0, sizeof(DestinationAddress), outOfBounds);
			i += Base::SafeCopy(data, dataCount, i, &DataCount, sizeof(DataCount), 0, sizeof(DataCount), outOfBounds);
			
			Base::_valid = _fixedContentSize == i && (dataCount - i) >= DataCount && !outOfBounds;
			
			if (Base::_valid)
			{
				Data = &data[i];
			}
		}
		
		int ToByteArray(unsigned char *buffer, int bufferSize, bool &outOfBounds)
		{
			if (!Base::_valid)
			{
				return 0;
			}
			
			int i = Base::ToByteArray(buffer, bufferSize, outOfBounds);
			
			i += Base::SafeCopy(&DestinationAddress, sizeof(DestinationAddress), 0, buffer, bufferSize, i, sizeof(DestinationAddress), outOfBounds);
			i += Base::SafeCopy(&DataCount, sizeof(DataCount), 0, buffer, bufferSize, i, sizeof(DataCount), outOfBounds);
			i += Base::SafeCopy(Data, DataCount, 0, buffer, bufferSize, i, DataCount, outOfBounds);
			
			return i;
		}
			
		static int GetMinPacketSize()
		{
			return _fixedContentSize;
		}
	};
	
	template
		<
			typename AddressType,						// тип идентификатора узла,
			typename TicketType,						// тип идентификатора сессии
			int MaxPacketSize							// максимальный размер пакета сессии
		>
	class SessionDataFastSingleDestinationWithRouting : public SessionDataBase<TicketType>
	{
	public:
		typedef SessionDataBase<TicketType> Base;
		static const SessionDataType RequiredDataType = SessionDataTypeFastSingleDestinationWithRouting;
	public:
		float DeliveryTimeout;
		AddressType InitialAddress;
		AddressType IntermediateAddress;
		AddressType FinalAddress;
		unsigned short DataCount;
		unsigned char *Data;
	protected:
		static const int _fixedContentSize = Base::_baseContentSize + sizeof(DeliveryTimeout) + sizeof(InitialAddress) + sizeof(IntermediateAddress) + sizeof(FinalAddress) + sizeof(DataCount);
	public:
		// ћаксимальный размер полезных данных в этом пакете
		static const int MaxDataSize = MaxPacketSize - _fixedContentSize;
	public:
		SessionDataFastSingleDestinationWithRouting(unsigned char packetId, TicketType ticket, float deliveryTimeout, AddressType initialAddress, AddressType intermediateAddress, AddressType finalAddress,
													unsigned char *data, int dataCount) : Base(RequiredDataType, packetId, ticket)
		{
			DeliveryTimeout = deliveryTimeout;
			InitialAddress = initialAddress;
			IntermediateAddress = intermediateAddress;
			FinalAddress = finalAddress;
			DataCount = dataCount;
			Data = data;
			
			Base::_valid = Data != 0 && DataCount > 0;
		}
		
		SessionDataFastSingleDestinationWithRouting(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
			if (!Base::_valid)
			{
				return;
			}
			
			int i = Base::_baseContentSize;
			bool outOfBounds = false;
			i += Base::SafeCopy(data, dataCount, i, &DeliveryTimeout, sizeof(DeliveryTimeout), 0, sizeof(DeliveryTimeout), outOfBounds);
			i += Base::SafeCopy(data, dataCount, i, &InitialAddress, sizeof(InitialAddress), 0, sizeof(InitialAddress), outOfBounds);
			i += Base::SafeCopy(data, dataCount, i, &IntermediateAddress, sizeof(IntermediateAddress), 0, sizeof(IntermediateAddress), outOfBounds);
			i += Base::SafeCopy(data, dataCount, i, &FinalAddress, sizeof(FinalAddress), 0, sizeof(FinalAddress), outOfBounds);
			i += Base::SafeCopy(data, dataCount, i, &DataCount, sizeof(DataCount), 0, sizeof(DataCount), outOfBounds);
			
			Base::_valid = _fixedContentSize == i && (dataCount - i) >= DataCount && !outOfBounds;
			
			if (Base::_valid)
			{
				Data = &data[i];
			}
		}
		
		int ToByteArray(unsigned char *buffer, int bufferSize, bool &outOfBounds)
		{
			if (!Base::_valid)
			{
				return 0;
			}
			
			int i = Base::ToByteArray(buffer, bufferSize, outOfBounds);
			
			i += Base::SafeCopy(&DeliveryTimeout, sizeof(DeliveryTimeout), 0, buffer, bufferSize, i, sizeof(DeliveryTimeout), outOfBounds);
			i += Base::SafeCopy(&InitialAddress, sizeof(InitialAddress), 0, buffer, bufferSize, i, sizeof(InitialAddress), outOfBounds);
			i += Base::SafeCopy(&IntermediateAddress, sizeof(IntermediateAddress), 0, buffer, bufferSize, i, sizeof(IntermediateAddress), outOfBounds);
			i += Base::SafeCopy(&FinalAddress, sizeof(FinalAddress), 0, buffer, bufferSize, i, sizeof(FinalAddress), outOfBounds);
			i += Base::SafeCopy(&DataCount, sizeof(DataCount), 0, buffer, bufferSize, i, sizeof(DataCount), outOfBounds);
			i += Base::SafeCopy(Data, DataCount, 0, buffer, bufferSize, i, DataCount, outOfBounds);
			
			return i;
		}
	};
	
	template
		<
			typename AddressType,						// тип идентификатора узла,
			typename TicketType,						// тип идентификатора сессии
			int MaxPacketSize							// максимальный размер пакета сессии
		>
	class SessionDataFastMultyDestination : public SessionDataBase<TicketType>
	{
	public:
		typedef SessionDataBase<TicketType> Base;
		static const SessionDataType RequiredDataType = SessionDataTypeFastMultyDestination;
	public:
		unsigned char DestinationsCount;
		unsigned short DataCount;
		AddressType *Destinations;
		unsigned char *Data;
	protected:
		static const int _fixedContentSize = Base::_baseContentSize + sizeof(DestinationsCount) + sizeof(DataCount);
	public:
		// ћаксимальное количество целевых узлов в этом пакете
		static const int MaxDestinationCount = (MaxPacketSize - _fixedContentSize) / sizeof(AddressType);
	public:
		SessionDataFastMultyDestination(unsigned char packetId, TicketType ticket, AddressType *dst, unsigned char dstCount, unsigned char *data, int dataCount) : Base(RequiredDataType, packetId, ticket)
		{
			DestinationsCount = dstCount;
			DataCount = dataCount;
			Destinations = dst;
			Data = data;
			Base::_valid = Destinations != 0 && DestinationsCount > 0 && Data != 0 && DataCount > 0;
		}
		
		SessionDataFastMultyDestination(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
			if (!Base::_valid)
			{
				return;
			}
			
			int i = Base::_baseContentSize;
			bool outOfBounds = false;
			i += Base::SafeCopy(data, dataCount, i, &DestinationsCount, sizeof(DestinationsCount), 0, sizeof(DestinationsCount), outOfBounds);
			i += Base::SafeCopy(data, dataCount, i, &DataCount, sizeof(DataCount), 0, sizeof(DataCount), outOfBounds);
			
			Base::_valid = _fixedContentSize == i && (dataCount - i) >= (DataCount + DestinationsCount * sizeof(AddressType)) && !outOfBounds;
			
			if (Base::_valid)
			{
				Destinations = &data[i];
				Data = &data[i + DestinationsCount];
			}
		}
		
		static int GetMaxDataSize(int destinationsCount)
		{
			return MaxPacketSize - _fixedContentSize - destinationsCount * sizeof(AddressType);
		}
		
		int ToByteArray(unsigned char *buffer, int bufferSize, bool &outOfBounds)
		{
			if (!Base::_valid)
			{
				return 0;
			}
			
			int i = Base::ToByteArray(buffer, bufferSize, outOfBounds);
			
			i += Base::SafeCopy(&DestinationsCount, sizeof(DestinationsCount), 0, buffer, bufferSize, i, sizeof(DestinationsCount), outOfBounds);
			i += Base::SafeCopy(&DataCount, sizeof(DataCount), 0, buffer, bufferSize, i, sizeof(DataCount), outOfBounds);
			i += Base::SafeCopy(Destinations, DestinationsCount, 0, buffer, bufferSize, i, DestinationsCount, outOfBounds);
			i += Base::SafeCopy(Data, DataCount, 0, buffer, bufferSize, i, DataCount, outOfBounds);
			
			return i;
		}
		
		bool DestinationsContains(AddressType node)
		{
			if (Destinations == 0)
			{
				return false;
			}
			
			for (int i = 0; i < DestinationsCount; i++)
			{
				if (Destinations[i] == node)
				{
					return true;
				}
			}
			
			return false;
		}
	};
	
	template
		<
			typename AddressType,						// тип идентификатора узла,
			typename TicketType,						// тип идентификатора сессии
			typename DataIdType,						// тип идентификатора подтверждени€ данных
			int MaxPacketSize							// максимальный размер пакета сессии
		>
	class SessionDataAck : public SessionDataBase<TicketType>
	{
	public:
		typedef SessionDataBase<TicketType> Base;
		static const SessionDataType RequiredDataType = SessionDataTypeAck;
	public:
		unsigned char IdsCount;
		DataIdType *DataIds;
		bool *FinalFlags;
	protected:
		static const int _fixedContentSize = Base::_baseContentSize + sizeof(IdsCount);
	public:
		// ћаксимальное кол-во подтверждений, которое помещаетс€ в этот пакет
		static const int MaxCount = (MaxPacketSize - _fixedContentSize) / (sizeof(DataIdType) + sizeof(bool));
	public:
		SessionDataAck(TicketType ticket, unsigned char *dataIds, bool *finalFlags, unsigned char idsCount) : Base(RequiredDataType, 0, ticket)
		{
			DataIds = dataIds;
			FinalFlags = finalFlags;
			IdsCount = idsCount;
			Base::_valid = IdsCount > 0 && DataIds != 0;
		}
		
		SessionDataAck(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
			if (!Base::_valid)
			{
				return;
			}
			
			int i = Base::_baseContentSize;
			bool outOfBounds = false;
			i += Base::SafeCopy(data, dataCount, i, &IdsCount, sizeof(IdsCount), 0, sizeof(IdsCount), outOfBounds);
			
			Base::_valid = _fixedContentSize == i && (dataCount - i) >= IdsCount * (sizeof(DataIdType) + sizeof(bool)) && !outOfBounds;
			
			if (Base::_valid)
			{
				DataIds = &data[i];
				FinalFlags = (bool *)&data[i + IdsCount];
			}
		}
		
		int ToByteArray(unsigned char *buffer, int bufferSize, bool &outOfBounds)
		{
			if (!Base::_valid)
			{
				return 0;
			}
			
			int i = Base::ToByteArray(buffer, bufferSize, outOfBounds);
			
			i += Base::SafeCopy(&IdsCount, sizeof(IdsCount), 0, buffer, bufferSize, i, sizeof(IdsCount), outOfBounds);
			i += Base::SafeCopy(DataIds, IdsCount, 0, buffer, bufferSize, i, IdsCount, outOfBounds);
			i += Base::SafeCopy(FinalFlags, IdsCount, 0, buffer, bufferSize, i, IdsCount, outOfBounds);
			
			return i;
		}
		
		static int CalckPacketSize(int count)
		{
			return _fixedContentSize + count * (sizeof(DataIdType) + sizeof(bool));
		}
	};
	
	template
		<
			typename AddressType,						// тип идентификатора узла,
			typename TicketType,						// тип идентификатора сессии
			typename DataIdType,						// тип идентификатора подтверждени€ данных
			int MaxPacketSize							// максимальный размер пакета сессии
		>
	class SessionDataNotifyIntermediateDelivery : public SessionDataBase<TicketType>
	{
	public:
		typedef SessionDataBase<TicketType> Base;
		static const SessionDataType RequiredDataType = SessionDataTypeNotifyIntermediateDelivery;
	public:
		AddressType DestinationAddress;
		DataIdType DataId;
	protected:
		static const int _fixedContentSize = Base::_baseContentSize + sizeof(DestinationAddress) + sizeof(DataId);
	public:
		// ћаксимальный размер полезных данных в этом пакете
		static const int MaxDataSize = MaxPacketSize - _fixedContentSize;
	public:
		SessionDataNotifyIntermediateDelivery(unsigned char packetId, TicketType ticket, AddressType dst, DataIdType dataId) : Base(RequiredDataType, packetId, ticket)
		{
			DestinationAddress = dst;
			DataId = dataId;
			Base::_valid = 1;
		}
		
		SessionDataNotifyIntermediateDelivery(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
			if (!Base::_valid)
			{
				return;
			}
			
			int i = Base::_baseContentSize;
			bool outOfBounds = false;
			i += Base::SafeCopy(data, dataCount, i, &DestinationAddress, sizeof(DestinationAddress), 0, sizeof(DestinationAddress), outOfBounds);
			i += Base::SafeCopy(data, dataCount, i, &DataId, sizeof(DataId), 0, sizeof(DataId), outOfBounds);
			
			Base::_valid = _fixedContentSize == i;
		}
		
		int ToByteArray(unsigned char *buffer, int bufferSize, bool &outOfBounds)
		{
			if (!Base::_valid)
			{
				return 0;
			}
			
			int i = Base::ToByteArray(buffer, bufferSize, outOfBounds);
			
			i += Base::SafeCopy(&DestinationAddress, sizeof(DestinationAddress), 0, buffer, bufferSize, i, sizeof(DestinationAddress), outOfBounds);
			i += Base::SafeCopy(&DataId, sizeof(DataId), 0, buffer, bufferSize, i, sizeof(DataId), outOfBounds);
			
			return i;
		}
			
		static int GetMinPacketSize()
		{
			return _fixedContentSize;
		}
	};
}
}
}
#endif