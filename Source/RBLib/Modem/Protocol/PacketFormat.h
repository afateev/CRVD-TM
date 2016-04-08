///////////////////////////////////////////////////////////////////////////////
//	Формат пакетов в радиоканале
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_PROTOCOL_PACKETFORMAT_H
#define MODEM_PROTOCOL_PACKETFORMAT_H

#include "../NetConfig/NetConfigStruct.h"
#include "../Tests/TestTypeCode.h"

namespace Rblib
{
namespace Modem
{
namespace Protocol
{
#pragma pack(push, 1)
	
	enum Command
	{
		CommandSyncRequest					= 0,	// Запрос синхронизации
		CommandSync							= 1,	// Данные синхронизации
		CommandActivityTable				= 2,	// Таблица активности (или её часть, если не влезла полностью в один пакет)
		CommandTransportSession				= 3,	// Сессия передачи данных
		CommandBeaconData					= 4,	// Данные радиомаяков
		CommandTestData						= 5,	// Тестовые данные
		CommandInvalid						= 0xFF	// Данные, которые не должны интерпретыроваться
	};
	
	typedef unsigned char AddressType;
	
	struct PacketHeader
	{
		Command Cmd;
		AddressType SourceAddress;
#ifdef RBLIB_MODEM_INCLUDE_NETID_TO_PACKET_HEADER
		unsigned int NetId;
#endif
		
		PacketHeader(Command cmd, AddressType sourceAddress, unsigned int netId)
		{
			Cmd = cmd;
			SourceAddress = sourceAddress;
#ifdef RBLIB_MODEM_INCLUDE_NETID_TO_PACKET_HEADER
			NetId = netId;
#endif
		}
	};
	
	struct PacketSyncRequest
	{
		PacketHeader Hdr;
		
		PacketSyncRequest(AddressType sourceAddress, unsigned int netId) : Hdr(CommandSyncRequest, sourceAddress, netId)
		{
			
		}
	};
	
	struct PacketSync
	{
		PacketHeader Hdr;
		float TxTime;
		unsigned short TimeSlot;
		NetConfig::NetConfigStruct NetConfig;
		unsigned int NetTime;
		bool NetTimeValid;
		
		PacketSync(AddressType sourceAddress, unsigned int netId, float txTime, unsigned short timeSlot, NetConfig::NetConfigStruct netConfig, unsigned int netTime, bool netTimeValid) : Hdr(CommandSync, sourceAddress, netId)
		{
			TxTime = txTime;
			TimeSlot = timeSlot;
			NetConfig = netConfig;
			NetTime = netTime;
			NetTimeValid = netTimeValid;
		}
	};
	
	template
		<
			int maxPacketSize
		>
	struct PacketActivityTable
	{
	public:
		PacketHeader Hdr;
		AddressType Owner;			// чья таблица
		unsigned char RelayMode;	// режим ретрансляции
		float Ticket;				// билетик (метка времени)
		unsigned char ItemsCount;	// сколько значимых элементов таблицы в передаваемом пакете
	protected:
		static const unsigned int _fixedPayloadSize = sizeof(Hdr) + sizeof(Owner) + sizeof(Ticket) + sizeof(ItemsCount);
	public:	
		static const unsigned int MaxItemsCountByBuffer = (maxPacketSize - _fixedPayloadSize) / 2;
		static const unsigned int MaxItemsCountBySpecification = 20;
		static const unsigned int MaxItemsCount = MaxItemsCountByBuffer > MaxItemsCountBySpecification ? MaxItemsCountBySpecification : MaxItemsCountByBuffer;
	public:
		AddressType Nodes[MaxItemsCount];		// Список узлов
		unsigned char Rates[MaxItemsCount];		// Список рейтингов соответствующих узлов
	public:
		PacketActivityTable(AddressType sourceAddress, unsigned int netId) : Hdr(CommandActivityTable, sourceAddress, netId)
		{
			Owner = 0;
			RelayMode = 0;
			Ticket = 0;
			ItemsCount = 0;
		}
	};
	
	template
		<
			int maxPacketSize
		>
	struct PacketTransportSession
	{
	public:
		PacketHeader Hdr;
		unsigned short DataCount;	// сколько полезных данных в передаваемом пакете
	protected:
		static const unsigned int _fixedPayloadSize = sizeof(Hdr) + sizeof(DataCount);
	public:	
		static const unsigned int MaxDataSize = (maxPacketSize - _fixedPayloadSize);
	public:
		unsigned char Data[2 * MaxDataSize];		// Данные
	public:
		PacketTransportSession(AddressType sourceAddress, unsigned int netId) : Hdr(CommandTransportSession, sourceAddress, netId)
		{
			DataCount = 0;
		}
		
		static unsigned int GetMinPacketSize()
		{
			return _fixedPayloadSize;
		}
		
		unsigned int GetBufferSize()
		{
			return sizeof(Data);
		}
		
