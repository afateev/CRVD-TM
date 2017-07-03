///////////////////////////////////////////////////////////////////////////////
//	 ласс управлени€ портами USART
//	ATMEGA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef USART_ATMEGA_H
#define USART_ATMEGA_H

#include "../IdObjectBase.h"
#include "../RegisterReference.h"

namespace Lib
{
namespace ATmega
{
namespace Usart
{
	// “ип значени€ порта
	typedef unsigned char ValueDataType;
	
	// UDRn Ц USART I/O Data Register n
	template
		<
			RegisterPointerType RegisterAddr
		>
	class IODataRegister
	{
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void WriteData(unsigned char val)
		{
			*RegRef::_register = val;
		}
		
		static unsigned char ReadData()
		{
			return *RegRef::_register;
		}
	};
	
	// UCSRnA Ц USART Control and Status Register A
	template
		<
			RegisterPointerType RegisterAddr
		>
	class ControlAndStatusARegister
	{
	public:
		struct RegisterStruct
		{
			ValueDataType MultiprocessorCommunicationMode 	: 1;
			ValueDataType DoubleTransmissionSpeed 			: 1;
			ValueDataType ParityError 						: 1;
			ValueDataType DataOverRun 						: 1;
			ValueDataType FrameError 						: 1;
			ValueDataType DataRegisterEmpty 				: 1;
			ValueDataType TransmitComplete 					: 1;
			ValueDataType ReceiveComplete 					: 1;
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static bool IsParityError()
		{
			return RegRef::_register->ParityError;
		}
		
		static bool IsDataOverRun()
		{
			return RegRef::_register->DataOverRun;
		}
		
		static bool IsFrameError()
		{
			return RegRef::_register->FrameError;
		}
		
		static bool IsDataRegisterEmpty()
		{
			return RegRef::_register->DataRegisterEmpty;
		}
		
		static bool IsTransmitComplete()
		{
			return RegRef::_register->TransmitComplete;
		}
		
		static bool IsReceiveComplete()
		{
			return RegRef::_register->ReceiveComplete;
		}
	};
	
	// UCSRnA Ц USART Control and Status Register B
	template
		<
			RegisterPointerType RegisterAddr
		>
	class ControlAndStatusBRegister
	{
	public:
		struct RegisterStruct
		{
			ValueDataType TransmitDataBit8 					: 1;
			ValueDataType ReceiveDataBit8 					: 1;
			ValueDataType CharacterSizeBit2					: 1;
			ValueDataType TransmitterEnable				 	: 1;
			ValueDataType ReceiverEnable 					: 1;
			ValueDataType DataRegisterEmptyInterruptEnable 	: 1;
			ValueDataType TxCompleteInterruptEnable 		: 1;
			ValueDataType RxCompleteInterruptEnable 		: 1;
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void TransmitterEnable(bool enable = true)
		{
			RegRef::_register->TransmitterEnable = enable;
		}
		
		static void ReceiverEnable(bool enable = true)
		{
			RegRef::_register->ReceiverEnable = enable;
		}
		
		static void DataRegisterEmptyInterruptEnable(bool enable = true)
		{
			RegRef::_register->DataRegisterEmptyInterruptEnable = enable;
		}
		
		static void TxCompleteInterruptEnable(bool enable = true)
		{
			RegRef::_register->TxCompleteInterruptEnable = enable;
		}
		
		static void RxCompleteInterruptEnable(bool enable = true)
		{
			RegRef::_register->RxCompleteInterruptEnable = enable;
		}
	};
	
	// UCSRnA Ц USART Control and Status Register C
	template
		<
			RegisterPointerType RegisterAddr
		>
	class ControlAndStatusCRegister
	{
	public:
		struct RegisterStruct
		{
			ValueDataType ClockPolarity 	: 1;
			ValueDataType CharacterSize 	: 2;
			ValueDataType StopBitSelect		: 1;
			ValueDataType ParityMode	 	: 2;
			ValueDataType ModeSelect 		: 2;
		};
		
		enum CharacterSize
		{
			CharacterSize5	=	0,
			CharacterSize6	=	1,
			CharacterSize7	=	2,
			CharacterSize8	=	3,
			CharacterSize9	=	7
		};
		
		enum StopBits
		{
			StopBits1	=	0,
			StopBits2	=	1
		};
		
