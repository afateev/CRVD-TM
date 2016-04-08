#ifndef RBLIB_MODBUSPDU_WRITESINGLEREGISTER_H
#define RBLIB_MODBUSPDU_WRITESINGLEREGISTER_H

#include "ModbusErrorCode.h"
#include "ModbusCommon.h"

namespace Rblib
{
namespace Driver
{
namespace Modbus
{
	class WriteSingleRegisterRequest
	{
	public:
		static const int AddressFieldOffset = 0;	// —мещение пол€ адреса относительно начала пакета
		static const int ValueFieldOffset = 2;		// —мещение пол€ значени€ относительно начала пакета
		static const int RequestLen = 4; 			// –азмер пакета фиксированный = 4 байта
	protected:
		unsigned short _address;
		unsigned short _value;
		bool _valid;
	public:
		WriteSingleRegisterRequest()
		{
			Reset();
		}
		
		void Reset()
		{
			_address = 0;
			_value = 0;
			_valid = false;
		}
		
		ErrorCode FromByteArray(unsigned char *data, int len)
		{
			Reset();
			
			if (data == 0 || len < RequestLen)
			{
				return ErrorCodeIvalidInputFrame;
			}
			
			_address = WordFromArray(&data[AddressFieldOffset]);
			_value = WordFromArray(&data[ValueFieldOffset]);
			_valid = true;
			
			return ErrorCodeNoError;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		unsigned short Address()
		{
			return _address;
		}
		
		unsigned short Value()
		{
			return _value;
		}
	};
	
	class WriteSingleRegisterResponse
	{
	public:
		unsigned short Address;
		unsigned short Value;
	public:
		WriteSingleRegisterResponse()
		{
			Address = 0;
			Value = 0;
		}
		
		int ToByteArray(unsigned char *buffer)
		{
			if (buffer == 0)
			{
				return 0;
			}
			
			int i = 0;
			
			WordToArray(Address, &buffer[i]);
			i += 2;
			
			WordToArray(Value, &buffer[i]);
			i += 2;
			
			return i;
		}
	};
}
}
}

#endif