////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		18/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef DRIVECONTROLLERINTERFACE_H
#define DRIVECONTROLLERINTERFACE_H

#include "DriveController.h"
#include <math.h>

class DriveControllerInterfaceBase
{
public:
	virtual unsigned short GetRegValue(unsigned int reg) = 0;
    virtual void SetRegValue(unsigned int reg, unsigned short value) = 0;
	virtual bool HasRegValue(unsigned int reg) = 0;
	virtual bool IsPrimary() = 0;
	virtual bool HasParams() = 0;
    virtual bool HasProblem() = 0;
	virtual unsigned short MinRegNumber() = 0;
	virtual unsigned short MaxRegNumber() = 0;
	virtual bool IsWaitRegistersResponse() = 0;
	virtual void SetDoOnlyLowerRegsRequest(bool value) = 0;
};

template<class DriveController, bool Primary>
class DriveControllerInterface : public DriveControllerInterfaceBase
{
public:
	virtual unsigned short GetRegValue(unsigned int reg)
	{
		return DriveController::GetRegValue(reg);
	}
    
    virtual void SetRegValue(unsigned int reg, unsigned short value)
    {
        DriveController::SetRegValue(reg, value);
    }
	
	virtual bool HasRegValue(unsigned int reg)
    {
        return DriveController::HasRegValue(reg);
    }
	
	virtual unsigned short MinRegNumber()
	{
		return DriveController::MinRegNumber();
	}
	
	virtual unsigned short MaxRegNumber()
	{
		return DriveController::MaxRegNumber();
	}
	
	virtual bool IsPrimary()
	{
		return Primary;
	}
	
	virtual bool HasParams()
	{
		return DriveController::HasRegValue(1);
	}
    
    virtual bool HasProblem()
    {
        return DriveController::HasProblem();
    }
	
	virtual bool IsWaitRegistersResponse()
	{
		return DriveController::IsWaitRegistersResponse();
	}
	
	virtual void SetDoOnlyLowerRegsRequest(bool value)
	{
		DriveController::DoOnlyLowerRegsRequest = value;
	}
};

template<class ControllerInterfaceType, class Config>
class DriveControllerParams
{
public:
	enum StateFlags
	{
		SfEnergizing	= 0,
		SfForcing		= 1,
		SfCosControl	= 3,
		SfRControl		= 4,
		SfSetDown		= 5,
		SfSetUp			= 6,
		SfHeatRot		= 7,
		SfCoolRot		= 8,
		SfAsynch		= 9,
		SfEngineOn		= 10,
		SfCheckout		= 11,
		SfMain			= 12
	};
	
	enum StopFlags
	{
		ProtAsynch,
		ProtLostEnergizin,
		ProtWrongMode,
		ProtLongStart,
		ProtKZ,
		ProtLowUst,
		ProtLowIst,
		ProtStator,
		StopButton
	};
	
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
        RegRotorCurrentMaxDisplay = 164,
		RegEnergizingReactCurrentSetup = 165,
		RegEnergizingReactPowerSetup = 167
	};
	
	enum IndicationParams
	{
		IndicationParamStatorVoltageMinDisplay = 0,
		IndicationParamStatorCurrentMinDisplay,
		IndicationParamRotorVoltageMinDisplay,
		IndicationParamRotorCurrentMinDisplay,
		
		IndicationParamLast	// не удалять, это количество элементов массива
	};
	
	enum ModbusParams
	{
		ModbusParamSlaveAddr = 0,
		ModbusParamBoudrate,
		ModbusParamParity,
		ModbusParamParityType,
		
		ModbusParamLast	// не удалять, это количество элементов массива
	};
protected:
	static ControllerInterfaceType *_activeController;
	static const float Pi;
	static const unsigned char StateReg = 0x06;
	static const unsigned char StopReg = 0x08;
	
	__no_init static unsigned long _currentRun;			// сколько прошло с момента текущего запуска
	static unsigned long _lastRun;              // премя предыдущего запуска
    static unsigned long _workTimeAllreadySaved;// то что уже сохранено после текущего запуска
	static unsigned long _workTimeTotal;		// то что накоплено в файле всего
	static bool _workTimeReaded;
	
	static const char _fileUptime[];
	static const char _fileIndicationParams[];
	static const char _fileModbusParams[];
	
	typedef bool FileCallback(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count);
	static FileCallback *_readFile;
	static FileCallback *_writeFile;
	
	static float _indicationParams[IndicationParamLast];
	static float _modbusParams[ModbusParamLast];
	
	static bool _loadedIndicationParams;
	static bool _savedIndicationParams;
	
	static bool _loadedModbusParams;
	static bool _savedModbusParams;
