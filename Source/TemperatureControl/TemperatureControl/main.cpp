
#include "../../RbLib/Rblib.h"
#include "Button.h"
#include "MainStateMachine.h"
#include "DirectStateMachine.h"
#include "LearnigStateMachine.h"

typedef Rblib::ResetAndClockControl Rcc;
typedef Rblib::Gpio Gpio;
typedef Rblib::Timer Timer;
typedef Rblib::Nvic Nvic;
typedef Rblib::InterruptMap InterruptMap;
typedef Rblib::Usart Usart;
typedef Rblib::FlashMemoryController FlashMemoryController;

static const unsigned int DirectOneWireTickFrequency = 100000;
class DirectOneWireInterface
{
public:
	static void PullDown()
	{
		Gpio::B::SetMode(0, Gpio::B::ModeOutput);
		Gpio::B::SetConfig(0, Gpio::B::ConfigOutputPushPull);
		Gpio::B::ClearBit(0);
	}
	
	static void Release()
	{
		Gpio::B::SetMode(0, Gpio::B::ModeInput);
		Gpio::B::SetConfig(0, Gpio::B::ConfigInputFloating);
	}
	
	static bool Read()
	{
		return Gpio::B::Read(0);
	}
};

typedef Rblib::Driver::OneWireProtocol<DirectOneWireInterface> OneWireProtocolType;
OneWireProtocolType OneWireProtocol;

static void OnTimer1Update()
{
	OneWireProtocol.Tick(DirectOneWireTickFrequency);
}

typedef Rblib::Driver::OneWire<OneWireProtocolType> DirectOneWireType;
DirectOneWireType DirectOneWire;

Button button1;

void GetButton1State(bool &pressed)
{
	pressed = Gpio::B::Read(8) == 0;
}

Button button2;

void GetButton2State(bool &pressed)
{
	pressed = Gpio::B::Read(9) == 0;
}

Button button3;

void GetButton3State(bool &pressed)
{
	pressed = Gpio::B::Read(10) == 0;
}

Button button4;

void GetButton4State(bool &pressed)
{
	pressed = Gpio::B::Read(11) == 0;
}

MainStateMachine mainStateMachine;

void OnButtonClick(void *callbackParam)
{
	mainStateMachine.OnButtonClick((int)callbackParam);
}

void OnButtonLongClick(void *callbackParam)
{
	mainStateMachine.OnButtonLongClick((int)callbackParam);
}

void LedOnOff(bool on, int channel)
{
	if (on)
	{
		switch(channel)
		{
		case 1:
			{
				Gpio::B::ClearBit(12);
				Gpio::B::SetBit(13);
				Gpio::B::SetBit(14);
				Gpio::B::SetBit(15);
			}
			break;
		case 2:
			{
				Gpio::B::SetBit(12);
				Gpio::B::ClearBit(13);
				Gpio::B::SetBit(14);
				Gpio::B::SetBit(15);
			}
			break;
		case 3:
			{
				Gpio::B::SetBit(12);
				Gpio::B::SetBit(13);
				Gpio::B::ClearBit(14);
				Gpio::B::SetBit(15);
			}
			break;
		case 4:
			{
				Gpio::B::SetBit(12);
				Gpio::B::SetBit(13);
				Gpio::B::SetBit(14);
				Gpio::B::ClearBit(15);
			}
			break;
		default:
			{
				Gpio::B::SetBit(12);
				Gpio::B::SetBit(13);
				Gpio::B::SetBit(14);
				Gpio::B::SetBit(15);
			}
			break;
		}
	}
	else
	{
		Gpio::B::SetBit(12);
		Gpio::B::SetBit(13);
		Gpio::B::SetBit(14);
		Gpio::B::SetBit(15);
	}
}

void BeepOnOff(bool on)
{
	if (on)
	{
		Gpio::B::SetMode(1, Gpio::B::ModeOutput);
	}
	else
	{
		Gpio::B::SetMode(1, Gpio::B::ModeInput);
	}
	Timer::Timer3::CC4Enable(on);
}

template
	<
		class Usart
	>
