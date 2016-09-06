///////////////////////////////////////////////////////////////////////////////
//	 ласс управлени€ портами USB
//	STM32
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef USB_STM32_F1_H
#define USB_STM32_F1_H

#include "Register.h"
#include "../../IdObjectBase.h"
#include "../../Utils/CallbackListWrapper.h"

namespace Rblib
{
namespace Stm32
{
namespace F4
{
namespace UsbImplementation
{
namespace Global
{
	enum Mode
	{
		ModeDevice	= 0,
		ModeHost	= 1,
	};
	
	// OTG_FS control and status register (OTG_FS_GOTGCTL)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class ControlAndStatusRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x00;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType SRQSCS		: 1;
			Core::RegisterValueType SRQ			: 1;
			Core::RegisterValueType 			: 6;
			Core::RegisterValueType HNGSCS		: 1;
			Core::RegisterValueType HNPRQ		: 1;
			Core::RegisterValueType HSHNPEN		: 1;
			Core::RegisterValueType DHNPEN		: 1;
			Core::RegisterValueType 			: 4;
			Core::RegisterValueType CIDSTS		: 1;
			Core::RegisterValueType DBCT		: 1;
			Core::RegisterValueType ASVLD		: 1;
			Core::RegisterValueType BSVLD		: 1;
			Core::RegisterValueType 			: 12;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS interrupt register (OTG_FS_GOTGINT)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class InterruptRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x04;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType SEDET		: 1;
			Core::RegisterValueType 			: 5;
			Core::RegisterValueType SRSSCHG		: 1;
			Core::RegisterValueType HNSSCHG		: 1;
			Core::RegisterValueType 			: 7;
			Core::RegisterValueType HNGDET		: 1;
			Core::RegisterValueType ADTOCHG		: 1;
			Core::RegisterValueType DBCDNE		: 1;
			Core::RegisterValueType 			: 12;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	
	// OTG_FS AHB configuration register (OTG_FS_GAHBCFG)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class AHBConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x08;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType GINT		: 1;
			Core::RegisterValueType 			: 6;
			Core::RegisterValueType TXFELVL		: 1;
			Core::RegisterValueType PTXFELVL	: 1;
			Core::RegisterValueType 			: 23;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void GlobalInterruptEnable(bool enable = true)
		{
			Register::Ptr()->GINT = enable;
		}
	};
	
	/*
	// OTG_HS AHB configuration register (OTG_HS_GAHBCFG)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class AHBConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x08;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType GINT		: 1;
			Core::RegisterValueType HBSTLEN		: 4;
			Core::RegisterValueType DMAEN		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType TXFELVL		: 1;
			Core::RegisterValueType PTXFELVL	: 1;
			Core::RegisterValueType 			: 23;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	*/
	
	// OTG_FS USB configuration register (OTG_FS_GUSBCFG)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class USBConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x0C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType TOCAL		: 3;
			Core::RegisterValueType 			: 3;
			Core::RegisterValueType PHYSEL		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType SRPCAP		: 1;
			Core::RegisterValueType HNPCAP		: 1;
			Core::RegisterValueType TRDT		: 4;
			Core::RegisterValueType 			: 15;
			Core::RegisterValueType FHMOD		: 1;
			Core::RegisterValueType FDMOD		: 1;
			Core::RegisterValueType CTXPKT		: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableForcedHostMode(bool enable = true)
		{
			Register::Ptr()->FHMOD = enable;
		}
	};
	
	/*
	// OTG_HS USB configuration register (OTG_HS_GUSBCFG)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class USBConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x0C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType TOCAL		: 3;
			Core::RegisterValueType 			: 3;
			Core::RegisterValueType PHSEL		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType SRPCAP		: 1;
			Core::RegisterValueType HNPCAP		: 1;
			Core::RegisterValueType TRDT		: 4;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType PHYLPCS		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType ULPIFSLS	: 1;
			Core::RegisterValueType ULPIAR		: 1;
			Core::RegisterValueType ULPICSM		: 1;
			Core::RegisterValueType ULPIEVBUSD	: 1;
			Core::RegisterValueType ULPIEVBUSI	: 1;
			Core::RegisterValueType TSDPS		: 1;
			Core::RegisterValueType PCCI		: 1;
			Core::RegisterValueType PTCI		: 1;
			Core::RegisterValueType 			: 3;
			Core::RegisterValueType FHMOD		: 1;
			Core::RegisterValueType FDMOD		: 1;
			Core::RegisterValueType CTXPKT		: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableForcedHostMode(bool enable = true)
		{
			Register::Ptr()->FHMOD = enable;
		}
	};
	*/
	
