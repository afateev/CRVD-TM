#ifndef RBLIB_MODBUSPDU_READHOLDINGREGISTERS_H
#define RBLIB_MODBUSPDU_READHOLDINGREGISTERS_H

#include "ModbusErrorCode.h"
#include "ModbusCommon.h"

namespace Rblib
{
namespace Driver
{
namespace Modbus
{
	class ReadHoldingRegistersRequest
	{
	public:
		static const int StartAddressFieldOffset = 0;	// �������� ���� ������ ������������ ������ ������
		static const int QuantityFieldOffset = 2;		// �������� ���� ���������� ������������ ������ ������
		static const int RequestLen = 4; 				// ������ ������ ������������� = 4 �����
	protected:
		unsigned short _startAddress;
		unsigned short _quantity;
		bool _valid;
	public:
		ReadHoldingRegistersRequest()
		{
			Reset();
		}
		
		void Reset()
		{
			_startAddress = 0;
			_quantity = 0;
			_valid = false;
		}
		
		ErrorCode FromByteArray(unsigned char *data, int len)
		{
			Reset();
			
			if (data == 0 || len < RequestLen)
			{
				return ErrorCodeIvalidInputFrame;
			}
			
			_startAddress = WordFromArray(&data[StartAddressFieldOffset]);
			_quantity = WordFromArray(&data[QuantityFieldOffset]);
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
	};
	
	class ReadHoldingRegistersResponse
	{
	public:
		static const int MaxRegisterCount = 125;	// ������������ ����� ��������� ������� ����� �������� � ����� ��������� �� ������������
	public:
		unsigned char RegisterCount;
		unsigned short RegisterValue[MaxRegisterCount];
	public:
		ReadHoldingRegistersResponse()
		{
			RegisterCount = 0;
			for (int i = 0; i < MaxRegisterCount; i++)
			{
				RegisterValue[i] = 0;
			}
		}
		
		int ToByteArray(unsigned char *buffer)
		{
			if (buffer == 0)
			{
				return 0;
			}
			
			int i = 0;
			
			// ������ ������ ��� ����� ����, ������� �������� ���������
			buffer[i] = RegisterCount * 2;
			i++;
			
			for (int j = 0; j < RegisterCount; j++)
			{
				WordToArray(RegisterValue[j], &buffer[i]);
				i += 2;
			}
			
			return i;
		}
	};
}
}
}

#endif