		enum ParityMode
		{
			ParityDisabled	=	0,
			ParityEven		=	2,
			ParityOdd		=	3,
		};
		
		enum Mode
		{
			ModeAsynchronous	=	0,
			ModeSynchronous		=	1,
			ModeMasterSpi		=	3
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void SetClockPolarity(bool val)
		{
			RegRef::_register->ClockPolarity = val;
		}
		
		static void SetCharacterSize(CharacterSize val)
		{
			RegRef::_register->CharacterSize = val;
		}
		
		static void SetStopBits(StopBits val)
		{
			RegRef::_register->StopBitSelect = val;
		}
		
		static void SetParityMode(ParityMode val)
		{
			RegRef::_register->ParityMode = val;
		}
		
		static void SetMode(Mode val)
		{
			RegRef::_register->ModeSelect = val;
		}
	};
	
	// UBRRnL Ц USART Baud Rate Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class BaudRateLRegister
	{
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void SetBaudRateL(unsigned char val)
		{
			*RegRef::_register = val;
		}
	};
	
	// UUBRRnH Ц USART Baud Rate Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class BaudRateHRegister
	{
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void SetBaudRateH(unsigned char val)
		{
			*RegRef::_register = val & 0x0F;
		}
	};
	
	template
		<
			unsigned int IdObj,
			RegisterPointerType IODataRegisterAddr,
			RegisterPointerType ControlAndStatusARegisterAddr,
			RegisterPointerType ControlAndStatusBRegisterAddr,
			RegisterPointerType ControlAndStatusCRegisterAddr,
			RegisterPointerType BaudRateLRegisterAddr,
			RegisterPointerType BaudRateHRegisterAddr
		>
	class Usart :
		public IODataRegister<IODataRegisterAddr>,
		public ControlAndStatusARegister<ControlAndStatusARegisterAddr>,
		public ControlAndStatusBRegister<ControlAndStatusBRegisterAddr>,
		public ControlAndStatusCRegister<ControlAndStatusCRegisterAddr>,
		public BaudRateLRegister<BaudRateLRegisterAddr>,
		public BaudRateHRegister<BaudRateHRegisterAddr>
	{
	public:
		typedef IODataRegister<IODataRegisterAddr> IODataRegister;
		typedef ControlAndStatusARegister<ControlAndStatusARegisterAddr> ControlAndStatusARegister;
		typedef ControlAndStatusBRegister<ControlAndStatusBRegisterAddr> ControlAndStatusBRegister;
		typedef ControlAndStatusCRegister<ControlAndStatusCRegisterAddr> ControlAndStatusCRegister;
		typedef BaudRateLRegister<BaudRateLRegisterAddr> BaudRateLRegister;
		typedef BaudRateHRegister<BaudRateHRegisterAddr> BaudRateHRegister;
		
		static unsigned char *_src;
		static unsigned int _srcPos;
		static unsigned int _txDataCount;
		static unsigned int _txCompleteCount;
	public:
		static void StartAsyncMode(unsigned long fOsc, unsigned long baudrate, typename ControlAndStatusCRegister::StopBits stopBits, typename ControlAndStatusCRegister::ParityMode parity, bool enableRx, bool enableTx)
		{
			ControlAndStatusCRegister::SetClockPolarity(false);
			ControlAndStatusCRegister::SetCharacterSize(ControlAndStatusCRegister::CharacterSize8);
			ControlAndStatusCRegister::SetStopBits(stopBits);
			ControlAndStatusCRegister::SetParityMode(parity);
			ControlAndStatusCRegister::SetMode(ControlAndStatusCRegister::ModeAsynchronous);
			
			unsigned long baudrateRegValue = fOsc;
			baudrateRegValue /= 16;
			if (baudrate > 0)
			{
				baudrateRegValue /= baudrate;
			}
			baudrateRegValue -= 1;
			
			BaudRateLRegister::SetBaudRateL(baudrateRegValue & 0xFF);
			BaudRateHRegister::SetBaudRateH((baudrateRegValue >> 8) & 0x0F);
			ControlAndStatusBRegister::RxCompleteInterruptEnable(enableRx);
			//ControlAndStatusBRegister::TxCompleteInterruptEnable(enableTx);
			//ControlAndStatusBRegister::DataRegisterEmptyInterruptEnable(enableTx);
			ControlAndStatusBRegister::ReceiverEnable(enableRx);
			ControlAndStatusBRegister::TransmitterEnable(enableTx);
		}
		
		static void Stop()
		{
			ControlAndStatusBRegister::TransmitterEnable(false);
			ControlAndStatusBRegister::ReceiverEnable(false);
			ControlAndStatusBRegister::DataRegisterEmptyInterruptEnable(false);
			ControlAndStatusBRegister::TxCompleteInterruptEnable(false);
			ControlAndStatusBRegister::RxCompleteInterruptEnable(false);
		}
		
		static void Write(void *data, unsigned char dataCount)
		{
			if (data == 0 || dataCount < 1)
			{
				return;
			}
			
			_src = (unsigned char *)data;
			_srcPos = 0;
			_txDataCount = dataCount;
			_txCompleteCount = 0;
			ControlAndStatusBRegister::TxCompleteInterruptEnable(true);
			ControlAndStatusBRegister::DataRegisterEmptyInterruptEnable(true);
			
			int j = 0;
			while (_txCompleteCount < 1)
			{
				j++;
			}
			
			ControlAndStatusBRegister::TxCompleteInterruptEnable(false);
			_src = 0;
			_txDataCount = 0;
		}
		
		static void OnDataRegisterEmptyInterrupt()
		{
			IODataRegister::WriteData(_src[_srcPos++]);
			if(_srcPos >= _txDataCount)
			{
				ControlAndStatusBRegister::DataRegisterEmptyInterruptEnable(false);
			}
		}
		
		static void OnTxCompleteInterrupt()
		{
			_txCompleteCount++;
		}
		
		static int Read(void *buffer, int bufferSize)
		{
			return 0;
		}
	};
	