	// OTG_FS reset register (OTG_FS_GRSTCTL)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class ResetRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x10;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType CSRST		: 1;
			Core::RegisterValueType HSRST		: 1;
			Core::RegisterValueType FCRST		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType RXFFLSH		: 1;
			Core::RegisterValueType TXFFLSH		: 1;
			Core::RegisterValueType TXFNUM		: 5;
			Core::RegisterValueType 			: 20;
			Core::RegisterValueType AHBIDL		: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS core interrupt register (OTG_FS_GINTSTS)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class CoreInterruptRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x14;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType CMOD		: 1;
			Core::RegisterValueType MMIS		: 1;
			Core::RegisterValueType OTGINT		: 1;
			Core::RegisterValueType SOF			: 1;
			Core::RegisterValueType RXFLVL		: 1;
			Core::RegisterValueType NPTXFE		: 1;
			Core::RegisterValueType GINAKEFF	: 1;
			Core::RegisterValueType GOUTNAKEFF	: 1;
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType ESUSP		: 1;
			Core::RegisterValueType USBSUSP		: 1;
			Core::RegisterValueType USBRST		: 1;
			Core::RegisterValueType ENUMDNE		: 1;
			Core::RegisterValueType ISOODRP		: 1;
			Core::RegisterValueType EOPF 		: 1;
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType IEPINT		: 1;
			Core::RegisterValueType OEPINT		: 1;
			Core::RegisterValueType IISOIXFR	: 1;
			Core::RegisterValueType PXFR_INCOMPISOOUT			: 1;
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType HPRTINT		: 1;
			Core::RegisterValueType HCINT		: 1;
			Core::RegisterValueType PTXFE		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType CIDSCHG		: 1;
			Core::RegisterValueType DISCINT		: 1;
			Core::RegisterValueType SRQINT		: 1;
			Core::RegisterValueType WKUINT		: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static Mode GetMode()
		{
			return (Mode)Register::Ptr()->CMOD;
		}
		
		static bool IsHostPortInterrupt()
		{
			return Register::Ptr()->HPRTINT;
		}
		
		static void ClearFlags()
		{
			Core::RegisterValueType tmp = *((Core::RegisterValueType *)Register::Ptr());
			*((Core::RegisterValueType *)Register::Ptr()) = tmp;
		}
	};
	
	// OTG_FS interrupt mask register (OTG_FS_GINTMSK)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class InterruptMaskRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x18;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType MMISM		: 1;
			Core::RegisterValueType OTGINT		: 1;
			Core::RegisterValueType SOFM		: 1;
			Core::RegisterValueType RXFLVLM		: 1;
			Core::RegisterValueType NPTXFEM		: 1;
			Core::RegisterValueType GINAKEFFM	: 1;
			Core::RegisterValueType GONAKEFFM	: 1;
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType ESUSPM		: 1;
			Core::RegisterValueType USBSUSPM	: 1;
			Core::RegisterValueType USBRST		: 1;
			Core::RegisterValueType ENUMDNEM	: 1;
			Core::RegisterValueType ISOODRPM	: 1;
			Core::RegisterValueType EOPFM		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType EPMISM		: 1;
			Core::RegisterValueType IEPINT		: 1;
			Core::RegisterValueType OEPINT		: 1;
			Core::RegisterValueType IISOIXFRM	: 1;
			Core::RegisterValueType IPXFRM_IISOOXFRM 	: 1;
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType PRTIM		: 1;
			Core::RegisterValueType HCIM		: 1;
			Core::RegisterValueType PTXFEM		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType CIDSCHGM	: 1;
			Core::RegisterValueType DISCINT		: 1;
			Core::RegisterValueType SRQIM		: 1;
			Core::RegisterValueType WUIM		: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableModeMismatchInterrupt()
		{
			Register::Ptr()->MMISM = 1;
		}
		
