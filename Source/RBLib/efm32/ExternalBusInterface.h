#ifndef EBI_EMF32_H
#define EBI_EMF32_H

#include "Register.h"

namespace Rblib
{
namespace Efm32
{
namespace ExternalBusInterface
{
	// Тип значения регистра
	typedef unsigned int ValueDataType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;
	
	// EBI_CTRL - Control Register
	template
		<
			RegisterAddressType Address
		>
	class ControlRegister
	{
	public:
		enum Mode
		{
			ModeD8A8		= 0,
			ModeD16A16ALE	= 1,
			ModeD8A24ALE	= 2,
			ModeD16			= 3
		};
	protected:
		struct RegStruct
		{
			ValueDataType MODE			: 2;
			ValueDataType MODE1			: 2;
			ValueDataType MODE2			: 2;
			ValueDataType MODE3			: 2;
			ValueDataType BANK0EN		: 1;
			ValueDataType BANK1EN		: 1;
			ValueDataType BANK2EN		: 1;
			ValueDataType BANK3EN		: 1;
			ValueDataType NOIDLE		: 1;
			ValueDataType NOIDLE1		: 1;
			ValueDataType NOIDLE2		: 1;
			ValueDataType NOIDLE3		: 1;
			ValueDataType ARDYEN		: 1;
			ValueDataType ARDYTODIS		: 1;
			ValueDataType ARDY1EN		: 1;
			ValueDataType ARDYTO1DIS	: 1;
			ValueDataType ARDY2EN		: 1;
			ValueDataType ARDYTO2DIS	: 1;
			ValueDataType ARDY3EN		: 1;
			ValueDataType ARDYTO3DIS	: 1;
			ValueDataType BL			: 1;
			ValueDataType BL1			: 1;
			ValueDataType BL2			: 1;
			ValueDataType BL3			: 1;
			ValueDataType 				: 2;
			ValueDataType ITS			: 1;
			ValueDataType ALTMAP		: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetMode(int bank, Mode mode)
		{
			switch (bank)
			{
			case 0:
				Register::Ptr()->MODE = mode;
				break;
			case 1:
				Register::Ptr()->MODE1 = mode;
				break;
			case 2:
				Register::Ptr()->MODE2 = mode;
				break;
			case 3:
				Register::Ptr()->MODE3 = mode;
				break;
			}
		}
		
		static void EnableBank(int bank, bool enable = true)
		{
			switch (bank)
			{
			case 0:
				Register::Ptr()->BANK0EN = enable;
				break;
			case 1:
				Register::Ptr()->BANK1EN = enable;
				break;
			case 2:
				Register::Ptr()->BANK2EN = enable;
				break;
			case 3:
				Register::Ptr()->BANK3EN = enable;
				break;
			}
		}
		
		static void DisableIdleStateInsertion(int bank, bool disable = true)
		{
			switch (bank)
			{
			case 0:
				Register::Ptr()->NOIDLE = disable;
				break;
			case 1:
				Register::Ptr()->NOIDLE1 = disable;
				break;
			case 2:
				Register::Ptr()->NOIDLE2 = disable;
				break;
			case 3:
				Register::Ptr()->NOIDLE3 = disable;
				break;
			}
		}
		
		static void EnableWait(int bank, bool enable = true)
		{
			switch (bank)
			{
			case 0:
				Register::Ptr()->ARDYEN = enable;
				break;
			case 1:
				Register::Ptr()->ARDY1EN = enable;
				break;
			case 2:
				Register::Ptr()->ARDY2EN = enable;
				break;
			case 3:
				Register::Ptr()->ARDY3EN = enable;
				break;
			}
		}
		
		static void DisableWaitTimeout(int bank, bool disable = true)
		{
			switch (bank)
			{
			case 0:
				Register::Ptr()->ARDYTODIS = disable;
				break;
			case 1:
				Register::Ptr()->ARDYTO1DIS = disable;
				break;
			case 2:
				Register::Ptr()->ARDYTO2DIS = disable;
				break;
			case 3:
				Register::Ptr()->ARDYTO3DIS = disable;
				break;
			}
		}
		
		static void EnableByteLane(int bank, bool enable = true)
		{
			switch (bank)
			{
			case 0:
				Register::Ptr()->BL = enable;
				break;
			case 1:
				Register::Ptr()->BL1 = enable;
				break;
			case 2:
				Register::Ptr()->BL2 = enable;
				break;
			case 3:
				Register::Ptr()->BL3 = enable;
				break;
			}
		}
		
		static void EnableIndividualBankSettings( bool enable = true)
		{
			Register::Ptr()->ITS = enable;
		}
	};
	
