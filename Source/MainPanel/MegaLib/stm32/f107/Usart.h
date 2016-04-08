////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		03/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef USART_STM32_F107_H
#define USART_STM32_F107_H

#include "UsartParts.h"

template<unsigned int BaseAddr>
class UsartHardwareBase:
	public UsartStatusRegister		<BaseAddr + 0x00>,
	public UsartDataRegister		<BaseAddr + 0x04>,
	public UsartBaudRateRegister	<BaseAddr + 0x08>,
	public UsartControl1Register	<BaseAddr + 0x0C>
{
};

template<unsigned int BaseAddr>
class UsartHardwareImpl:
	public UsartHardwareBase<BaseAddr>
{
public:
	typedef UsartHardwareBase<BaseAddr> Base;
	
	typedef void (OnRxCallbackType)(unsigned char data);
	typedef bool (OnTxCallbackType)(unsigned char &data);
	
	static OnRxCallbackType *OnRxCallback;
	static OnTxCallbackType *OnTxCallback;
public:
	inline static void InterruptHandler()
	{
		if (Base::IsRxNotEmpty() && Base::IsRxInterruptEnabled())
		{
			unsigned char data = Base::ReadData();
			if (OnRxCallback)
			{
				OnRxCallback(data);
			}
		}
		if (Base::IsTxEmpty() && Base::IsTxInterruptEnabled())
		{
			if (OnTxCallback)
			{
				unsigned char data;
				if (OnTxCallback(data))
				{
					Base::WriteData(data);
				}
				else
				{
					Base::TxInterruptEnable(false);
				}
			}
			else
			{
				Base::TxInterruptEnable(false);
			}
		}
	}
};

template<unsigned int BaseAddr>
typename UsartHardwareImpl<BaseAddr>::OnRxCallbackType *
UsartHardwareImpl<BaseAddr>::OnRxCallback = 0;

template<unsigned int BaseAddr>
typename UsartHardwareImpl<BaseAddr>::OnTxCallbackType *
UsartHardwareImpl<BaseAddr>::OnTxCallback = 0;


class UsartHardware
{
public:
	typedef UsartHardwareImpl<0x40013800> _A;
	typedef UsartHardwareImpl<0x40004400> _B;
	typedef UsartHardwareImpl<0x40004800> _C;
	typedef UsartHardwareImpl<0x40004C00> _D;
	typedef UsartHardwareImpl<0x40005000> _E;
};


template<class Hardware, class BufferType>
class UsartWrapper
{
protected:
	static BufferType RxBuffer;
	static BufferType TxBuffer;

	typedef void (CallbackType)(void);
	static CallbackType *OnNewChar;
	static CallbackType *OnTxEnd;
	
public:
	inline static void Init(unsigned long CoreFrequency, unsigned int baudrate)
	{
		Hardware::OnRxCallback = OnRx;
		Hardware::OnTxCallback = OnTx;
		
		if (baudrate == 0)
			baudrate = 1;
		
		float d = CoreFrequency;
		d /= 16.0;
		d /= (float)baudrate;
		unsigned short mantissa = (int)d;
		d -= mantissa;
		d *= 16.0;
		unsigned char fraction = (int)d;
		
		Hardware::SetBaudRate(mantissa, fraction);
		Hardware::Enable();
		Hardware::RxInterruptEnable();
	}
	
	inline static void ParityControlEnable(bool enable = true)
	{
		Hardware::ParityControlEnable(enable);
	}
	
	inline static void ParitySelection(bool odd)
	{
		// true = odd
		// false = even
		Hardware::ParitySelection(odd);
	}
	
	inline static void OnRx(unsigned char data)
	{
		RxBuffer.push(data);
		if (OnNewChar)
			OnNewChar();
	}
	
	inline static bool OnTx(unsigned char &data)
	{
		if (TxBuffer.empty())
		{
			if (OnTxEnd)
				OnTxEnd();
			return false;
		}
		data = TxBuffer.front();
		TxBuffer.pop();
		return true;
	}
	
	// Чтение (копирование данных из приемного буфера)
	static inline unsigned int Read(unsigned char *data, unsigned int len)
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
	static inline unsigned int Write(unsigned char *data, unsigned int len)
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
		
		Hardware::TxInterruptEnable(true);
		
		// сколько фактически помещено в буфер
		return copy;
	}
	
	inline static void SetOnNewCharCallback(CallbackType *callback)
	{
		OnNewChar = callback;
	}
	
	inline static void SetOnTxEndCallback(CallbackType *callback)
	{
		OnTxEnd = callback;
	}
};

template<class Hardware, class BufferType>
BufferType UsartWrapper<Hardware, BufferType>::RxBuffer;

template<class Hardware, class BufferType>
BufferType UsartWrapper<Hardware, BufferType>::TxBuffer;

template<class Hardware, class BufferType>
typename UsartWrapper<Hardware, BufferType>::CallbackType *
UsartWrapper<Hardware, BufferType>::OnNewChar = 0;

template<class Hardware, class BufferType>
typename UsartWrapper<Hardware, BufferType>::CallbackType *
UsartWrapper<Hardware, BufferType>::OnTxEnd = 0;

typedef unsigned short UartFastFifoCounterType;
typedef unsigned char UartFastFifoDataType;

template<UartFastFifoCounterType BufSize, unsigned int Id>
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

class Usart
{
public:
	typedef UsartWrapper<UsartHardware::_A, UartFastFifo<255, 1> > _A;
	typedef UsartWrapper<UsartHardware::_B, UartFastFifo<255, 2> > _B;
	typedef UsartWrapper<UsartHardware::_C, UartFastFifo<255, 3> > _C;
	typedef UsartWrapper<UsartHardware::_D, UartFastFifo<255, 4> > _D;
	typedef UsartWrapper<UsartHardware::_E, UartFastFifo<255, 5> > _E;
};

#endif