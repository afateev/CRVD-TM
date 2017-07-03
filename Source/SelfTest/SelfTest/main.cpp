#include "..\Lib\Lib.h"
#include "SignalAnalogInput.h"
#include "SignalDiscreteInput.h"
#include "SignalDiscreteOutput.h"
#include "Logic.h"

static const unsigned long ExtClockSourceFreq = 16000000;

typedef Lib::Timer::Timer0 Timer;
typedef Lib::Timer::Timer2 ScanTimer;
typedef Lib::Usart::Usart0 TestSerialBus;
typedef Lib::Usart::Usart1 Serial485;
typedef Lib::Adc::Adc0 Adc;

template
	<
		class Signal
	>
class SignalPulseDetector
{
public:
	typedef Lib::CallbackWrapper<> CallbackType;
	static CallbackType OnFront;
	static const int NormalTickCount = 200;
protected:
	static bool _state;
	static unsigned int _counter;
	static float _frequency;
	static const unsigned int MaxCount = 200 * 10 * 2;
	static const unsigned int TimeoutThreshold = 200 * 10;
public:
	static void Init()
	{
		Reset();
	}
	
	static void Reset()
	{
		_state = false;
		_counter = 0;
		_frequency = 0xFFFF;
	}
	
	static void OnTick()
	{
		bool newState = Signal::Get();
		// спад
		if ((_state != newState) && (newState == 0))
		{
			if (_counter > 0)
			{
				_frequency = 10000 / (_counter + 1);
			}
			else
			{
				_frequency = 10000;
			}
			_counter = 0;
			OnFront.Call();
		}
		else
		{
			if (_counter < MaxCount)
			{
				_counter++;
			}
			else
			{
				_frequency = 0xFFFF;
			}
		}
		
		_state = newState;
	}
	
	static unsigned int Get()
	{
		return _counter;
	}
	
	static bool Timeout()
	{
		return _counter >= TimeoutThreshold;
	}
	
	static float GetFrequency()
	{
		return  _frequency;
	}
};

template <class Signal>
bool SignalPulseDetector<Signal>::_state = false;

template <class Signal>
unsigned int SignalPulseDetector<Signal>::_counter = 0;

template <class Signal>
float SignalPulseDetector<Signal>::_frequency = 0;

template <class Signal>
SignalPulseDetector<Signal>::CallbackType SignalPulseDetector<Signal>::OnFront;

static const float AREF = 4.5;
typedef SignalAnalogInput<Adc, Adc::AnalogChannel, Adc::AnalogChannel0, 0x3FF> SignalPos5VC1;
typedef SignalAnalogInput<Adc, Adc::AnalogChannel, Adc::AnalogChannel1, 0x3FF> SignalPos3V3;
typedef SignalAnalogInput<Adc, Adc::AnalogChannel, Adc::AnalogChannel2, 0x3FF> SignalPos5VA1;
typedef SignalAnalogInput<Adc, Adc::AnalogChannel, Adc::AnalogChannel3, 0x3FF> SignalNeg5VA1;
typedef SignalAnalogInput<Adc, Adc::AnalogChannel, Adc::AnalogChannel4, 0x3FF> SignalPos5VA2;
typedef SignalAnalogInput<Adc, Adc::AnalogChannel, Adc::AnalogChannel5, 0x3FF> SignalNeg5VA2;
typedef SignalAnalogInput<Adc, Adc::AnalogChannel, Adc::AnalogChannel6, 0x3FF> SignalPos5VC2;
typedef SignalDiscreteInput<Lib::Gpio::B, 0, true> SignalD_SA;
typedef SignalPulseDetector<SignalD_SA> SignalSAPulseDetector;
typedef SignalDiscreteInput<Lib::Gpio::B, 1, true> SignalD_SB;
typedef SignalPulseDetector<SignalD_SB> SignalSBPulseDetector;
typedef SignalDiscreteInput<Lib::Gpio::B, 2, true> SignalD_SC;
typedef SignalPulseDetector<SignalD_SC> SignalSCPulseDetector;
typedef SignalDiscreteInput<Lib::Gpio::B, 3, true> SignalD_50HzImp;
typedef SignalPulseDetector<SignalD_50HzImp> Signal50HzPulseDetector;
typedef SignalDiscreteInput<Lib::Gpio::B, 4, true> SignalD_MainReg;
typedef SignalDiscreteInput<Lib::Gpio::D, 5, true> SignalD_Rdy;
typedef SignalDiscreteOutput<Lib::Gpio::C, 0, false> SignalDO_IPCtrl_R;
typedef SignalDiscreteOutput<Lib::Gpio::C, 1, false> SignalDO_IPCtrl_G;
typedef SignalDiscreteOutput<Lib::Gpio::C, 2, true> SignalDO_RPrt;
typedef SignalDiscreteOutput<Lib::Gpio::C, 3, true> SignalDO_YPrt;