	// EBI_ROUTE - I/O Routing Register
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
			Location2 = 2
		};
	protected:
		struct RegStruct
		{
			ValueDataType EBIPEN		: 1;
			ValueDataType CS0PEN		: 1;
			ValueDataType CS1PEN		: 1;
			ValueDataType CS2PEN		: 1;
			ValueDataType CS3PEN		: 1;
			ValueDataType ALEPEN		: 1;
			ValueDataType ARDYPEN		: 1;
			ValueDataType BLPEN			: 1;
			ValueDataType 				: 4;
			ValueDataType NANDPEN		: 1;
			ValueDataType 				: 3;
			ValueDataType ALB			: 2;
			ValueDataType APEN			: 5;
			ValueDataType 				: 1;
			ValueDataType TFTPEN		: 1;
			ValueDataType DATAENPEN		: 1;
			ValueDataType CSTFTPEN		: 1;
			ValueDataType 				: 1;
			ValueDataType LOCATION		: 3;
			ValueDataType 				: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnablePinBase(bool enable = true)
		{
			Register::Ptr()->EBIPEN = enable;
		}
		
		static void EnablePinCS(bool enable0, bool enable1, bool enable2, bool enable3)
		{
			Register::Ptr()->CS0PEN = enable0;
			Register::Ptr()->CS1PEN = enable1;
			Register::Ptr()->CS2PEN = enable2;
			Register::Ptr()->CS3PEN = enable3;
		}
		
		static void EnablePinALE(bool enable = true)
		{
			Register::Ptr()->ALEPEN = enable;
		}
		
		static void EnablePinWAIT(bool enable = true)
		{
			Register::Ptr()->ARDYPEN = enable;
		}
		
		static void EnablePinBL(bool enable = true)
		{
			Register::Ptr()->BLPEN = enable;
		}
		
		static void SetAddressBusWidth(unsigned char val)
		{
			Register::Ptr()->APEN = val;
		}
		
		static void SetIOLocation(Location val)
		{
			Register::Ptr()->LOCATION = val;
		}
	};
	
	// EBI_ADDRTIMING - Address Timing Register
	template
		<
			RegisterAddressType Address
		>
	class AddressTimingRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType ADDRSETUP		: 2;
			ValueDataType 				: 6;
			ValueDataType ADDRHOLD		: 2;
			ValueDataType 				: 18;
			ValueDataType HALFALE		: 1;
			ValueDataType 				: 3;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetAddressTiming(unsigned char setup, unsigned char hold, bool halfAle = false)
		{
			Register::Ptr()->ADDRSETUP = setup;
			Register::Ptr()->ADDRHOLD = hold;
			Register::Ptr()->HALFALE = halfAle;
		}
	};
	
	// EBI_RDTIMING - Read Timing Register
	template
		<
			RegisterAddressType Address
		>
	class ReadTimingRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType RDSETUP		: 2;
			ValueDataType 				: 6;
			ValueDataType RDSTRB		: 6;
			ValueDataType 				: 2;
			ValueDataType RDHOLD		: 2;
			ValueDataType 				: 10;
			ValueDataType HALFRE		: 1;
			ValueDataType PREFETCH		: 1;
			ValueDataType PAGEMODE		: 1;
			ValueDataType 				: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetReadTiming(unsigned char setup, unsigned char strobe, unsigned char hold, bool halfRe = false, bool prefetch = false, bool pageMode = false)
		{
			Register::Ptr()->RDSETUP = setup;
			Register::Ptr()->RDSTRB = strobe;
			Register::Ptr()->RDHOLD = hold;
			Register::Ptr()->HALFRE = halfRe;
			Register::Ptr()->PREFETCH = prefetch;
			Register::Ptr()->PAGEMODE = pageMode;
		}
	};
	
