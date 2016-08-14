
#include "../RbLib/Rblib.h"

typedef Rblib::ResetAndClockControl Rcc;
typedef Rblib::Gpio Gpio;
typedef Rblib::Nvic Nvic;
typedef Rblib::Usart Usart;
typedef Rblib::Timer Timer;
typedef Rblib::InterruptMap InterruptMap;
typedef Rblib::FlashMemoryController FlashMemoryController;

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
			Gpio::A::SetBit(11);
		}
		else
		{
			Gpio::A::ClearBit(11);
		}
	}
};

template <class Usart>
Rs485InterfaceImpl<Usart>::OnReadCallbackType Rs485InterfaceImpl<Usart>::OnReadCallback;

template <class Usart>
Rs485InterfaceImpl<Usart>::OnWriteCompleteCallbackType Rs485InterfaceImpl<Usart>::OnWriteCompleteCallback;

typedef Rs485InterfaceImpl<Usart::Usart1> Rs485Interface;

template
	<
		class ChipSelectPort,
		int ChipSelectPin,
		class SerialClockPort,
		int SerialClockPin,
		class DataInputPort,
		int DataInputPin
	>
class TLC5615Connection
{
public:
	class ChipSelect
	{
	public:
		static void Init()
		{
			ChipSelectPort::SetMode(ChipSelectPin, ChipSelectPort::ModeOutput);
			ChipSelectPort::SetConfig(ChipSelectPin, ChipSelectPort::ConfigOutputPushPull);
		}
		
		static void Set()
		{
			ChipSelectPort::SetBit(ChipSelectPin);
		}
		
		static void Clear()
		{
			ChipSelectPort::ClearBit(ChipSelectPin);
		}
	};
	
	class SerialClock
	{
	public:
		static void Init()
		{
			SerialClockPort::SetMode(SerialClockPin, SerialClockPort::ModeOutput);
			SerialClockPort::SetConfig(SerialClockPin, SerialClockPort::ConfigOutputPushPull);
		}
		
		static void Set()
		{
			SerialClockPort::SetBit(SerialClockPin);
		}
		
		static void Clear()
		{
			SerialClockPort::ClearBit(SerialClockPin);
		}
	};
	
	// дл€ нас это выход, дл€ микросхемы вход
	class DataInput
	{
	public:
		static void Init()
		{
			DataInputPort::SetMode(DataInputPin, DataInputPort::ModeOutput);
			DataInputPort::SetConfig(DataInputPin, DataInputPort::ConfigOutputPushPull);
		}
		
		static void Set()
		{
			DataInputPort::SetBit(DataInputPin);
		}
		
		static void Clear()
		{
			DataInputPort::ClearBit(DataInputPin);
		}
	};
public:
	static void Init()
	{
		ChipSelect::Init();
		SerialClock::Init();
		DataInput::Init();
	}
};

typedef TLC5615Connection<Gpio::A, 1, Gpio::A, 5, Gpio::A, 7> IOut1Connection;
typedef Rblib::Driver::TLC5615<IOut1Connection> IOut1;
typedef TLC5615Connection<Gpio::A, 2, Gpio::A, 5, Gpio::A, 7> IOut2Connection;
typedef Rblib::Driver::TLC5615<IOut2Connection> IOut2;
typedef TLC5615Connection<Gpio::A, 3, Gpio::A, 5, Gpio::A, 7> IOut3Connection;
typedef Rblib::Driver::TLC5615<IOut3Connection> IOut3;

template
	<
		class Port,
		int Pin
	>
class ReleConnection
{
public:
	static void Init()
	{
		Port::SetMode(Pin, Port::ModeOutput);
		Port::SetConfig(Pin, Port::ConfigOutputPushPull);
	}
	
	static void Set()
	{
		Port::SetBit(Pin);
	}
	
	static void Clear()
	{
		Port::ClearBit(Pin);
	}
};

template
	<
		class Connection
	>
class Rele
{
public:
	static void SetOutput(bool close)
	{
		// замкнуто
		if (close)
		{
			Connection::Set();
		}
		else
		{
			Connection::Clear();
		}
	}
};

typedef ReleConnection<Gpio::B, 15> Rele1Connection;
typedef Rele<Rele1Connection> Rele1;
typedef ReleConnection<Gpio::B, 14> Rele2Connection;
typedef Rele<Rele2Connection> Rele2;
typedef ReleConnection<Gpio::B, 13> Rele3Connection;
typedef Rele<Rele3Connection> Rele3;

typedef Rblib::Driver::Rs485<Rs485Interface> Rs485Type;
Rs485Type Rs485;

static const unsigned int FastTickFrequency = 5000;
unsigned int oneSecondCounter = 0;
bool oneSecondFlag = false;
void OnTimer1Update()
{
	Rs485.Tick(FastTickFrequency);
	if (oneSecondCounter < FastTickFrequency)
	{
		oneSecondCounter++;
	}
	else
	{
		oneSecondCounter = 0;
		oneSecondFlag = true;
	}
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
		ModbusAddress = 4;
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
};

Storage storage;

