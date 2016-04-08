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
	// ����� �������� FIFO ��� Uart'�
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
			
			// �.�. _head � _tail volatile, �� � ����� �� ��� �������� ����� ��������� ����������
			FastFifoCounterType tmp = _head;
	   		// ���� ����� ������������
			if (tmp == _tail)
     		{
				// ������ ���� ���� � ������
				_tail++;
				if (_tail == BufSize)
					_tail = 0;
				// TODO ������� ������������
    		}
			_buf[_head] = data;
		}

		inline FastFifoDataType &front()
		{
			// �� ����� ���� ������� ��������� �� ���������� �� �����
			FastFifoCounterType tmp = _tail;
			tmp++;
			if (tmp == BufSize)
				tmp = 0;
			return _buf[tmp];
		}

		inline void pop()
		{
			// �.�. _head � _tail volatile, �� � ����� �� ��� �������� ����� ��������� ����������
			FastFifoCounterType tmp = _head;
		  	// ���������, ��� �� �����
			if (tmp == _tail)
				return;

			_tail++;
			if (_tail == BufSize)
				_tail = 0;
		}

		inline bool empty() const
		{
			// �.�. _head � _tail volatile, �� � ����� �� ��� �������� ����� ��������� ����������
			FastFifoCounterType tmp = _head;
		  	return tmp == _tail;
		}

		inline FastFifoCounterType size()
		{
			// �.�. _head � _tail volatile, �� � ����� �� ��� �������� ����� ��������� ����������
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
	
	// ��� �������� ��������
	typedef Usart::ValueDataType ValueDataType;
	// ��� ������ ��������
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
		// �������������
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
		
		// ������ ������ �� �����
		static unsigned int Read(unsigned char* data, int length)
		{
			if (0 == data || length < 1)
			{
				return 0;
			}
			
			InterruptEnableRegister::InterruptDisable(InterruptEnableRegister::InterruptRxDataValid);
			
			// ������� ���� ������
			unsigned int remain = _rxFifo.size();
			// ����� ���������� �� ������, ��� ���� ������ � ������
			unsigned int copy = remain >= length ? length : remain;
			
			// ��������
			for (unsigned int i = 0; i < copy; i++)
			{
				data[i] = _rxFifo.front();
				_rxFifo.pop();
			}
			
			InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptRxDataValid);
			
			return copy;
		}
		
		// ������ ������ � ����
		static void Write(unsigned char* data, int length)
		{
			if (0 == data || length < 1)
			{
				return;
			}
			
			unsigned int remain = length;	// ������� ����� ������ ���������� � ����� �� ��������
			
			// ��� ���� ��� ������ �� ����� �������� � ����� �� ��������
			while (remain > 0)
			{
				// ��� ������������ ��������� ���������� �� �������
				InterruptEnableRegister::InterruptDisable(InterruptEnableRegister::InterruptTxBufferLevel);
				// �������� ������� �������� ������ ��������� � ����� �� ��������
				// ������� ���� ����� � ������
				unsigned int free = _txFifo.Capacity - _txFifo.size();
				// ���� ��� �����, �� ������ ������
				if (free < 1)
				{
					// �������� ���������� �� ��������
					InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptTxBufferLevel);
					continue;
				}
				// ����� ���������� �� ������, ��� ���� ����� � ������
				unsigned int copy = free >= remain ? remain : free;
				// ��������
				for (unsigned int i = 0; i < copy; i++)
				{
					_txFifo.push(data[i]);
				}
				// ������� ��������� �� �������� ������
				data += copy;
				// ��������� ������� �������� �����������
				remain -= copy;
				// �������� ���������� �� ��������
				InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptTxBufferLevel);
			}
			/*
			// ��������� ���������� �������� (�� ��� �� �������)
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