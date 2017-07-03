////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		05/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H

#include "ModBusCrc.h"

class ModBusMaster
{
public:
	// построить команду запроса регистров хранения
	static unsigned int BuildReadHoldingRegisters(unsigned char *data, unsigned char addr, unsigned short firstAddr, unsigned short quantity)
	{
		if (0 == data)
			return 0;
		
		data[0] = addr;
		data[1] = 0x03;
		data[2] = (firstAddr >> 8) & 0xFF;
		data[3] = firstAddr & 0xFF;
		data[4] = (quantity >> 8) & 0xFF;
		data[5] = quantity & 0xFF;
		
		unsigned short crc = ModBusCrc::GetCrc(data, 6);
		
		data[6] = (crc >> 8) & 0xFF;
		data[7] = crc & 0xFF;
		
		return 8;
	}
	
	// построить команду записи регистра хранения
	static unsigned int BuildWriteHoldingRegister(unsigned char *data, unsigned char addr, unsigned short regAddr, unsigned short regValue)
	{
		if (0 == data)
			return 0;
		
		data[0] = addr;
		data[1] = 0x06;
		data[2] = (regAddr >> 8) & 0xFF;
		data[3] = regAddr & 0xFF;
		data[4] = (regValue >> 8) & 0xFF;
		data[5] = regValue & 0xFF;
		
		unsigned short crc = ModBusCrc::GetCrc(data, 6);
		
		data[6] = (crc >> 8) & 0xFF;
		data[7] = crc & 0xFF;
		
		return 8;
	}
	
	// построить команду запроса области буфера осциллограмм
	static unsigned int BuildReadOscPoints(unsigned char *data, unsigned char addr, unsigned short firstPoint, unsigned short quantity)
	{
		if (0 == data)
			return 0;
		
		data[0] = addr;
		data[1] = 0x66;
		data[2] = (firstPoint >> 8) & 0xFF;
		data[3] = firstPoint & 0xFF;
		data[4] = (quantity >> 8) & 0xFF;
		data[5] = quantity & 0xFF;
		
		unsigned short crc = ModBusCrc::GetCrc(data, 6);
		
		data[6] = (crc >> 8) & 0xFF;
		data[7] = crc & 0xFF;
		
		return 8;
	}
};

#endif