///////////////////////////////////////////////////////////////////////////////
//	Классы UART
//	MSP430
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef UART_MSP430_H
#define UART_MSP430_H

#include "../IdObjectBase.h"
#include "UsciParts.h"
#include "Usci.h"
#include "../Common.h"

namespace Rblib
{
namespace Msp430
{
	// Тип значения регистра
	typedef unsigned char UartRegisterDataType;
	// Тип адреса регистра
	typedef unsigned int  UartRegisterAddressType;

	// Предварительное объявление класса
	template<unsigned int Id>
	class UartErrorCounters;
  
  ////////////////////////////////////////////////////
	// Общий для всех семейств класс UART
	////////////////////////////////////////////////////

	template<
		unsigned int Id,
		class Hardware,
		class Fifo,
		class ErrorCounters = UartErrorCounters<Id>
		>
	class Uart :
		public IdObjectBase<Id>,
		public Hardware,
		public Fifo,
		public ErrorCounters
	{
	public:
		// Инициализация
		static void Init(typename Hardware::DataSize dataSize, typename Hardware::Parity parity, typename Hardware::StopBits stopBits, long baudRate, typename Hardware::ClockSource source, long frequency)
		{
			// Связываем аппаратную часть и программную
			Hardware::OnRxCallback = Fifo::OnRx;
			Hardware::OnTxCallback = Fifo::OnTx;
			Hardware::OnReadCallback = Fifo::OnRead;
			Hardware::OnWriteCallback = Fifo::OnWrite;
			Hardware::OnOverrunCallback = ErrorCounters::OnHardwareInputBufferOverflow;
            Hardware::OnOutputBufferEmptyCallback = Fifo::OutputBufferEmpty;
            
			// Инициализируем аппаратную часть
			Hardware::Init(dataSize, parity, stopBits, baudRate, source, frequency);
		}
	};
	
	///////////////////////////////////////////////////////////
	// Шаблон аппаратной части UART общий для всех семейств
	//////////////////////////////////////////////////////////
	template<
		UartRegisterAddressType Control0RegisterAddr,
		UartRegisterAddressType Control1RegisterAddr,
		UartRegisterAddressType BaudRateControl0RegisterAddr,
		UartRegisterAddressType BaudRateControl1RegisterAddr,
		UartRegisterAddressType ModulationControlRegisterAddr,
		UartRegisterAddressType StatusRegisterAddr,
		UartRegisterAddressType RxBufferRegisterAddr,
		UartRegisterAddressType TxBufferRegisterAddr,
		class InterruptControl,
		class RxPort, unsigned char RxPin,
		class TxPort, unsigned char TxPin
		>
	class UartHardware :
		public UsciHardwareUart<
			UsciControl< UartRegisterDataType, UartRegisterAddressType,
				// адреса
				Control0RegisterAddr, Control1RegisterAddr,
				// структуры
				UsciControlRegister0Uart<UartRegisterDataType>, UsciControlRegister1Uart<UartRegisterDataType>
			>,
			UsciBaudRateControl< UartRegisterDataType, UartRegisterAddressType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>,
			UsciModulationControl< UartRegisterDataType, UartRegisterAddressType,
				// адрес
				ModulationControlRegisterAddr,
				// структура
				UsciModulationControlRegister<UartRegisterDataType>
			>,
			UsciStatus< UartRegisterDataType, UartRegisterAddressType,
				// адрес
				StatusRegisterAddr,
				// структура
				UsciStatusRegisterUart<UartRegisterDataType>
			>,
			UsciBuffers< UartRegisterDataType, UartRegisterAddressType, RxBufferRegisterAddr, TxBufferRegisterAddr>,
			InterruptControl,
			GpioPeripheralFunctionControl<RxPort, RxPin, TxPort, TxPin>,
      UartConstants
		>
	{
	
	};

	////////////////////////////////////////////////////
	// Класс буферов Uart
	////////////////////////////////////////////////////

