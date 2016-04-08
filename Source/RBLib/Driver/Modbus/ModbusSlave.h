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
	// Коды функций
	// (здесь не полная спецификация, только те коды, которые поддерживаются)
	enum FunctionCode
	{
		FunctionCodeReadHoldingRegisters		= 0x03,
		FunctionCodeWriteSingleRegister			= 0x06,
		FunctionCodeDiagnostics 				= 0x08,
		FunctionCodeWriteMultipleRegisters 		= 0x10,
	};
	
	// Коды исключений
	enum ExceptionCode
	{
		ExceptionCodeNoError			= 0x00,
		ExceptionCodeIllegalFunction	= 0x01,
		ExceptionCodeIllegalDataAddress	= 0x02,
		ExceptionCodeIllegalDataValue	= 0x03,
	};
	
	// Коды подфункций функции Diagnostics
	// (здесь не полная спецификация, только те коды, которые поддерживаются)
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
	
	// Инициализация модуля
	void Init()
	{
	}
	
	// Обработчик входящего запроса
	// frame - фрейм данных с нижнего уровня
	// len - длина фрейма данных с нижнего уровня
	// response - адрес буфера, куда будет записан ответ
	int ProcessFrame(unsigned char *frame, unsigned int len, unsigned char *response)
	{
		// конструируем Application Data Unit из потока байт
		Modbus::ApplicationDataUnit adu;
		Modbus::ErrorCode errorCode = adu.FromByteArray(frame, len);
		
		// разбираем результат
		if (errorCode != Modbus::ErrorCodeNoError || !adu.Valid())
		{
			return errorCode;
		}
		
		unsigned char addr = adu.Address();
		// на широковещательные не должен отвечать никто
		if (addr == 0xFF)
		{
			return 0;
		}
		
		// нам?
		bool forMe = addr == MyAddress();
#ifdef MODBUS_USE_DEFAULT_ADDRESS
		forMe |= addr == MODBUS_DEFAULT_ADDRESS;
#endif		
		// не нам
		if (!forMe)
		{
			return 0;
		}
		
		// получаем массив байт Protocol Data Unit из ADU
		unsigned char *pduDataPtr = adu.PduDataPtr();
		int pduLen = adu.GetPduLen();
		
		// конструируем Protocol Data Unit из массива байт
		Modbus::ProtocolDataUnitRequest pduRequest;
		errorCode = pduRequest.FromByteArray(pduDataPtr, pduLen);
		
		// разбираем результат
		if (errorCode != Modbus::ErrorCodeNoError || !pduRequest.Valid())
		{
			return errorCode;
		}
		
		unsigned char requestFunctionCode = pduRequest.FunctionCode();
		unsigned char *requestData = pduRequest.RequestData();
		int requestDataLen = pduRequest.RequestDataLen();
		
		Modbus::ProtocolDataUnitResponse pduResponse(requestFunctionCode, pduDataPtr);
		
		ExceptionCode exceptionCode = ExceptionCodeIllegalFunction;
		
		// разбираем коды запросов
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
		
		// конструируем запрос из потока байт
		Modbus::ReadHoldingRegistersRequest request;
		Modbus::ErrorCode errorCode = request.FromByteArray(data, dataLen);
		
		// разбираем результат
		if (errorCode != Modbus::ErrorCodeNoError || !request.Valid())
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		unsigned short firstAddr = request.StartAddress();
		unsigned short quantity = request.Quantity();
		
		// ничего не спросили, не надо такое делать
		if (quantity == 0)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		// не влезет столько в буфер
		if (quantity > Modbus::ReadHoldingRegistersResponse::MaxRegisterCount)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		// спросили больше, чем может быть адресовано начиная с указанной стартовой позиции
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
		
		// конструируем запрос из потока байт
		Modbus::WriteSingleRegisterRequest request;
		Modbus::ErrorCode errorCode = request.FromByteArray(data, dataLen);
		
		// разбираем результат
		if (errorCode != Modbus::ErrorCodeNoError || !request.Valid())
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		unsigned short addr = request.Address();
		unsigned short val = request.Value();
		
		bool res = SetRegValue(addr, val);
		
		// получилось
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
		
		// конструируем запрос из потока байт
		Modbus::DiagnosticsRequest request;
		Modbus::ErrorCode errorCode = request.FromByteArray(data, dataLen);
		
		// разбираем результат
		if (errorCode != Modbus::ErrorCodeNoError || !request.Valid())
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		unsigned short subfunction = request.Subfunction();
		
		switch (subfunction)
		{
		case DiagnosticsSubfunctionCodeReturnQueryData:
			{
				// дигностическая функция должна возвращать эхо
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
		
		// конструируем запрос из потока байт
		Modbus::WriteMultipleRegistersRequest request;
		Modbus::ErrorCode errorCode = request.FromByteArray(data, dataLen);
		
		// разбираем результат
		if (errorCode != Modbus::ErrorCodeNoError || !request.Valid())
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		unsigned short firstAddr = request.StartAddress();
		unsigned short quantity = request.Quantity();
		
		// не попросили записать ни один регистр, не надо такое делать
		if (quantity == 0)
		{
			return ExceptionCodeIllegalDataAddress;
		}
		
		bool res = true;
		
		// записываем регистры
		for (int i = 0; i < quantity; i++)
		{
			unsigned short addr = firstAddr + i;
			
			unsigned short val = request.GetRegisterValueByIndex(i);
			
			res |= SetRegValue(addr, val);
		}
			
		// получилось
		if (res)
		{
			Modbus::WriteMultipleRegistersResponse response;
			response.StartAddress = firstAddr;
			response.Quantity = quantity;
			
			*responseLen = response.ToByteArray(responseBuffer);
			
			return ExceptionCodeNoError;
		}
		
		// не получилось, значит был неправильный адрес
		return ExceptionCodeIllegalDataAddress;
	}
};

}
}
}

#endif