		static void EnableOtgInterrupt()
		{
			Register::Ptr()->OTGINT = 1;
		}
		
		static void EnableHostPortInterrupt()
		{
			Register::Ptr()->PRTIM = 1;
		}
		
		static void EnableNonPeriodicTxFifoEmptyInterrupt()
		{
			Register::Ptr()->NPTXFEM = 1;
		}
		
		static void EnablePeriodicTxFifoEmptyInterrupt()
		{
			Register::Ptr()->PTXFEM = 1;
		}
	};
	
	// OTG_FS Receive status debug read/OTG status read and pop registers (OTG_FS_GRXSTSR/OTG_FS_GRXSTSP)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class ReceiveStatusRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffsetRead = 0x1C;
		static const Core::RegisterAddressType AddressOffsetPop = 0x20;
		static const Core::RegisterAddressType AddressRead = BaseAddress + AddressOffsetRead;
		static const Core::RegisterAddressType AddressPop = BaseAddress + AddressOffsetPop;
	protected:
		struct RegStructHostMode
		{
			Core::RegisterValueType CHNUM		: 4;
			Core::RegisterValueType BCNT		: 11;
			Core::RegisterValueType DPID		: 2;
			Core::RegisterValueType PKTSTS		: 4;
			Core::RegisterValueType 			: 11;
		};
		
		struct RegStructDeviceMode
		{
			Core::RegisterValueType CHNUM		: 4;
			Core::RegisterValueType BCNT		: 11;
			Core::RegisterValueType DPID		: 2;
			Core::RegisterValueType PKTSTS		: 4;
			Core::RegisterValueType FRMNUM		: 4;
			Core::RegisterValueType 			: 7;
		};
		
		typedef Register<AddressRead, RegStructHostMode> RegisterHostMode;
		typedef Register<AddressRead, RegStructDeviceMode> RegisterDeviceMode;
		typedef Register<AddressPop, unsigned int> RegisterPop;
	public:
		static void PopRxFifo()
		{
			unsigned int dummy = *RegisterPop::Ptr();
		}
	};
	
	// OTG_FS Receive FIFO size register (OTG_FS_GRXFSIZ)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class ReceiveFifoSizeRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x24;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType RXFD		: 16;
			Core::RegisterValueType 			: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS Host non-periodic transmit FIFO size register (OTG_FS_HNPTXFSIZ)/Endpoint 0 Transmit FIFO size (OTG_FS_DIEPTXF0)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class NonPeriodicOrEnpoint0TransmitFifoSizeRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x28;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStructHostMode
		{
			Core::RegisterValueType NPTXFSA		: 16;
			Core::RegisterValueType NPTXFD		: 16;
		};
		
		struct RegStructDeviceMode
		{
			Core::RegisterValueType TX0FSA		: 16;
			Core::RegisterValueType TX0FD		: 16;
		};
		
		typedef Register<Address, RegStructHostMode> RegisterHostMode;
		typedef Register<Address, RegStructDeviceMode> RegisterDeviceMode;
	public:
		
	};
	
	// OTG_FS non-periodic transmit FIFO/queue status register (OTG_FS_HNPTXSTS)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class NonPeriodicTransmitFifoQueueStatusRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x2C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType NPTXFSAV	: 16;
			Core::RegisterValueType NPTQXSAV	: 8;
			Core::RegisterValueType NPTXQTOP	: 7;
			Core::RegisterValueType 			: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS general core configuration register (OTG_FS_GCCFG)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class GeneralCoreConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x38;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType 			: 16;
			Core::RegisterValueType PWRDWN		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType VBUSASEN	: 1;
			Core::RegisterValueType VBUSBSEN	: 1;
			Core::RegisterValueType SOFOUTEN	: 1;
			Core::RegisterValueType NOVBUSSENS	: 1;
			Core::RegisterValueType 			: 10;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS core ID register (OTG_FS_CID)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class CodeIdRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x3C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType PRODUCT_ID	: 32;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS Host periodic transmit FIFO size register (OTG_FS_HPTXFSIZ)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostPeriodicTransmitFifoSizeRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x100;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType PTXSA		: 16;
			Core::RegisterValueType PTXFSIZ		: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS device IN endpoint transmit FIFO size register (OTG_FS_DIEPTXFx) (x = 1..3, where x is the FIFO_number)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class DeviceInTransmitFifoSizeRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x100;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType INEPTXSA		: 16;
			Core::RegisterValueType INEPTXFD		: 16;
		};
		
		typedef Register<Address + 4 * 0, RegStruct> Register1;
		typedef Register<Address + 4 * 1, RegStruct> Register2;
		typedef Register<Address + 4 * 2, RegStruct> Register3;
	public:
		
	};
	
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class Registers :
		public ControlAndStatusRegister<BaseAddress>,
		public InterruptRegister<BaseAddress>,
		public AHBConfigurationRegister<BaseAddress>,
		public USBConfigurationRegister<BaseAddress>,
		public ResetRegister<BaseAddress>,
		public CoreInterruptRegister<BaseAddress>,
		public InterruptMaskRegister<BaseAddress>,
		public ReceiveStatusRegister<BaseAddress>,
		public ReceiveFifoSizeRegister<BaseAddress>,
		public NonPeriodicOrEnpoint0TransmitFifoSizeRegister<BaseAddress>,
		public NonPeriodicTransmitFifoQueueStatusRegister<BaseAddress>,
		public GeneralCoreConfigurationRegister<BaseAddress>,
		public CodeIdRegister<BaseAddress>,
		public HostPeriodicTransmitFifoSizeRegister<BaseAddress>,
		public DeviceInTransmitFifoSizeRegister<BaseAddress>
	{
	};
}

