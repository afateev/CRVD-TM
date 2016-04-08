////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		01/10/2015
////////////////////////////////////////////////////////////////////////////////

#ifndef MODBUSSLAVE_H
#define MODBUSSLAVE_H

#include "../Utils/CallbackWrapper.h"
#include "ModbusCrc.h"
#include "ModbusErrorCode.h"
#include "ModbusAdu.h"
#include "ModbusPdu.h"
#include "ModbusPduReadHoldingRegisters.h"
#include "ModbusPduWriteSingleRegister.h"
#include "ModbusPduDiagnostics.h"
#include "ModbusPduWriteMultipleRegisters.h"

namespace Rblib
{
namespace Driver
{
namespace Modbus
{

class ModbusSlave
{
public:
	// ���� �������
	// (����� �� ������ ������������, ������ �� ����, ������� ��������������)
	enum FunctionCode
	{
		FunctionCodeReadHoldingRegisters		= 0x03,
		FunctionCodeWriteSingleRegister			= 0x06,
		FunctionCodeDiagnostics 				= 0x08,
		FunctionCodeWriteMultipleRegisters 		= 0x10,
	};
	
	// ���� ����������
	enum ExceptionCode
	{
		ExceptionCodeNoError			= 0x00,
		ExceptionCodeIllegalFunction	= 0x01,
		ExceptionCodeIllegalDataAddress	= 0x02,
		ExceptionCodeIllegalDataValue	= 0x03,
	};
	
	// ���� ���������� ������� Diagnostics
	// (����� �� ������ ������������, ������ �� ����, ������� ��������������)
	enum DiagnosticsSubfunctionCode
	{
		DiagnosticsSubfunctionCodeReturnQueryData	= 0x00,
	};
	
	static const int ResponseBufferLen = 255;
public:
	CallbackWrapper<unsigned char &> GetSelfAddress;
	CallbackWrapper<unsigned short, unsigned short &> GetRegisterValue;
	CallbackWrapper<unsigned short, unsigned short, bool &> SetRegisterValue;
public:
	ModbusSlave()
	{
	}
	
	// ������������� ������
	void Init()
	{
	}
	
	// ���������� ��������� �������
	// frame - ����� ������ � ������� ������
	// len - ����� ������ ������ � ������� ������
	// response - ����� ������, ���� ����� ������� �����
	int ProcessFrame(unsigned char *frame, unsigned int len, unsigned char *response)
	{
		// ������������ Application Data Unit �� ������ ����
		Modbus::ApplicationDataUnit adu;
		Modbus::ErrorCode errorCode = adu.FromByteArray(frame, len);
		
		// ��������� ���������
		if (errorCode != Modbus::ErrorCodeNoError || !adu.Valid())
		{
			return errorCode;
		}
		
		unsigned char addr = adu.Address();
		// �� ����������������� �� ������ �������� �����
		if (addr == 0xFF)
		{
			return 0;
		}
		
		// ���?
		bool forMe = addr == MyAddress();
#ifdef MODBUS_USE_DEFAULT_ADDRESS
		forMe |= addr == MODBUS_DEFAULT_ADDRESS;
#endif		
		// �� ���
		if (!forMe)
		{
			return 0;
		}
		
		// �������� ������ ���� Protocol Data Unit �� ADU
		unsigned char *pduDataPtr = adu.PduDataPtr();
		int pduLen = adu.GetPduLen();
		
		// ������������ Protocol Data Unit �� ������� ����
		Modbus::ProtocolDataUnitRequest pduRequest;
		errorCode = pduRequest.FromByteArray(pduDataPtr, pduLen);
		
		// ��������� ���������
		if (errorCode != Modbus::ErrorCodeNoError || !pduRequest.Valid())
		{
			return errorCode;
		}
		
		unsigned char requestFunctionCode = pduRequest.FunctionCode();
		unsigned char *requestData = pduRequest.RequestData();
		int requestDataLen = pduRequest.RequestDataLen();
		
		Modbus::ProtocolDataUnitResponse pduResponse(requestFunctionCode, pduDataPtr);
		
		ExceptionCode exceptionCode = ExceptionCodeIllegalFunction;
		
		// ��������� ���� ��������
		switch(requestFunctionCode)
		{
		case FunctionCodeReadHoldingRegisters:
			{
				exceptionCode = OnReadHoldingRegisters(requestData, requestDataLen, pduResponse.DataPtr(), pduResponse.DataLenPtr());
			}
			break;
		case FunctionCodeWriteSingleRegister:
			{
				exceptionCode = OnWriteSingleRegister(requestData, requestDataLen, pduResponse.DataPtr(), pduResponse.DataLenPtr());
			}
			break;
		case FunctionCodeDiagnostics:
			{
				exceptionCode = OnDiagnostics(requestData, requestDataLen, pduResponse.DataPtr(), pduResponse.DataLenPtr());
			}
		case FunctionCodeWriteMultipleRegisters:
			{
				exceptionCode = OnWriteMultipleRegisters(requestData, requestDataLen, pduResponse.DataPtr(), pduResponse.DataLenPtr());
			}
			break;
		}
		
		int responsePduLen = 0;
		if (exceptionCode == ExceptionCodeNoError)
		{
			responsePduLen = pduResponse.ToByteArray(pduDataPtr);
		}
		else
		{
			Modbus::ExceptionResponse exceptionResponse;
			exceptionResponse.FunctionCode = requestFunctionCode;
			exceptionResponse.ExceptionCode = exceptionCode;
			responsePduLen = exceptionResponse.ToByteArray(pduDataPtr);
		}
		
		adu.SetPduLen(responsePduLen);
		return adu.ToByteArray(response);
	}
protected:
	unsigned char MyAddress()
	{
		unsigned char myAddress = 0xFF;
		GetSelfAddress(myAddress);
		return myAddress;
	}
	
