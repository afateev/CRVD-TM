////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		05/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef MODBUSSTATEMACHINE_H
#define MODBUSSTATEMACHINE_H

class TimeoutDetector
{
public:
	typedef void (CallbackType)(void);
protected:
	unsigned int _counter;
	unsigned int _maximum;
	
	CallbackType *_callback;
public:
	TimeoutDetector()
	{
		ResetCounter();
		SetMaxCount(0);
	}
	
	void ResetCounter()
	{
		_counter = 0;
	}
	
	void SetMaxCount(unsigned int max)
	{
		_maximum = max;
	}
	
	void SetCallback(CallbackType callback)
	{
		_callback = callback;
	}
	
	void Tick()
	{
		_counter++;
		if (_counter >= _maximum)
		{
			if (_callback)
				_callback();
			ResetCounter();
		}
	}
};

template<class Uart, unsigned int BufferSize>
class ModBusMasterStateMachine
{
public:
	typedef Rblib::CallbackWrapper<> OnTxCompleteCallbackType;
	static OnTxCompleteCallbackType OnTxCompleteCallback;
	
	static bool WaitLastByteTx;
protected:
	enum State
	{
		StateNotInited,		// надо вызвать Init
		StateReady,			// ничего не делаем, готовы к отправке
		StateWrite,			// запись данных в Uart
		StateWaitWriteEnd,	// ожидание, пока данные отправ€тс€
		StateWaitWriteLastByte,	// ожидание, пока данные отправ€тс€
		StateWaitRead,		// ожидание приема данных и / или таймаута (~ 3.5 байта)
		StateReadComplete	// вычитывание данных закончено
	};
	
	static volatile State _state;
	static TimeoutDetector _packetDetector;		// детектор таймаута приема
	static TimeoutDetector _lastByteDetector;	// детектор окончани€ отправки последнего байта
	static unsigned char _buffer[BufferSize];	// буфер л€ приема и отправки
	static unsigned int RequestSize;			// размер запроса (данные в буфере)
	static unsigned int ResponseSize;			// размер ответа (данные в буфере)
public:
	static void Init()
	{
		_state = StateReady;
		Uart::OnWriteComplete = OnWriteEnd;
		Uart::OnRead = OnRead;
		_packetDetector.SetMaxCount(350);
		_packetDetector.SetCallback(OnTimeOut);
		_lastByteDetector.SetMaxCount(350);
		_lastByteDetector.SetCallback(OnByteTimeOut);
	}
	
	static bool IsReady()
	{
		return StateReady == _state;
	}
	
	static bool IsReadComplete()
	{
		return StateReadComplete == _state;
	}

	static void ChangeSpeed(unsigned long CoreFrequency, unsigned int baudrate)
    {
        Uart::Init(CoreFrequency, baudrate, Uart::StopBits1, Uart::ParityNone);
        if (baudrate == 115200)
            //_packetDetector.SetMaxCount(35); // TODO ѕ–ќ¬≈–»“№ “ј…ћј”“џ
			_packetDetector.SetMaxCount(70);
        else
            _packetDetector.SetMaxCount(500);
		
		if (baudrate < 1)
		{
			baudrate = 1;
		}
		float oneBitTime = 1.0 / baudrate;
		float oneByteTime = oneBitTime * 12; // 12 бит в байте, включа€ старт четность стопы
		float ticksPerByte = 10000.0 * oneByteTime;	// тики от таймера 10 к√ц
		int ticksPerByteInt = (int)ticksPerByte;
		// меньше не целесообразно, слишком часто прерывани€ будут
		if (ticksPerByteInt < 10)
		{
			ticksPerByteInt = 10;
		}
		
		_lastByteDetector.SetMaxCount(ticksPerByteInt);
    }
    
	static void SendRequest(unsigned char *data, unsigned int dataLen)
	{
		if (0 == data)
			return;
		unsigned int count = dataLen;
		if (count > BufferSize)
			count = BufferSize;
		for (int i = 0; i < count; i++)
			_buffer[i] = data[i];
		
		RequestSize = count;
		ResponseSize = 0;
		
		// есть что отправить
		if (RequestSize > 0)
		{
			// начинаем
			_state = StateWrite;
		}
	}
	
	static unsigned char * GetResponse(unsigned int &dataLen)
	{
		if (IsReadComplete())
		{
			_state = StateReady;
		}
		dataLen = ResponseSize;
		return _buffer;
	}
	
	static void PacketTimeoutDetectorTick()
	{
		_packetDetector.Tick();
		if (StateWaitWriteLastByte == _state)
		{
			_lastByteDetector.Tick();
		}
	}
	