public:
	static void Init(FileCallback *read, FileCallback *write)
	{
		_readFile = read;
		_writeFile = write;
		memset(_indicationParams, 0, sizeof(_indicationParams));
		memset(_modbusParams, 0, sizeof(_modbusParams));
		_modbusParams[ModbusParamSlaveAddr] = 1;
		_modbusParams[ModbusParamBoudrate] = 9600;
		_modbusParams[ModbusParamParity] = 0;
		_modbusParams[ModbusParamParityType] = 0;
	}
	
	static void SetActiveController(ControllerInterfaceType *controller)
	{
		_activeController = controller;
	}
	
	static bool IsPrimaryActive()
	{
		if (0 == _activeController)
			return false;
		return _activeController->IsPrimary();
	}
	
	static bool HasRegValue(unsigned int reg)
	{
		if (0 == _activeController)
		{
			return false;
		}
		
		return _activeController->HasRegValue(reg);
	}
	
	static bool GetRegValues(unsigned char *buffer, unsigned int bufferLen, unsigned short firstAddr, unsigned short quantity)
	{
		if (0 == _activeController)
		{
			return false;
		}
		
		unsigned short minReg = _activeController->MinRegNumber();
		unsigned short maxReg = _activeController->MaxRegNumber();
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
			unsigned short regVal = _activeController->GetRegValue(i);

			buffer[j] = (regVal >> 8) & 0xFF;
			buffer[j + 1] = regVal & 0xFF;
			
			j += 2;
		}
		
		return true;
	}
	
	static bool HasParams()
	{
		if (0 == _activeController)
			return false;
		return _activeController->HasParams();
	}
    
    static bool HasProblem()
    {
        if (0 == _activeController)
			return false;
        return _activeController->HasProblem();
    }
	
	static float GetUst()
	{
		if (0 == _activeController)
			return 0.0;
		
		unsigned short data = _activeController->GetRegValue(RegUst);
		float res = data;
		if (res < _indicationParams[IndicationParamStatorVoltageMinDisplay])
			res = 0.0;
		return res;
	}
	
	static float GetIst()
	{
		if (0 == _activeController)
			return 0.0;
		
		unsigned short data = _activeController->GetRegValue(RegIst);
		float res = data;
		res /= 10.0;
		if (res < _indicationParams[IndicationParamStatorCurrentMinDisplay])
			res = 0.0;
		return res;
	}
	static float GetUrot()
	{
		if (0 == _activeController)
			return 0.0;
		
		unsigned short data = _activeController->GetRegValue(RegUrot);
		float res = data;
		res /= 10.0;
		if (res < _indicationParams[IndicationParamRotorVoltageMinDisplay])
			res = 0.0;
		return res;
	}
	
	static float GetIrot()
	{
		if (0 == _activeController)
			return 0.0;
		
		unsigned short data = _activeController->GetRegValue(RegIrot);
		float res = data;
		res /= 10.0;
		if (res < _indicationParams[IndicationParamRotorCurrentMinDisplay])
			res = 0.0;
		return res;
	}
	
	static float GetPhi()
	{
		if (0 == _activeController)
			return 0.0;
		
		unsigned short data = _activeController->GetRegValue(RegPhi);
        short a = data;
		float res = a;
		res /= 10.0;
		return res;
	}
	
	static float GetCosPhi()
	{
		float phi = GetPhi();
		float signPhi = phi >= 0 ? 1 : -1;
		float res = cos(phi * Pi / 180.0) * signPhi;
		return res;
	}
	
	static float GetP()
	{
		float Ust = GetUst();
        float Ist = GetIst();
		float cosPhi = GetCosPhi();
        float sqrt3 = 1.7320508075688772935274463415059;
		//float res = fabs(Ist * cosPhi);
        float res = fabs(Ust * Ist * cosPhi * sqrt3);
        res /= 1000000;
		return res;
	}
	
	static float GetQ()
	{
		float Ust = GetUst();
        float Ist = GetIst();
		float phi = GetPhi();
        float sqrt3 = 1.7320508075688772935274463415059;
		//float res = Ist * sin(phi * Pi / 180.0);
        float res = Ust * Ist * sin(phi * Pi / 180.0) * sqrt3;
        res /= 1000000;
		return res;
	}
	
	static int GetCosControl()
	{
		int bit0 = GetFlag(StateReg, SfCosControl - 1);
		int bit1 = GetFlag(StateReg, SfCosControl);
		int val = bit1 << 1 | bit0;
		return val + 1;
	}
	
	static bool GetFlagRControl()
	{
		return GetFlag(StateReg, SfRControl);
	}
	
	static bool GetFlagEngineOn()
	{
		return GetFlag(StateReg, SfEngineOn);
	}
	
	static bool GetFlagCheckout()
	{
		return GetFlag(StateReg, SfCheckout);
	}
	
	static unsigned char GetStopFlag()
	{
		if (GetFlag(StopReg, StopButton))
			return StopButton;
		
		if (0 == _activeController)
			return 0xFF;
		
		unsigned short data = _activeController->GetRegValue(StopReg);
		for(unsigned char i = 0; i < 8; i++)
		{
			if ((data >> i) & 0x01)
				return i;
		}
		
		return 0xFF;
	}
	
	static unsigned short GetStateReg()
	{
		if (0 == _activeController)
			return 0;
		
		unsigned short data = _activeController->GetRegValue(StateReg);
		return data;
	}
	
	static unsigned short GetProtectionStateReg()
	{
		if (0 == _activeController)
			return 0;
		
		unsigned short data = _activeController->GetRegValue(StopReg);
		return data;
	}
	
	static unsigned short GetOscCurPos()
	{
		unsigned short data = 0xFFFF;
		
		if (0 == _activeController)
		{
			return data;
		}
		
		if (_activeController->HasRegValue(RegOscCurPos))
		{
			data = _activeController->GetRegValue(RegOscCurPos);
		}
		
		return data;
	}
	
	static unsigned short GetOscEngineStart()
	{
		unsigned short data = 0xFFFF;
		
		if (0 == _activeController)
		{
			return data;
		}
		
		if (_activeController->HasRegValue(RegOscEngineStart))
		{
			data = _activeController->GetRegValue(RegOscEngineStart);
		}
		
		return data;
	}
	
	static unsigned short GetOscEngineStop()
	{
		unsigned short data = 0xFFFF;
		
		if (0 == _activeController)
		{
			return data;
		}
		
		if (_activeController->HasRegValue(RegOscEngineStop))
		{
			data = _activeController->GetRegValue(RegOscEngineStop);
		}
		
		return data;
	}
	
	static unsigned short GetOscEngineEvent()
	{
		unsigned short data = 0xFFFF;
		
		if (0 == _activeController)
		{
			return data;
		}
		
		if (_activeController->HasRegValue(RegOscEngineEvent))
		{
			data = _activeController->GetRegValue(RegOscEngineEvent);
		}
		
		return data;
	}
	
	static unsigned short GetOscCheckoutStart()
	{
		unsigned short data = 0xFFFF;
		
		if (0 == _activeController)
		{
			return data;
		}
		
		if (_activeController->HasRegValue(RegOscChechoutStart))
		{
			data = _activeController->GetRegValue(RegOscChechoutStart);
		}
		
		return data;
	}
	
	static unsigned short GetOscCheckoutStop()
	{
		unsigned short data = 0xFFFF;
		
		if (0 == _activeController)
		{
			return data;
		}
		
		if (_activeController->HasRegValue(RegOscChechoutStop))
		{
			data = _activeController->GetRegValue(RegOscChechoutStop);
		}
		
		return data;
	}
	
	static bool OscRequest(unsigned char *dst, unsigned int StartPoint, unsigned int PointsCount)
	{
		if (0 == _activeController)
		{
			return false;
		}
		
		return _activeController->OscRequest(dst, StartPoint, PointsCount);
	}
	
	static inline bool OscRequestWait()
	{
		if (0 == _activeController)
		{
			return false;
		}
		
		return _activeController->OscRequestWait();
	}
	
	static bool OscResponseReady(unsigned int &StartPoint, unsigned int &PointsCount)
	{
		if (0 == _activeController)
		{
			return false;
		}
		
		return _activeController->OscResponseReady(StartPoint, PointsCount);
	}
	
	static bool IsWaitRegistersResponse()
	{
		if (0 == _activeController)
		{
			return false;
		}
		
		return _activeController->IsWaitRegistersResponse();
	}
	
	static void SetDoOnlyLowerRegsRequest(bool value)
	{
		if (0 == _activeController)
		{
			return;
		}
		
		_activeController->SetDoOnlyLowerRegsRequest(value);
	}
	
    static float GetNominalStatorCurrent()
    {
        return GetRegValueFloat(RegNominalStatorCurrent);
    }
    
    static void SetNominalStatorCurrent(float value)
    {
    	SetRegValueFloat(RegNominalStatorCurrent, value);
    }
	
	template<unsigned char reg>
	static float GetRegValue_float()
	{
		return GetRegValueFloat(reg);
	}
	
	template<unsigned char reg>
	static void SetRegValue_float(float value)
	{
		SetRegValueFloat(reg, value);
	}
    
    template<unsigned char reg>
	static int GetRegValue_int()
	{
		return GetRegValue(reg);
	}
	
	template<unsigned char reg>
	static void SetRegValue_int(int value)
	{
		SetRegValue(reg, value);
	}
	
	static float GetAngleSetupDisplay()
	{
		float regVal = GetRegValueFloat(RegAngleSetupDisplay);
		float sign = regVal < 0 ? -1 : 1;
		float res = cos(regVal * Pi / 180.0);
		res *= sign;
		return res;
	}
	
	static void SetAngleSetupDisplay(float val)
	{
		float sign = val < 0 ? -1 : 1;
		if (sign < 0)
		{
			val *= sign;
		}
		
		if (val > 1.0)
		{
			val = 1.0;
		}
		
		float regVal = acos(val) * 180.0 / Pi;
		regVal *= sign;
		SetRegValueFloat(RegAngleSetupDisplay, regVal);
	}
	
	//////////////////////
	template<unsigned char param>
	static float GetIndicationParamValue_float()
	{
		return GetIndicationParam(param);
	}
	
	template<unsigned char param>
	static void SetIndicationParamValue_float(float value)
	{
		SetIndicationParam(param, value);
	}
    
    template<unsigned char param>
	static int GetIndicationParamValue_int()
	{
		return (int)GetIndicationParam(param);
	}
	
	template<unsigned char param>
	static void SetIndicationParamValue_int(int value)
	{
		SetIndicationParam(param, value);
	}
	
	//////////////////////
	template<unsigned char param>
	static float GetModbusParamValue_float()
	{
		return GetModbusParam(param);
	}
	
	template<unsigned char param>
	static void SetModbusParamValue_float(float value)
	{
		SetModbusParam(param, value);
	}
    
    template<unsigned char param>
	static int GetModbusParamValue_int()
	{
		return (int)GetModbusParam(param);
	}
	
	template<unsigned char param>
	static void SetModbusParamValue_int(int value)
	{
		SetModbusParam(param, value);
	}
	
	//////////////////////
	
	static void Tick()
	{
		static unsigned int oneSecondCounter = 0;
		static const unsigned int OneSecondMaxCount = 1000;
		
		oneSecondCounter++;
		if (oneSecondCounter >= OneSecondMaxCount)
		{
			oneSecondCounter = 0;
			if (GetFlagEngineOn())
				_currentRun++;
		}
	}
	
	static void Run()
	{
		if (!_readFile)
			return;
		if (!_writeFile)
			return;
		
		if (!_workTimeReaded)
		{
			unsigned long savedTime = 0;
			// вообще читается?
			if (!_readFile(_fileUptime, 0, 0, (unsigned char *)&savedTime, 0))
				return;
			// если пусто записшем нолик
			if (!_readFile(_fileUptime, 0, 0, (unsigned char *)&savedTime, sizeof(savedTime)))
			{
				if (!_writeFile(_fileUptime, 0, 0, (unsigned char *)&savedTime, sizeof(savedTime)))
				return;
			}
			// не пустой, прочитаем что есть
			if (!_readFile(_fileUptime, 0, 0, (unsigned char *)&savedTime, sizeof(savedTime)))
				return;
			// прочитали
			_workTimeTotal = savedTime;
			_workTimeReaded = true;
		}
		else
		{
			bool needWrite = false; // пора обновить файлик
			if (_currentRun > 0)
            {
                unsigned long notSaved = _currentRun - _workTimeAllreadySaved;
                // если натикало с момента последнего сохранения больше чем час, то пора сохранить
                if (notSaved >= 3600)
                    needWrite = true;
                if (!GetFlagEngineOn() && notSaved > 0)
                {
                    needWrite = true;
                    
                }
                if (needWrite)
                {
                    unsigned long toSave = _workTimeTotal + notSaved;
                    if (!_writeFile(_fileUptime, 0, 0, (unsigned char *)&toSave, sizeof(toSave)))
                        return;
                    
                    _workTimeAllreadySaved += notSaved;
                    _workTimeTotal = toSave;
                    
                    if (!GetFlagEngineOn())
                    {
                        _lastRun = _currentRun;
                        _currentRun = 0;
                        _workTimeAllreadySaved = 0;
                    }
                }
            }
		}
		
		// Параметры индикации
		if (!_loadedIndicationParams)
		{
			float tmp[IndicationParamLast];
			// вообще читается?
			if (!_readFile(_fileIndicationParams, 0, 0, (unsigned char *)tmp, 0))
				return;
			// если пусто или размер не тот запишем что есть
			if (!_readFile(_fileIndicationParams, 0, 0, (unsigned char *)tmp, sizeof(tmp)))
			{
				if (!_writeFile(_fileIndicationParams, 0, 0, (unsigned char *)_indicationParams, sizeof(_indicationParams)))
				return;
			}
			// не пустой, прочитаем что есть
			if (!_readFile(_fileIndicationParams, 0, 0, (unsigned char *)tmp, sizeof(tmp)))
				return;
			// прочитали
			for (int i = 0; i < IndicationParamLast; i++)
			{
				_indicationParams[i] = tmp[i];
			}
			_loadedIndicationParams = true;
			_savedIndicationParams = true;
		}
		else
		{
			if (!_savedIndicationParams)
			{
				if (!_writeFile(_fileIndicationParams, 0, 0, (unsigned char *)_indicationParams, sizeof(_indicationParams)))
                	return;
				_savedIndicationParams = true;
			}
		}
		
		// Параметры Modbus
		if (!_loadedModbusParams)
		{
			float tmp[ModbusParamLast];
			// вообще читается?
			if (!_readFile(_fileModbusParams, 0, 0, (unsigned char *)tmp, 0))
				return;
			// если пусто или размер не тот запишем что есть
			if (!_readFile(_fileModbusParams, 0, 0, (unsigned char *)tmp, sizeof(tmp)))
			{
				if (!_writeFile(_fileModbusParams, 0, 0, (unsigned char *)_modbusParams, sizeof(_modbusParams)))
				return;
			}
			// не пустой, прочитаем что есть
			if (!_readFile(_fileModbusParams, 0, 0, (unsigned char *)tmp, sizeof(tmp)))
				return;
			// прочитали
			for (int i = 0; i < ModbusParamLast; i++)
			{
				_modbusParams[i] = tmp[i];
			}
			_loadedModbusParams = true;
			_savedModbusParams = true;
		}
		else
		{
			if (!_savedModbusParams)
			{
				if (!_writeFile(_fileModbusParams, 0, 0, (unsigned char *)_modbusParams, sizeof(_modbusParams)))
                	return;
				_savedModbusParams = true;
			}
		}
	}
    
    static void ResetOperatingTime()
    {
        unsigned long toSave = 0;
        if (!_writeFile(_fileUptime, 0, 0, (unsigned char *)&toSave, sizeof(toSave)))
            return;
        
        _workTimeTotal = 0;
    }
	
	static long GetUpTime()
	{
		if (GetFlagEngineOn())
            return _currentRun;
        return _lastRun;
	}
	
	static void ResetUpTime()
	{
		_currentRun = 0;
	}
	
	static long GetOperatingTime()
	{
		return _workTimeTotal - _workTimeAllreadySaved + _currentRun;
	}
	
	static int GetRegValue(unsigned char reg)
	{
		if (0 == _activeController)
			return 0;
        return _activeController->GetRegValue(reg);
	}
	
	static float GetRegValueFloat(unsigned char reg)
	{
		if (0 == _activeController)
			return 0.0;
        unsigned short dataHi = _activeController->GetRegValue(reg);
        unsigned short dataLo = _activeController->GetRegValue(reg + 1);
        unsigned int data;
        data = dataHi;
        data <<= 16;
        data |= dataLo;
        float res = 0;
        memcpy(&res, &data, 4);
        return res;
	}
