#ifndef UART_EMF32_H
#define UART_EMF32_H

#include "../IdObjectBase.h"
#include "Usart.h"

namespace Rblib
{
namespace Efm32
{
namespace Uart
{
	////////////////////////////////////////////////////
	// Класс быстрого FIFO для Uart'а
	////////////////////////////////////////////////////
	
	typedef unsigned short FastFifoCounterType;
	typedef unsigned char FastFifoDataType;

	template<FastFifoCounterType BufSize>
	class FastFifo
	{
	protected:
		volatile FastFifoCounterType _head;
		volatile FastFifoCounterType _tail;
		FastFifoDataType _buf[BufSize];
	public:
		static const FastFifoCounterType Capacity = BufSize - 1;

		FastFifo() : _head(0), _tail(0)
		{}

		inline void push(FastFifoDataType &data)
		{
			_head++;
			if(_head == BufSize)
				_head = 0;
			
			// т.к. _head и _tail volatile, то с одной из них работаем через временную переменную
			FastFifoCounterType tmp = _head;
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

		inline FastFifoDataType &front()
		{
			// на самом деле элемент находится за указателем на хвост
			FastFifoCounterType tmp = _tail;
			tmp++;
			if (tmp == BufSize)
				tmp = 0;
			return _buf[tmp];
		}

		inline void pop()
		{
			// т.к. _head и _tail volatile, то с одной из них работаем через временную переменную
			FastFifoCounterType tmp = _head;
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
			FastFifoCounterType tmp = _head;
		  	return tmp == _tail;
		}

		inline FastFifoCounterType size()
		{
			// т.к. _head и _tail volatile, то с одной из них работаем через временную переменную
			FastFifoCounterType tmp = _head;
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
	
	// Тип значения регистра
	typedef Usart::ValueDataType ValueDataType;
	// Тип адреса регистра
	typedef Usart::RegisterAddressType RegisterAddressType;
	
	template
		<
			unsigned int IdObj,
			RegisterAddressType BaseAddr,
			RegisterAddressType ControlRegisterRegAddr,
			RegisterAddressType FrameFormatRegAddr,
			RegisterAddressType TriggerControlRegAddr,
			RegisterAddressType CommandRegAddr,
			RegisterAddressType StatusAddr,
			RegisterAddressType ClockControlRegAddr,
			RegisterAddressType RxBufferDataExtendedRegAddr,
			RegisterAddressType RxBufferDataRegAddr,
			RegisterAddressType RxBufferDoubleDataExtendedRegAddr,
			RegisterAddressType RxBufferDoubleDataRegAddr,
			RegisterAddressType RxBufferDataExtendedPeekRegAddr,
			RegisterAddressType RxBufferDoubleDataExtendedPeekRegAddr,
			RegisterAddressType TxBufferDataExtendedRegAddr,
			RegisterAddressType TxBufferDataRegAddr,
			RegisterAddressType TxBufferDoubleDataExtendedRegAddr,
			RegisterAddressType TxBufferDoubleDataRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr,
			RegisterAddressType InterruptEnableRegAddr,
			RegisterAddressType IrDAControlRegAddr,
			RegisterAddressType IORoutingRegAddr,
			RegisterAddressType InputRegAddr,
			RegisterAddressType I2SControlRegAddr,
			unsigned int FifoSize
		>
	class Uart :
		public IdObjectBase<IdObj>,
		public Usart::ControlRegister<BaseAddr + ControlRegisterRegAddr>,
		public Usart::FrameFormatRegister<BaseAddr + FrameFormatRegAddr>,
		public Usart::IORoutingRegister<BaseAddr + IORoutingRegAddr>
	{
	public:
		typedef typename Usart::ControlRegister<BaseAddr + ControlRegisterRegAddr> ControlRegister;
		typedef typename Usart::FrameFormatRegister<BaseAddr + FrameFormatRegAddr> FrameFormatRegister;
		typedef typename Usart::CommandRegister<BaseAddr + CommandRegAddr> CommandRegister;
		typedef typename Usart::ClockControlRegister<BaseAddr + ClockControlRegAddr> ClockControlRegister;
		typedef typename Usart::RxBufferDataExtendedRegister<BaseAddr + RxBufferDataExtendedRegAddr> RxBufferDataExtendedRegister;
		typedef typename Usart::RxBufferDataRegister<BaseAddr + RxBufferDataRegAddr> RxBufferDataRegister;
		typedef typename Usart::RxBufferDoubleDataExtendedRegister<BaseAddr + RxBufferDoubleDataExtendedRegAddr> RxBufferDoubleDataExtendedRegister;
		typedef typename Usart::RxBufferDoubleDataRegister<BaseAddr + RxBufferDoubleDataRegAddr> RxBufferDoubleDataRegister;
		typedef typename Usart::TxBufferDataExtendedRegister<BaseAddr + TxBufferDataExtendedRegAddr> TxBufferDataExtendedRegister;
		typedef typename Usart::TxBufferDataRegister<BaseAddr + TxBufferDataRegAddr> TxBufferDataRegister;
		typedef typename Usart::TxBufferDoubleDataExtendedRegister<BaseAddr + TxBufferDoubleDataExtendedRegAddr> TxBufferDoubleDataExtendedRegister;
		typedef typename Usart::TxBufferDoubleDataRegister<BaseAddr + TxBufferDoubleDataRegAddr> TxBufferDoubleDataRegister;
		typedef typename Usart::InterruptFlagRegister<BaseAddr + InterruptFlagRegAddr> InterruptFlagRegister;
		typedef typename Usart::InterruptFlagSetRegister<BaseAddr + InterruptFlagSetRegAddr> InterruptFlagSetRegister;
		typedef typename Usart::InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr> InterruptFlagClearRegister;
		typedef typename Usart::InterruptEnableRegister<BaseAddr + InterruptEnableRegAddr> InterruptEnableRegister;
		typedef typename Usart::IORoutingRegister<BaseAddr + IORoutingRegAddr> IORoutingRegister;
		
		typedef typename ControlRegister::Oversampling Oversampling;
		typedef typename FrameFormatRegister::DataSize DataSize;
		typedef typename FrameFormatRegister::Parity Parity;
		typedef typename FrameFormatRegister::StopBits StopBits;
		typedef typename IORoutingRegister::Location Location;
	protected:
		typedef FastFifo<FifoSize> FifoType;
		static FifoType _rxFifo;
		static FifoType _txFifo;
	public:
		// Инициализация
    	static void Init(DataSize dataSize, Parity parity, StopBits stopBits, unsigned long baudRate, unsigned long clockSourcefrequency, Oversampling oversampling = ControlRegister::Oversampling16)
		{
			if (baudRate < 1)
			{
				return;
			}
			
			double divider = clockSourcefrequency;
			switch (oversampling)
			{
			case ControlRegister::Oversampling16:
				{
					divider /= 16;
				}
				break;
			case ControlRegister::Oversampling8:
				{
					divider /= 8;
				}
				break;
			case ControlRegister::Oversampling6:
				{
					divider /= 6;
				}
				break;
			case ControlRegister::Oversampling4:
				{
					divider /= 4;
				}
				break;
			}
			
			divider /= baudRate;
			
			if (divider < 1)
			{
				return;
			}
			
			divider -= 1;
			divider *= 256;
			
			ControlRegister::SetSynchronousMode(false);
			ControlRegister::SetOversampling(oversampling);
			
			FrameFormatRegister::SetDataBitMode(dataSize);
			FrameFormatRegister::SetParityBitMode(parity);
			FrameFormatRegister::SetStopBitMode(stopBits);
			
			ClockControlRegister::SetClockDivider(divider);
			
			CommandRegister::Do(CommandRegister::CommandReceiverEnable);
			CommandRegister::Do(CommandRegister::CommandTransmitterEnable);
			
			InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptTxBufferLevel);
			InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptRxDataValid);
		}
		
		// Чтение данных из порта
		static unsigned int Read(unsigned char* data, int length)
		{
			if (0 == data || length < 1)
			{
				return 0;
			}
			
			InterruptEnableRegister::InterruptDisable(InterruptEnableRegister::InterruptRxDataValid);
			
			// сколько есть данных
			unsigned int remain = _rxFifo.size();
			// будем копировать не больше, чем есть данных в буфере
			unsigned int copy = remain >= length ? length : remain;
			
			// копируем
			for (unsigned int i = 0; i < copy; i++)
			{
				data[i] = _rxFifo.front();
				_rxFifo.pop();
			}
			
			InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptRxDataValid);
			
			return copy;
		}
		