typedef Logic<SignalPos5VC1,
		SignalPos3V3,
		SignalPos5VA1, SignalNeg5VA1,
		SignalPos5VA2, SignalNeg5VA2,
		SignalPos5VC2,
		Signal50HzPulseDetector,
		SignalD_Rdy,
		SignalDO_IPCtrl_R,
		SignalDO_IPCtrl_G,
		SignalSAPulseDetector,
		SignalSBPulseDetector,
		SignalSCPulseDetector,
		SignalDO_YPrt
		> LogicType;

LogicType logic;

class ModBusSlaveState
{
public:
	static const int BufferLen = 128;
};

typedef ModBusSlave<ModBusSlaveState>				ModBus485Slave;

const unsigned int SerialBufferSize = 128;
unsigned char serialBuffer[SerialBufferSize];
volatile unsigned char serialBufferRxPos = 0;
volatile unsigned char serialBufferTxCount = 0;
volatile unsigned int serialBufferRxTimeoutCounter = 0;
bool rxCompleteFlag = false;

void On1sec()
{
	
}

void On1ms()
{
	static unsigned int counter1Sec = 0;
	counter1Sec += 1;
	
	if (counter1Sec >= 1000)
	{
		counter1Sec = 0;
		On1sec();
	}
	
	if (counter1Sec % 100 == 0)
	{
		logic.Update();
		SignalDO_IPCtrl_R::Update();
		SignalDO_IPCtrl_G::Update();
		SignalDO_RPrt::Update();
		SignalDO_YPrt::Update();
	}
	
	if (serialBufferRxPos > 0)
	{	
		if (serialBufferRxTimeoutCounter < 5)
		{
			serialBufferRxTimeoutCounter++;
		}
		else
		{
			rxCompleteFlag = true;
		}
	}
}

void OnTimerCompareMatchA(void *callbackParam)
{
	On1ms();
}

void OnScanTimerCompareMatchA(void *callbackParam)
{
	SignalSAPulseDetector::OnTick();
	SignalSBPulseDetector::OnTick();
	SignalSCPulseDetector::OnTick();
	Signal50HzPulseDetector::OnTick();
}

void OnUsartRx(void *callbackParam)
{
	unsigned char tmp = Serial485::ReadData();
	serialBufferRxTimeoutCounter = 0;
	if (serialBufferRxPos < SerialBufferSize)
	{
		serialBuffer[serialBufferRxPos] = tmp;
		serialBufferRxPos++;
	}
}

bool OnRxComplete()
{
	if (serialBufferRxPos < 1)
	{
		return false;
	}
	
	int dataLen = serialBufferRxPos;
	serialBufferTxCount = ModBus485Slave::OnRequest(serialBuffer, dataLen);
	serialBufferRxPos = 0;
	return serialBufferTxCount > 0;
}

void TxEnable485(bool txEnable)
{
	if (txEnable)
	{
		Lib::Gpio::D::SetBit(4);
	}
	else
	{
		Lib::Gpio::D::ClearBit(4);
	}
}

class EepVariables
{
public:
#pragma pack (push, 1)
	struct DataStruct
	{
		unsigned char MainModbusAddress;
		unsigned char ReservModbusAddress;
		unsigned char Crc;
	};
#pragma pack (pop)
	static const unsigned int DataSize = sizeof(DataStruct) - sizeof(unsigned char);
protected:	
	static __eeprom DataStruct _data;
public:
	static void Check()
	{
		if (!Valid())
		{
			_data.MainModbusAddress = 1;
			_data.ReservModbusAddress = 2;
			_data.Crc = CalcCrc();
		}
	}
	
	static unsigned char GetMainModbusAddress()
	{
		return _data.MainModbusAddress;
	}
	
	// не вызывать из прерываний
	static void SetMainModbusAddress(unsigned char val)
	{
		_data.MainModbusAddress = val;
		_data.Crc = CalcCrc();
	}
	