	static void Run()
	{
		switch(_state)
		{
		case StateReady:
			{
				// есть что отправить
				if (RequestSize > 0)
				{
					// начинаем
					_state = StateWrite;
				}
			}
			break;
		case StateWrite:
			{
				// отправл€ем
				Uart::Write(_buffer, RequestSize);
				RequestSize = 0;
				_state = StateWaitWriteEnd;
			}
			break;
		case StateWaitWriteEnd:
			{
				// ждЄм пока Uart вызовет OnWriteEnd
			}
			break;
		case StateWaitWriteLastByte:
			{
				// ждЄм пока _lastByteDetector вызовет OnByteTimeOut
			}
			break;
		case StateWaitRead:
			{
				// ждЄм пока _packetDetector вызовет OnTimeOut
			}
			break;
		case StateReadComplete:
			{
				// данные в количестве ResponseSize лежат в _buffer
				// ожидание вызова GetResponse
			}
			break;
		};
	}
	static void OnWriteEnd()
	{
		if (StateWaitWriteEnd == _state)
		{
			// забудем что было
			for (int i = 0; i < BufferSize; i++)
			{
				_buffer[i] = 0;
			}

			_packetDetector.ResetCounter();
			//Gpio::_D::SetOutputPin(2);
			//Gpio::_D::SetBit(2);
			if (WaitLastByteTx)
			{
				_lastByteDetector.ResetCounter();
				_state = StateWaitWriteLastByte;
			}
			else
			{
				_state = StateWaitRead;
				OnTxCompleteCallback();
			}
		}
	}
	static void OnRead(unsigned int nextByte)
	{
		_packetDetector.ResetCounter();
		
		if (StateWaitRead == _state)
		{
			if (ResponseSize < BufferSize)
			{
				_buffer[ResponseSize] = nextByte;
				ResponseSize++;
			}
		}
	}
	
	static void OnTimeOut()
	{
		//Gpio::_D::SetOutputPin(2);
		//Gpio::_D::ClearBit(2);
		if (StateWaitRead == _state)
			_state = StateReadComplete;
	}
	
	static void OnByteTimeOut()
	{
		_lastByteDetector.ResetCounter();
		if (StateWaitWriteLastByte == _state)
		{
			_state = StateWaitRead;
			OnTxCompleteCallback();
		}
	}
	
	// check CRC is ok
	static bool CrcOk(unsigned char *data, unsigned int len)
	{
		if (0 == data)
			return false;
		
		if (len < 3)
		{
			return false;
		}
		
		unsigned short crcCalculated = ModBusCrc::GetCrc(data, len - 2);
		unsigned short crcExpected = data[len - 2];
		crcExpected <<= 8;
		crcExpected |= data[len - 1];
		
		return crcCalculated == crcExpected;
	}
};

template<class Uart, unsigned int BufferSize>
bool ModBusMasterStateMachine<Uart, BufferSize>::WaitLastByteTx = false;

template<class Uart, unsigned int BufferSize>
ModBusMasterStateMachine<Uart, BufferSize>::OnTxCompleteCallbackType ModBusMasterStateMachine<Uart, BufferSize>::OnTxCompleteCallback;

template<class Uart, unsigned int BufferSize>
typename ModBusMasterStateMachine<Uart, BufferSize>::State
ModBusMasterStateMachine<Uart, BufferSize>::_state = ModBusMasterStateMachine<Uart, BufferSize>::StateNotInited;

template<class Uart, unsigned int BufferSize>
TimeoutDetector ModBusMasterStateMachine<Uart, BufferSize>::_packetDetector;

template<class Uart, unsigned int BufferSize>
TimeoutDetector ModBusMasterStateMachine<Uart, BufferSize>::_lastByteDetector;

template<class Uart, unsigned int BufferSize>
unsigned char ModBusMasterStateMachine<Uart, BufferSize>::_buffer[BufferSize];

template<class Uart, unsigned int BufferSize>
unsigned int ModBusMasterStateMachine<Uart, BufferSize>::RequestSize = 0;

template<class Uart, unsigned int BufferSize>
unsigned int ModBusMasterStateMachine<Uart, BufferSize>::ResponseSize = 0;

////////////////////////// SLAVE //////////////////////////////////////////////

