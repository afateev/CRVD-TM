#ifndef SPI_EMF32_H
#define SPI_EMF32_H

#include "../IdObjectBase.h"
#include "Usart.h"

namespace Rblib
{
namespace Efm32
{
namespace Spi
{
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
			RegisterAddressType I2SControlRegAddr
		>
	class Master :
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
		
		typedef void (OnCompleteCallbackType)();
	protected:
		typedef unsigned char BufferType;
		static volatile BufferType *_src;			// ������ ����� ������
		static volatile BufferType *_dst;			// ���� ���������� ������
		static volatile unsigned int _txPos;		// ������� � ������ �� ��������
		static volatile unsigned int _rxPos;		// ������� � ������ �� �����
		static unsigned int _bytesCount;			// ������� ��������� ��������� / �������
		static OnCompleteCallbackType *_onCompleteCallback;
	public:
		// �������������
    	static void Init(DataSize dataSize, bool msbFirst, unsigned long bitRate, unsigned long clockSourcefrequency, bool clockPolarity, bool clockPhase)
		{
			if (bitRate < 1)
			{
				return;
			}
			
			double divider = clockSourcefrequency;
			divider /= 2.0;
			
			divider /= bitRate;
			
			if (divider < 1)
			{
				return;
			}
			
			divider -= 1;
			divider *= 256;
			
			ControlRegister::SetSynchronousMode(true);
			ControlRegister::SetMostSignificantBitFirst(msbFirst);
			ControlRegister::SetClockPolarity(clockPolarity);
			ControlRegister::SetClockEdge(clockPhase);
			
			FrameFormatRegister::SetDataBitMode(dataSize);
			
			ClockControlRegister::SetClockDivider(divider);
			
			CommandRegister::Do(CommandRegister::CommandReceiverEnable);
			CommandRegister::Do(CommandRegister::CommandTransmitterEnable);
			CommandRegister::Do(CommandRegister::CommandMasterEnable);
			
			//InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptTxBufferLevel);
			//InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptRxDataValid);
		}
		
		// �������� ������ � ������������� ������� � ����� dst
		static void Write(unsigned char* src, unsigned char* dst, int length, OnCompleteCallbackType *onCompleteCallback = 0)
		{
			ResetBuffers();
			InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptRxDataValid);
			
			if (0 == src || 0 == dst || 0 == length)
			{
				return;
			}
			
			_src = src;
			_dst = dst;
			_bytesCount = length;
			_onCompleteCallback = onCompleteCallback;
			
			// �������� ����������, ������� �����
			InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptTxBufferLevel);
			
			// ���� �� �������� ����� �� ������� ��������� ������, ������ ���������� ��������
			if (0 == _onCompleteCallback)
			{
				int j = 0;
				while(_rxPos < _bytesCount)
				{
					j++;
				}
			}
		}
		
		// �������� ������ � ������������� ������� � ����� dst ��� ������������� ����������
		static void WriteWithoutInterrupts(unsigned char* src, unsigned char* dst, int length)
		{
			ResetBuffers();
			InterruptEnableRegister::InterruptDisable(InterruptEnableRegister::InterruptRxDataValid);
			
			if (0 == src || 0 == dst || 0 == length)
			{
				return;
			}
			
			_src = src;
			_dst = dst;
			_bytesCount = length;
			_onCompleteCallback = 0;

			while(_rxPos < _bytesCount)
			{
				// ��� ����� ������������ ��������� ������
				while(!InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptTxBufferLevel))
				{
					;
				}
				// �������� ������ �� ��������
				unsigned char data = 0;
				if (OnTx(&data))
				{
					TxBufferDataRegister::Write(data);
				}
				// ��� ����� ���������� �������� ������, ��� ����� �������� �������� �������� �����
				while (!InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptRxDataValid))
				{
					;
				}
				data = 0;
				
				RxBufferDataRegister::Read(&data);
				
