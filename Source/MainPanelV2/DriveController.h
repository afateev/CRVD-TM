////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		18/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef DRIVECONTROLLER_H
#define DRIVECONTROLLER_H

#include "../MainPanel/MegaLib/Modbus/ModbusMaster.h"
#include "Config.h"

struct RequestInfo
    {
        unsigned int FirstReg;
        unsigned int RegsCount;
        RequestInfo(unsigned int firstReg, unsigned int regsCount)
        {
            FirstReg = firstReg;
            RegsCount = regsCount;
        }
    };

struct OscRequestInfo
{
	unsigned char *DstBuffer;
	unsigned int StartPoint;
	unsigned int PointsCount;
	
	OscRequestInfo (unsigned char *dstBuffer, unsigned int startPoint, unsigned int pointsCount)
	{
		DstBuffer = dstBuffer;
		StartPoint = startPoint;
		pointsCount = pointsCount;
	}
};

template<class ModBus, unsigned char MainAddress, unsigned char AdditionalAddress, int oscRecordSize>
class DriveController
{
public:
	enum Registers
	{
		RegUst = 1,
		RegIst = 2,
		RegUrot = 3,
		RegIrot = 4,
		RegPhi = 5,
		RegOscCurPos = 9,
		RegOscEngineStart = 10,
		RegOscEngineStop = 11,
		RegOscEngineEvent = 12,
		RegOscChechoutStart = 13,
		RegOscChechoutStop = 14,
		RegOscSync = 15,
        // Параметры регулятора
        RegNominalStatorCurrent = 100,
        RegStartStatorCurrent = 102,
        RegNominalRotorCurrent = 104,
        RegStartRotorCurrent = 106,
		RegRotorCurrentMax = 108,
		RegRotorCurrentMin = 110,
        RegCosChannel = 112,
		RegAngleSetupDisplay = 113,
		RegEnergizingCurrentSetup = 115,
		RegGlideSetup = 117,
        RegGlideStart = 119,
		//RegH_ka_w_c0 = 121,
		RegH_ka_f_I0 = 122,
		RegH_ka_f_tg = 124,
		RegH_ka_f_v = 126,
        RegLowStatorVoltageProtection = 128,
		RegLowStatorVoltageProtectionThreshold = 129,
        RegLowStatorVoltageProtectionTimer = 131,
        RegLowStatorCurrentProtection = 132,
		RegLowStatorCurrentProtectionThreshold = 133,
        RegLowStatorCurrentProtectionTimer = 135,
        RegAsyncRotorRotationCount = 136,
        RegAsyncCountdownTimer = 137,
        RegAsyncProtectionTimer = 138,
        RegEnergizingLostTimer = 139,
        RegLongStartTimer = 140,
		RegShortCircuitProtectionThreshold = 141,
        RegShortCircuitProtectionTimer = 143,
		RegOverloadProtectionThreshold = 145,
		RegRotorCoolingCurrent = 147,
		RegRotorCoolingTime = 149,
		RegHeatingTimeOnOverload0_10 = 151,
		RegHeatingTimeOnOverload10_20 = 153,
		RegHeatingTimeOnOverload20 = 155,
        RegWaitAfterDriveOn = 157,
        RegWaitAfterStatorCurrenLow = 158,
        RegWaitAfterenergizingOn = 159,
        RegStartForsingTime = 160,
        RegStatorVoltageMaxDisplay = 161,
        RegStatorCurrentMaxDisplay = 162,
        RegRotorVoltageMaxDisplay = 163,
        RegRotorCurrentMaxDisplay = 164
	};
	
	enum OscType
	{
		OscEngineStart			= 0,
		OscEngineStop			= 1,
		OscEngineEvent			= 2,
		OscChechoutStart		= 3,
		OscChechoutStop			= 4,
		// кол-во типов осциллограмм
		OscTypeCount			= 5,
	};
	
	static const int OscRecordSize = oscRecordSize;
	static const int OscRequestMaxPortionSize = 250 / OscRecordSize;
	