template<class Uart, unsigned int BufferSize>
class ModBusSlaveStateMachine
{
public:
	static const unsigned int BufferLen = BufferSize;
	static unsigned int CurrentBoudrate;
	static bool CurrentParityEnabled;
	static bool CurrentParityEven;
protected:
	enum State
	{
		StateNotInited,		// надо вызвать Init
		StateWrite,			// запись данных в Uart
		StateWaitWriteEnd,	// ожидание, пока данные отправ€тс€
		StateWaitWriteLastByte,	// ожидание, пока данные отправ€тс€
		StateWaitRead,		// ожидание приема данных и / или таймаута (~ 3.5 байта)
		StateRead,			// вычитывание данных
		StateBuildResponse	// построение ответа
	};
	
	static volatile State _state;
	static TimeoutDetector _packetDetector;		// детектор таймаута приема
	static TimeoutDetector _lastByteDetector;	// детектор окончани€ отправки последнего байта
	static unsigned char _buffer[BufferSize];	// буфер л€ приема и отправки
	static volatile unsigned int RequestSize;			// размер запроса (данные в буфере)
	static volatile unsigned int ResponseSize;			// размер ответа (данные в буфере)
	typedef unsigned int (BuildResponseCallbackType)(unsigned char *data, unsigned int dataLen);
	typedef void (TxEnableCallbackType)(bool txEnable);
	static BuildResponseCallbackType *BuildResponseCallback;
	static TxEnableCallbackType *TxEnableCallback;
public:
	static void Init()
	{
		RequestSize = 0;
		_state = StateWaitRead;
		Uart::SetOnTxEndCallback(OnWriteEnd);
		Uart::SetOnNewCharCallback(OnRead);
		_packetDetector.SetMaxCount(350);
		_packetDetector.SetCallback(OnTimeOut);
		_lastByteDetector.SetMaxCount(350);
		_lastByteDetector.SetCallback(OnByteTimeOut);
	}
	
	static void ChangeSpeed(unsigned long CoreFrequency, unsigned int baudrate)
    {
        Uart::Init(CoreFrequency, baudrate);
        CurrentBoudrate = baudrate;
		
		if (baudrate < 1)
		{
			baudrate = 1;
		}
		float oneBitTime = 1.0 / baudrate;
		float oneByteTime = oneBitTime * 12; // 12 бит в байте, включа€ старт четность стопы
		float ticksPerByte = 10000.0 * oneByteTime;	// тики от таймера 10 к√ц
		int ticksPerByteInt = (int)ticksPerByte;
		// меньше не целесообразно, слишком часто прерывани€ будут
		if (ticksPerByteInt < 10)
		{
			ticksPerByteInt = 10;
		}
		
		_lastByteDetector.SetMaxCount(ticksPerByteInt);
		
		float tecksPer3_5Bytes = ticksPerByte * 3.5;
		int tecksPer3_5BytesInt = (int)tecksPer3_5Bytes;
		// меньше не целесообразно, слишком часто прерывани€ будут
		if (tecksPer3_5BytesInt <= 20)
		{
			tecksPer3_5BytesInt = 20;
		}
		
		_packetDetector.SetMaxCount(tecksPer3_5BytesInt);
		/*
		if (baudrate == 115200)
		{
            //_packetDetector.SetMaxCount(35); // TODO ѕ–ќ¬≈–»“№ “ј…ћј”“џ
			_packetDetector.SetMaxCount(70);
		}
        else
		{
            _packetDetector.SetMaxCount(2000);
		}
		*/
    }
	
	static void ChangeParity(bool enabled, bool even)
	{
		Uart::ParityControlEnable(enabled);
		Uart::ParitySelection(!even);
		CurrentParityEnabled = enabled;
		CurrentParityEven = even;
	}
    
	static unsigned int BuildResponse(unsigned char *data, unsigned int dataLen)
	{
		// в data данные запроса
		// dataLen длина запроса
		
		// длина ответа
		unsigned int responseLen = 0;
		// данные ответа будут в data
		if (BuildResponseCallback)
		{
			responseLen = BuildResponseCallback(data, dataLen);
		}
		
		return responseLen;
	}
	
	inline static void SetBuildResponseCallback(BuildResponseCallbackType *callback)
	{
		BuildResponseCallback = callback;
	}
	
	static void TxEnable(bool txEnable)
	{
		if (TxEnableCallback)
		{
			TxEnableCallback(txEnable);
		}
	}
	
	inline static void SetTxEnableCallback(TxEnableCallbackType *callback)
	{
		TxEnableCallback = callback;
	}
	
