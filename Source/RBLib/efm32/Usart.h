#ifndef USART_EMF32_H
#define USART_EMF32_H

#include "Register.h"

namespace Rblib
{
namespace Efm32
{
namespace Usart
{
	// Тип значения регистра
	typedef unsigned int ValueDataType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;
	
	// USARTn_CTRL - Control Register
	template
		<
			RegisterAddressType Address
		>
	class ControlRegister
	{
	public:
		enum Oversampling
		{
			Oversampling16	= 0,
			Oversampling8	= 1,
			Oversampling6	= 2,
			Oversampling4	= 3
		};
	protected:
		struct RegStruct
		{
			ValueDataType SynchronousMode					: 1;
			ValueDataType LoopbackEnable					: 1;
			ValueDataType CollisionCheckEnable				: 1;
			ValueDataType MultiProcessorMode				: 1;
			ValueDataType MultiProcessorAddressBit			: 1;
			ValueDataType Oversampling						: 2;
			ValueDataType 									: 1;
			ValueDataType ClockPolarity						: 1;
			ValueDataType ClockEdge							: 1;
			ValueDataType MostSignificantBitFirst			: 1;
			ValueDataType ActionOnSlaveSelectInMasterMode	: 1;
			ValueDataType TxBufferInterruptLevel			: 1;
			ValueDataType ReceiverInputInvert				: 1;
			ValueDataType TransmitterOutputInvert			: 1;
			ValueDataType ChipSelectInvert					: 1;
			ValueDataType AutomaticChipSelect				: 1;
			ValueDataType AutomaticTxTristate				: 1;
			ValueDataType SmartCardMode						: 1;
			ValueDataType SmartCardRetransmit				: 1;
			ValueDataType SkipParityErrorFrames				: 1;
			ValueDataType Bit8DefaultValue					: 1;
			ValueDataType HaltDmaOnError					: 1;
			ValueDataType DisableRxOnError					: 1;
			ValueDataType DisableTxOnError					: 1;
			ValueDataType 									: 1;
			ValueDataType TxDelayTransmission				: 2;
			ValueDataType ByteswapInDoubleAccesses			: 1;
			ValueDataType AlwaysTransmitWhenRxNotFull		: 1;
			ValueDataType MajorityVoteDisable				: 1;
			ValueDataType 									: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetSynchronousMode(bool val = true)
		{
			Register::Ptr()->SynchronousMode = val;
		}
		
		static bool IsSynchronousMode()
		{
			return Register::Ptr()->SynchronousMode;
		}
		
		static void SetOversampling(Oversampling val)
		{
			Register::Ptr()->Oversampling = val;
		}
		
		static void SetMostSignificantBitFirst(bool val)
		{
			Register::Ptr()->MostSignificantBitFirst = val;
		}
		
		static void SetClockPolarity(bool val)
		{
			Register::Ptr()->ClockPolarity = val;
		}
		
		static void SetClockEdge(bool val)
		{
			Register::Ptr()->ClockEdge = val;
		}
	};
	
	// USARTn_FRAME - USART Frame Format Register
	template
		<
			RegisterAddressType Address
		>
	class FrameFormatRegister
	{
	public:
		// Размер данных
		enum DataSize
		{
			DataSize4Bit = 1,
			DataSize5Bit = 2,
			DataSize6Bit = 3,
			DataSize7Bit = 4,
			DataSize8Bit = 5,
			DataSize9Bit = 6,
			DataSize10Bit = 7,
			DataSize11Bit = 8,
			DataSize12Bit = 9,
			DataSize13Bit = 10,
			DataSize14Bit = 11,
			DataSize15Bit = 12,
			DataSize16Bit = 13
		};
		// Четность
		enum Parity
		{
			ParityNone	= 0,
			ParityOdd	= 2,
			ParityEven	= 3
		};
		// Число стоповых битов
		enum StopBits
		{
			StopBitsHalf		= 0,
			StopBitsOne			= 1,
			StopBitsOneAndHalf	= 2,
			StopBitsTwo			= 3
		};
	protected:
		struct RegStruct
		{
			ValueDataType DataBitMode	: 4;
			ValueDataType 				: 4;
			ValueDataType ParityBitMode	: 2;
			ValueDataType 				: 2;
			ValueDataType StopBitMode	: 2;
			ValueDataType 				: 18;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetDataBitMode(DataSize val)
		{
			Register::Ptr()->DataBitMode = val;
		}
		