	static unsigned char GetReservModbusAddress()
	{
		return _data.ReservModbusAddress;
	}
	
	// не вызывать из прерываний
	static void SetReservModbusAddress(unsigned char val)
	{
		_data.ReservModbusAddress = val;
		_data.Crc = CalcCrc();
	}
protected:
	static bool Valid()
	{
		return CalcCrc() == _data.Crc;
	}
	
	static unsigned char CalcCrc()
	{
		unsigned char crc = 0x5A;
		unsigned char __eeprom * ptr = (unsigned char __eeprom *)&_data;
		for (int i = 0; i < DataSize; i++)
		{
			crc += ptr[i];
		}
		
		return crc;
	}
};

__eeprom EepVariables::DataStruct EepVariables::_data;

enum MudbusReg
{
	ModbusRegMBAddrMain		= 1,
	ModbusRegMBAddrRes		= 2,
	ModbusRegCtrl			= 3,
	ModbusRegPwrMng			= 4,
	ModbusRegSyncMng		= 5,
	ModbusRegPMMng			= 6,
	ModbusRegPos5VC1		= 7,
	ModbusRegPos3V3			= 8,
	ModbusRegPos5VA1		= 9,
	ModbusRegNeg5VA1		= 10,
	ModbusRegPos5VA2		= 11,
	ModbusRegNeg5VA2		= 12,
	ModbusRegPos5VC2		= 13,
	ModbusRegPhAB			= 14,
	ModbusRegPhBC			= 15,
	ModbusRegPhCA			= 16,
	ModbusRegPMFrq			= 17
};

struct ModbusRegPwrMngStruct
{
	unsigned short Pos5VC2Error		: 1;
	unsigned short Neg5VA2Error		: 1;
	unsigned short Pos5VA2Error		: 1;
	unsigned short Neg5VA1Error		: 1;
	unsigned short Pos5VA1Error		: 1;
	unsigned short Pos3V3Error		: 1;
	unsigned short Pos5VC1Error		: 1;
	unsigned short					: 9;
};

struct ModbusRegSyncMngStruct
{
	unsigned short PhError			: 1;
	unsigned short SyncError		: 1;
	unsigned short SAError			: 1;
	unsigned short SBError			: 1;
	unsigned short SCError			: 1;
	unsigned short DRdy				: 1;
	unsigned short					: 10;
};

struct ModbusRegPmMngStruct
{
	unsigned short PmError			: 1;
	unsigned short					: 15;
};

struct ModbusRegCtrlStruct
{
	unsigned short YPrt				: 1;
	unsigned short RPrt				: 1;
	unsigned short					: 14;
};


unsigned short GetRegValue(unsigned short reg)
{
	switch((MudbusReg)reg)
	{
	case ModbusRegMBAddrMain:
		{
			return EepVariables::GetMainModbusAddress();
		}
	case ModbusRegMBAddrRes:
		{
			return EepVariables::GetReservModbusAddress();
		}
	case ModbusRegCtrl:
		{
			ModbusRegCtrlStruct reg;
			reg.YPrt = SignalDO_YPrt::Get();
			reg.RPrt = SignalDO_RPrt::Get();
			return *((unsigned short *)&reg);
		}
	case ModbusRegPwrMng:
		{
			ModbusRegPwrMngStruct reg;
			reg.Pos5VC2Error = logic.Pos5VC2Error;
			reg.Neg5VA2Error = logic.Neg5VA2Error;
			reg.Pos5VA2Error = logic.Pos5VA2Error;
			reg.Neg5VA1Error = logic.Neg5VA1Error;
			reg.Pos5VA1Error = logic.Pos5VA1Error;
			reg.Pos3V3Error = logic.Pos3V3Error;
			reg.Pos5VC1Error = logic.Pos5VC1Error;
			return *((unsigned short *)&reg);
		}
	case ModbusRegSyncMng:
		{
			ModbusRegSyncMngStruct reg;
			reg.PhError = logic.PhError;
			reg.SyncError = logic.SyncError;
			reg.SAError = logic.SAError;
			reg.SBError = logic.SBError;
			reg.SCError = logic.SCError;
			reg.DRdy = logic.DRdy;
			return *((unsigned short *)&reg);
		}
	case ModbusRegPMMng:
		{
			ModbusRegPmMngStruct reg;
			reg.PmError = logic.PmError;
			return *((unsigned short *)&reg);
		}
	case ModbusRegPos5VC1:
		{
			float val = logic.Pos5VC1;
			val *= 100;
			signed short res = (signed short)val;
			return res;
		}
	case ModbusRegPos3V3:
		{
			float val = logic.Pos3V3;
			val *= 100;
			signed short res = (signed short)val;
			return res;
		}
	case ModbusRegPos5VA1:
		{
			float val = logic.Pos5VA1;
			val *= 100;
			signed short res = (signed short)val;
			return res;
		}
	case ModbusRegNeg5VA1:
		{
			float val = logic.Neg5VA1;
			val *= 100;
			signed short res = (signed short)val;
			return res;
		}
	case ModbusRegPos5VA2:
		{
			float val = logic.Pos5VA2;
			val *= 100;
			signed short res = (signed short)val;
			return res;
		}
	case ModbusRegNeg5VA2:
		{
			float val = logic.Neg5VA2;
			val *= 100;
			signed short res = (signed short)val;
			return res;
		}
	case ModbusRegPos5VC2:
		{
			float val = logic.Pos5VC2;
			val *= 100;
			signed short res = (signed short)val;
			return res;
		}
	case ModbusRegPhAB:
		{
			return logic.DegPhAB;
		}
	case ModbusRegPhBC:
		{
			return logic.DegPhBC;
		}
	case ModbusRegPhCA:
		{
			return logic.DegPhCA;
		}
	case ModbusRegPMFrq:
		{
			return (unsigned short)logic.PMFreq;
		}
	}
	
	return 0xFFFF;
}