	typedef Rblib::CallbackWrapper<> ModbusSelectCallbackType;
	typedef Rblib::CallbackWrapper<bool &> AllowOscReadCallbackType;
	typedef Rblib::CallbackWrapper<unsigned int, unsigned char *, int> OnOscReadedCallbackType;
	typedef Rblib::CallbackWrapper<OscType, unsigned int> OnOscEventCallbackType;
protected:
	enum State
	{
		StateRequest,		// отправка запроса
		StateWait,			// ожидание ответа
		StateResponse,		// обработка ответа
        StateCheckChanges,  // проверить изменения
        StateWaitWrite,     // ждём окончание записи
        StateWriteResponse, // обработка ответа на запись
		StateComplete		// закончили цикл
	};
	
	struct RegFlags
	{
		unsigned char Has 		: 1;
		unsigned char Edited 	: 1;
		unsigned char Reserved 	: 6;
	};
    
	static const unsigned int _registersCount = 256;
	
    static const RequestInfo requests[2];
	
	static State _state;
	static unsigned char _request[_registersCount];
	static unsigned char *_response;
	
	static unsigned char _address;
	static unsigned short _registers[_registersCount];
    static unsigned short _editedRegisters[_registersCount];
	static RegFlags _regState[_registersCount];
	//static bool _hasReg[_registersCount];
    //static bool _editReg[_registersCount];
    
    static unsigned char _currentRequest;
	static unsigned int _regRequestCountdown[2];
	static unsigned int _oscRequestCountdown;
    static bool _writeReg97;    // надо записать чтобы сохранился EEPROM вв регуляторе
    static bool _readOneShot;   // Перечитать один раз несмотря на чтение осциллограмм
    static unsigned int _problemCount;
    
	static unsigned int _oscLoadedPos;
	static int _oscEventPointer[OscTypeCount];
	
	static bool _oscRequestWait;
	static bool _oscResponseReady;
	static OscRequestInfo _oscRequest;
public:	
	static bool DoOnlyLowerRegsRequest;
	static ModbusSelectCallbackType ModbusSelectCallback;
	static AllowOscReadCallbackType AllowOscReadCallback;
	static OnOscReadedCallbackType OnOscReadedCallback;
	static OnOscEventCallbackType OnOscEventCallback;
public:
	static void Init()
	{
		ResetOscEventPointers();
		SetRegValue(RegOscSync, 0);
		_oscLoadedPos = 0;
	}
	
	static bool ImActive()
	{
		return _address == 1;
	}
	
	static void ResetOscEventPointers()
	{
		for (int i = 0; i < OscTypeCount; i++)
		{
			_oscEventPointer[i] = -1;
		}
	}
	
	static void UpdateOscEventPointers()
	{
		for (int i = 0; i < OscTypeCount; i++)
		{
			OscType oscType = (OscType)i;
			unsigned short currentPtr = 0xFFFF;
			switch (oscType)
			{
			case OscEngineStart:
				{
					currentPtr = GetRegValue(RegOscEngineStart);
				}
				break;
			case OscEngineStop:
				{
					currentPtr = GetRegValue(RegOscEngineStop);
				}
				break;
			case OscEngineEvent:
				{
					currentPtr = GetRegValue(RegOscEngineEvent);
				}
				break;
			case OscChechoutStart:
				{
					currentPtr = GetRegValue(RegOscChechoutStart);
				}
				break;
			case OscChechoutStop:
				{
					currentPtr = GetRegValue(RegOscChechoutStop);
				}
				break;
			}
			
			// если не было события, а сейчас возникло
			// при инициализации -1, при первом чтении просто перезапишется
			// если в данный момент есть событие но неизвестно было ли оно до запуска, т.е. в буфере -1, то считаем что оно возникло до запуска и не обрабатываем
			if (_oscEventPointer[i] > 0 && currentPtr != 0xFFFF && currentPtr != _oscEventPointer[i])
			{
				// обарбатываем возникновение события
				OnOscEventCallback(oscType, currentPtr);
			}
			
			_oscEventPointer[i] = currentPtr;
		}
	}
	