	template <unsigned int IdObj, RegisterPointerType IODataRegisterAddr, RegisterPointerType ControlAndStatusARegisterAddr, RegisterPointerType ControlAndStatusBRegisterAddr, RegisterPointerType ControlAndStatusCRegisterAddr, RegisterPointerType BaudRateLRegisterAddr, RegisterPointerType BaudRateHRegisterAddr>
		unsigned char *Usart<IdObj, IODataRegisterAddr, ControlAndStatusARegisterAddr, ControlAndStatusBRegisterAddr, ControlAndStatusCRegisterAddr, BaudRateLRegisterAddr, BaudRateHRegisterAddr>::_src = 0;
	
	template <unsigned int IdObj, RegisterPointerType IODataRegisterAddr, RegisterPointerType ControlAndStatusARegisterAddr, RegisterPointerType ControlAndStatusBRegisterAddr, RegisterPointerType ControlAndStatusCRegisterAddr, RegisterPointerType BaudRateLRegisterAddr, RegisterPointerType BaudRateHRegisterAddr>
		unsigned int Usart<IdObj, IODataRegisterAddr, ControlAndStatusARegisterAddr, ControlAndStatusBRegisterAddr, ControlAndStatusCRegisterAddr, BaudRateLRegisterAddr, BaudRateHRegisterAddr>::_srcPos = 0;
	
	template <unsigned int IdObj, RegisterPointerType IODataRegisterAddr, RegisterPointerType ControlAndStatusARegisterAddr, RegisterPointerType ControlAndStatusBRegisterAddr, RegisterPointerType ControlAndStatusCRegisterAddr, RegisterPointerType BaudRateLRegisterAddr, RegisterPointerType BaudRateHRegisterAddr>
		unsigned int Usart<IdObj, IODataRegisterAddr, ControlAndStatusARegisterAddr, ControlAndStatusBRegisterAddr, ControlAndStatusCRegisterAddr, BaudRateLRegisterAddr, BaudRateHRegisterAddr>::_txDataCount = 0;
	
	template <unsigned int IdObj, RegisterPointerType IODataRegisterAddr, RegisterPointerType ControlAndStatusARegisterAddr, RegisterPointerType ControlAndStatusBRegisterAddr, RegisterPointerType ControlAndStatusCRegisterAddr, RegisterPointerType BaudRateLRegisterAddr, RegisterPointerType BaudRateHRegisterAddr>
		unsigned int Usart<IdObj, IODataRegisterAddr, ControlAndStatusARegisterAddr, ControlAndStatusBRegisterAddr, ControlAndStatusCRegisterAddr, BaudRateLRegisterAddr, BaudRateHRegisterAddr>::_txCompleteCount = 0;
}
}
}

#endif