		// Запись данных в порт
		static void Write(unsigned char* data, int length)
		{
			if (0 == data || length < 1)
			{
				return;
			}
			
			unsigned int remain = length;	// сколько нужно данных записхнуть в буфер на отправку
			
			// ждём пока все данные не будут помещены в буфер на отправку
			while (remain > 0)
			{
				// для безопасности выключаем прерывания на отпраку
				InterruptEnableRegister::InterruptDisable(InterruptEnableRegister::InterruptTxBufferLevel);
				// пытаемся сколько осталось данных запихнуть в буфер на отправку
				// сколько есть места в буфере
				unsigned int free = _txFifo.Capacity - _txFifo.size();
				// если нет места, то нечего делать
				if (free < 1)
				{
					// включаем прерывания на отправку
					InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptTxBufferLevel);
					continue;
				}
				// будем копировать не больше, чем есть места в буфере
				unsigned int copy = free >= remain ? remain : free;
				// копируем
				for (unsigned int i = 0; i < copy; i++)
				{
					_txFifo.push(data[i]);
				}
				// двигаем указатель на исходные данные
				data += copy;
				// вычисляем сколько осталось скопировать
				remain -= copy;
				// включаем прерывания на отправку
				InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptTxBufferLevel);
			}
			/*
			// подождать завершения передачи (но это по желанию)
			bool empty = false;
			while(!empty)
			{
				InterruptEnableRegister::InterruptDisable(InterruptEnableRegister::InterruptTxBufferLevel);
				empty = _txFifo.empty();
				InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptTxBufferLevel);
			}
			*/
		}
		