	static int GetOscLoadCount()
	{
		int res = 0;
		
		if (HasRegValue(RegOscCurPos))
		{
			unsigned int curPos = GetRegValue(RegOscCurPos);
			res = curPos - _oscLoadedPos;
			bool lastPart = false;
			if (curPos < _oscLoadedPos)
			{
				res = 0xFFFF - _oscLoadedPos;
				lastPart = true;
			}
			
			if (res > OscRequestMaxPortionSize)
			{
				res = OscRequestMaxPortionSize;
			}
			
			if (res < OscRequestMaxPortionSize && !lastPart)
			{
				res = 0;
			}
		}
		
		return res;
	}
	
	static bool Run()
	{
		bool allowOscRead = false;
		AllowOscReadCallback(allowOscRead);
		
		for (int i = 0; i < 2; i++)
		{
			if (_regRequestCountdown[i] > 0)
			{
				_regRequestCountdown[i]--;
			}
		}
		
		if (_oscRequestCountdown > 0)
		{
			_oscRequestCountdown--;
		}
		
		switch(_state)
		{
		case StateRequest:
			{
				if (ModBus::IsReady())
				{
                    ModBus::ChangeSpeed(Config::MainComPortClockSourceFrequency, 115200);
					ModBus::WaitLastByteTx = false;
										
					
					{
						bool requestSended = false;
						if (_regRequestCountdown[_currentRequest] < 1)
						{
							ModbusSelectCallback();
							unsigned int first = requests[_currentRequest].FirstReg;
							unsigned int count = requests[_currentRequest].RegsCount;
							unsigned int requestSize = ModBusMaster::BuildReadHoldingRegisters(_request, _address, first, count);
							for (int i = first; i < first + count; i++)
								_regState[i].Has = false;
							ModBus::SendRequest(_request, requestSize);
							requestSended = true;
							
							switch (_currentRequest)
							{
							case 0:
								{
									_regRequestCountdown[_currentRequest] = ImActive() ? 100 : 500;
								}
								break;
							case 1:
								{
									_regRequestCountdown[_currentRequest] = 500;
								}
								break;
							}
						}
						
						_currentRequest++;
						if (_currentRequest >= (sizeof(requests) / sizeof(RequestInfo)))
						{
							_currentRequest = 0;
						}
						if (DoOnlyLowerRegsRequest && !_readOneShot && _currentRequest > 0)
						{
							_currentRequest = 0;
						}
						
						if (requestSended)
						{
							_state = StateWait;
							break;
						}
					}
					
					if (_oscRequestCountdown < 1 )
					{
						// осциллограмму вычитываем только из активного регулятора, а он имеет всегда адрес №1
						if (ImActive())
						{
							int loadCount = GetOscLoadCount();
							
							if (loadCount >= OscRequestMaxPortionSize)
							{
								if (allowOscRead)
								{
										ModbusSelectCallback();
										unsigned int requestSize = ModBusMaster::BuildReadOscPoints(_request, _address, _oscLoadedPos, loadCount);
										ModBus::SendRequest(_request, requestSize);
										
										_oscRequestCountdown = 50;
										_state = StateWait;
										break;
								}
							}
						}
					}
				}
				else
				{
					__no_operation();
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
					_problemCount = 0;
                    
                    unsigned int firstReg = _request[2];
					firstReg <<= 8;
					firstReg |= _request[3];
					
					unsigned int regQuantity = _request[4];
					regQuantity <<= 8;
					regQuantity |= _request[5];
					
					if ((_request[0] == _response[0]) &&
						(_request[1] == _response[1] && ModBus::CrcOk(_response, readed))
					   )
					{
						if ((_response[1] == 0x03) && (2 * regQuantity == _response[2]))
						{
							unsigned int regPtr = 3;
                            if (_readOneShot && firstReg >= 100)
                            {
                                _readOneShot = false;
                            }
							for (unsigned int reg = firstReg; reg < firstReg + regQuantity; reg ++)
							{
								if (regPtr + 2 >= readed)
									break;
								unsigned short regValue = _response[regPtr];
								regValue <<= 8;
								regValue |= _response[regPtr + 1];
								
								_registers[reg] = regValue;
                            	//_editedRegisters[reg] = _registers[reg];
								_regState[reg].Has = true;
								
								regPtr += 2;
							}
							
							if (ImActive())
							{
								// если получены свежие данные об указателях осциллограмм
								if (firstReg <= RegOscEngineStart && firstReg + regQuantity >= RegOscChechoutStop)
								{
									UpdateOscEventPointers();
								}
							}
						}
						
						if (_response[1] == 0x66)
						{
							_oscRequestWait = false;
							
							if (OscRecordSize * regQuantity == _response[2])
							{
								OnOscReadedCallback(_oscLoadedPos * OscRecordSize, &_response[3], OscRecordSize * regQuantity);
								
								_oscLoadedPos += regQuantity;
								
								if (_oscLoadedPos >= 0xFFFF)
								{
									_oscLoadedPos = 0;
								}
								
								/*
								_oscResponseReady = true;
								
								if (_oscRequest.DstBuffer != 0)
								{
									memcpy(_oscRequest.DstBuffer, &_response[3], oscSize * regQuantity);
								}*/
							}
						}
					}
				}
				else
				{
					if (_problemCount < 1000000)
                    {
                        _problemCount++;
                    }
                    
                    if (_address == MainAddress)
						_address = AdditionalAddress;
					else
						_address = MainAddress;
				}
				//_state = StateComplete;
                _state = StateCheckChanges;
			}
			break;
        case StateCheckChanges:
            {
                if (ModBus::IsReady())
                {    
                    bool changed = false;
                    for (unsigned int i = 0; i < 256; i++)
                    {
                        if (i != RegOscSync && i < 100)
						{
							continue;
						}
						
						if (i == 0)
                        {
                            _writeReg97 = false;
                        }
                        
                        if (_regState[i].Has && _regState[i].Edited)
                        {
                            if (_registers[i] != _editedRegisters[i] || i == RegOscSync)
                            {
                                ModbusSelectCallback();
								unsigned int requestSize = ModBusMaster::BuildWriteHoldingRegister(_request, _address, i, _editedRegisters[i]);
					            
					            ModBus::SendRequest(_request, requestSize);
                            
                                //_editedRegisters[i] = _registers[i];
                                changed = true;
                                _writeReg97 = true;
                                _regState[i].Edited = false;
                                break;
                            }
                        }
                    }
                    
                    // новых изменений нет, но раньше были
                    if (!changed && _writeReg97)
                    {
                        ModbusSelectCallback();
						unsigned int requestSize = ModBusMaster::BuildWriteHoldingRegister(_request, _address, 97, 0xAA);
			            ModBus::SendRequest(_request, requestSize);
                        changed = true;
                        _writeReg97 = false;
                        _readOneShot = true;
                    }
                    
                    if (changed)
                    {
                        _state = StateWaitWrite;
                    }
                    else
                    {
                        int loadCount = GetOscLoadCount();
						
						if (loadCount > 0 && allowOscRead && ImActive())
						{
							_state = StateRequest;
						}
						else
						{
							_state = StateComplete;
						}
                    }
                }
            }
            break;
        case StateWaitWrite:
			{
				if (ModBus::IsReadComplete())
					_state = StateWriteResponse;
			}
			break;
        case StateWriteResponse:
			{
				if (!ModBus::IsReadComplete())
				{
					break;
				}
				
				unsigned int readed = 0;
				_response = ModBus::GetResponse(readed);
				if (readed > 0 && 0 != _response)
				{
					_state = StateCheckChanges;
                    /*
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
                            _editedRegisters[reg] = _registers[reg];
							_hasReg[reg] = true;
								
							regPtr += 2;
						}
					}
                    */
				}
				_state = StateCheckChanges;
			}
			break;
		case StateComplete:
			{
				_state = StateRequest;
			}
			break;
		}
		return StateComplete == _state && _currentRequest == 0;
	}
	