	// EBI_WRTIMING - Write Timing Register
	template
		<
			RegisterAddressType Address
		>
	class WriteTimingRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType WRSETUP		: 2;
			ValueDataType 				: 6;
			ValueDataType WRSTRB		: 6;
			ValueDataType 				: 2;
			ValueDataType WRHOLD		: 2;
			ValueDataType 				: 10;
			ValueDataType HALFWE		: 1;
			ValueDataType WBUFDIS		: 1;
			ValueDataType 				: 2;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetWriteTiming(unsigned char setup, unsigned char strobe, unsigned char hold, bool halfWe = false, bool writeBufferDisable = false)
		{
			Register::Ptr()->WRSETUP = setup;
			Register::Ptr()->WRSTRB = strobe;
			Register::Ptr()->WRHOLD = hold;
			Register::Ptr()->HALFWE = halfWe;
			Register::Ptr()->WBUFDIS = writeBufferDisable;
		}
	};
	
	template
		<
			RegisterAddressType BaseAddr,
			RegisterAddressType ControlRegAddr,
			RegisterAddressType AddressTimingRegAddr,
			RegisterAddressType ReadTimingRegAddr,
			RegisterAddressType WriteTimingRegAddr,
			RegisterAddressType PolarityRegAddr,
			RegisterAddressType IORoutingRegAddr,
			RegisterAddressType AddressTiming1RegAddr,
			RegisterAddressType ReadTiming1RegAddr,
			RegisterAddressType WriteTiming1RegAddr,
			RegisterAddressType Polarity1RegAddr,
			RegisterAddressType AddressTiming2RegAddr,
			RegisterAddressType ReadTiming2RegAddr,
			RegisterAddressType WriteTiming2RegAddr,
			RegisterAddressType Polarity2RegAddr,
			RegisterAddressType AddressTiming3RegAddr,
			RegisterAddressType ReadTiming3RegAddr,
			RegisterAddressType WriteTiming3RegAddr,
			RegisterAddressType Polarity3RegAddr,
			RegisterAddressType PageControlRegAddr,
			RegisterAddressType NANDControlRegAddr,
			RegisterAddressType CommandRegAddr,
			RegisterAddressType StatusRegAddr,
			RegisterAddressType ECCParityRegAddr,
			RegisterAddressType TFTControlRegAddr,
			RegisterAddressType TFTStatusRegAddr,
			RegisterAddressType TFTFrameBaseRegAddr,
			RegisterAddressType TFTStrideRegAddr,
			RegisterAddressType TFTSizeRegAddr,
			RegisterAddressType TFTHorizontalPorchRegAddr,
			RegisterAddressType TFTVerticalPorchRegAddr,
			RegisterAddressType TFTTimingRegAddr,
			RegisterAddressType TFTPolarityRegAddr,
			RegisterAddressType TFTDirectDriveDataRegAddr,
			RegisterAddressType TFTAlphaBlendingRegAddr,
			RegisterAddressType TFTPixel0RegAddr,
			RegisterAddressType TFTPixel1RegAddr,
			RegisterAddressType TFTAlphaBlendingResultPixelRegAddr,
			RegisterAddressType TFTMaskingRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr,
			RegisterAddressType InterruptEnableRegAddr
		>
	class ExternalBusInterface :
		public ControlRegister<BaseAddr + ControlRegAddr>,
		public IORoutingRegister<BaseAddr + IORoutingRegAddr>
	{
	protected:
		typedef AddressTimingRegister<BaseAddr + AddressTimingRegAddr> AddressTimingReg0;
		typedef AddressTimingRegister<BaseAddr + AddressTiming1RegAddr> AddressTimingReg1;
		typedef AddressTimingRegister<BaseAddr + AddressTiming2RegAddr> AddressTimingReg2;
		typedef AddressTimingRegister<BaseAddr + AddressTiming3RegAddr> AddressTimingReg3;
		
		typedef ReadTimingRegister<BaseAddr + ReadTimingRegAddr> ReadTimingReg0;
		typedef ReadTimingRegister<BaseAddr + ReadTiming1RegAddr> ReadTimingReg1;
		typedef ReadTimingRegister<BaseAddr + ReadTiming2RegAddr> ReadTimingReg2;
		typedef ReadTimingRegister<BaseAddr + ReadTiming3RegAddr> ReadTimingReg3;
		
		typedef WriteTimingRegister<BaseAddr + WriteTimingRegAddr> WriteTimingReg0;
		typedef WriteTimingRegister<BaseAddr + WriteTiming1RegAddr> WriteTimingReg1;
		typedef WriteTimingRegister<BaseAddr + WriteTiming2RegAddr> WriteTimingReg2;
		typedef WriteTimingRegister<BaseAddr + WriteTiming3RegAddr> WriteTimingReg3;
	public:
		static void SetAddressTiming(int bank, unsigned char setup, unsigned char hold, bool halfAle = false)
		{
			switch (bank)
			{
			case 0:
				AddressTimingReg0::SetAddressTiming(setup, hold, halfAle);
				break;
			case 1:
				AddressTimingReg1::SetAddressTiming(setup, hold, halfAle);
				break;
			case 2:
				AddressTimingReg2::SetAddressTiming(setup, hold, halfAle);
				break;
			case 3:
				AddressTimingReg3::SetAddressTiming(setup, hold, halfAle);
				break;
			}
		}
		
		static void SetReadTiming(int bank, unsigned char setup, unsigned char strobe, unsigned char hold, bool halfRe = false, bool prefetch = false, bool pageMode = false)
		{
			switch (bank)
			{
			case 0:
				ReadTimingReg0::SetReadTiming(setup, strobe, hold, halfRe, prefetch, pageMode);
				break;
			case 1:
				ReadTimingReg1::SetReadTiming(setup, strobe, hold, halfRe, prefetch, pageMode);
				break;
			case 2:
				ReadTimingReg2::SetReadTiming(setup, strobe, hold, halfRe, prefetch, pageMode);
				break;
			case 3:
				ReadTimingReg3::SetReadTiming(setup, strobe, hold, halfRe, prefetch, pageMode);
				break;
			}
		}
		
		static void SetWriteTiming(int bank, unsigned char setup, unsigned char strobe, unsigned char hold, bool halfWe = false, bool writeBufferDisable = false)
		{
			switch (bank)
			{
			case 0:
				WriteTimingReg0::SetWriteTiming(setup, strobe, hold, halfWe, writeBufferDisable);
				break;
			case 1:
				WriteTimingReg1::SetWriteTiming(setup, strobe, hold, halfWe, writeBufferDisable);
				break;
			case 2:
				WriteTimingReg2::SetWriteTiming(setup, strobe, hold, halfWe, writeBufferDisable);
				break;
			case 3:
				WriteTimingReg3::SetWriteTiming(setup, strobe, hold, halfWe, writeBufferDisable);
				break;
			}
		}
	};
}
}
}

#endif