				OnRx(data, false);
			}
		}
		
		
		/*
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
				unsigned int rem = _txFifo.Capacity - _txFifo.size();
				// ���� ��� �����, �� ������ ������
				if (rem < 1)
					continue;
				// ����� ���������� �� ������, ��� ���� ����� � ������
				unsigned int copy = rem >= length ? length : rem;
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
		}
		*/
		
		static void SetIORouting(Location location, bool rxPinEnable = true, bool txPinEnable = true, bool clkPinEnable = true)
		{
			IORoutingRegister::SetPinEnable(rxPinEnable, txPinEnable, false, clkPinEnable);
			IORoutingRegister::SetIOLocation(location);
		}
		
		static void RxInterruptHandler()
		{
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptRxDataValid))
			{
				unsigned char data = 0;
				
				RxBufferDataRegister::Read(&data);
				
				OnRx(data);
			}
		}
		
		static void TxInterruptHandler()
		{
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptTxBufferLevel))
			{
				// ���� �������� �������� ����� ������ ���������
				InterruptEnableRegister::InterruptDisable(InterruptEnableRegister::InterruptTxBufferLevel);
				unsigned char data = 0;
				if (OnTx(&data))
				{
					TxBufferDataRegister::Write(data);
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
		static void ResetBuffers()
		{
			_src = 0;
			_dst = 0;
			_txPos = 0;
			_rxPos = 0;
			_bytesCount = 0;
			_onCompleteCallback = 0;
		}
		
		static void OnRx(unsigned char data, bool enableInterrupt = true)
		{
			if (0 != _dst)
			{
				_dst[_rxPos] = data;
				_rxPos++;
				
				// ������� ������� ����
				if (_rxPos >= _bytesCount)
				{
					// ������� �� ����
					if (_onCompleteCallback)
					{
						_onCompleteCallback();
					}
					// ������� ���������
					ResetBuffers();
				}
				else
				{
					if (enableInterrupt)
					{
						// ���� ��� ������, � ����� ��������� ���� ���-�� ���������
						InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptTxBufferLevel);
					}
				}
			}
		}
		
		static bool OnTx(unsigned char *data)
		{
			if (0 != _src)
			{
				if (_txPos < _bytesCount)
				{
					// ���������� ����
					if (data)
					{
						*data = _src[_txPos];
					}
					_txPos++;
					return true;
				}
			}
			return false;
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
			RegisterAddressType I2SControlRegAddr
		>
	volatile Master<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr>::BufferType *
		Master<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr>::_src;
	
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
			RegisterAddressType I2SControlRegAddr
		>
	volatile Master<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr>::BufferType *
		Master<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr>::_dst;
	
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
			RegisterAddressType I2SControlRegAddr
		>
	volatile unsigned int Master<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr>::_txPos;
	
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
			RegisterAddressType I2SControlRegAddr
		>
	volatile unsigned int Master<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr>::_rxPos;
	
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
			RegisterAddressType I2SControlRegAddr
		>
	unsigned int Master<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr>::_bytesCount;
	
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
			RegisterAddressType I2SControlRegAddr
		>
	Master<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr>::OnCompleteCallbackType *
		Master<IdObj, BaseAddr, ControlRegisterRegAddr, FrameFormatRegAddr, TriggerControlRegAddr, CommandRegAddr, StatusAddr, ClockControlRegAddr, RxBufferDataExtendedRegAddr, RxBufferDataRegAddr, RxBufferDoubleDataExtendedRegAddr,
			 RxBufferDoubleDataRegAddr, RxBufferDataExtendedPeekRegAddr, RxBufferDoubleDataExtendedPeekRegAddr, TxBufferDataExtendedRegAddr, TxBufferDataRegAddr, TxBufferDoubleDataExtendedRegAddr, TxBufferDoubleDataRegAddr,
			 InterruptFlagRegAddr, InterruptFlagSetRegAddr, InterruptFlagClearRegAddr, InterruptEnableRegAddr, IrDAControlRegAddr, IORoutingRegAddr, InputRegAddr, I2SControlRegAddr>::_onCompleteCallback;
}
}
}

#endif