static const int OutputCount = 4;
static const int OutputTimeout = 5; // секунд
unsigned char outputValue[OutputCount] = {0, 0, 0, 0};
int outputCountdown[OutputCount] = {0, 0, 0, 0};

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
	RegIOut1			= 2,
	RegIOut2			= 3,
	RegIOut3			= 4,
	RegRelOut			= 5,
};

void GetRegisterValue(unsigned short reg, unsigned short &val)
{
	switch(reg)
	{
	case RegMbAddress:
		val = storage.GetModbusAddress();
		break;
	case RegIOut1:
		val = outputValue[0];
		break;
	case RegIOut2:
		val = outputValue[1];
		break;
	case RegIOut3:
		val = outputValue[2];
		break;
	case RegRelOut:
		val = outputValue[3] & 0x07;
		break;
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
	case RegIOut1:
		outputValue[0] = val & 0xFF;
		outputCountdown[0] = OutputTimeout;
		result = true;
		break;
	case RegIOut2:
		outputValue[1] = val & 0xFF;
		outputCountdown[1] = OutputTimeout;
		result = true;
		break;
	case RegIOut3:
		outputValue[2] = val & 0xFF;
		outputCountdown[2] = OutputTimeout;
		result = true;
		break;
	case RegRelOut:
		outputValue[3] = val & 0x07;
		outputCountdown[3] = OutputTimeout;
		result = true;
		break;
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
	// HSE без делени€
	Rcc::SetPLLHSEDivider(Rcc::HSEDiv1);
	// PLL = 48 MHz = 8 * 6
	Rcc::SetPLLMultiplicationFactor(Rcc::PLLMul6);
	// ¬ключаем
	Rcc::PLLEnable();
	while(!Rcc::PLLReady())
	{
		;
	}
	// ядро от PLL
	Rcc::SetSystemClock(Rcc::SystemClockPLL);
	
	FlashMemoryController::Unlock();
	
	Rcc::EnableClockPortA();
	Rcc::EnableClockPortB();
	Rcc::EnableClockTimer1();
	Rcc::EnableClockUsart1();
	
	IOut1Connection::Init();
	IOut1::Init();
	IOut2Connection::Init();
	IOut2::Init();
	IOut3Connection::Init();
	IOut3::Init();
	
	Rele1Connection::Init();
	Rele2Connection::Init();
	Rele3Connection::Init();
	
	Timer::Timer1::SetPrescaler(48);
	Timer::Timer1::SetMaxCount(1000000 / FastTickFrequency);
	Timer::Timer1::UpdateInterruptHandler = OnTimer1Update;
	Timer::Timer1::Start();
	Timer::Timer1::ClearUpdateInterruptFlag();
	Timer::Timer1::EnableUpdateInterrupt();
	Nvic::InterruptEnable(Nvic::InterruptVector_TIM1_UP);
	
	// USART1
	Gpio::A::SetMode(9, Gpio::A::ModeOutput);
	Gpio::A::SetConfig(9, Gpio::A::ConfigOutputAlternatePushPull);
	
	Gpio::A::SetMode(10, Gpio::A::ModeInput);
	Gpio::A::SetConfig(10, Gpio::A::ConfigInputFloating);
	
	// 485
	// RE/DE
	Gpio::A::SetMode(11, Gpio::A::ModeOutput);
	Gpio::A::SetConfig(11, Gpio::A::ConfigOutputPushPull);
	Gpio::A::ClearBit(11);
	
	Nvic::InterruptEnable(Nvic::InterruptVector_USART1);
	
	Rs485Interface::Init();
	Rs485.Init(0.0025);
	Rs485.OnReadComplete = OnRs485DataReceived;
	
	ModbusSlave.GetSelfAddress = GetModbusAddress;
	ModbusSlave.GetRegisterValue = GetRegisterValue;
	ModbusSlave.SetRegisterValue = SetRegisterValue;
	ModbusSlave.Init();
	
	InterruptMap::GlobalInterruptEnable();
	
	storage.Load();
	
	while(1)
	{
		if (oneSecondFlag)
		{
			oneSecondFlag = false;
			for (int i = 0; i < OutputCount; i++)
			{
				if (outputCountdown[i] > 0)
				{
					outputCountdown[i]--;
				}
				else
				{
					outputValue[i] = 0;
				}
			}
		}
		
		if (_rxRs485Count)
		{
			int writeCount = ModbusSlave.ProcessFrame(inputBufferRs485, _rxRs485Count, outputBufferRs485);
			if (writeCount > 0)
			{
				Rs485.Write(outputBufferRs485, writeCount);
			}
			_rxRs485Count = 0;
		}
		
		// сдвиг на 1 чтобы попасть в диапазон 4 - 20 mA
		IOut1::Write(outputValue[0] << 1);
		IOut2::Write(outputValue[1] << 1);
		IOut3::Write(outputValue[2] << 1);
		Rele1::SetOutput(outputValue[3] & (1 << 0));
		Rele2::SetOutput(outputValue[3] & (1 << 1));
		Rele3::SetOutput(outputValue[3] & (1 << 2));
	}
}