	static unsigned short GetRegValue(unsigned short reg)
	{
		if (reg > MaxRegNumber())
			return 0;
		
		return _registers[reg];
	}
	
	static void GetRegValue(int reg, unsigned short &value)
	{
		value = GetRegValue(reg);
	}
    
    static void SetRegValue(unsigned short reg, unsigned short value)
    {
        if (reg > MaxRegNumber())
			return;
		
		_editedRegisters[reg] = value;
        _regState[reg].Edited = true;
    }
	
	static bool HasRegValue(unsigned short reg)
	{
		if (reg > MaxRegNumber())
			return false;
		
		return _regState[reg].Has;
	}
    
    static bool HasProblem()
    {
        return _problemCount > 10;
    }
	
	static unsigned char GetAddress()
	{
		return _address;
	}
	
	static void GetAddress(unsigned char &value)
	{
		value = GetAddress();
	}
	
	static inline unsigned short MinRegNumber()
	{
		return 0;
	}
	
	static inline unsigned short MaxRegNumber()
	{
		return _registersCount - 1;
	}
	
	static bool OscRequest(unsigned char *dst, unsigned int StartPoint, unsigned int PointsCount)
	{
		if (_oscRequestWait)
		{
			return false;
		}
		
		_oscResponseReady = false;
		_oscRequest.DstBuffer = dst;
		
		_oscRequestWait = true;
		
		_oscRequest.StartPoint = StartPoint;
		
		unsigned int count = PointsCount;
		// в поле данных влезет не более (250 / OscRecordSize) записей
		if (count > OscRequestMaxPortionSize)
		{
			count = OscRequestMaxPortionSize;
		}
		unsigned int end = StartPoint + count;
		if (end > 0xFFFF)
		{
			count = 0xFFFF - StartPoint + 1;
		}
		
		_oscRequest.PointsCount = count;
		
		return true;
	}
	