		static void SetParityBitMode(Parity val)
		{
			Register::Ptr()->ParityBitMode = val;
		}
		
		static void SetStopBitMode(StopBits val)
		{
			Register::Ptr()->StopBitMode = val;
		}
	};
	
	// USARTn_CMD - Command Register
	template
		<
			RegisterAddressType Address
		>
	class CommandRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		enum Command
		{
			CommandReceiverEnable				 = 0,
			CommandReceiverDisable				 = 1,
			CommandTransmitterEnable			 = 2,
			CommandTransmitterDisable			 = 3,
			CommandMasterEnable					 = 4,
			CommandMasterDisable				 = 5,
			CommandReceiverBlockEnable			 = 6,
			CommandReceiverBlockDisable			 = 7,
			CommandTransmitterTristateEnable	 = 8,
			CommandTransmitterTristateDisable	 = 9,
			CommandClearTx						 = 10,
			CommandClearRx						 = 11,
		};
	public:
		static void Do(Command i)
		{
			*Register::Ptr() = (1 << (unsigned char)i);
		}
	};
	
	// USARTn_CLKDIV - Clock Control Register
	template
		<
			RegisterAddressType Address
		>
	class ClockControlRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void SetClockDivider(double divider)
		{
			*(ValueDataType *)Register::Ptr() = (ValueDataType)divider;
		}
	};
	
	// USARTn_RXDATAX - RX Buffer Data Extended Register
	template
		<
			RegisterAddressType Address
		>
	class RxBufferDataExtendedRegister
	{
	protected:
		union RegStruct
		{
			struct
			{
				ValueDataType RxData					: 9;
				ValueDataType 							: 5;
				ValueDataType DataParityError			: 1;
				ValueDataType DataFramingError			: 1;
				ValueDataType 							: 16;
			} Struct;
			ValueDataType Val;
		};
		
		typedef Register<Address, ValueDataType> Register;
	public:
		static void Read(unsigned short *data, bool *parityError, bool *framingError)
		{
			RegStruct tmp;
			tmp.Val = *Register::Ptr();
			
			if (data)
			{
				*data = tmp.Struct.RxData;
			}
			
			if (parityError)
			{
				*parityError = tmp.Struct.DataParityError;
			}
			
			if (framingError)
			{
				*framingError = tmp.Struct.DataFramingError;
			}
		}
	};
	
	// USARTn_RXDATA - RX Buffer Data Register
	template
		<
			RegisterAddressType Address
		>
	class RxBufferDataRegister
	{
	protected:
		union RegStruct
		{
			struct
			{
				ValueDataType RxData					: 8;
				ValueDataType 							: 24;
			} Struct;
			ValueDataType Val;
		};
		
		typedef Register<Address, ValueDataType> Register;
	public:
		static void Read(unsigned char *data)
		{
			RegStruct tmp;
			tmp.Val = *Register::Ptr();
			
			if (data)
			{
				*data = tmp.Struct.RxData;
			}
		}
	};
	
	// USARTn_RXDOUBLEX - RX Buffer Double Data Extended Register
	template
		<
			RegisterAddressType Address
		>
	class RxBufferDoubleDataExtendedRegister
	{
	protected:
		union RegStruct
		{
			struct
			{
				ValueDataType RxData0					: 9;
				ValueDataType 							: 5;
				ValueDataType DataParityError0			: 1;
				ValueDataType DataFramingError0			: 1;
				ValueDataType RxData1					: 9;
				ValueDataType 							: 5;
				ValueDataType DataParityError1			: 1;
				ValueDataType DataFramingError1			: 1;
			} Struct;
			ValueDataType Val;
		};
		