protected:
	static bool GetFlag(unsigned char reg, unsigned char bit)
	{
		if (0 == _activeController)
			return false;
		
		unsigned short data = _activeController->GetRegValue(reg);
		bool res = (data >> bit) & 0x01;
		return res;
	}
	
	static void SetRegValueFloat(unsigned char reg, float value)
    {
        unsigned int data = 0;
        memcpy(&data, &value, 4);
        unsigned short dataHi = (data >> 16) & 0xFFFF;
        unsigned short dataLo = data & 0xFFFF;
        
        _activeController->SetRegValue(reg, dataHi);
        _activeController->SetRegValue(reg + 1, dataLo);
    }
	
	static void SetRegValue(unsigned char reg, int value)
    {
        _activeController->SetRegValue(reg, value);
    }
	
	static float GetIndicationParam(unsigned char param)
	{
		return _indicationParams[param];
	}
	
	static void SetIndicationParam(unsigned char param, float value)
    {
        _indicationParams[param] = value;
		_savedIndicationParams = false;
    }
	
	static float GetModbusParam(unsigned char param)
	{
		return _modbusParams[param];
	}
	
	static void SetModbusParam(unsigned char param, float value)
    {
        _modbusParams[param] = value;
		_savedModbusParams = false;
    }
};

template<class ControllerInterfaceType, class Config>
ControllerInterfaceType *DriveControllerParams<ControllerInterfaceType, Config>::_activeController = 0;

