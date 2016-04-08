#ifndef RBLIB_MODBUSADU_H
#define RBLIB_MODBUSADU_H

#include "ModbusCrc.h"
#include "ModbusErrorCode.h"
#include "ModbusCommon.h"

namespace Rblib
{
namespace Driver
{
namespace Modbus
{
	class ApplicationDataUnit
	{
	public:
		static const int AdrressFieldOffset = 0;		// Смещение поля адреса относительно начала пакета
		static const int PduFieldOffset = 1;			// Смещение поля PDU относительно начала пакета
		static const int CrcFieldOffsetFromTail = 2;	// Смещение поля СКС относительно конца пакета
		static const int MinAduLen = 3; 				// Минимальный размер пакета это адрес (1 байт) и CRC (2 байта)
	protected:
		unsigned char _address;
		unsigned char *_pdu;
		int _pduLen;
		bool _valid;
	public:
		ApplicationDataUnit()
		{
			Reset();
		}
		
		void Reset()
		{
			_address = 0xFF;
			_pdu = 0;
			_pduLen = 0;
			_valid = false;
		}
		
		ErrorCode FromByteArray(unsigned char *frame, int len)
		{
			Reset();
			
			// если нет ничего или нет ничего кроме адреса и поля CRC, считаем фрейм недопустимым
			if (frame == 0 || len <= MinAduLen)
			{
				return ErrorCodeIvalidInputFrame;
			}
			
			_address = frame[AdrressFieldOffset];
			unsigned short crcActual = WordFromArray(&frame[len - CrcFieldOffsetFromTail]);
			unsigned short crcExpected = ModbusCrc::GetCrc(frame, len - CrcFieldOffsetFromTail);
			
			if (crcActual != crcExpected)
			{
				return ErrorCodeInvalidCrc;
			}
			
			_pdu = &frame[PduFieldOffset];
			_pduLen = len - MinAduLen;
			_valid = true;
			
			return ErrorCodeNoError;
		}
		
		int ToByteArray(unsigned char *responseFrame)
		{
			if (responseFrame == 0)
			{
				return 0;
			}
			
			int i = 0;
			
			responseFrame[i] = _address;
			i++;
			
			for (int j = 0; j < _pduLen; j++)
			{
				responseFrame[i] = _pdu[j];
				i++;
			}
			
			unsigned short crc = ModbusCrc::GetCrc(responseFrame, i);
			
			WordToArray(crc, &responseFrame[i]);
			i += 2;
			
			return i;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		unsigned char Address()
		{
			return _address;
		}
		
		unsigned char *PduDataPtr()
		{
			return _pdu;
		}
		
		int GetPduLen()
		{
			return _pduLen;
		}
		
		void SetPduLen(int pduLen)
		{
			_pduLen = pduLen;
		}
	};
}
}
}

#endif