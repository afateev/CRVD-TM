///////////////////////////////////////////////////////////////////////////////
//	������ UART
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
	// ��� �������� ��������
	typedef unsigned char UartRegisterDataType;
	// ��� ������ ��������
	typedef unsigned int  UartRegisterAddressType;

	// ��������������� ���������� ������
	template<unsigned int Id>
	class UartErrorCounters;
  
  ////////////////////////////////////////////////////
	// ����� ��� ���� �������� ����� UART
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
		// �������������
		static void Init(typename Hardware::DataSize dataSize, typename Hardware::Parity parity, typename Hardware::StopBits stopBits, long baudRate, typename Hardware::ClockSource source, long frequency)
		{
			// ��������� ���������� ����� � �����������
			Hardware::OnRxCallback = Fifo::OnRx;
			Hardware::OnTxCallback = Fifo::OnTx;
			Hardware::OnReadCallback = Fifo::OnRead;
			Hardware::OnWriteCallback = Fifo::OnWrite;
			Hardware::OnOverrunCallback = ErrorCounters::OnHardwareInputBufferOverflow;
            Hardware::OnOutputBufferEmptyCallback = Fifo::OutputBufferEmpty;
            
			// �������������� ���������� �����
			Hardware::Init(dataSize, parity, stopBits, baudRate, source, frequency);
		}
	};
	
	///////////////////////////////////////////////////////////
	// ������ ���������� ����� UART ����� ��� ���� ��������
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
				// ������
				Control0RegisterAddr, Control1RegisterAddr,
				// ���������
				UsciControlRegister0Uart<UartRegisterDataType>, UsciControlRegister1Uart<UartRegisterDataType>
			>,
			UsciBaudRateControl< UartRegisterDataType, UartRegisterAddressType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>,
			UsciModulationControl< UartRegisterDataType, UartRegisterAddressType,
				// �����
				ModulationControlRegisterAddr,
				// ���������
				UsciModulationControlRegister<UartRegisterDataType>
			>,
			UsciStatus< UartRegisterDataType, UartRegisterAddressType,
				// �����
				StatusRegisterAddr,
				// ���������
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
	// ����� ������� Uart
	////////////////////////////////////////////////////

	template<unsigned int Id, class InputBuffer, class OutputBuffer>
	class UartFifo :
		public IdObjectBase<Id>
	{
	protected:
		RBLIB_NO_INIT static InputBuffer RxBuffer;
		RBLIB_NO_INIT static OutputBuffer TxBuffer;
#ifndef RBLIB_BOOTLOADER
		static bool _notifyRxTxStart;	// ���������� ����������� � ������ ������ / �������� ������
		static bool _notifyTxFlag;		// ���� "����� � ��������� ��� ��������� ����������� � ������ ��������"
#endif // RBLIB_BOOTLOADER
	public:
        static inline bool OutputBufferEmpty()
        {
            return TxBuffer.empty();
        }
		// �����
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
		// �������� (���� ���� ������ �� ��������, ����������� data � ������������ true)
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
				// ������ �� ���� ���������� �����������, ���� ����� ��������� �� ����� ���������
				_notifyTxFlag = false;
				// ���������� �����������
				SendTxNotifyEvent();
			}
#endif // RBLIB_BOOTLOADER
			data = TxBuffer.front();
			TxBuffer.pop();
			return true;
		}
		// ������ (����������� ������ �� ��������� ������)
		static inline unsigned int OnRead(unsigned char *data, unsigned int len)
		{
			if (RxBuffer.empty())
				return 0;
			// ������� ���� ������
			unsigned int remain = RxBuffer.size();
			// ����� ���������� �� ������, ��� ���� ������ � ������
			unsigned int copy = remain >= len ? len : remain;
			// ��������
			for (unsigned int i = 0; i < copy; i++)
			{
				data[i] = RxBuffer.front();
				RxBuffer.pop();
			}
			return copy;
		}
		// �������� (����������� ������ � ����� �� ��������)
		static inline unsigned int OnWrite(unsigned char *data, unsigned int len)
		{
			// ������� ���� ����� � ������
			unsigned int remain = TxBuffer.Capacity - TxBuffer.size();
			// ���� ��� �����, �� ������ ������
			if (remain < 1)
				return 0;
			// ����� ���������� �� ������, ��� ���� ����� � ������
			unsigned int copy = remain >= len ? len : remain;
			// ��������
			for (unsigned int i = 0; i < copy; i++)
			{
				TxBuffer.push(data[i]);
			}
			// ������� ���������� �������� � �����
			return copy;
		}
