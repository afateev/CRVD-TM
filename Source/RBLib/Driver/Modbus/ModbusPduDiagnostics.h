#ifndef RBLIB_MODBUSPDU_DIAGNOSTICS_H
#define RBLIB_MODBUSPDU_DIAGNOSTICS_H

#include "ModbusErrorCode.h"
#include "ModbusCommon.h"

namespace Rblib
{
namespace Driver
{
namespace Modbus
{
	class DiagnosticsRequest
	{
	public:
		static const int SubfunctionFieldOffset = 0;	// Смещение поля кода функции диагностики относительно начала пакета
		static const int DataFieldOffset = 2;			// Смещение поля данных относительно начала пакета
		static const int RequestMinLen = 3; 			// Минимальный размер пакета фиксированный = 3 байта
	protected:
		unsigned short _subfunction;
		unsigned char *_data;
		unsigned char _dataLen;
		bool _valid;
	public:
		DiagnosticsRequest()
		{
			Reset();
		}
		
		void Reset()
		{
			_subfunction = 0;
			_data = 0;
			_dataLen = 0;
			_valid = false;
		}
		
		ErrorCode FromByteArray(unsigned char *data, int len)
		{
			Reset();
			
			if (data == 0 || len < RequestMinLen)
			{
				return ErrorCodeIvalidInputFrame;
			}
			
			_subfunction = WordFromArray(&data[SubfunctionFieldOffset]);
			_dataLen = len - DataFieldOffset;

			if (_dataLen > 0)
			{
				_data = &data[DataFieldOffset];
			}

			_valid = true;
			
			return ErrorCodeNoError;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		unsigned short Subfunction()
		{
			return _subfunction;
		}
		
		unsigned char *Data()
		{
			return _data;
		}
		
		unsigned char DataLen()
		{
			return _dataLen;
		}
	};
	
	class DiagnosticsResponse
	{
	public:
		unsigned short Subfunction;
		unsigned char *Data;
		unsigned char DataLen;
	public:
		DiagnosticsResponse()
		{
			Subfunction = 0;
			Data = 0;
			DataLen = 0;
		}
		
		int ToByteArray(unsigned char *buffer)
		{
			if (buffer == 0)
			{
				return 0;
			}
			
			int i = 0;
			
			WordToArray(Subfunction, &buffer[i]);
			i += 2;
			
			for (int j = 0; j < DataLen; j++)
			{
				buffer[i] = Data[j];
				i++;
			}

			return i;
		}
	};
}
}
}
#endif