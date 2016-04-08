#ifndef RBLIB_MODBUSPDU_WRITEMULTIPLEREGISTERS_H
#define RBLIB_MODBUSPDU_WRITEMULTIPLEREGISTERS_H

#include "ModbusErrorCode.h"
#include "ModbusCommon.h"

namespace Rblib
{
namespace Driver
{
namespace Modbus
{
	class WriteMultipleRegistersRequest
	{
	public:
		static const int StartAddressFieldOffset = 0;	// Смещение поля адреса относительно начала пакета
		static const int QuantityFieldOffset = 2;		// Смещение поля количества регистров относительно начала пакета
		static const int ByteCountFieldOffset = 4;		// Смещение поля количества байт относительно начала пакета
		static const int ValueFieldOffset = 5;			// Смещение поля значений регистров относительно начала пакета
		static const int MinRequestLen = 5; 			// Размер пакета фиксированный = 4 байта
		static const int MaxRegCount = 123;				// Максимальное число регистров которое можно записать за 1 запрос
	protected:
		unsigned short _startAddress;
		unsigned short _quantity;
		unsigned char _byteCount;
		unsigned char *_registersValueData;
		bool _valid;
	public:
		WriteMultipleRegistersRequest()
		{
			Reset();
		}
		
		void Reset()
		{
			_startAddress = 0;
			_quantity = 0;
			_byteCount = 0;
			_registersValueData = 0;
			_valid = false;
		}
		
		ErrorCode FromByteArray(unsigned char *data, int len)
		{
			Reset();
			
			if (data == 0 || len < MinRequestLen)
			{
				return ErrorCodeIvalidInputFrame;
			}
			
			_startAddress = WordFromArray(&data[StartAddressFieldOffset]);
			_quantity = WordFromArray(&data[QuantityFieldOffset]);
			_byteCount = data[ByteCountFieldOffset];
			
			// данных не хватает
			if (_byteCount > len - ByteCountFieldOffset)
			{
				return ErrorCodeIvalidInputFrame;
			}
			
			// не сходятся значения полей
			if (_byteCount / 2 != _quantity)
			{
				return ErrorCodeIvalidInputFrame;
			}
			
			_registersValueData = &data[ValueFieldOffset];
			
			_valid = true;
			
			return ErrorCodeNoError;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		unsigned short StartAddress()
		{
			return _startAddress;
		}
		
		unsigned short Quantity()
		{
			return _quantity;
		}
		
		unsigned short GetRegisterValueByIndex(int index)
		{
			return WordFromArray(&_registersValueData[index * 2]);
		}
	};
	
	class WriteMultipleRegistersResponse
	{
	public:
		unsigned short StartAddress;
		unsigned short Quantity;
	public:
		WriteMultipleRegistersResponse()
		{
			StartAddress = 0;
			Quantity = 0;
		}
		
		int ToByteArray(unsigned char *buffer)
		{
			if (buffer == 0)
			{
				return 0;
			}
			
			int i = 0;
			
			WordToArray(StartAddress, &buffer[i]);
			i += 2;
			
			WordToArray(Quantity, &buffer[i]);
			i += 2;
			
			return i;
		}
	};
}
}
}

#endif