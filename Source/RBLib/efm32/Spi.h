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
		static volatile BufferType *_src;			// откуда брать данные
		static volatile BufferType *_dst;			// куда складывать данные
		static volatile unsigned int _txPos;		// позиция в буфере на отправку
		static volatile unsigned int _rxPos;		// позиция в буфере на прием
		static unsigned int _bytesCount;			// сколько требуется отправить / принять
		static OnCompleteCallbackType *_onCompleteCallback;
	public:
		// Инициализация
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
		
		// Отправка данных с одновременным чтением в буфер dst
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
			
			// включаем прерывания, процесс пошел
			InterruptEnableRegister::InterruptEnable(InterruptEnableRegister::InterruptTxBufferLevel);
			
			// если не передали адрес на функцию обратного вызова, значит синхронная передача
			if (0 == _onCompleteCallback)
			{
				int j = 0;
				while(_rxPos < _bytesCount)
				{
					j++;
				}
			}
		}
		
		// Отправка данных с одновременным чтением в буфер dst без использования прерываний
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
				// ждём флага освобождения приемного буфера
				while(!InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptTxBufferLevel))
				{
					;
				}
				// помещаем данные на отправку
				unsigned char data = 0;
				if (OnTx(&data))
				{
					TxBufferDataRegister::Write(data);
				}
				// ждём флага готовности входящих данных, что также озночает успешную отправку байта
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
				unsigned int rem = _txFifo.Capacity - _txFifo.size();
				// если нет места, то нечего делать
				if (rem < 1)
					continue;
				// будем копировать не больше, чем есть места в буфере
				unsigned int copy = rem >= length ? length : rem;
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
				// пока тормозим отправку чтобы успеть прочитать
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
				
				// приняли сколько надо
				if (_rxPos >= _bytesCount)
				{
					// сообщим об этом
					if (_onCompleteCallback)
					{
						_onCompleteCallback();
					}
					// сбросим состояние
					ResetBuffers();
				}
				else
				{
					if (enableInterrupt)
					{
						// есть что читать, а чтобы прочитать надо что-то отправить
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
					// отправляем байт
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