bool OnGetRegValues(unsigned char *buffer, unsigned int bufferLen, unsigned short firstAddr, unsigned short quantity)
{
	for (unsigned short i = 0; i < quantity; i++)
	{
		unsigned short regVal = GetRegValue(firstAddr + i);
		buffer[i * 2] = (regVal >> 8) & 0xFF;
		buffer[i * 2 + 1] = regVal & 0xFF;
	}
	
	return true;
}

bool OnSetRegValue(unsigned short regAddr, unsigned short value)
{
	switch((MudbusReg)regAddr)
	{
	case ModbusRegMBAddrMain:
		{
			if (value >= 1 && value <= 31)
			{
				EepVariables::SetMainModbusAddress(value);
				return true;
			}
			
			return false;
		}
	case ModbusRegMBAddrRes:
		{
			if (value >= 1 && value <= 31)
			{
				EepVariables::SetReservModbusAddress(value);
				return true;
			}
			
			return false;
		}
	case ModbusRegCtrl:
		{
			ModbusRegCtrlStruct *reg = (ModbusRegCtrlStruct *)&value;
			SignalDO_YPrt::Set(reg->YPrt);
			SignalDO_RPrt::Set(reg->RPrt);
			return true;
		}
	}
	
	return false;
}
/*
void OnPCINT1(void *callbackParam)
{
	if (SignalD_SA::Get() == 0)
	{
		SignalSAPulseDetector::OnInterrupt();
	}
	if (SignalD_SB::Get() == 0)
	{
		SignalSBPulseDetector::OnInterrupt();
	}
	
	if (SignalD_SC::Get() == 0)
	{
		SignalSCPulseDetector::OnInterrupt();
	}
	
	if (SignalD_50HzImp::Get() == 0)
	{
		Signal50HzPulseDetector::OnInterrupt();
	}
}*/