	static inline bool OscRequestWait()
	{
		return _oscRequestWait;
	}
	
	static bool OscResponseReady(unsigned int &StartPoint, unsigned int &PointsCount)
	{
		StartPoint = _oscRequest.StartPoint;
		PointsCount = _oscRequest.PointsCount;
		return _oscResponseReady;
	}
	
	static bool IsWaitRegistersResponse()
	{
		return _state == StateWait || _state == StateResponse;
	}
};

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::ModbusSelectCallbackType DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::ModbusSelectCallback;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::AllowOscReadCallbackType DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::AllowOscReadCallback;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::OnOscReadedCallbackType DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::OnOscReadedCallback;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::OnOscEventCallbackType DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::OnOscEventCallback;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
typename DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::State
DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_state = 
DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::StateRequest;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
unsigned char DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_request[256];

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
unsigned char *DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_response;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
unsigned char DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_address = MainAddres;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
unsigned short DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_registers[256];

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
unsigned short DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_editedRegisters[256];
/*
template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress>
bool DriveController<ModBus, MainAddres, AdditionalAddress>::_hasReg[256];

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress>
bool DriveController<ModBus, MainAddres, AdditionalAddress>::_editReg[256];
*/

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::RegFlags DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_regState[256];

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
unsigned char DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_currentRequest = 0;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
unsigned int DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_regRequestCountdown[] = {0, 0};

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
unsigned int DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_oscRequestCountdown = 0;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
const RequestInfo DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::requests[2] = {RequestInfo(1, 15), RequestInfo(100, 69)};

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
bool DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_writeReg97 = false;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
bool DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_readOneShot = false;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
unsigned int DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_problemCount = 0;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
unsigned int DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_oscLoadedPos = 0;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
int DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_oscEventPointer[];

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
bool DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_oscRequestWait = false;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
bool DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_oscResponseReady = false;

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
OscRequestInfo DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::_oscRequest = OscRequestInfo(0, 0, 0);

template<class ModBus, unsigned char MainAddres, unsigned char AdditionalAddress, int oscRecordSize>
bool DriveController<ModBus, MainAddres, AdditionalAddress, oscRecordSize>::DoOnlyLowerRegsRequest = false;

#endif