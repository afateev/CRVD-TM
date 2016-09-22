////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		27/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef INSULATIONCONTROL_H
#define INSULATIONCONTROL_H

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
class InsulationControl
{
public:
	static const int NoResponseTimeout = 2;
	
	typedef Rblib::CallbackWrapper<> ModbusSelectCallbackType;
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
						ModBus::WaitLastByteTx = false;
						ModbusSelectCallback();
						unsigned char address = BusAddres;
						unsigned int requestSize = ModBusMaster::BuildReadHoldingRegisters(_request, address, FirstReg, RegCount);
						ModBus::SendRequest(_request, requestSize);
						
						_regRequestCountdown = 30000;
						
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
};

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::ModbusSelectCallbackType InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::ModbusSelectCallback;

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
typename InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::State
InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::_state = 
InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::StateRequest;

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
unsigned int InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::_regRequestCountdown;

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
unsigned char InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::_request[256];

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
unsigned char *InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::_response;

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
unsigned int InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::_registers[];

template<class ModBus, unsigned char BusAddres, int FirstReg, int RegCount>
unsigned int InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::_noResponseTimeoutCounter = InsulationControl<ModBus, BusAddres, FirstReg, RegCount>::NoResponseTimeout;

template<class ModBus, unsigned char BusAddres>
class InsulationControl_
{
protected:
	enum State
	{
		StateRequest,		// отправка запроса
		StateWait,			// ожидание ответа
		StateResponse,		// обработка ответа
		StateComplete		// закончили цикл
	};
	
	static State _state;
	static unsigned char _request[256];
	static unsigned char *_response;
	
	static unsigned char _address;
	static unsigned int _registers[64];
public:
	static bool Run()
	{
		switch(_state)
		{
		case StateRequest:
			{
				if (ModBus::IsReady())
				{
					ModBus::ChangeSpeed(Config::MainComPortClockSourceFrequency, 9600);
					ModBus::WaitLastByteTx = false;
                    unsigned int requestSize = ModBusMaster::BuildReadHoldingRegisters(_request, _address, 1, 10);
					ModBus::SendRequest(_request, requestSize);
					_state = StateWait;
				}
			}
			break;
		case StateWait:
			{
				if (ModBus::IsReady())
					_state = StateResponse;
			}
			break;
		case StateResponse:
			{
				unsigned int readed = 0;
				_response = ModBus::GetResponse(readed);
				if (readed > 0 && 0 != _response)
				{
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
	
	static unsigned char GetAddress()
	{
		return _address;
	}
	
	static void GetAddress(unsigned char &value)
	{
		value = GetAddress();
	}
};

template<class ModBus, unsigned char BusAddres>
typename InsulationControl_<ModBus, BusAddres>::State
InsulationControl_<ModBus, BusAddres>::_state = 
InsulationControl_<ModBus, BusAddres>::StateRequest;

template<class ModBus, unsigned char BusAddres>
unsigned char InsulationControl_<ModBus, BusAddres>::_request[256];

template<class ModBus, unsigned char BusAddres>
unsigned char *InsulationControl_<ModBus, BusAddres>::_response;

template<class ModBus, unsigned char BusAddres>
unsigned char InsulationControl_<ModBus, BusAddres>::_address = BusAddres;

template<class ModBus, unsigned char BusAddres>
unsigned int InsulationControl_<ModBus, BusAddres>::_registers[64];

#endif