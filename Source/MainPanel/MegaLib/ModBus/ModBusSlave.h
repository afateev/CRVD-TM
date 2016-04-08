////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		11/04/2013
////////////////////////////////////////////////////////////////////////////////

#ifndef MODBUSSLAVE_H
#define MODBUSSLAVE_H

#include "ModBusCrc.h"

template <class ModBusStateMachine>
class ModBusSlave
{
public:
	static unsigned char MyAddress;
	typedef bool (GetRegValuesCallbackType)(unsigned char *buffer, unsigned int bufferLen, unsigned short firstAddr, unsigned short quantity);
	static GetRegValuesCallbackType *GetRegValuesCallback;
public:
	// Обработчик входящего запроса
	static unsigned int OnRequest(unsigned char *data, unsigned int dataLen)
	{
		if (0 == data)
		{
			return 0;
		}
		
		if (dataLen < 2)
		{
			return 0;
		}
		
		if (!IsCrcValid(data, dataLen))
		{
			return 0;
		}
		
		unsigned char addr = GetAddress(data, dataLen);
		// на широковещательные не должен отвечать никто
		if (addr == 0xFF)
		{
			return 0;
		}
		
		// не нам
		if (addr != MyAddress)
		{
			return 0;
		}
		
		unsigned char cmd = GetFunction(data, dataLen);
		switch(cmd)
		{
		// нулевой код функции по спецификации протокола считается недопустимым
		case 0x00:
				return 0;
		case 0x03:
				return OnReadHoldingRegisters(data, dataLen, ModBusStateMachine::BufferLen);
		default:
				return OnNotSupported(data, ModBusStateMachine::BufferLen);
		}
	}
	
	static bool IsCrcValid(unsigned char *data, unsigned int dataLen)
	{
		if (0 == data)
		{
			return 0;
		}
		
		if (dataLen < 4)
		{
			return false;
		}
		
		unsigned short crcActual = data[dataLen - 2];
		crcActual <<= 8;
		crcActual |= data[dataLen - 1];
		
		unsigned short crcExpected = ModBusCrc::GetCrc(data, dataLen - 2);
		return crcExpected == crcActual;
	}
	
	static unsigned char GetAddress(unsigned char *data, unsigned int dataLen)
	{
		if (0 == data)
		{
			return 0;
		}
		
		if (dataLen < 1)
		{
			return 0xFF;
		}
		
		return data[0];
	}
	
	static unsigned char GetFunction(unsigned char *data, unsigned int dataLen)
	{
		if (0 == data)
		{
			return 0;
		}
		
		if (dataLen < 2)
		{
			return 0;
		}
		
		return data[1];
	}
	
	static unsigned int OnReadHoldingRegisters(unsigned char *data, unsigned int dataLen, unsigned int bufferLen)
	{
		if (!GetRegValuesCallback)
		{
			return OnNotSupported(data, bufferLen);
		}
		
		if (0 == data)
		{
			return 0;
		}
		
		if (dataLen < 6)
		{
			return OnInvalidParams(data, ModBusStateMachine::BufferLen);
		}
		
		unsigned short firstAddr = data[2];
		firstAddr <<= 8;
		firstAddr |= data[3];
		
		unsigned short quantity = data[4];
		quantity <<= 8;
		quantity |= data[5];
		
		// сколько может за 1 раз поместиться в буфер
		unsigned int maxRegCount = (bufferLen - 1 - 1 - 1 - 2) / 2;
		
		// ничего не спросили, не надо такое делать
		if (quantity == 0)
		{
			return OnInvalidParams(data, bufferLen);
		}
		
		// не влезет столько в буфер
		if (quantity > maxRegCount)
		{
			return OnInvalidParams(data, bufferLen);
		}
		
		// спросили больше, чем может быть адресовано начиная с указанной стартовой позиции
		unsigned short elapsedRegsCount = 0xFFFF - firstAddr;
		if (elapsedRegsCount < quantity)
		{
			return OnInvalidParams(data, bufferLen);
		}
		
		// предполагаем что в data хранится исходный запрос, поэтому правим не все поля
		
		// кол-во байт данных
		data[2] = 2 * quantity;
		// заполняем содержимое поля данных
		bool res = GetRegValuesCallback(&data[3], bufferLen, firstAddr, quantity);
		if (res)
		{
			unsigned int crcPos = 1 + 1 + 1 + quantity * 2;
			unsigned short crc = ModBusCrc::GetCrc(data, crcPos);
		
			data[crcPos] = (crc >> 8) & 0xFF;
			data[crcPos + 1] = crc & 0xFF;
			
			return crcPos + 2;
		}
			
		return 0;
	}
	
	static unsigned int OnNotSupported(unsigned char *data, unsigned int bufferLen)
	{
		if (0 == data)
		{
			return 0;
		}
		
		// предполагаем что в data хранится исходный запрос, поэтому правим не все поля
		
		data[1] |= 0x80; // признак ошибки
		data[2] = 0x01;	// код ощибки "Illegal Function"
		
		unsigned int crcPos = 1 + 1 + 1;
		unsigned short crc = ModBusCrc::GetCrc(data, crcPos);
		
		data[crcPos] = (crc >> 8) & 0xFF;
		data[crcPos + 1] = crc & 0xFF;
			
		return crcPos + 2;
	}
	
	static unsigned int OnInvalidParams(unsigned char *data, unsigned int bufferLen)
	{
		if (0 == data)
		{
			return 0;
		}
		
		// предполагаем что в data хранится исходный запрос, поэтому правим не все поля
		
		data[1] |= 0x80; // признак ошибки
		data[2] = 0x02;	// код ощибки "Illegal Data Address"
		
		unsigned int crcPos = 1 + 1 + 1;
		unsigned short crc = ModBusCrc::GetCrc(data, crcPos);
		
		data[crcPos] = (crc >> 8) & 0xFF;
		data[crcPos + 1] = crc & 0xFF;
			
		return crcPos + 2;
	}
};

template <class ModBusStateMachine>
unsigned char ModBusSlave<ModBusStateMachine>::MyAddress = 0xFF;


template <class ModBusStateMachine>
typename ModBusSlave<ModBusStateMachine>::GetRegValuesCallbackType *
ModBusSlave<ModBusStateMachine>::GetRegValuesCallback = 0;


#endif