template<class ControllerInterfaceType, class Config>
float DriveControllerParams<ControllerInterfaceType, Config>::Pi = 3.1415926535897932384626433832795;

template<class ControllerInterfaceType, class Config>
unsigned long DriveControllerParams<ControllerInterfaceType, Config>::_currentRun;

template<class ControllerInterfaceType, class Config>
unsigned long DriveControllerParams<ControllerInterfaceType, Config>::_lastRun = 0;

template<class ControllerInterfaceType, class Config>
unsigned long DriveControllerParams<ControllerInterfaceType, Config>::_workTimeAllreadySaved = 0;

template<class ControllerInterfaceType, class Config>
unsigned long DriveControllerParams<ControllerInterfaceType, Config>::_workTimeTotal = 0;

template<class ControllerInterfaceType, class Config>
bool DriveControllerParams<ControllerInterfaceType, Config>::_workTimeReaded = false;

template<class ControllerInterfaceType, class Config>
DriveControllerParams<ControllerInterfaceType, Config>::FileCallback *DriveControllerParams<ControllerInterfaceType, Config>::_readFile = 0;

template<class ControllerInterfaceType, class Config>
DriveControllerParams<ControllerInterfaceType, Config>::FileCallback *DriveControllerParams<ControllerInterfaceType, Config>::_writeFile = 0;