class Ds2480bInterfaceImpl
{
public:
	typedef Rblib::CallbackWrapper<unsigned char> OnReadCallbackType;
	typedef Rblib::CallbackWrapper<> OnWriteCompleteCallbackType;
public:
	static OnReadCallbackType OnReadCallback;
	static OnWriteCompleteCallbackType OnWriteCompleteCallback;
public:
	static void Init()
	{
		Usart::Init(48000000, 9600, Usart::StopBits1, Usart::ParityNone);
		Usart::OnRead = OnUsartRead;
		Usart::OnWriteComplete = OnWriteComplete;
		Usart::Enable();
	}
	
	static void Write(void *data, unsigned int len)
	{
		Usart::Write((unsigned char *)data, len);
	}
	
	static void OnUsartRead(unsigned int data)
	{
		OnReadCallback(data);
	}
	
	static void OnWriteComplete()
	{
		OnWriteCompleteCallback();
	}
	
	static void SendBreak()
	{
		Usart::SendBreak();
	}
};

template <class Usart>
Ds2480bInterfaceImpl<Usart>::OnReadCallbackType Ds2480bInterfaceImpl<Usart>::OnReadCallback;

template <class Usart>
Ds2480bInterfaceImpl<Usart>::OnWriteCompleteCallbackType Ds2480bInterfaceImpl<Usart>::OnWriteCompleteCallback;

typedef Ds2480bInterfaceImpl<Usart::Usart2> Ds2480bInterface;

typedef Rblib::Driver::Ds2480b<Ds2480bInterface> OneWireType;
OneWireType OneWire;

template
	<
		class Usart
	>
class Rs485InterfaceImpl
{
public:
	typedef Rblib::CallbackWrapper<unsigned char> OnReadCallbackType;
	typedef Rblib::CallbackWrapper<> OnWriteCompleteCallbackType;
public:
	static OnReadCallbackType OnReadCallback;
	static OnWriteCompleteCallbackType OnWriteCompleteCallback;
public:
	static void Init()
	{
		Usart::Init(48000000, 9600, Usart::StopBits1, Usart::ParityNone);
		Usart::OnRead = OnUsartRead;
		Usart::OnWriteComplete = OnWriteComplete;
		Usart::Enable();
	}
	
	static void Write(void *data, unsigned int len)
	{
		Usart::Write((unsigned char *)data, len);
	}
	
	static void OnUsartRead(unsigned int data)
	{
		OnReadCallback(data);
	}
	
	static void OnWriteComplete()
	{
		OnWriteCompleteCallback();
	}
	
	static void WriteEnable(bool enable)
	{
		if(enable)
		{
			Gpio::A::SetBit(8);
		}
		else
		{
			Gpio::A::ClearBit(8);
		}
	}
};

template <class Usart>
Rs485InterfaceImpl<Usart>::OnReadCallbackType Rs485InterfaceImpl<Usart>::OnReadCallback;

template <class Usart>
Rs485InterfaceImpl<Usart>::OnWriteCompleteCallbackType Rs485InterfaceImpl<Usart>::OnWriteCompleteCallback;

typedef Rs485InterfaceImpl<Usart::Usart1> Rs485Interface;

typedef Rblib::Driver::Rs485<Rs485Interface> Rs485Type;
Rs485Type Rs485;

DirectStateMachine directStateMachine;

static const unsigned int OneWireTickFrequency = 5000;
void OnTimer2Update()
{
	button1.Tick(OneWireTickFrequency);
	button2.Tick(OneWireTickFrequency);
	button3.Tick(OneWireTickFrequency);
	button4.Tick(OneWireTickFrequency);
	Rs485.Tick(OneWireTickFrequency);
	DirectOneWire.Tick(OneWireTickFrequency);
	OneWire.Tick(OneWireTickFrequency);
	mainStateMachine.Tick(OneWireTickFrequency);
	directStateMachine.Tick(OneWireTickFrequency);
}