		typedef Register<Address, ValueDataType> Register;
	public:
		static void Read(unsigned short *data0, bool *parityError0, bool *framingError0, unsigned short *data1, bool *parityError1, bool *framingError1)
		{
			RegStruct tmp;
			tmp.Val = *Register::Ptr();
			
			if (data0)
			{
				*data0 = tmp.Struct.RxData0;
			}
			
			if (parityError0)
			{
				*parityError0 = tmp.Struct.DataParityError0;
			}
			
			if (framingError0)
			{
				*framingError0 = tmp.Struct.DataFramingError0;
			}
			
			if (data1)
			{
				*data1 = tmp.Struct.RxData1;
			}
			
			if (parityError1)
			{
				*parityError1 = tmp.Struct.DataParityError1;
			}
			
			if (framingError1)
			{
				*framingError1 = tmp.Struct.DataFramingError1;
			}
		}
	};
	
	// USARTn_RXDOUBLE - RX FIFO Double Data Register
	template
		<
			RegisterAddressType Address
		>
	class RxBufferDoubleDataRegister
	{
	protected:
		union RegStruct
		{
			struct
			{
				ValueDataType RxData0					: 8;
				ValueDataType RxData1					: 8;
				ValueDataType 							: 16;
			} Struct;
			ValueDataType Val;
		};
		
		typedef Register<Address, ValueDataType> Register;
	public:
		static void Read(unsigned char *data0, unsigned char *data1)
		{
			RegStruct tmp;
			tmp.Val = *Register::Ptr();
			
			if (data0)
			{
				*data0 = tmp.Struct.RxData0;
			}
			
			if (data1)
			{
				*data1 = tmp.Struct.RxData1;
			}
		}
	};
	
	// USARTn_TXDATAX - TX Buffer Data Extended Register
	template
		<
			RegisterAddressType Address
		>
	class TxBufferDataExtendedRegister
	{
	protected:
		union RegStruct
		{
			struct
			{
				ValueDataType TxData						: 9;
				ValueDataType 								: 2;
				ValueDataType UnblockRxAfterTransmission	: 1;
				ValueDataType SetTxTriAfterTransmission		: 1;
				ValueDataType TransmitDataAsBreak			: 1;
				ValueDataType ClearTxEnAfterTransmission	: 1;
				ValueDataType EnableRxAfterTransmission		: 1;
				ValueDataType 								: 16;
			} Struct;
			ValueDataType Val;
		};
		
		typedef Register<Address, ValueDataType> Register;
	public:
		static void Write(unsigned short data, bool unblockRxAfterTransmission, bool setTxTriAfterTransmission, bool transmitDataAsBreak, bool clearTxEnAfterTransmission, bool enableRxAfterTransmission)
		{
			RegStruct tmp;
			
			tmp.Struct.TxData = data;
			tmp.Struct.UnblockRxAfterTransmission = unblockRxAfterTransmission;
			tmp.Struct.SetTxTriAfterTransmission = setTxTriAfterTransmission;
			tmp.Struct.TransmitDataAsBreak = transmitDataAsBreak;
			tmp.Struct.ClearTxEnAfterTransmission = clearTxEnAfterTransmission;
			tmp.Struct.EnableRxAfterTransmission = enableRxAfterTransmission;
			
			*Register::Ptr() = tmp.Val;
		}
	};
	
	// USARTn_TXDATA - TX Buffer Data Register
	template
		<
			RegisterAddressType Address
		>
	class TxBufferDataRegister
	{
	protected:
		union RegStruct
		{
			struct
			{
				ValueDataType TxData						: 8;
				ValueDataType 								: 24;
			} Struct;
			ValueDataType Val;
		};
		
		typedef Register<Address, ValueDataType> Register;
	public:
		static void Write(unsigned char data)
		{
			RegStruct tmp;
			
			tmp.Struct.TxData = data;
			
			*Register::Ptr() = tmp.Val;
		}
	};
	
