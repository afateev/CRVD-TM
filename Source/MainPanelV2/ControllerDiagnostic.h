////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		17/14/2016
////////////////////////////////////////////////////////////////////////////////

#ifndef CONTROLLERDIAGNOSTIC_H
#define CONTROLLERDIAGNOSTIC_H

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
class ControllerDiagnostic
{
public:
	static const int NoResponseTimeout = 2;
	
	typedef Rblib::CallbackWrapper<bool> TxEnableCallbackType;
	typedef Rblib::CallbackWrapper<> ModbusSelectCallbackType;
	static TxEnableCallbackType TxEnableCallback;
	static ModbusSelectCallbackType ModbusSelectCallback;
protected:
	enum State
	{
		StateRequest,		// отправка запроса
		StateWait,			// ожидание ответа
		StateResponse,		// обработка ответа
		StateComplete		// закончили цикл
	};
	
	static State _state;
	static unsigned int _regRequestCountdown;
	static unsigned char _request[256];
	static unsigned char *_response;
	
	static unsigned int _registers[RegCount];
	
	static unsigned int _noResponseTimeoutCounter;
public:
	static bool Run()
	{
		if (_regRequestCountdown > 0)
		{
			_regRequestCountdown--;
		}
		
		switch(_state)
		{
		case StateRequest:
			{
				if (ModBus::IsReady())
				{
					if (_regRequestCountdown < 1)
					{
						ModBus::ChangeSpeed(Config::MainComPortClockSourceFrequency, 9600);
						ModBus::WaitLastByteTx = true;
						ModbusSelectCallback();
						unsigned char address = BusAddres;
						unsigned int requestSize = ModBusMaster::BuildReadHoldingRegisters(_request, address, FirstReg, RegCount);
						ModBus::OnTxCompleteCallback = OnTxCompleteCallback;
						TxEnableCallback(true);
						ModBus::SendRequest(_request, requestSize);
						
						_regRequestCountdown = 1000;
						
						_state = StateWait;
					}
				}
			}
			break;
		case StateWait:
			{
				if (ModBus::IsReadComplete())
					_state = StateResponse;
			}
			break;
		case StateResponse:
			{
				if (!ModBus::IsReadComplete())
				{
					break;
				}
				
				unsigned int readed = 0;
				_response = ModBus::GetResponse(readed);
				if (readed > 0 && 0 != _response)
				{
					_noResponseTimeoutCounter = 0;
					
					unsigned int firstReg = _request[2];
					firstReg <<= 8;
					firstReg |= _request[3];
					
					unsigned int regQuantity = _request[4];
					regQuantity <<= 8;
					regQuantity |= _request[5];
					
					if ((_request[0] == _response[0]) &&
						(_request[1] == _response[1]) && 
						(2 * regQuantity == _response[2])
					   )
					{
						unsigned int regPtr = 3;
						for (unsigned int reg = firstReg; reg < firstReg + regQuantity; reg ++)
						{
							unsigned short regValue = _response[regPtr];
							regValue <<= 8;
							regValue |= _response[regPtr + 1];
							
							_registers[reg] = regValue;
								
							regPtr += 2;
						}
					}
				}
				else
				{
					if (_noResponseTimeoutCounter < NoResponseTimeout)
					{
						_noResponseTimeoutCounter++;
					}
				}
				_state = StateComplete;
			}
			break;
		case StateComplete:
			{
				_state = StateRequest;
			}
			break;
		}
		return StateComplete == _state;
	}
	
	static unsigned char GetAddress()
	{
		return BusAddres;
	}
	
	static void GetAddress(unsigned char &value)
	{
		value = GetAddress();
	}
	
	static bool NoResponse()
	{
		return _noResponseTimeoutCounter >= NoResponseTimeout;
	}
	
	static void NoResponse(bool &noResponse)
	{
		noResponse = NoResponse();
	}
	
	static unsigned short GetRegValue(unsigned char reg)
	{
		if (reg > 63)
			return 0;
		
		return _registers[reg];
	}
	
	static void GetRegValue(int reg, unsigned short &value)
	{
		value = GetRegValue(reg);
	}
	
	static bool GetRegValues(unsigned char *buffer, unsigned int bufferLen, unsigned short firstAddr, unsigned short quantity)
	{
		unsigned short minReg = 0;
		unsigned short maxReg = 63;
		unsigned int firstReg = firstAddr;
		unsigned int lastReg = firstReg + quantity - 1;
		
		if (quantity < 1)
		{
			return false;
		}
		
		if (bufferLen / 2 < quantity)
		{
			return false;
		}
		
		if (firstReg < minReg || firstReg > maxReg || lastReg < minReg || lastReg > maxReg)
		{
			return false;
		}
		
		unsigned short j = 0;
		for (unsigned short i = firstReg; i <= lastReg; i++)
		{
			unsigned short regVal = GetRegValue(i);

			buffer[j] = (regVal >> 8) & 0xFF;
			buffer[j + 1] = regVal & 0xFF;
			
			j += 2;
		}
		
		return true;
	}
	
	static void OnTxCompleteCallback()
	{
		ModBus::OnTxCompleteCallback.Reset();
		TxEnableCallback(false);
	}
};

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::TxEnableCallbackType ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::TxEnableCallback;

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::ModbusSelectCallbackType ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::ModbusSelectCallback;

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
typename ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::State
ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::_state = 
ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::StateRequest;

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
unsigned int ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::_regRequestCountdown;

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
unsigned char ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::_request[256];

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
unsigned char *ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::_response;

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
unsigned int ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::_registers[];

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
unsigned int ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::_noResponseTimeoutCounter = ControllerDiagnostic<ModBus, BusAddres, FirstReg, RegCount>::NoResponseTimeout;

#endif