template<class ControllerInterfaceType, class Config>
float DriveControllerParams<ControllerInterfaceType, Config>::_indicationParams[DriveControllerParams<ControllerInterfaceType, Config>::IndicationParamLast];

template<class ControllerInterfaceType, class Config>
float DriveControllerParams<ControllerInterfaceType, Config>::_modbusParams[DriveControllerParams<ControllerInterfaceType, Config>::ModbusParamLast];

#ifdef SD_STORAGE
template<class ControllerInterfaceType, class Config>
const char DriveControllerParams<ControllerInterfaceType, Config>::_fileUptime[] = "/uptime.bin";

template<class ControllerInterfaceType, class Config>
const char DriveControllerParams<ControllerInterfaceType, Config>::_fileIndicationParams[] = "/i_params.bin";

template<class ControllerInterfaceType, class Config>
const char DriveControllerParams<ControllerInterfaceType, Config>::_fileModbusParams[] = "/mbus_params.bin";
#endif

#ifdef USB_STORAGE
template<class ControllerInterfaceType, class Config>
const char DriveControllerParams<ControllerInterfaceType, Config>::_fileUptime[] = "uptime.bin";

template<class ControllerInterfaceType, class Config>
const char DriveControllerParams<ControllerInterfaceType, Config>::_fileIndicationParams[] = "i_params.bin";

template<class ControllerInterfaceType, class Config>
const char DriveControllerParams<ControllerInterfaceType, Config>::_fileModbusParams[] = "mbus_params.bin";
#endif

template<class ControllerInterfaceType, class Config>
bool DriveControllerParams<ControllerInterfaceType, Config>::_loadedIndicationParams = false;

template<class ControllerInterfaceType, class Config>
bool DriveControllerParams<ControllerInterfaceType, Config>::_savedIndicationParams = true;

template<class ControllerInterfaceType, class Config>
bool DriveControllerParams<ControllerInterfaceType, Config>::_loadedModbusParams = false;

template<class ControllerInterfaceType, class Config>
bool DriveControllerParams<ControllerInterfaceType, Config>::_savedModbusParams = true;

#endif