	unsigned short GetRegValue(unsigned short reg)
	{
		unsigned short val = 0;
		GetRegisterValue(reg, val);
		
		return val;
	}
	
	bool SetRegValue(unsigned short reg, unsigned short value)
	{
		bool res = false;
		SetRegisterValue(reg, value, res);
		
		return res;
	}
	
	ExceptionCode OnReadHoldingRegisters(unsigned char *data, unsigned int dataLen, unsigned char *responseBuffer, int *responseLen)
	{
		if (responseBuffer == 0 || responseLen == 0)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		// ������������ ������ �� ������ ����
		Modbus::ReadHoldingRegistersRequest request;
		Modbus::ErrorCode errorCode = request.FromByteArray(data, dataLen);
		
		// ��������� ���������
		if (errorCode != Modbus::ErrorCodeNoError || !request.Valid())
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		unsigned short firstAddr = request.StartAddress();
		unsigned short quantity = request.Quantity();
		
		// ������ �� ��������, �� ���� ����� ������
		if (quantity == 0)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		// �� ������ ������� � �����
		if (quantity > Modbus::ReadHoldingRegistersResponse::MaxRegisterCount)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		// �������� ������, ��� ����� ���� ���������� ������� � ��������� ��������� �������
		unsigned short elapsedRegsCount = 0xFFFF - firstAddr;
		if (elapsedRegsCount < quantity)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		Modbus::ReadHoldingRegistersResponse response;
		response.RegisterCount = quantity;
		for (int i = 0; i < quantity; i++)
		{
			response.RegisterValue[i] = GetRegValue(firstAddr + i);
		}
		
		*responseLen = response.ToByteArray(responseBuffer);
		return ExceptionCodeNoError;
	}
	
	ExceptionCode OnWriteSingleRegister(unsigned char *data, unsigned int dataLen, unsigned char *responseBuffer, int *responseLen)
	{
		if (responseBuffer == 0 || responseLen == 0)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		// ������������ ������ �� ������ ����
		Modbus::WriteSingleRegisterRequest request;
		Modbus::ErrorCode errorCode = request.FromByteArray(data, dataLen);
		
		// ��������� ���������
		if (errorCode != Modbus::ErrorCodeNoError || !request.Valid())
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		unsigned short addr = request.Address();
		unsigned short val = request.Value();
		
		bool res = SetRegValue(addr, val);
		
		// ����������
		if (res)
		{
			Modbus::WriteSingleRegisterResponse response;
			response.Address = addr;
			response.Value = val;
			*responseLen = response.ToByteArray(responseBuffer);
			
			return ExceptionCodeNoError;
		}
		
		return ExceptionCodeIllegalDataAddress;
	}
	
	ExceptionCode OnDiagnostics(unsigned char *data, unsigned int dataLen, unsigned char *responseBuffer, int *responseLen)
	{
		if (responseBuffer == 0 || responseLen == 0)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		// ������������ ������ �� ������ ����
		Modbus::DiagnosticsRequest request;
		Modbus::ErrorCode errorCode = request.FromByteArray(data, dataLen);
		
		// ��������� ���������
		if (errorCode != Modbus::ErrorCodeNoError || !request.Valid())
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		unsigned short subfunction = request.Subfunction();
		
		switch (subfunction)
		{
		case DiagnosticsSubfunctionCodeReturnQueryData:
			{
				// �������������� ������� ������ ���������� ���
				Modbus::DiagnosticsResponse response;
				response.Subfunction = subfunction;
				response.Data = request.Data();
				response.DataLen = request.DataLen();
				*responseLen = response.ToByteArray(responseBuffer);
			
				return ExceptionCodeNoError;
			}
		}
		
		return ExceptionCodeIllegalDataValue;
	}
	
	ExceptionCode OnWriteMultipleRegisters(unsigned char *data, unsigned int dataLen, unsigned char *responseBuffer, int *responseLen)
	{
		if (responseBuffer == 0 || responseLen == 0)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		// ������������ ������ �� ������ ����
		Modbus::WriteMultipleRegistersRequest request;
		Modbus::ErrorCode errorCode = request.FromByteArray(data, dataLen);
		
		// ��������� ���������
		if (errorCode != Modbus::ErrorCodeNoError || !request.Valid())
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		unsigned short firstAddr = request.StartAddress();
		unsigned short quantity = request.Quantity();
		
		// �� ��������� �������� �� ���� �������, �� ���� ����� ������
		if (quantity == 0)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		bool res = true;
		
		// ���������� ��������
		for (int i = 0; i < quantity; i++)
		{
			unsigned short addr = firstAddr + i;
			
			unsigned short val = request.GetRegisterValueByIndex(i);
			
			res |= SetRegValue(addr, val);
		}
			
		// ����������
		if (res)
		{
			Modbus::WriteMultipleRegistersResponse response;
			response.StartAddress = firstAddr;
			response.Quantity = quantity;
			
			*responseLen = response.ToByteArray(responseBuffer);
			
			return ExceptionCodeNoError;
		}
		
		// �� ����������, ������ ��� ������������ �����
		return ExceptionCodeIllegalDataAddress;
	}
};

}
}
}

#endif