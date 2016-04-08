///////////////////////////////////////////////////////////////////////////////
//	Структура конфигурации узла сети
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_NODECONFIG_NODECONFIGSTRUCT_H
#define MODEM_NODECONFIG_NODECONFIGSTRUCT_H

#include "../NetConfig/NetConfigStruct.h"

namespace Rblib
{
namespace Modem
{
namespace NodeConfig
{
#pragma pack(push, 1)

	struct NodeConfigStructVer1
	{
		static const unsigned char CurrentStructureVersion = 1;
		
		unsigned char StructureVersion;
		NetConfig::NetConfigStructVer1 NetConfig;
		unsigned char Id;
		unsigned int SerialNumber;
		unsigned char Crc;
		
		// Конструктор по умолчанию
		NodeConfigStructVer1()
		{
			Reset();
		}
		
		void Reset()
		{
			StructureVersion = CurrentStructureVersion;
			NetConfig.Reset();

#ifdef NETCONFIG_DEFAULT_MOBILECOMPLEX
			Id = 0;
			SerialNumber = 0;
#else
			Id = 0xFF;
			SerialNumber = 0xFFFFFFFF;
#endif

			UpdateCrc();
		}
		
		unsigned char CalcCrc()
		{
			unsigned char crc = 0xFF;
			unsigned char *buffer = (unsigned char *)this;
			int count = sizeof(NodeConfigStructVer1) - sizeof(Crc);
			
			for (int i = 0; i < count; i++)
			{
				crc += buffer[i];
			}
			
			return crc;
		}
		
		bool VersionValid()
		{
			return StructureVersion == CurrentStructureVersion;
		}
		
		bool CrcValid()
		{
			return Crc == CalcCrc();
		}
		
		void UpdateCrc()
		{
			Crc = CalcCrc();
		}
	};
	
	struct NodeConfigStructVer2
	{
		static const unsigned char CurrentStructureVersion = 2;
		
		unsigned char StructureVersion;
		NetConfig::NetConfigStructVer2 NetConfig;
		unsigned char Id;
		unsigned int SerialNumber;
		unsigned char Crc;
		
		// Конструктор по умолчанию
		NodeConfigStructVer2()
		{
			Reset();
		}
		
		void Reset()
		{
			StructureVersion = CurrentStructureVersion;
			NetConfig.Reset();

#ifdef NETCONFIG_DEFAULT_MOBILECOMPLEX
			Id = 0;
			SerialNumber = 0;
#else
			Id = 0xFF;
			SerialNumber = 0xFFFFFFFF;
#endif

			UpdateCrc();
		}
		
		unsigned char CalcCrc()
		{
			unsigned char crc = 0xFF;
			unsigned char *buffer = (unsigned char *)this;
			int count = sizeof(NodeConfigStructVer2) - sizeof(Crc);
			
			for (int i = 0; i < count; i++)
			{
				crc += buffer[i];
			}
			
			return crc;
		}
		
		bool VersionValid()
		{
			return StructureVersion == CurrentStructureVersion;
		}
		
		bool CrcValid()
		{
			return Crc == CalcCrc();
		}
		
		void UpdateCrc()
		{
			Crc = CalcCrc();
		}
	};
	
	struct NodeConfigStructVer3
	{
		static const unsigned char CurrentStructureVersion = 3;
		
		unsigned char StructureVersion;
		NetConfig::NetConfigStruct3 NetConfig;
		unsigned char Id;
		unsigned int SerialNumber;
		unsigned char Crc;
		
		// Конструктор по умолчанию
		NodeConfigStructVer3()
		{
			Reset();
		}
		
		void Reset()
		{
			StructureVersion = CurrentStructureVersion;
			NetConfig.Reset();

#ifdef NETCONFIG_DEFAULT_MOBILECOMPLEX
			Id = 0;
			SerialNumber = 0;
#else
			Id = 0xFF;
			SerialNumber = 0xFFFFFFFF;
#endif

			UpdateCrc();
		}
		
		unsigned char CalcCrc()
		{
			unsigned char crc = 0xFF;
			unsigned char *buffer = (unsigned char *)this;
			int count = sizeof(NodeConfigStructVer3) - sizeof(Crc);
			
			for (int i = 0; i < count; i++)
			{
				crc += buffer[i];
			}
			
			return crc;
		}
		
		bool VersionValid()
		{
			return StructureVersion == CurrentStructureVersion;
		}
		
		bool CrcValid()
		{
			return Crc == CalcCrc();
		}
		
		void UpdateCrc()
		{
			Crc = CalcCrc();
		}
	};
	
	struct NodeConfigStruct
	{
		static const unsigned char CurrentStructureVersion = 4;
		
		unsigned char StructureVersion;
		NetConfig::NetConfigStruct NetConfig;
		unsigned char Id;
		unsigned int SerialNumber;
		unsigned char Crc;
		
		// Конструктор по умолчанию
		NodeConfigStruct()
		{
			Reset();
		}
		
		void Reset()
		{
			StructureVersion = CurrentStructureVersion;
			NetConfig.Reset();

#ifdef NETCONFIG_DEFAULT_MOBILECOMPLEX
			Id = 3;
			SerialNumber = 0;
#else
			Id = 0xFF;
			SerialNumber = 0xFFFFFFFF;
#endif

			UpdateCrc();
		}
		
		unsigned char CalcCrc()
		{
			unsigned char crc = 0xFF;
			unsigned char *buffer = (unsigned char *)this;
			int count = sizeof(NodeConfigStruct) - sizeof(Crc);
			
			for (int i = 0; i < count; i++)
			{
				crc += buffer[i];
			}
			
			return crc;
		}
		
		bool VersionValid()
		{
			return StructureVersion == CurrentStructureVersion;
		}
		
		bool CrcValid()
		{
			return Crc == CalcCrc();
		}
		
		void UpdateCrc()
		{
			Crc = CalcCrc();
		}
	};

#pragma pack (pop)
}
}
}

#endif