#ifndef RBLIB_BOOTLOADER
		// �������� ����������� � ������ ������ / �������� ������
		static void EnableReceiveTrasmitStartEvents(bool enable = true)
		{
			_notifyRxTxStart = enable;
		}
#endif // RBLIB_BOOTLOADER
	protected:
#ifndef RBLIB_BOOTLOADER
		// ��������� ����������� � ������ ������ ������
		static void SendRxNotifyEvent()
		{
			Event e(EventUartReceiveStartNotify, Id);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
		}
		// ��������� ����������� � ������ �������� ������
		static void SendTxNotifyEvent()
		{
			Event e(EventUartTransmitStartNotify, Id);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
		}
#endif // RBLIB_BOOTLOADER
	};

	// ��������������� ����������� ����������
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
	// ����� �������� FIFO ��� Uart'�
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
			
			// �.�. _head � _tail volatile, �� � ����� �� ��� �������� ����� ��������� ����������
			UartFastFifoCounterType tmp = _head;
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

		inline UartFastFifoDataType &front()
		{
			// �� ����� ���� ������� ��������� �� ���������� �� �����
			UartFastFifoCounterType tmp = _tail;
			tmp++;
			if (tmp == BufSize)
				tmp = 0;
			return _buf[tmp];
		}

		inline void pop()
		{
			// �.�. _head � _tail volatile, �� � ����� �� ��� �������� ����� ��������� ����������
			UartFastFifoCounterType tmp = _head;
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
			UartFastFifoCounterType tmp = _head;
		  	return tmp == _tail;
		}

		inline UartFastFifoCounterType size()
		{
			// �.�. _head � _tail volatile, �� � ����� �� ��� �������� ����� ��������� ����������
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
	
	// ����� ��������� ������ Uart
	template<unsigned int Id>
	class UartErrorCounters :
		public IdObjectBase<Id>
	{
	public:
		typedef unsigned long CounterType;
	protected:
		// ������� ������������ �������� ����������� ������
		static volatile CounterType _hardwareInputBufferOverflowCount;
		// ������� ������������ �������� ����������� ������
		static volatile CounterType _softwareInputBufferOverflowCount;
	
	protected:
		static inline void OnHardwareInputBufferOverflow()
		{
			_hardwareInputBufferOverflowCount++;
		}
	public:
		// �������� ������� ������������ �������� ����������� ������
		static CounterType GetHardwareInputBufferOverflowCount()
		{
			return _hardwareInputBufferOverflowCount;
		}
		// �������� ������� ������������ �������� ����������� ������
		static void ClearHardwareInputBufferOverflowCount()
		{
			_hardwareInputBufferOverflowCount = 0;
		}

		// �������� ������� ������������ �������� ������������ ������
		static CounterType GetSoftwareInputBufferOverflowCount()
		{
			return _softwareInputBufferOverflowCount;
		}
		// �������� ������� ������������ �������� ������������ ������
		static void ClearSoftwareInputBufferOverflowCount()
		{
			_softwareInputBufferOverflowCount = 0;
		}
	};

	// ��������������� ����������� ����������
	template<unsigned int Id>
	volatile typename UartErrorCounters<Id>::CounterType UartErrorCounters<Id>::_hardwareInputBufferOverflowCount = 0;
	template<unsigned int Id>
	volatile typename UartErrorCounters<Id>::CounterType UartErrorCounters<Id>::_softwareInputBufferOverflowCount = 0;
}
}

#endif