		static void SetIORouting(Location location, bool rxPinEnable = true, bool txPinEnable = true)
		{
			IORoutingRegister::SetPinEnable(rxPinEnable, txPinEnable, false, false);
			IORoutingRegister::SetIOLocation(location);
		}
		
		static void RxInterruptHandler()
		{
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptRxDataValid))
			{
				unsigned short data = 0;
				bool parityError = false;
				bool framingError = false;
				
				RxBufferDataExtendedRegister::Read(&data, &parityError, &framingError);
				
				if (!parityError && ! framingError)
				{
					OnRx(data & 0xFF);
				}
			}
			
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptRxOverflow))
			{
				__no_operation();
			}
		}
		
		static void TxInterruptHandler()
		{
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptTxBufferLevel))
			{
				unsigned char data = 0;
				if (OnTx(&data))
				{
					TxBufferDataRegister::Write(data);
				}
				else
				{
					InterruptEnableRegister::InterruptDisable(InterruptEnableRegister::InterruptTxBufferLevel);
				}
			}
		}
		
		static void *GetRxBufferDataRegAddr()
		{
			return (void *)(BaseAddr + RxBufferDataRegAddr);
		}
		
		static void *GetTxBufferDataRegAddr()
		{
			return (void *)(BaseAddr + TxBufferDataRegAddr);
		}
	protected:
		static void OnRx(unsigned char data)
		{
			_rxFifo.push(data);
		}
		
		static bool OnTx(unsigned char *data)
		{
			if (_txFifo.empty())
			{
				return false;
			}
			
			unsigned char nextByte = _txFifo.front();
			_txFifo.pop();
			
			if (data)
			{
				*data = nextByte;
			}
			return true;
		}
	};
	
	template
		<
			unsigned int IdObj,
			RegisterAddressType BaseAddr,
			RegisterAddressType ControlRegisterRegAddr,
			RegisterAddressType FrameFormatRegAddr,
			RegisterAddressType TriggerControlRegAddr,
			RegisterAddressType CommandRegAddr,
			RegisterAddressType StatusAddr,
			RegisterAddressType ClockControlRegAddr,
			RegisterAddressType RxBufferDataExtendedRegAddr,
			RegisterAddressType RxBufferDataRegAddr,
			RegisterAddressType RxBufferDoubleDataExtendedRegAddr,
			RegisterAddressType RxBufferDoubleDataRegAddr,
			RegisterAddressType RxBufferDataExtendedPeekRegAddr,
			RegisterAddressType RxBufferDoubleDataExtendedPeekRegAddr,
			RegisterAddressType TxBufferDataExtendedRegAddr,
			RegisterAddressType TxBufferDataRegAddr,
			RegisterAddressType TxBufferDoubleDataExtendedRegAddr,
			RegisterAddressType TxBufferDoubleDataRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr,
			RegisterAddressType InterruptEnableRegAddr,
			RegisterAddressType IrDAControlRegAddr,
			RegisterAddressType IORoutingRegAddr,
			RegisterAddressType InputRegAddr,
			RegisterAddressType I2SControlRegAddr,
			unsigned int FifoSize
		>
	Uart<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr, FifoSize>::FifoType
		Uart<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr, FifoSize>::_rxFifo;
	
	template
		<
			unsigned int IdObj,
			RegisterAddressType BaseAddr,
			RegisterAddressType ControlRegisterRegAddr,
			RegisterAddressType FrameFormatRegAddr,
			RegisterAddressType TriggerControlRegAddr,
			RegisterAddressType CommandRegAddr,
			RegisterAddressType StatusAddr,
			RegisterAddressType ClockControlRegAddr,
			RegisterAddressType RxBufferDataExtendedRegAddr,
			RegisterAddressType RxBufferDataRegAddr,
			RegisterAddressType RxBufferDoubleDataExtendedRegAddr,
			RegisterAddressType RxBufferDoubleDataRegAddr,
			RegisterAddressType RxBufferDataExtendedPeekRegAddr,
			RegisterAddressType RxBufferDoubleDataExtendedPeekRegAddr,
			RegisterAddressType TxBufferDataExtendedRegAddr,
			RegisterAddressType TxBufferDataRegAddr,
			RegisterAddressType TxBufferDoubleDataExtendedRegAddr,
			RegisterAddressType TxBufferDoubleDataRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr,
			RegisterAddressType InterruptEnableRegAddr,
			RegisterAddressType IrDAControlRegAddr,
			RegisterAddressType IORoutingRegAddr,
			RegisterAddressType InputRegAddr,
			RegisterAddressType I2SControlRegAddr,
			unsigned int FifoSize
		>
	Uart<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr, FifoSize>::FifoType
		Uart<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr, FifoSize>::_txFifo;
}
}
}

#endif