namespace Host
{
	// OTG_FS Host configuration register (OTG_FS_HCFG)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x400;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType FSLSPCS		: 2;
			Core::RegisterValueType FSLSS		: 1;
			Core::RegisterValueType 			: 29;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS Host frame interval register (OTG_FS_HFIR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostFrameIntervalRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x404;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType FRIVL		: 16;
			Core::RegisterValueType 			: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS Host frame number/frame time remaining register (OTG_FS_HFNUM)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostFrameNumberFrameTimeRemainingRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x408;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType FRNUM		: 16;
			Core::RegisterValueType FTREM		: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS_Host periodic transmit FIFO/queue status register (OTG_FS_HPTXSTS)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostPeriodicTransmitFifoQueueStatusRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x410;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType PTXFSAVL	: 16;
			Core::RegisterValueType PTXQSAV		: 16;
			Core::RegisterValueType PTXQTOP		: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS Host all channels interrupt register (OTG_FS_HAINT)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostAllChannelsInterruptRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x414;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType HAINT		: 16;
			Core::RegisterValueType 			: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS Host all channels interrupt mask register (OTG_FS_HAINT)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostAllChannelsInterruptMaskRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x418;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType HAINTM		: 16;
			Core::RegisterValueType 			: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS Host port control and status register (OTG_FS_HPRT)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostPortControlAndStatusRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x440;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
		enum Speed
		{
			SpeedFull = 1,
			SpeedLow = 2
		};
	protected:
		struct RegStruct
		{
			Core::RegisterValueType PCSTS		: 1;
			Core::RegisterValueType PCDET		: 1;
			Core::RegisterValueType PENA		: 1;
			Core::RegisterValueType PENCHNG		: 1;
			Core::RegisterValueType POCA		: 1;
			Core::RegisterValueType POCCHNG		: 1;
			Core::RegisterValueType PRES		: 1;
			Core::RegisterValueType PSUSP		: 1;
			Core::RegisterValueType PRST		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType PLSTS		: 2;
			Core::RegisterValueType PPWR		: 1;
			Core::RegisterValueType PTCTL		: 4;
			Core::RegisterValueType PSPD		: 2;
			Core::RegisterValueType 			: 13;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void PowerOn(bool on)
		{
			Register::Ptr()->PPWR = on;
		}
		
		static bool IsDeviceConnecting()
		{
			return Register::Ptr()->PCDET;
		}
		
		static bool PortEnabled()
		{
			return Register::Ptr()->PENA;
		}
		
		static bool IsPortEnableChanging()
		{
			return Register::Ptr()->PENCHNG;
		}
		
		static void Reset(bool reset)
		{
			Register::Ptr()->PRST = reset;
		}
		
		static Speed GetSpeed()
		{
			return (Speed) Register::Ptr()->PSPD;
		}
		
		static void ClearFlags()
		{
			Core::RegisterValueType tmp = *((Core::RegisterValueType *)Register::Ptr());
			RegStruct *write = (RegStruct *)&tmp;
			write->PENA = 0; // не сбрасывать этот бит
			*((Core::RegisterValueType *)Register::Ptr()) = tmp;
		}
	};
	
	// OTG_FS Host channel-x characteristics register (OTG_FS_HCCHARx) (x = 0..7, where x = Channel_number)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostChannelCharacteristicsRegister
	{
	public:
		// Address offset: 0x500 + (Channel_number * 0x20)
		static const Core::RegisterAddressType AddressOffset = 0x500;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType MPSIZ		: 11;
			Core::RegisterValueType EPNUM		: 4;
			Core::RegisterValueType EPDIR		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType LSDEV		: 1;
			Core::RegisterValueType EPTYP		: 2;
			Core::RegisterValueType MCNT		: 2;
			Core::RegisterValueType DAD			: 7;
			Core::RegisterValueType ODDFRM		: 1;
			Core::RegisterValueType CHDIS		: 1;
			Core::RegisterValueType CHENA		: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS Host channel-x interrupt register (OTG_FS_HCINTx) (x = 0..7, where x = Channel_number)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostChannelInterruptRegister
	{
	public:
		// Address offset: 0x508 + (Channel_number * 0x20)
		static const Core::RegisterAddressType AddressOffset = 0x508;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType XFRC		: 1;
			Core::RegisterValueType CHH			: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType STALL		: 1;
			Core::RegisterValueType NAK			: 1;
			Core::RegisterValueType ACK			: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType TXERR		: 1;
			Core::RegisterValueType BBERR		: 1;
			Core::RegisterValueType FRMOR		: 1;
			Core::RegisterValueType DTERR		: 1;
			Core::RegisterValueType 			: 21;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS Host channel-x interrupt mask register (OTG_FS_HCINTMSKx) (x = 0..7, where x = Channel_number)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostChannelInterruptMaskRegister
	{
	public:
		// Address offset: 0x50C + (Channel_number * 0x20)
		static const Core::RegisterAddressType AddressOffset = 0x50C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType XFRCM		: 1;
			Core::RegisterValueType CHHM		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType STALLM		: 1;
			Core::RegisterValueType NAKM		: 1;
			Core::RegisterValueType ACKM		: 1;
			Core::RegisterValueType NYET		: 1;
			Core::RegisterValueType TXERRM		: 1;
			Core::RegisterValueType BBERRM		: 1;
			Core::RegisterValueType FRMORM		: 1;
			Core::RegisterValueType DTERRM		: 1;
			Core::RegisterValueType 			: 21;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	// OTG_FS Host channel-x transfer size register (OTG_FS_HCTSIZx) (x = 0..7, where x = Channel_number)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class HostChannelTransferSizeRegister
	{
	public:
		// Address offset: 0x510 + (Channel_number * 0x20)
		static const Core::RegisterAddressType AddressOffset = 0x510;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType XFRSIZ		: 19;
			Core::RegisterValueType PKTCNT		: 10;
			Core::RegisterValueType DPID		: 2;
			Core::RegisterValueType 			: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		
	};
	
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class Registers :
		public HostConfigurationRegister<BaseAddress>,
		public HostFrameIntervalRegister<BaseAddress>,
		public HostFrameNumberFrameTimeRemainingRegister<BaseAddress>,
		public HostPeriodicTransmitFifoQueueStatusRegister<BaseAddress>,
		public HostAllChannelsInterruptRegister<BaseAddress>,
		public HostAllChannelsInterruptMaskRegister<BaseAddress>,
		public HostPortControlAndStatusRegister<BaseAddress>,
		public HostChannelCharacteristicsRegister<BaseAddress>,
		public HostChannelInterruptRegister<BaseAddress>,
		public HostChannelInterruptMaskRegister<BaseAddress>,
		public HostChannelTransferSizeRegister<BaseAddress>
	{
	};
}

	template
		<
			int IdObj
		>
	class StaticParams
	{
	public:
		typedef CallbackWrapper<bool> PowerOnCallbackType;
		typedef CallbackWrapper<bool> OnDeviceConnectedCallbackType;
	public:
		static PowerOnCallbackType PowerOnCallback;
		static OnDeviceConnectedCallbackType OnDeviceConnectedCallback;
	protected:
	};
	
	template <int IdObj>
		StaticParams<IdObj>::PowerOnCallbackType StaticParams<IdObj>::PowerOnCallback;
	
	template <int IdObj>
		StaticParams<IdObj>::OnDeviceConnectedCallbackType StaticParams<IdObj>::OnDeviceConnectedCallback;

	template
		<
			unsigned int IdObj,
			Core::RegisterAddressType BaseAddress
		>
	class OtgFs :
		public IdObjectBase<IdObj>,
		public StaticParams<IdObj>
	{
	public:
		typedef Global::Registers<BaseAddress> GlobalRegisters;
		typedef Host::Registers<BaseAddress> HostRegisters;
		typedef StaticParams<IdObj> StaticParams;
		
		enum State
		{
			StateNotInited,
			StateInit,
			StateModeDetect,
			StateHostWaitConnection,
			StateHostStartReset,
			StateHostWaitReset,
			StateHostWaitEnableChanging,
			StateDeviceConnected
		};
	protected:
		static State _state;
		static unsigned int _counter;
	public:
		static void Init()
		{
			PowerOn(false);
			
			// 1. Program the following fields in the OTG_FS_GAHBCFG register:
			// Ц Global interrupt mask bit GINTMSK = 1
			// Ц RxFIFO non-empty (RXFLVL bit in OTG_FS_GINTSTS)
			// Ц Periodic TxFIFO empty level
			GlobalRegisters::GlobalInterruptEnable();
			
			// 2. Program the following fields in the OTG_FS_GUSBCFG register:
			// Ц HNP capable bit
			// Ц SRP capable bit
			// Ц FS timeout calibration field
			// Ц USB turnaround time field
			
			// 3. The software must unmask the following bits in the OTG_FS_GINTMSK register:
			// OTG interrupt mask
			// Mode mismatch interrupt mask
			GlobalRegisters::EnableOtgInterrupt();
			GlobalRegisters::EnableModeMismatchInterrupt();
			
			// 4. The software can read the CMOD bit in OTG_FS_GINTSTS to determine whether the
			// OTG_FS controller is operating in host or device mode.
			_state = StateInit;
		}
		
		static void OnTick(unsigned int tickFrequency)
		{
			switch(_state)
			{
			case StateInit:
				{
					_state = StateModeDetect;
					_counter = tickFrequency * 0.100;
				}
				break;
			case StateModeDetect:
				{
					if (_counter > 0)
					{
						_counter--;
					}
					else
					{
						Global::Mode mode = GlobalRegisters::GetMode();
						switch (mode)
						{
						case Global::ModeHost:
							{
								InitHostStage1();
							}
							break;
						}
					}
				}
				break;
			case StateHostStartReset:
				{
					_state = StateHostWaitReset;
					_counter = tickFrequency * 0.010;
					HostRegisters::Reset(true);
				}
				break;
			case StateHostWaitReset:
				{
					if (_counter > 0)
					{
						_counter--;
					}
					else
					{
						_state = StateHostWaitEnableChanging;
						HostRegisters::Reset(false);
					}
				}
				break;
			}
		}
		
		static void InterruptHandler()
		{
			GlobalRegisters::ClearFlags();
			
			if (GlobalRegisters::IsHostPortInterrupt())
			{
				if (HostRegisters::IsDeviceConnecting())
				{
					switch(_state)
					{
					case StateHostWaitConnection:
						{
							_state = StateHostStartReset;
						}
						break;
					}
				}
				
				if (HostRegisters::IsPortEnableChanging())
				{
					//HostRegisters::PortEnableChangingClear();
					
					switch(_state)
					{
					case StateHostWaitEnableChanging:
						{
							if (HostRegisters::PortEnabled())
							{
								InitHostStage2();
								OnDeviceConnected();
							}
						}
						break;
					case StateDeviceConnected:
						{
							if (!HostRegisters::PortEnabled())
							{
								OnDeviceDisconnected();
							}
						}
						break;
					}
				}
				
				HostRegisters::ClearFlags();
			}
		}
	protected:
		
		static void PowerOn(bool on)
		{
			StaticParams::PowerOnCallback(on);
		}
		
		static void OnDeviceConnected()
		{
			_state = StateDeviceConnected;
			StaticParams::OnDeviceConnectedCallback(true);
		}
		
		static void OnDeviceDisconnected()
		{
			_state = StateHostWaitConnection;
			StaticParams::OnDeviceConnectedCallback(false);
		}
			
		static void InitHostStage1()
		{
			// 1. Program the HPRTINT in the OTG_FS_GINTMSK register to unmask
			GlobalRegisters::EnableHostPortInterrupt();
			
			// 2. Program the OTG_FS_HCFG register to select full-speed host
			// оставл€ем по умолчанию
			
			// 3. Program the PPWR bit in OTG_FS_HPRT to 1. This drives VBUS on the USB.
			_state = StateHostWaitConnection;
			HostRegisters::PowerOn(true);
			PowerOn(true);
			// 4. Wait for the PCDET interrupt in OTG_FS_HPRT0. This indicates that a device is
			// connecting to the port.
			// 5. Program the PRST bit in OTG_FS_HPRT to 1. This starts the reset process.
			// 6. Wait at least 10 ms for the reset process to complete.
			// 7. Program the PRST bit in OTG_FS_HPRT to 0.
			// 8. Wait for the PENCHNG interrupt in OTG_FS_HPRT.
			
		}
		
		static void InitHostStage2()
		{
			// 9. Read the PSPD bit in OTG_FS_HPRT to get the enumerated speed.
			typename HostRegisters::Speed speed = HostRegisters::GetSpeed();
			// 10. Program the HFIR register with a value corresponding to the selected PHY clock 1
			// 11. Program the FSLSPCS field in the OTG_FS_HCFG register following the speed of the
			// device detected in step 9. If FSLSPCS has been changed a port reset must be
			// performed.
			// 12. Program the OTG_FS_GRXFSIZ register to select the size of the receive FIFO.
			// 13. Program the OTG_FS_HNPTXFSIZ register to select the size and the start address of
			// the Non-periodic transmit FIFO for non-periodic transactions.
			// 14. Program the OTG_FS_HPTXFSIZ register to select the size and start address of the
			// periodic transmit FIFO for periodic transactions.
			
			/*
			// The application must initialize one or more channels before it can communicate with
			// connected devices. To initialize and enable a channel, the application must perform the
			// following steps:
			// 1. Program the OTG_FS_GINTMSK register to unmask the following:
			// 2. Channel interrupt
			// Ц Non-periodic transmit FIFO empty for OUT transactions (applicable when
			// operating in pipelined transaction-level with the packet count field programmed
			// with more than one).
			GlobalRegisters::EnableNonPeriodicTxFifoEmptyInterrupt();
			// Ц Non-periodic transmit FIFO half-empty for OUT transactions (applicable when
			// operating in pipelined transaction-level with the packet count field programmed
			// with more than one).
			GlobalRegisters::EnablePeriodicTxFifoEmptyInterrupt();*/
		}
	};
	
	template <unsigned int IdObj, Core::RegisterAddressType BaseAddress>
		OtgFs<IdObj, BaseAddress>::State OtgFs<IdObj, BaseAddress>::_state = OtgFs<IdObj, BaseAddress>::StateNotInited;
	
	template <unsigned int IdObj, Core::RegisterAddressType BaseAddress>
		unsigned int OtgFs<IdObj, BaseAddress>::_counter = 0;
	
	class Usb
	{
	public:
		typedef OtgFs<1, MemoryMap::USBOTGFS> OtgFs;
	};
}

typedef UsbImplementation::Usb Usb;

}
}
}

#endif