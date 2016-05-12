#ifndef USART_STM32_F4_H
#define USART_STM32_F4_H

#include "Register.h"
#include "../../IdObjectBase.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Stm32
{
namespace F4
{
namespace UsartImplementation
{
	// Status register (USART_SR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class StatusRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x00;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType PE			: 1;
			Core::RegisterValueType FE			: 1;
			Core::RegisterValueType NF			: 1;
			Core::RegisterValueType ORE			: 1;
			Core::RegisterValueType IDLE		: 1;
			Core::RegisterValueType RXNE		: 1;
			Core::RegisterValueType TC			: 1;
			Core::RegisterValueType TXE			: 1;
			Core::RegisterValueType LBD			: 1;
			Core::RegisterValueType CTS			: 1;
			Core::RegisterValueType 			: 22;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static bool ReadDataRegisterNotEmpty()
		{
			return Register::Ptr()->RXNE;
		}
		
		static bool TransmissionComplete()
		{
			return Register::Ptr()->TC;
		}
		
		static bool TransmitDataRegisterEmpty()
		{
			return Register::Ptr()->TXE;
		}
		
		static bool Overrun()
		{
			return Register::Ptr()->ORE;
		}
	};
	
	// Data register (USART_DR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class DataRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x04;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType DataValue		: 9;
			Core::RegisterValueType 				: 23;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static Core::RegisterValueType Read()
		{
			return Register::Ptr()->DataValue;
		}
		
		static void Write(Core::RegisterValueType val)
		{
			Register::Ptr()->DataValue = val;
		}
	};
	
	// Baud rate register (USART_BRR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class BaudRateRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x08;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType DIV_Fraction	: 4;
			Core::RegisterValueType DIV_Mantissa	: 12;
			Core::RegisterValueType 				: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetDividerFraction(Core::RegisterValueType val)
		{
			Register::Ptr()->DIV_Fraction = val;
		}
		
		static void SetDividerMantissa(Core::RegisterValueType val)
		{
			Register::Ptr()->DIV_Mantissa = val;
		}
	};
	
	// Control register 1 (USART_CR1)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class Control1Register
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x0C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
		enum Parity
		{
			ParityEven	= 0,
			ParityOdd	= 1
		};
	protected:
		struct RegStruct
		{
			Core::RegisterValueType SBK			: 1;
			Core::RegisterValueType RWU			: 1;
			Core::RegisterValueType RE			: 1;
			Core::RegisterValueType TE			: 1;
			Core::RegisterValueType IDLEIE		: 1;
			Core::RegisterValueType RXNEIE		: 1;
			Core::RegisterValueType TCIE		: 1;
			Core::RegisterValueType TXEIE		: 1;
			Core::RegisterValueType PEIE		: 1;
			Core::RegisterValueType PS			: 1;
			Core::RegisterValueType PCE			: 1;
			Core::RegisterValueType WAKE		: 1;
			Core::RegisterValueType M			: 1;
			Core::RegisterValueType UE			: 1;
			Core::RegisterValueType 			: 18;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableReceiver(bool enable = true)
		{
			Register::Ptr()->RE = enable;
		}
		
		static void EnableTransmitter(bool enable = true)
		{
			Register::Ptr()->TE = enable;
		}
		
		static void Enable(bool enable = true)
		{
			Register::Ptr()->UE = enable;
		}
		
		static void EnableReadDataRegisterNotEmptyInterrupt(bool enable = true)
		{
			Register::Ptr()->RXNEIE = enable;
		}
		
		static bool ReadDataRegisterNotEmptyInterruptEnabled()
		{
			return Register::Ptr()->RXNEIE;
		}
		
		static void EnableTransmissionCompleteInterrupt(bool enable = true)
		{
			Register::Ptr()->TCIE = enable;
		}
		
		static bool TransmissionCompleteInterruptEnabled()
		{
			return Register::Ptr()->TCIE;
		}
		
		static void EnableTransmitDataRegisterEmptyInterrupt(bool enable = true)
		{
			Register::Ptr()->TXEIE = enable;
		}
		
		static bool TransmitDataRegisterEmptyInterruptEnabled()
		{
			return Register::Ptr()->TXEIE;
		}
		
		static void EnableParityControl(bool enable = true)
		{
			Register::Ptr()->PCE = enable;
		}
		
		static void SetParity(Parity val)
		{
			Register::Ptr()->PS = val;
		}
		
		static void SendBreak()
		{
			Register::Ptr()->SBK = 1;
		}
	};
	
	// Control register 2 (USART_CR2)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class Control2Register
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x10;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
		enum StopBits
		{
			StopBits1		= 0,
			StopBits0_5		= 1,
			StopBits2		= 2,
			StopBits1_5		= 3,
		};
	protected:
		struct RegStruct
		{
			Core::RegisterValueType ADD				: 4;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType LBDL			: 1;
			Core::RegisterValueType LBDIE			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType LBCL			: 1;
			Core::RegisterValueType CPHA			: 1;
			Core::RegisterValueType CPOL			: 1;
			Core::RegisterValueType CLKEN			: 1;
			Core::RegisterValueType STOP			: 2;
			Core::RegisterValueType LINEN			: 1;
			Core::RegisterValueType 				: 17;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetStopBits(StopBits val)
		{
			Register::Ptr()->STOP = val;
		}
	};
	
	// Guard time and prescaler register (USART_GTPR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class GuardTimeAndPrescalerRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x18;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType PSC				: 8;
			Core::RegisterValueType GT				: 8;
			Core::RegisterValueType 				: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetPrescaler(Core::RegisterValueType val)
		{
			Register::Ptr()->PSC = val - 1;
		}
	};
	
	template
		<
			int IdObj
		>
	class StaticParams
	{
	public:
		typedef CallbackWrapper<> OnWriteCompleteCallbackType;
		typedef CallbackWrapper<unsigned int> OnReadCallbackType;
	public:
		static OnWriteCompleteCallbackType OnWriteComplete;
		static OnReadCallbackType OnRead;
	protected:
		static bool _busy;
		static unsigned char *_txData;
		static unsigned int _txCount;
		static unsigned int _txPos;
	};
	
	template <int IdObj>
		StaticParams<IdObj>::OnWriteCompleteCallbackType StaticParams<IdObj>::OnWriteComplete;
	
	template <int IdObj>
		StaticParams<IdObj>::OnReadCallbackType StaticParams<IdObj>::OnRead;
	
	template <int IdObj>
		bool StaticParams<IdObj>::_busy = false;
	
	template <int IdObj>
		unsigned char * StaticParams<IdObj>::_txData = 0;
	
	template <int IdObj>
		unsigned int StaticParams<IdObj>::_txCount = 0;
	
	template <int IdObj>
		unsigned int StaticParams<IdObj>::_txPos = 0;
	
	template
		<
			int IdObj,
			Core::RegisterAddressType BaseAddress
		>
	class UsartImplementation :
		public StatusRegister<BaseAddress>,
		public DataRegister<BaseAddress>,
		public BaudRateRegister<BaseAddress>,
		public Control1Register<BaseAddress>,
		public Control2Register<BaseAddress>,
		public GuardTimeAndPrescalerRegister<BaseAddress>,
		public StaticParams<IdObj>
	{
	public:
		typedef StatusRegister<BaseAddress> StatusRegister;
		typedef DataRegister<BaseAddress> DataRegister;
		typedef BaudRateRegister<BaseAddress> BaudRateRegister;
		typedef Control1Register<BaseAddress> Control1Register;
		typedef Control2Register<BaseAddress> Control2Register;
		typedef GuardTimeAndPrescalerRegister<BaseAddress> GuardTimeAndPrescalerRegister;
		typedef StaticParams<IdObj> StaticParams;
		
		enum StopBits
		{
			StopBits1		= 0,
			StopBits0_5		= 1,
			StopBits2		= 2,
			StopBits1_5		= 3,
		};
		
		enum Parity
		{
			ParityNone		= 0,
			ParityEven		= 1,
			ParityOdd		= 2
		};
	public:
		static void Init(unsigned int sourceClockFrequency, unsigned int baudRate, StopBits stopBits, Parity parity)
		{
			SetBaudRate(sourceClockFrequency, baudRate);
			
			Control2Register::SetStopBits((typename Control2Register::StopBits)stopBits);
			
			switch(parity)
			{
			case ParityNone:
				{
					Control1Register::EnableParityControl(false);
				}
				break;
			case ParityEven:
				{
					Control1Register::EnableParityControl(true);
					Control1Register::SetParity(Control1Register::ParityEven);
				}
				break;
			case ParityOdd:
				{
					Control1Register::EnableParityControl(true);
					Control1Register::SetParity(Control1Register::ParityOdd);
				}
				break;
			}
		}
		
		static void Enable()
		{
			Control1Register::EnableReadDataRegisterNotEmptyInterrupt();
			Control1Register::EnableReceiver();
			Control1Register::EnableTransmitter();
			Control1Register::Enable();
		}
		
		static void Disable()
		{
			Control1Register::EnableReadDataRegisterNotEmptyInterrupt(false);
			Control1Register::EnableReceiver(false);
			Control1Register::EnableTransmitter(false);
			Control1Register::Enable(false);
		}
		
		static void SetBaudRate(unsigned int sourceClockFrequency, unsigned int baudRate)
		{
			if (baudRate == 0)
			{
				baudRate = 1;
			}
		
			float d = sourceClockFrequency;
			d /= 16.0;
			d /= (float)baudRate;
			unsigned short mantissa = (int)d;
			d -= mantissa;
			d *= 16.0;
			unsigned char fraction = (int)d;
			
			BaudRateRegister::SetDividerFraction(fraction);
			BaudRateRegister::SetDividerMantissa(mantissa);
		}
		
		static bool Write(unsigned char *data, unsigned int count)
		{
			if (StaticParams::_busy)
			{
				return false;
			}
			
			if (data == 0 || count < 1)
			{
				return false;
			}
			
			StaticParams::_busy = true;
			StaticParams::_txData = data;
			StaticParams::_txCount = count;
			StaticParams::_txPos = 0;
			
			Control1Register::EnableTransmitDataRegisterEmptyInterrupt();
			Control1Register::EnableTransmissionCompleteInterrupt();
			
			return false;
		}
		
		static void InterruptHandler()
		{
			if ((StatusRegister::ReadDataRegisterNotEmpty() || StatusRegister::Overrun()) && Control1Register::ReadDataRegisterNotEmptyInterruptEnabled())
			{
				unsigned int data = DataRegister::Read();
				StaticParams::OnRead(data);
			}
			
			if (StatusRegister::TransmitDataRegisterEmpty() && Control1Register::TransmitDataRegisterEmptyInterruptEnabled())
			{
				if (StaticParams::_txPos < StaticParams::_txCount)
				{
					DataRegister::Write(StaticParams::_txData[StaticParams::_txPos]);
					StaticParams::_txPos++;
				}
				
				if (StaticParams::_txCount < 2)
				{
					Control1Register::EnableTransmitDataRegisterEmptyInterrupt(false);
					Control1Register::EnableTransmissionCompleteInterrupt(false);
					StaticParams::_busy = false;
					StaticParams::OnWriteComplete();
				}
			}
			
			if (StatusRegister::TransmissionComplete() && Control1Register::TransmissionCompleteInterruptEnabled())
			{
				if (StaticParams::_txPos >= StaticParams::_txCount)
				{
					Control1Register::EnableTransmitDataRegisterEmptyInterrupt(false);
					Control1Register::EnableTransmissionCompleteInterrupt(false);
					StaticParams::_busy = false;
					StaticParams::OnWriteComplete();
				}
			}
		}
	};
	
	class Usart
	{
	public:
		typedef UsartImplementation<1, 0x40011000> Usart1;
		typedef UsartImplementation<2, 0x40004400> Usart2;
		typedef UsartImplementation<3, 0x40004800> Usart3;
		typedef UsartImplementation<4, 0x40004C00> Uart4;
		typedef UsartImplementation<5, 0x40005000> Uart5;
		typedef UsartImplementation<6, 0x40011400> Usart6;
		typedef UsartImplementation<7, 0x40007800> Usart7;
		typedef UsartImplementation<8, 0x40007C00> Usart8;
	};
}

typedef UsartImplementation::Usart Usart;

}
}
}

#endif