int main()
{
	// 485En
	Lib::Gpio::D::SetOutputPin(4);
	TxEnable485(false);
	
	Lib::Gpio::C::SetOutputPin(5);
	Lib::Gpio::C::ClearBit(5);
	
	Timer::SetWaveformGenerationMode(Timer::WaveformGenerationModeClearTimerOnCompareMatch);
	// ExtClockSourceFreq / 256 / 1000 = 62.5 это типа 1000 раз в секунду
	Timer::SetOutputCompareAValue(62);
	Timer::SetInterruptEnable(Timer::InterruptOutputCompareMatchA);
	Timer::Start(Timer::ClockSourceFromPrescalerDiv256);
	
	ScanTimer::SetWaveformGenerationMode(ScanTimer::WaveformGenerationModeClearTimerOnCompareMatch);
	// ExtClockSourceFreq / 8 / 10000 = 200 это типа 10000 раз в секунду
	ScanTimer::SetOutputCompareAValue(200);
	ScanTimer::SetInterruptEnable(ScanTimer::InterruptOutputCompareMatchA);
	ScanTimer::Start(ScanTimer::ClockSourceFromPrescalerDiv8);
	
	// Входы АЦП
	Lib::Gpio::A::SetInputPin(0);
	Lib::Gpio::A::SetInputPin(1);
	Lib::Gpio::A::SetInputPin(2);
	Lib::Gpio::A::SetInputPin(3);
	Lib::Gpio::A::SetInputPin(4);
	Lib::Gpio::A::SetInputPin(5);
	Lib::Gpio::A::SetInputPin(6);
	
	Adc::Enable(Adc::Divider128, Adc::ReferenceAREF);
	Adc::DisableInput(0);
	
	Lib::InterruptMap::HandlerTIMER0_COMPA.Set(OnTimerCompareMatchA, 0);
	Lib::InterruptMap::HandlerTIMER2_COMPA.Set(OnScanTimerCompareMatchA, 0);
	Lib::InterruptMap::HandlerUSART1_RX.Set(OnUsartRx, 0);
	/*
	Lib::InterruptMap::HandlerPCINT1.Set(OnPCINT1, 0);
	Lib::Gpio::B::SetInputPin(0);
	Lib::Gpio::B::SetInputPin(1);
	Lib::Gpio::B::SetInputPin(2);
	Lib::Gpio::B::SetInputPin(3);
	Lib::ExternalInterrupts::PinChangeMask1SetBit(0);
	Lib::ExternalInterrupts::PinChangeMask1SetBit(1);
	Lib::ExternalInterrupts::PinChangeMask1SetBit(2);
	Lib::ExternalInterrupts::PinChangeMask1SetBit(3);
	Lib::ExternalInterrupts::InterruptEnable(Lib::ExternalInterrupts::Interrupt1);*/
	
	Serial485::StartAsyncMode(ExtClockSourceFreq, 9600, Serial485::StopBits1, Serial485::ParityDisabled, true, true);
	TestSerialBus::StartAsyncMode(ExtClockSourceFreq, 115200, TestSerialBus::StopBits1, TestSerialBus::ParityDisabled, false, true);
	
	SignalPos5VC1::Init(AREF);
	SignalPos3V3::Init(AREF);
	SignalPos5VA1::Init(AREF);
	SignalNeg5VA1::Init(AREF);
	SignalPos5VA2::Init(AREF);
	SignalNeg5VA2::Init(AREF);
	SignalPos5VC2::Init(AREF);
	SignalD_SA::Init();
	SignalD_SB::Init();
	SignalD_SC::Init();
	SignalD_50HzImp::Init();
	SignalD_MainReg::Init();
	SignalD_Rdy::Init();
	SignalDO_IPCtrl_R::Init();
	SignalDO_IPCtrl_G::Init();
	SignalDO_RPrt::Init();
	SignalDO_YPrt::Init();
	
	SignalSAPulseDetector::Init();
	SignalSBPulseDetector::Init();
	SignalSCPulseDetector::Init();
	Signal50HzPulseDetector::Init();
	
	logic.Init();
	
	EepVariables::Check();
	
	ModBus485Slave::GetRegValuesCallback = OnGetRegValues;
	ModBus485Slave::SetRegValueCallback = OnSetRegValue;

	Lib::InterruptMap::GlobalInterruptEnable();
	
	char test[] = {0, 1, 2, 3, 4};//"SelfTest board init complete\r\n";
	TestSerialBus::Write(test, sizeof(test));
	
	TxEnable485(true);
	Serial485::Write(test, sizeof(test));
	TxEnable485(false);
	
	while (1)
	{
		SignalPos5VC1::Update();
		SignalPos3V3::Update();
		SignalPos5VA1::Update();
		SignalNeg5VA1::Update();
		SignalPos5VA2::Update();
		SignalNeg5VA2::Update();
		SignalPos5VC2::Update();
		
		if (SignalD_MainReg::Get())
		{
			ModBus485Slave::MyAddress = GetRegValue(ModbusRegMBAddrMain);
		}
		else
		{
			ModBus485Slave::MyAddress = GetRegValue(ModbusRegMBAddrRes);
		}
		
		if (rxCompleteFlag)
		{
			rxCompleteFlag = false;
			
			if (OnRxComplete())
			{
				TxEnable485(true);
				Serial485::Write(serialBuffer, serialBufferTxCount);
				TxEnable485(false);
			}
		}
	
		
		//ModBusSlaveState::Run();
	}
}