	// USARTn_TXDOUBLEX - TX Buffer Double Data Extended Register
	template
		<
			RegisterAddressType Address
		>
	class TxBufferDoubleDataExtendedRegister
	{
	protected:
		union RegStruct
		{
			struct
			{
				ValueDataType TxData0						: 9;
				ValueDataType 								: 2;
				ValueDataType UnblockRxAfterTransmission0	: 1;
				ValueDataType SetTxTriAfterTransmission0	: 1;
				ValueDataType TransmitDataAsBreak0			: 1;
				ValueDataType ClearTxEnAfterTransmission0	: 1;
				ValueDataType EnableRxAfterTransmission0	: 1;
				ValueDataType TxData1						: 9;
				ValueDataType 								: 2;
				ValueDataType UnblockRxAfterTransmission1	: 1;
				ValueDataType SetTxTriAfterTransmission1	: 1;
				ValueDataType TransmitDataAsBreak1			: 1;
				ValueDataType ClearTxEnAfterTransmission1	: 1;
				ValueDataType EnableRxAfterTransmission1	: 1;
			} Struct;
			ValueDataType Val;
		};
		
		typedef Register<Address, ValueDataType> Register;
	public:
		static void Write(unsigned short data0, bool unblockRxAfterTransmission0, bool setTxTriAfterTransmission0, bool transmitDataAsBreak0, bool clearTxEnAfterTransmission0, bool enableRxAfterTransmission0,
						  unsigned short data1, bool unblockRxAfterTransmission1, bool setTxTriAfterTransmission1, bool transmitDataAsBreak1, bool clearTxEnAfterTransmission1, bool enableRxAfterTransmission1)
		{
			RegStruct tmp;
			
			tmp.Struct.TxData0 = data0;
			tmp.Struct.UnblockRxAfterTransmission0 = unblockRxAfterTransmission0;
			tmp.Struct.SetTxTriAfterTransmission0 = setTxTriAfterTransmission0;
			tmp.Struct.TransmitDataAsBreak0 = transmitDataAsBreak0;
			tmp.Struct.ClearTxEnAfterTransmission0 = clearTxEnAfterTransmission0;
			tmp.Struct.EnableRxAfterTransmission0 = enableRxAfterTransmission0;
			tmp.Struct.TxData1 = data1;
			tmp.Struct.UnblockRxAfterTransmission1 = unblockRxAfterTransmission1;
			tmp.Struct.SetTxTriAfterTransmission1 = setTxTriAfterTransmission1;
			tmp.Struct.TransmitDataAsBreak1 = transmitDataAsBreak1;
			tmp.Struct.ClearTxEnAfterTransmission1 = clearTxEnAfterTransmission1;
			tmp.Struct.EnableRxAfterTransmission1 = enableRxAfterTransmission1;
			
			*Register::Ptr() = tmp.Val;
		}
	};
	
	// USARTn_TXDOUBLE - TX Buffer Double Data Register
	template
		<
			RegisterAddressType Address
		>
	class TxBufferDoubleDataRegister
	{
	protected:
		union RegStruct
		{
			struct
			{
				ValueDataType TxData0						: 8;
				ValueDataType TxData1						: 8;
				ValueDataType 								: 16;
			} Struct;
			ValueDataType Val;
		};
		
		typedef Register<Address, ValueDataType> Register;
	public:
		static void Write(unsigned char data0, unsigned char data1)
		{
			RegStruct tmp;
			
			tmp.Struct.TxData0 = data0;
			tmp.Struct.TxData1 = data1;
			
			*Register::Ptr() = tmp.Val;
		}
	};
	
	// IUSARTn_IF - Interrupt Flag Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		enum Interrupt
		{
			InterruptTxComplete					 = 0,
			InterruptTxBufferLevel				 = 1,
			InterruptRxDataValid				 = 2,
			InterruptRxBufferFull				 = 3,
			InterruptRxOverflow					 = 4,
			InterruptRxUnderflow				 = 5,
			InterruptTxOverflow					 = 6,
			InterruptTxUnderflow				 = 7,
			InterruptParityError				 = 8,
			InterruptFramingError				 = 9,
			InterruptMultiProcessorAddressFrame	 = 10,
			InterruptSlaveSelectInMasterMode	 = 11,
			InterruptCollisionCheckFail			 = 12
		};
	public:
		static bool GetInterruptFlag(Interrupt i)
		{
			return *Register::Ptr() & (1 << (unsigned char)i);
		}
	};
	