void SelectChannel(int channel)
{
	switch(channel)
	{
	case 1:
		Gpio::A::ClearBit(4);
		Gpio::A::ClearBit(5);
		break;
	case 2:
		Gpio::A::ClearBit(4);
		Gpio::A::SetBit(5);
		break;
	case 3:
		Gpio::A::SetBit(4);
		Gpio::A::ClearBit(5);
		break;
	case 4:
		Gpio::A::SetBit(4);
		Gpio::A::SetBit(5);
		break;
	}
}

void SearchRom()
{
	OneWire.SearchRom();
}

void OnDeviceFound(unsigned char *deviceId)
{
	mainStateMachine.OnDeviceFound(deviceId);
}

void MeasureTemperature()
{
	OneWire.SkipRom(0x44, 0, 0);
}

void OnSkipRomResultReaded(unsigned char *data, int count)
{
}

void ReadTemperature(unsigned char *deviceId)
{
	unsigned char dummyData[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	OneWire.MatchRom(deviceId, 0xBE, dummyData, sizeof(dummyData));
}

void OnMatchRomResultReaded(unsigned char *data, int count)
{
	unsigned char crcExpeted = CalcCrc(data, count - 1);
	unsigned char crcActual = data[count - 1];
	
	signed short t = 0;
	for (int i = 0; i < 2; i++)
	{
		((unsigned char *)&t)[i] = data[i];
	}
	t /= 2;
	mainStateMachine.OnTemperatureReaded(t, crcExpeted == crcActual);
}

static const char StorageMemeoryId[] = "TemperatureControlStorage";

struct Storage
{
	static const int MaxDevCnt = 6;
	static const int ChannelCnt = 4;
	static const unsigned int StoragePtr = 0x08007C00;
	
	struct Channel
	{
		unsigned char DeviceCount;
		unsigned char Devices[MaxDevCnt][8];
	};
public:
	char MemoryId[sizeof(StorageMemeoryId)];
	unsigned char ModbusAddress;
	int UHeat;
	int UCool;
	Channel Channels[ChannelCnt];
	unsigned short SelfTempSource;
public:
	void Reset()
	{
		ModbusAddress = 3;
		UHeat = 10;
		UCool = 30;
		SelfTempSource = 0;
		
		for (int i = 0; i < sizeof(StorageMemeoryId); i++)
		{
			MemoryId[i] = StorageMemeoryId[i];
		}
		
		for (int i = 0; i < ChannelCnt; i++)
		{
			Channels[i].DeviceCount = 0;
			for (int j = 0; j < MaxDevCnt; j++)
			{
				for (int k = 0; k < 8; k++)
				{
					Channels[i].Devices[j][k] = 0xFF;
				}
			}
		}
	}
	
	bool Valid()
	{
		bool res = true;
		for (int i = 0; i < sizeof(StorageMemeoryId); i++)
		{
			res &= MemoryId[i] == StorageMemeoryId[i];
		}
		
		return res;
	}
	
	void Load()
	{
		unsigned short *dst = (unsigned short *)this;
		unsigned short *src = (unsigned short *)StoragePtr;
		unsigned int count = sizeof(Storage);
		if (count % 2 > 0)
		{
			count++;
		}
		count /= 2;
		
		for (int i = 0; i < count; i++)
		{
			dst[i] = src[i];
		}
		
		if (!Valid())
		{
			Reset();
			Save();
		}
	}
	
	void Save()
	{
		unsigned short *dst = (unsigned short *)StoragePtr;
		unsigned short *src = (unsigned short *)this;
		unsigned int count = sizeof(Storage);
		if (count % 2 > 0)
		{
			count++;
		}
		count /= 2;
		
		FlashMemoryController::Erase(StoragePtr);
		FlashMemoryController::EnableProgramming(true);
		for (int i = 0; i < count; i++)
		{
			while(FlashMemoryController::Busy())
			{
				;
			}
			dst[i] = src[i];
		}
		FlashMemoryController::EnableProgramming(false);
	}
	
	unsigned char GetModbusAddress()
	{
		return ModbusAddress;
	}
	
	void SetModbusAddress(unsigned char value)
	{
		ModbusAddress = value;
		Save();
	}
	
	int GetUHeat()
	{
		return UHeat;
	}
	
	void SetUHeat(int value)
	{
		UHeat = value;
		Save();
	}
	
	int GetUCool()
	{
		return UCool;
	}
	
	void SetUCool(int value)
	{
		UCool = value;
		Save();
	}
	
	void ClearDevList(unsigned int channel)
	{
		if (channel > ChannelCnt)
		{
			return;
		}
		
		Channels[channel - 1].DeviceCount = 0;
		Save();
	}
	
	void StoreDevId(unsigned int channel, unsigned int pos, unsigned int count, unsigned char *devId)
	{
		if (channel > ChannelCnt || pos > MaxDevCnt)
		{
			return;
		}
		
		for (int i = 0; i < 8; i++)
		{
			Channels[channel - 1].Devices[pos - 1][i] = devId[i];
		}
		
		Channels[channel - 1].DeviceCount = count;
		
		Save();
	}
	
	void GetDevId(unsigned int channel, unsigned int pos, unsigned char *devId)
	{
		if (channel > ChannelCnt || pos > GetDevCount(channel))
		{
			return;
		}
		
		for (int i = 0; i < 8; i++)
		{
			devId[i] = Channels[channel - 1].Devices[pos - 1][i];
		}
	}
	
	int GetDevCount(unsigned int channel)
	{
		if (channel < 1 || channel > ChannelCnt)
		{
			return 0;
		}
		
		return Channels[channel - 1].DeviceCount;
	}
	
	unsigned short GetSelfTempSource()
	{
		return SelfTempSource;
	}
	
	void SetSelfTempSource(unsigned short val)
	{
		SelfTempSource = val;
		
		Save();
	}
};

Storage storage;

void StorageClearDevList(unsigned int channel)
{
	storage.ClearDevList(channel);
}

void StoreDevId(unsigned int channel, unsigned int pos, unsigned int count, unsigned char *devId)
{
	storage.StoreDevId(channel, pos, count, devId);
}

void GetDevCount(unsigned int channel, unsigned int &count)
{
	count = storage.GetDevCount(channel);
}

void GetDevId(unsigned int channel, unsigned int pos, unsigned char *devId)
{
	storage.GetDevId(channel, pos, devId);
}

int temperature[Storage::ChannelCnt][Storage::MaxDevCnt];
int internalTemperature = 0x7FFF;

void StoreTempt(unsigned int channel, unsigned int pos, int t)
{
	if (channel > Storage::ChannelCnt || pos > Storage::MaxDevCnt)
	{
		return;
	}
	
	temperature[channel - 1][pos - 1] = t;
}

int GetInternalTemperature()
{
	return internalTemperature;
}

int GetTemp(unsigned int channel, unsigned int pos)
{
	if (channel > Storage::ChannelCnt || pos > storage.GetDevCount(channel))
	{
		return 0x7FFF;
	}
	
	return temperature[channel - 1][pos - 1];
}

Rblib::Driver::ModbusSlave ModbusSlave;

unsigned char inputBufferRs485[256];
int _rxRs485Count = 0;
unsigned char outputBufferRs485[256];
void OnRs485DataReceived(unsigned char *data, int count)
{
	if (_rxRs485Count > 0)
	{
		return;
	}
	
	for (int i = 0; i < count; i++)
	{
		inputBufferRs485[i] = data[i];
	}
	
	_rxRs485Count = count;
}

void GetModbusAddress(unsigned char &address)
{
	address = storage.GetModbusAddress();
}

enum RegisterMap
{
	RegMbAddress		= 1,
	RegUHeat			= 2,
	RegUCool			= 3,
	RegSelfTemp			= 4,
	RegSensCnt1			= 5,
	RegSensCnt2			= 6,
	RegSensCnt3			= 7,
	RegSensCnt4			= 8,
	RegTempSens1Start	= 9,
	RegTempSens1End		= 14,
	RegTempSens2Start	= 15,
	RegTempSens2End		= 20,
	RegTempSens3Start	= 21,
	RegTempSens3End		= 26,
	RegTempSens4Start	= 27,
	RegTempSens4End		= 32,
	RegSelfTempSource	= 33
};

void GetRegisterValue(unsigned short reg, unsigned short &val)
{
	switch(reg)
	{
	case RegMbAddress:
		val = storage.GetModbusAddress();
		break;
	case RegUHeat:
		{
			signed short t = storage.GetUHeat();
			unsigned char *src = (unsigned char *)&t;
			unsigned char *dst = (unsigned char *)&val;
			for (int i = 0; i < sizeof(val); i++)
			{
				dst[i] = src[i];
			}
		}
		break;
	case RegUCool:
		{
			signed short t = storage.GetUCool();
			unsigned char *src = (unsigned char *)&t;
			unsigned char *dst = (unsigned char *)&val;
			for (int i = 0; i < sizeof(val); i++)
			{
				dst[i] = src[i];
			}
		}
		break;
	case RegSelfTemp:
		{
			unsigned char *src = (unsigned char *)&internalTemperature;
			unsigned char *dst = (unsigned char *)&val;
			for (int i = 0; i < sizeof(val); i++)
			{
				dst[i] = src[i];
			}
		}
		break;
	case RegSensCnt1:
		val = storage.GetDevCount(1);
		break;
	case RegSensCnt2:
		val = storage.GetDevCount(2);
		break;
	case RegSensCnt3:
		val = storage.GetDevCount(3);
		break;
	case RegSensCnt4:
		val = storage.GetDevCount(4);
		break;
	case RegSelfTempSource:
		val = storage.GetSelfTempSource();
		break;
	default:
		{
			if (reg >= RegTempSens1Start && reg <= RegTempSens1End)
			{
				int pos = reg - RegTempSens1Start;
				signed short t = GetTemp(1, pos + 1);
				unsigned char *src = (unsigned char *)&t;
				unsigned char *dst = (unsigned char *)&val;
				for (int i = 0; i < sizeof(val); i++)
				{
					dst[i] = src[i];
				}
			}
			
			if (reg >= RegTempSens2Start && reg <= RegTempSens2End)
			{
				int pos = reg - RegTempSens2Start;
				signed short t = GetTemp(2, pos + 1);
				unsigned char *src = (unsigned char *)&t;
				unsigned char *dst = (unsigned char *)&val;
				for (int i = 0; i < sizeof(val); i++)
				{
					dst[i] = src[i];
				}
			}
			
			if (reg >= RegTempSens3Start && reg <= RegTempSens3End)
			{
				int pos = reg - RegTempSens3Start;
				signed short t = GetTemp(3, pos + 1);
				unsigned char *src = (unsigned char *)&t;
				unsigned char *dst = (unsigned char *)&val;
				for (int i = 0; i < sizeof(val); i++)
				{
					dst[i] = src[i];
				}
			}
			
			if (reg >= RegTempSens4Start && reg <= RegTempSens4End)
			{
				int pos = reg - RegTempSens4Start;
				signed short t = GetTemp(4, pos + 1);
				unsigned char *src = (unsigned char *)&t;
				unsigned char *dst = (unsigned char *)&val;
				for (int i = 0; i < sizeof(val); i++)
				{
					dst[i] = src[i];
				}
			}
		}
	}
}

void SetRegisterValue(unsigned short reg, unsigned short val, bool &result)
{
	switch(reg)
	{
	case RegMbAddress:
		if (val >= 1 && val <= 31)
		{
			storage.SetModbusAddress(val);
			result = true;
		}
		break;
	case RegUHeat:
		{
			signed short t = 0;
			unsigned char *src = (unsigned char *)&val;
			unsigned char *dst = (unsigned char *)&t;
			for (int i = 0; i < sizeof(val); i++)
			{
				dst[i] = src[i];
			}
			storage.SetUHeat(t);
			result = true;
		}
		break;
	case RegUCool:
		{
			signed short t = 0;
			unsigned char *src = (unsigned char *)&val;
			unsigned char *dst = (unsigned char *)&t;
			for (int i = 0; i < sizeof(val); i++)
			{
				dst[i] = src[i];
			}
			storage.SetUCool(t);
			result = true;
		}
		break;
	case RegSelfTempSource:
		{
			storage.SetSelfTempSource(val);
			result = true;
		}
		break;
	}
}

void DirectMeasureTemperature()
{
	DirectOneWire.SkipRom(0x44, 0);
}

void DirectReadTemperature()
{
	DirectOneWire.SkipRom(0xBE, 9);
}

void OnDirectOneWireBusReadResult(unsigned char *data, int count)
{
	if (count < 1)
	{
		return;
	}
	
	unsigned char crcExpeted = CalcCrc(data, count - 1);
	unsigned char crcActual = data[count - 1];
	
	signed short t = 0;
	for (int i = 0; i < 2; i++)
	{
		((unsigned char *)&t)[i] = data[i];
	}
	t /= 2;
	directStateMachine.OnTemperatureReaded(t, crcExpeted == crcActual);
}

void DirectStoreTemp(int t)
{
	internalTemperature = t;
}

void GetHeatThreshold(int &t)
{
	t = storage.GetUHeat();
}

void GetCoolThreshold(int &t)
{
	t = storage.GetUCool();
}

void GetTermostatTemp(int &t)
{
	int source = storage.GetSelfTempSource();
	t = GetInternalTemperature();
	
	if (source > 0)
	{
		source--; // чтобы правильно делилось и вычислялись остатки от деления
		int channel = source / Storage::MaxDevCnt;
		channel++; // каналы нумеруются с единицы
		int device = source % Storage::MaxDevCnt;
		device++; // датчики нумеруются с единицы
		
		if (channel < Storage::ChannelCnt)
		{
			if (device <= storage.GetDevCount(channel))
			{
				t = GetTemp(channel, device);
			}
		}
	}
}

void HeatOnOff(bool on)
{
	if (on)
	{
		Gpio::A::SetBit(11);
	}
	else
	{
		Gpio::A::ClearBit(11);
	}
}

void CoolOnOff(bool on)
{
	if (on)
	{
		Gpio::A::SetBit(12);
	}
	else
	{
		Gpio::A::ClearBit(12);
	}
}

int main()
{
	// HSE - внешнее тактирование 8 MHz
	Rcc::HSEEnable();
	while(!Rcc::HSIReady())
	{
		;
	}
	
	// PLL от HSE
	Rcc::SetPLLSource(Rcc::PLLSourceHSE);
	// HSE без деления
	Rcc::SetPLLHSEDivider(Rcc::HSEDiv1);
	// PLL = 48 MHz = 8 * 6
	Rcc::SetPLLMultiplicationFactor(Rcc::PLLMul6);
	// Включаем
	Rcc::PLLEnable();
	while(!Rcc::PLLReady())
	{
		;
	}
	// Ядро от PLL
	Rcc::SetSystemClock(Rcc::SystemClockPLL);
	
	// Вывод тактирования для проверки (ПЕРЕСЕКАЕТСЯ С RS485 RE/DE)
	Rcc::SetClockOutput(Rcc::ClockOutputPllDiv2);
	
	FlashMemoryController::Unlock();
	
	Rcc::EnableClockPortA();
	Rcc::EnableClockPortB();
	Rcc::EnableClockTimer1();
	Rcc::EnableClockTimer2();
	Rcc::EnableClockTimer3();
	Rcc::EnableClockUsart1();
	Rcc::EnableClockUsart2();
	
	//Вывод тактирования
	//Gpio::A::SetMode(8, Gpio::A::ModeOutput);
	//Gpio::A::SetConfig(8, Gpio::A::ConfigOutputAlternatePushPull);
	
	// кнопки
	Gpio::B::SetMode(8, Gpio::B::ModeInput);
	Gpio::B::SetConfig(8, Gpio::B::ConfigInputFloating);
	
	Gpio::B::SetMode(9, Gpio::B::ModeInput);
	Gpio::B::SetConfig(9, Gpio::B::ConfigInputFloating);
	
	Gpio::B::SetMode(10, Gpio::B::ModeInput);
	Gpio::B::SetConfig(10, Gpio::B::ConfigInputFloating);
	
	Gpio::B::SetMode(11, Gpio::B::ModeInput);
	Gpio::B::SetConfig(11, Gpio::B::ConfigInputFloating);
	
	// светодиоды
	Gpio::B::SetMode(12, Gpio::B::ModeOutput);
	Gpio::B::SetConfig(12, Gpio::B::ConfigOutputOpenDrain);
	Gpio::B::SetBit(12);
	
	Gpio::B::SetMode(13, Gpio::B::ModeOutput);
	Gpio::B::SetConfig(13, Gpio::B::ConfigOutputOpenDrain);
	Gpio::B::SetBit(13);
	
	Gpio::B::SetMode(14, Gpio::B::ModeOutput);
	Gpio::B::SetConfig(14, Gpio::B::ConfigOutputPushPull);
	Gpio::B::SetBit(14);
	
	Gpio::B::SetMode(15, Gpio::B::ModeOutput);
	Gpio::B::SetConfig(15, Gpio::B::ConfigOutputPushPull);
	Gpio::B::SetBit(15);
	
	//HEATER_ON
	Gpio::A::SetMode(11, Gpio::A::ModeOutput);
	Gpio::A::SetConfig(11, Gpio::A::ConfigOutputPushPull);
	Gpio::A::ClearBit(11);
	
	//COOLER_ON
	Gpio::A::SetMode(12, Gpio::A::ModeOutput);
	Gpio::A::SetConfig(12, Gpio::A::ConfigOutputPushPull);
	Gpio::A::ClearBit(12);
	
	// Внутренняя шина 1-Wire
	Gpio::B::SetMode(0, Gpio::B::ModeInput);
	Gpio::B::SetConfig(0, Gpio::B::ConfigInputFloating);
	Gpio::B::ClearBit(0);
	
	Timer::Timer1::SetPrescaler(48);
	Timer::Timer1::SetMaxCount(1000000 / DirectOneWireTickFrequency);
	Timer::Timer1::UpdateInterruptHandler = OnTimer1Update;
	Timer::Timer1::Start();
	Timer::Timer1::ClearUpdateInterruptFlag();
	Timer::Timer1::EnableUpdateInterrupt();
	Nvic::InterruptEnable(Nvic::InterruptVector_TIM1_UP);
	
	// пищалка
	Gpio::B::SetMode(1, Gpio::B::ModeOutput);
	Gpio::B::SetConfig(1, Gpio::B::ConfigOutputAlternatePushPull);
	
	// генератор для пищалки 3.2 КГц
	Timer::Timer3::SetPrescaler(48);
	Timer::Timer3::SetMaxCount(156);
	Timer::Timer3::CC4SetMode(3);
	Timer::Timer3::Start();
	Timer::Timer3::CC4Enable(false);
	
	// генератор для 1-Wire драйвера
	Timer::Timer2::SetPrescaler(48);
	Timer::Timer2::SetMaxCount(1000000 / OneWireTickFrequency);
	Timer::Timer2::UpdateInterruptHandler = OnTimer2Update;
	Timer::Timer2::Start();
	Timer::Timer2::ClearUpdateInterruptFlag();
	Timer::Timer2::EnableUpdateInterrupt();
	Nvic::InterruptEnable(Nvic::InterruptVector_TIM2);
	
	// 485
	// RE/DE
	Gpio::A::SetMode(8, Gpio::A::ModeOutput);
	Gpio::A::SetConfig(8, Gpio::A::ConfigOutputPushPull);
	Gpio::A::ClearBit(8);
	
	// USART1
	Gpio::A::SetMode(9, Gpio::A::ModeOutput);
	Gpio::A::SetConfig(9, Gpio::A::ConfigOutputAlternatePushPull);
	
	Gpio::A::SetMode(10, Gpio::A::ModeInput);
	Gpio::A::SetConfig(10, Gpio::A::ConfigInputFloating);
	
	Nvic::InterruptEnable(Nvic::InterruptVector_USART1);
	
	Rs485Interface::Init();
	Rs485.Init(0.0025);
	Rs485.OnReadComplete = OnRs485DataReceived;
	
	ModbusSlave.GetSelfAddress = GetModbusAddress;
	ModbusSlave.GetRegisterValue = GetRegisterValue;
	ModbusSlave.SetRegisterValue = SetRegisterValue;
	ModbusSlave.Init();
	
	// USART2
	Gpio::A::SetMode(2, Gpio::A::ModeOutput);
	Gpio::A::SetConfig(2, Gpio::A::ConfigOutputAlternatePushPull);
	
	Gpio::A::SetMode(3, Gpio::A::ModeInput);
	Gpio::A::SetConfig(3, Gpio::A::ConfigInputFloating);
	
	Nvic::InterruptEnable(Nvic::InterruptVector_USART2);
	
	Ds2480bInterface::Init();
	OneWire.Init();
	OneWire.OnDeviceFound = OnDeviceFound;
	OneWire.OnSkipRomResultReaded = OnSkipRomResultReaded;
	OneWire.OnMatchRomResultReaded = OnMatchRomResultReaded;
	
	// 1-WIRE COMM1
	Gpio::A::SetMode(4, Gpio::A::ModeOutput);
	Gpio::A::SetConfig(4, Gpio::A::ConfigOutputPushPull);
	Gpio::A::ClearBit(4);
	
	// 1-WIRE COMM2
	Gpio::A::SetMode(5, Gpio::A::ModeOutput);
	Gpio::A::SetConfig(5, Gpio::A::ConfigOutputPushPull);
	Gpio::A::ClearBit(5);
	
	button1.GetState = GetButton1State;
	button1.OnClick.Set(OnButtonClick, (void *)1);
	button1.OnLongClick.Set(OnButtonLongClick, (void *)1);
	button2.GetState = GetButton2State;
	button2.OnClick.Set(OnButtonClick, (void *)2);
	button2.OnLongClick.Set(OnButtonLongClick, (void *)2);
	button3.GetState = GetButton3State;
	button3.OnClick.Set(OnButtonClick, (void *)3);
	button3.OnLongClick.Set(OnButtonLongClick, (void *)3);
	button4.GetState = GetButton4State;
	button4.OnClick.Set(OnButtonClick, (void *)4);
	button4.OnLongClick.Set(OnButtonLongClick, (void *)4);
	
	InterruptMap::GlobalInterruptEnable();
	
	storage.Load();
	
	mainStateMachine.Start();
	mainStateMachine.LedOnOff = LedOnOff;
	mainStateMachine.BeepOnOff = BeepOnOff;
	mainStateMachine.SelectChannel = SelectChannel;
	mainStateMachine.SearchRom = SearchRom;
	mainStateMachine.MeasureTemperature = MeasureTemperature;
	mainStateMachine.ReadTemperature = ReadTemperature;
	mainStateMachine.ClearDevList = StorageClearDevList;
	mainStateMachine.StoreDevId = StoreDevId;
	mainStateMachine.GetDevCount = GetDevCount;
	mainStateMachine.GetDevId = GetDevId;
	mainStateMachine.StoreTempt = StoreTempt;
	
	DirectOneWire.Init(&OneWireProtocol);
	DirectOneWire.OnSkipRomResultReaded = OnDirectOneWireBusReadResult;
	
	directStateMachine.Start();
	directStateMachine.MeasureTemperature = DirectMeasureTemperature;
	directStateMachine.ReadTemperature = DirectReadTemperature;
	directStateMachine.StoreTemp = DirectStoreTemp;
	directStateMachine.GetTemp = GetTermostatTemp;
	directStateMachine.GetHeatThreshold = GetHeatThreshold;
	directStateMachine.GetCoolThreshold = GetCoolThreshold;
	directStateMachine.HeatOnOff = HeatOnOff;
	directStateMachine.CoolOnOff = CoolOnOff;
	
	while(1)
	{
		if (_rxRs485Count)
		{
			int writeCount = ModbusSlave.ProcessFrame(inputBufferRs485, _rxRs485Count, outputBufferRs485);
			if (writeCount > 0)
			{
				Rs485.Write(outputBufferRs485, writeCount);
			}
			_rxRs485Count = 0;
		}
	}
}