	template<unsigned int Id, class InputBuffer, class OutputBuffer>
	class UartFifo :
		public IdObjectBase<Id>
	{
	protected:
		RBLIB_NO_INIT static InputBuffer RxBuffer;
		RBLIB_NO_INIT static OutputBuffer TxBuffer;
#ifndef RBLIB_BOOTLOADER
		static bool _notifyRxTxStart;	// Отправлять уведомления о начале приема / передачи данных
		static bool _notifyTxFlag;		// Флаг "нужно в следующий раз отправить уведомление о начале передачи"
#endif // RBLIB_BOOTLOADER
	public:
        static inline bool OutputBufferEmpty()
        {
            return TxBuffer.empty();
        }
		// Прием
		static inline void OnRx(unsigned char data)
		{
#ifndef RBLIB_BOOTLOADER
			if (_notifyRxTxStart)
			{
				if (RxBuffer.empty())
				{
					SendRxNotifyEvent();
				}
			}
#endif // RBLIB_BOOTLOADER
			RxBuffer.push(data);
		}
		// Передача (если есть данные на передачу, заполняется data и возвращается true)
		static inline bool OnTx(unsigned char &data)
		{
			if (TxBuffer.empty())
			{
#ifndef RBLIB_BOOTLOADER
				_notifyTxFlag = true;
#endif // RBLIB_BOOTLOADER
				return false;
			}
#ifndef RBLIB_BOOTLOADER
			if (_notifyRxTxStart && _notifyTxFlag)
			{
				// больше не надо отправлять уведомление, пока буфер полностью не будет отправлен
				_notifyTxFlag = false;
				// отправляем уведомление
				SendTxNotifyEvent();
			}
#endif // RBLIB_BOOTLOADER
			data = TxBuffer.front();
			TxBuffer.pop();
			return true;
		}
		// Чтение (копирование данных из приемного буфера)
		static inline unsigned int OnRead(unsigned char *data, unsigned int len)
		{
			if (RxBuffer.empty())
				return 0;
			// сколько есть данных
			unsigned int remain = RxBuffer.size();
			// будем копировать не больше, чем есть данных в буфере
			unsigned int copy = remain >= len ? len : remain;
			// копируем
			for (unsigned int i = 0; i < copy; i++)
			{
				data[i] = RxBuffer.front();
				RxBuffer.pop();
			}
			return copy;
		}
		// Отправка (копирование данных в буфер на передачу)
		static inline unsigned int OnWrite(unsigned char *data, unsigned int len)
		{
			// сколько есть места в буфере
			unsigned int remain = TxBuffer.Capacity - TxBuffer.size();
			// если нет места, то нечего делать
			if (remain < 1)
				return 0;
			// будем копировать не больше, чем есть места в буфере
			unsigned int copy = remain >= len ? len : remain;
			// копируем
			for (unsigned int i = 0; i < copy; i++)
			{
				TxBuffer.push(data[i]);
			}
			// сколько фактически помещено в буфер
			return copy;
		}
#ifndef RBLIB_BOOTLOADER
		// Включить уведомления о начале приема / передачи данных
		static void EnableReceiveTrasmitStartEvents(bool enable = true)
		{
			_notifyRxTxStart = enable;
		}
#endif // RBLIB_BOOTLOADER
	protected:
#ifndef RBLIB_BOOTLOADER
		// Отправить уведомление о начале приема данных
		static void SendRxNotifyEvent()
		{
			Event e(EventUartReceiveStartNotify, Id);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
		}
		// Отправить уведомление о начале отправки данных
		static void SendTxNotifyEvent()
		{
			Event e(EventUartTransmitStartNotify, Id);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
		}
#endif // RBLIB_BOOTLOADER
	};

	// Инстанцирование статических переменных
	template<unsigned int Id, class InputBuffer, class OutputBuffer>
	InputBuffer UartFifo<Id, InputBuffer, OutputBuffer> :: RxBuffer;

	template<unsigned int Id, class InputBuffer, class OutputBuffer>
	OutputBuffer UartFifo<Id, InputBuffer, OutputBuffer> :: TxBuffer;

#ifndef RBLIB_BOOTLOADER    
	template<unsigned int Id, class InputBuffer, class OutputBuffer>
	bool UartFifo<Id, InputBuffer, OutputBuffer> :: _notifyRxTxStart = false;