	// USARTn_IFS - Interrupt Flag Set Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagSetRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		enum Interrupt
		{
			InterruptTxComplete					 = 0,
			InterruptRxBufferFull				 = 3,
			InterruptRxOverflow					 = 4,
			InterruptRxUnderflow				 = 5,
			InterruptTxOverflow					 = 6,
			InterruptTxUnderflow				 = 7,
			InterruptParityError				 = 8,
			InterruptFramingError				 = 9,
			InterruptMultiProcessorAddressFrame	 = 10,
			InterruptSlaveSelectInMasterMode	 = 11,
			InterruptCollisionCheckFail			 = 12
		};
	public:
		static void SetInterruptFlag(Interrupt i)
		{
			*Register::Ptr() = (1 << (unsigned char)i);
		}
	};
	
	// USARTn_IFC - Interrupt Flag Clear Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagClearRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		enum Interrupt
		{
			InterruptTxComplete					 = 0,
			InterruptRxBufferFull				 = 3,
			InterruptRxOverflow					 = 4,
			InterruptRxUnderflow				 = 5,
			InterruptTxOverflow					 = 6,
			InterruptTxUnderflow				 = 7,
			InterruptParityError				 = 8,
			InterruptFramingError				 = 9,
			InterruptMultiProcessorAddressFrame	 = 10,
			InterruptSlaveSelectInMasterMode	 = 11,
			InterruptCollisionCheckFail			 = 12
		};
	public:
		static void ClearInterruptFlag(Interrupt i)
		{
			*Register::Ptr() = (1 << (unsigned char)i);
		}
	};
	
	// USARTn_IEN - Interrupt Enable Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptEnableRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		enum Interrupt
		{
			InterruptTxComplete					 = 0,
			InterruptTxBufferLevel				 = 1,
			InterruptRxDataValid				 = 2,
			InterruptRxBufferFull				 = 3,
			InterruptRxOverflow					 = 4,
			InterruptRxUnderflow				 = 5,
			InterruptTxOverflow					 = 6,
			InterruptTxUnderflow				 = 7,
			InterruptParityError				 = 8,
			InterruptFramingError				 = 9,
			InterruptMultiProcessorAddressFrame	 = 10,
			InterruptSlaveSelectInMasterMode	 = 11,
			InterruptCollisionCheckFail			 = 12
		};
	public:
		static void InterruptEnable(Interrupt i)
		{
			*Register::Ptr() |= (1 << (unsigned char)i);
		}
		
		static void InterruptDisable(Interrupt i)
		{
			*Register::Ptr() &= ~(1 << (unsigned char)i);
		}
	};
	
	// USARTn_ROUTE - I/O Routing Register
	template
		<
			RegisterAddressType Address
		>
	class IORoutingRegister
	{
	public:
		enum Location
		{
			Location0 = 0,
			Location1 = 1,
			Location2 = 2,
			Location3 = 3,
			Location4 = 4,
			Location5 = 5
		};
	protected:
		struct RegStruct
		{
			ValueDataType RxPinEnable	: 1;
			ValueDataType TxPinEnable	: 1;
			ValueDataType CsPinEnable	: 1;
			ValueDataType ClkPinEnable	: 1;
			ValueDataType 				: 4;
			ValueDataType IOLocation	: 3;
			ValueDataType 				: 21;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetPinEnable(bool rx, bool tx, bool cs, bool clk)
		{
			Register::Ptr()->RxPinEnable = rx;
			Register::Ptr()->TxPinEnable = tx;
			Register::Ptr()->CsPinEnable = cs;
			Register::Ptr()->ClkPinEnable = clk;
		}
		
		static void SetIOLocation(Location val)
		{
			Register::Ptr()->IOLocation = val;
		}
	};
}
}
}

#endif