		unsigned int GetMaxDataSize()
		{
			return MaxDataSize;
		}
		
		unsigned int GetRealPacketSize()
		{
			return _fixedPayloadSize + DataCount;
		}
	};
	
	template
		<
			int maxPacketSize
		>
	struct PacketBeaconData
	{
	public:
		PacketHeader Hdr;
		unsigned char PacketId;			// идентификатор пакета
		unsigned char InitialAddress;	// изначальный отправитель пакета
		unsigned char DestinationCount;	// кол-во адресатов
		unsigned short DataCount;		// кол-во данных
	protected:
		static const unsigned int _fixedPayloadSize = sizeof(Hdr) + sizeof(PacketId) + sizeof(InitialAddress) + sizeof(DestinationCount) + sizeof(DataCount);
	public:	
		static const unsigned int BufferSize = maxPacketSize - _fixedPayloadSize;
		static const unsigned int MaxDestinationsCount = 255;
		static const unsigned int MaxDataCount = BufferSize - MaxDestinationsCount;
	protected:
		unsigned char _buffer[BufferSize];
	public:
		PacketBeaconData(AddressType sourceAddress, unsigned int netId, unsigned char packetId, AddressType initialAddress, unsigned char *destinations, unsigned char destinationCount, unsigned char *data, int dataCount) : Hdr(CommandBeaconData, sourceAddress, netId)
		{
			PacketId = packetId;
			InitialAddress = initialAddress;
			DestinationCount = destinationCount;
			
			if (destinations == 0)
			{
				DestinationCount = 0;
			}
			
			DataCount = dataCount;
			if (DataCount > MaxDataCount)
			{
				DataCount = MaxDataCount;
			}
			
			if (data == 0)
			{
				DataCount = 0;
			}
			
			int i = 0;
			
			for (int j = 0; j < DestinationCount; j++, i++)
			{
				_buffer[i] = destinations[j];
			}
			
			for (int j = 0; j < DataCount; j++, i++)
			{
				_buffer[i] = data[j];
			}
		}
		
		unsigned char *GetDestinationPtr()
		{
			return DestinationCount > 0 ? _buffer : 0;
		}
		
		unsigned char *GetDataPtr()
		{
			return DataCount > 0 ? &_buffer[DestinationCount] : 0;
		}
		
		static unsigned int GetMinPacketSize()
		{
			return _fixedPayloadSize;
		}
		
		int GetRealPacketSize()
		{
			return _fixedPayloadSize + DestinationCount + DataCount;
		}
	};
	
	template
		<
			int maxPacketSize
		>
	struct PacketTestData
	{
	public:
		PacketHeader Hdr;
		unsigned short DataCount;	// сколько полезных данных в передаваемом пакете
	protected:
		static const unsigned int _fixedPayloadSize = sizeof(Hdr) + sizeof(DataCount);
	public:	
		static const unsigned int MaxDataSize = (maxPacketSize - _fixedPayloadSize);
	public:
		unsigned char Data[MaxDataSize];		// Данные
	public:
		PacketTestData(AddressType sourceAddress, unsigned int netId, unsigned char *data, int dataCount) : Hdr(CommandTestData, sourceAddress, netId)
		{
			DataCount = dataCount;
			if (DataCount > MaxDataSize)
			{
				DataCount = MaxDataSize;
			}
			
			if (data == 0)
			{
				DataCount = 0;
			}
			
			for (int i = 0; i < DataCount; i++)
			{
				Data[i] = data[i];
			}
		}
		
		static unsigned int GetMinPacketSize()
		{
			return _fixedPayloadSize;
		}
		
		unsigned int GetBufferSize()
		{
			return sizeof(Data);
		}
		
		unsigned int GetMaxDataSize()
		{
			return MaxDataSize;
		}
		
		unsigned int GetRealPacketSize()
		{
			return _fixedPayloadSize + DataCount;
		}
	};
	
	class TestPacketDataFormat
	{
	public:
		unsigned char TestType;
		int TestId;
	public:
		TestPacketDataFormat()
		{
			TestType = 0;
			TestId = -1;
		}
	};
	
	class TestPacketSensitivityDataFormat : public TestPacketDataFormat
	{
	public:
		enum PacketTypeCode
		{
			PacketTypeStart		= 0,
			PacketTypeWork		= 1,
			PacketTypeStop		= 2,
		};
	public:
		unsigned char AttenuatorStep;
		float PowerLevel;
		unsigned int NumberOfRepeats;
		unsigned char PacketType;
	public:
		TestPacketSensitivityDataFormat()
		{
			TestType = Tests::TestTypeCodeSensitivity;
			TestId = -1;
			AttenuatorStep = 0;
			PowerLevel = 0;
			NumberOfRepeats = 0;
			PacketType = 0;
		}
	};
	
#pragma pack (pop)
}
}
}

#endif