#ifndef RBLIB_MODBUSPDU_H
#define RBLIB_MODBUSPDU_H

#include "ModbusErrorCode.h"
#include "ModbusCommon.h"

namespace Rblib
{
namespace Driver
{
namespace Modbus
{
	class ProtocolDataUnit
	{
	public:
		static const int FunctionCodeFieldOffset = 0;		// Смещение поля кода функции относительно начала пакета
		static const int DataFieldOffset = 1;				// Смещение поля данных запроса относительно начала пакета
		static const int MinPduLen = 1; 					// Минимальный размер пакета это код функции (1 байт)
	protected:
		unsigned char _functionCode;
		unsigned char *_data;
		int _dataLen;
		bool _valid;
	public:
		ProtocolDataUnit()
		{
			Reset();
		}
		
		void Reset()
		{
			_functionCode = 0;
			_data = 0;
			_dataLen = 0;
			_valid = false;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class ProtocolDataUnitRequest : public ProtocolDataUnit
	{
	public:
		ProtocolDataUnitRequest()
		{
		}
		
		ErrorCode FromByteArray(unsigned char *pduData, int len)
		{
			Reset();
			
			// если нет ничего или нет ничего кроме адреса и поля CRC, считаем фрейм недопустимым
			if (pduData == 0 || len < MinPduLen)
			{
				return ErrorCodeIvalidInputFrame;
			}
			
			_functionCode = pduData[FunctionCodeFieldOffset];
			// если поле данных есть
			if (len > MinPduLen)
			{
				_data = &pduData[DataFieldOffset];
				_dataLen = len - MinPduLen;
			}
			_valid = true;
			
			return ErrorCodeNoError;
		}
		
		unsigned char FunctionCode()
		{
			return _functionCode;
		}
		
		unsigned char *RequestData()
		{
			return _data;
		}
		
		int RequestDataLen()
		{
			return _dataLen;
		}
	};
	
	class ProtocolDataUnitResponse : public ProtocolDataUnit
	{
	public:
		ProtocolDataUnitResponse(unsigned char functionCode, unsigned char *dataBuffer)
		{
			_functionCode = functionCode;
			_data = &dataBuffer[DataFieldOffset];
			_dataLen = 0;
		}

		unsigned char *DataPtr()
		{
			return _data;
		}
		
		int *DataLenPtr()
		{
			return &_dataLen;
		}
		
		int ToByteArray(unsigned char *buffer)
		{
			if (buffer == 0)
			{
				return 0;
			}
			
			int i = 0;
			
			buffer[i] = _functionCode;
			i++;
			
			for (int j = 0; j < _dataLen; j++)
			{
				buffer[i] = _data[j];
				i++;
			}
			
			return i;
		}
	};
	
	class ExceptionResponse
	{
	public:
		unsigned char FunctionCode;
		unsigned char ExceptionCode;
	public:
		ExceptionResponse()
		{
			FunctionCode = 0;
			ExceptionCode = 0;
		}
		
		int ToByteArray(unsigned char *buffer)
		{
			if (buffer == 0)
			{
				return 0;
			}
			
			int i = 0;
			
			buffer[i] = FunctionCode;
			// признак возврата исключения
			buffer[i] |= (1 << 7);
			i++;

			buffer[i] = ExceptionCode;
			i++;
			
			return i;
		}
	};
}
}
}

#endif