	static void PacketTimeoutDetectorTick()
	{
		/*
        static unsigned int test = 0;
		test++;
		if (test >= (10000 * 3))
		{
			RequestSize = 8;
			_buffer[0] = 0x01;
			_buffer[1] = 0x03;
			_buffer[2] = 0x00;
			_buffer[3] = 0x01;
			_buffer[4] = 0x00;
			_buffer[5] = 0x03;
			_buffer[6] = 0x54;
			_buffer[7] = 0x0B;
			test = 0;
		}
		*/
		// первый байт запроса езЄ не получен, нечего ждать
		if (RequestSize > 0 && StateWaitRead == _state)
		{
			_packetDetector.Tick();
		}
		
		if (StateWaitWriteLastByte == _state)
		{
			_lastByteDetector.Tick();
		}
	}
	
	static void Run()
	{
		switch(_state)
		{
		case StateWrite:
			{
				// вычитать всЄ из буферов перед отправкой
				unsigned char tmp[256];
				unsigned int readed = Uart::Read(tmp, 256);
				
				
				// отправл€ем
				TxEnable(true);
				Uart::Write(_buffer, ResponseSize);
				ResponseSize = 0;
				_state = StateWaitWriteEnd;
				// забудем что было
				for (int i = 0; i < BufferSize; i++)
					_buffer[i] = 0;
			}
			break;
		case StateWaitWriteEnd:
			{
				// ждЄм пока Uart вызовет OnWriteEnd
			}
			break;
		case StateWaitRead:
			{
				// ждЄм пока _packetDetector вызовет OnTimeOut
			}
			break;
		case StateWaitWriteLastByte:
			{
				// ждЄм пока _lastByteDetector вызовет OnByteTimeOut
			}
			break;
		case StateRead:
			{
				RequestSize = 0;
				unsigned int requestSize = Uart::Read(_buffer, 256);
				ResponseSize = BuildResponse(_buffer, requestSize);
				if (ResponseSize > 0)
				{
					_state = StateWrite;
				}
				else
				{
					_state = StateWaitRead;
				}
			}
			break;
		};
	}
	static void OnWriteEnd()
	{
		if (StateWaitWriteEnd == _state)
		{
			_packetDetector.ResetCounter();
			//Gpio::_D::SetOutputPin(2);
			//Gpio::_D::SetBit(2);
			_lastByteDetector.ResetCounter();
			_state = StateWaitWriteLastByte;
		}
	}
	static void OnRead()
	{
		RequestSize++;
		_packetDetector.ResetCounter();
	}
	
	static void OnTimeOut()
	{
		_packetDetector.ResetCounter();
		if (StateWaitRead == _state)
		{
			_state = StateRead;
		}
	}
	
	static void OnByteTimeOut()
	{
		_lastByteDetector.ResetCounter();
		if (StateWaitWriteLastByte == _state)
		{
			TxEnable(false);
			_state = StateWaitRead;
		}
	}
};

template<class Uart, unsigned int BufferSize>
unsigned int ModBusSlaveStateMachine<Uart, BufferSize>::CurrentBoudrate = 0;

template<class Uart, unsigned int BufferSize>
bool ModBusSlaveStateMachine<Uart, BufferSize>::CurrentParityEnabled = 0;

template<class Uart, unsigned int BufferSize>
bool ModBusSlaveStateMachine<Uart, BufferSize>::CurrentParityEven = 0;

template<class Uart, unsigned int BufferSize>
typename ModBusSlaveStateMachine<Uart, BufferSize>::State
ModBusSlaveStateMachine<Uart, BufferSize>::_state = ModBusSlaveStateMachine<Uart, BufferSize>::StateNotInited;

template<class Uart, unsigned int BufferSize>
TimeoutDetector ModBusSlaveStateMachine<Uart, BufferSize>::_packetDetector;

template<class Uart, unsigned int BufferSize>
TimeoutDetector ModBusSlaveStateMachine<Uart, BufferSize>::_lastByteDetector;

template<class Uart, unsigned int BufferSize>
unsigned char ModBusSlaveStateMachine<Uart, BufferSize>::_buffer[BufferSize];

template<class Uart, unsigned int BufferSize>
unsigned int ModBusSlaveStateMachine<Uart, BufferSize>::RequestSize = 0;

template<class Uart, unsigned int BufferSize>
unsigned int ModBusSlaveStateMachine<Uart, BufferSize>::ResponseSize = 0;

template<class Uart, unsigned int BufferSize>
typename ModBusSlaveStateMachine<Uart, BufferSize>::BuildResponseCallbackType *
ModBusSlaveStateMachine<Uart, BufferSize>::BuildResponseCallback = 0;

template<class Uart, unsigned int BufferSize>
typename ModBusSlaveStateMachine<Uart, BufferSize>::TxEnableCallbackType *
ModBusSlaveStateMachine<Uart, BufferSize>::TxEnableCallback = 0;

#endif