	template<unsigned int Id, class InputBuffer, class OutputBuffer>
	bool UartFifo<Id, InputBuffer, OutputBuffer> :: _notifyTxFlag = false;
#endif // RBLIB_BOOTLOADER

	////////////////////////////////////////////////////
	// Класс быстрого FIFO для Uart'а
	////////////////////////////////////////////////////
	
	typedef unsigned short UartFastFifoCounterType;
	typedef unsigned char UartFastFifoDataType;

	template<UartFastFifoCounterType BufSize>
	class UartFastFifo
	{
	protected:
		volatile UartFastFifoCounterType _head;
		volatile UartFastFifoCounterType _tail;
		UartFastFifoDataType _buf[BufSize];
	public:
		static const UartFastFifoCounterType Capacity = BufSize;

		UartFastFifo() : _head(0), _tail(0)
		{}

		inline void push(UartFastFifoDataType &data)
		{
			_head++;
			if(_head == BufSize)
				_head = 0;
			
			// т.к. _head и _tail volatile, то с одной из них работаем через временную переменную
			UartFastFifoCounterType tmp = _head;
	   		// если буфер переполнился
			if (tmp == _tail)
     		{
				// теряем один байт с хвоста
				_tail++;
				if (_tail == BufSize)
					_tail = 0;
				// TODO счётчик переполнений
    		}
			_buf[_head] = data;
		}

		inline UartFastFifoDataType &front()
		{
			// на самом деле элемент находится за указателем на хвост
			UartFastFifoCounterType tmp = _tail;
			tmp++;
			if (tmp == BufSize)
				tmp = 0;
			return _buf[tmp];
		}

		inline void pop()
		{
			// т.к. _head и _tail volatile, то с одной из них работаем через временную переменную
			UartFastFifoCounterType tmp = _head;
		  	// проверяем, что не пусто
			if (tmp == _tail)
				return;

			_tail++;
			if (_tail == BufSize)
				_tail = 0;
		}

		inline bool empty() const
		{
			// т.к. _head и _tail volatile, то с одной из них работаем через временную переменную
			UartFastFifoCounterType tmp = _head;
		  	return tmp == _tail;
		}

		inline UartFastFifoCounterType size()
		{
			// т.к. _head и _tail volatile, то с одной из них работаем через временную переменную
			UartFastFifoCounterType tmp = _head;
		  	if (tmp >= _tail)
			{
				return tmp - _tail;
			}
			else
			{
				return (BufSize - _tail) + tmp;
			}
		}
	};
	
	// Класс счетчиков ошибок Uart
	template<unsigned int Id>
	class UartErrorCounters :
		public IdObjectBase<Id>
	{
	public:
		typedef unsigned long CounterType;
	protected:
		// Счетчик переполнений входного аппаратного буфера
		static volatile CounterType _hardwareInputBufferOverflowCount;
		// Счетчик переполнений входного аппаратного буфера
		static volatile CounterType _softwareInputBufferOverflowCount;
	
	protected:
		static inline void OnHardwareInputBufferOverflow()
		{
			_hardwareInputBufferOverflowCount++;
		}
	public:
		// Получить счетчик переполнений входного аппаратного буфера
		static CounterType GetHardwareInputBufferOverflowCount()
		{
			return _hardwareInputBufferOverflowCount;
		}
		// Сбросить счетчик переполнений входного аппаратного буфера
		static void ClearHardwareInputBufferOverflowCount()
		{
			_hardwareInputBufferOverflowCount = 0;
		}

		// Получить счетчик переполнений входного программного буфера
		static CounterType GetSoftwareInputBufferOverflowCount()
		{
			return _softwareInputBufferOverflowCount;
		}
		// Сбросить счетчик переполнений входного программного буфера
		static void ClearSoftwareInputBufferOverflowCount()
		{
			_softwareInputBufferOverflowCount = 0;
		}
	};

	// Инстанцирование статических переменных
	template<unsigned int Id>
	volatile typename UartErrorCounters<Id>::CounterType UartErrorCounters<Id>::_hardwareInputBufferOverflowCount = 0;
	template<unsigned int Id>
	volatile typename UartErrorCounters<Id>::CounterType UartErrorCounters<Id>::_softwareInputBufferOverflowCount = 0;
}
}

#endif