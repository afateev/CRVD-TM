#ifndef DMA_EMF32_H
#define DMA_EMF32_H

#include "Register.h"
#include "../Utils/CallbackListWrapper.h"

namespace Rblib
{
namespace Efm32
{
namespace Dma
{
	// Тип значения регистра
	typedef unsigned int ValueDataType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;
	
	// DMA_CONFIG - DMA Configuration Register
	template
		<
			RegisterAddressType Address
		>
	class ConfigurationRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Enable						: 1;
			ValueDataType 								: 4;
			ValueDataType ChannelProtectionControl		: 1;
			ValueDataType 								: 26;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Enable(bool enable = true)
		{
			Register::Ptr()->Enable = enable;
		}
	};
	
	// DMA_CTRLBASE - Channel Control Data Base Pointer Register
	template
		<
			RegisterAddressType Address
		>
	class ChannelControlDataBasePointerRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void SetControlDataBasePointer(ValueDataType p)
		{
			*Register::Ptr() = p;
		}
	};
	
	// DMA_CHSWREQ - Channel Software Request Register
	template
		<
			RegisterAddressType Address
		>
	class ChannelSoftwareRequestRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void ChannelSoftwareRequest(unsigned char channel)
		{
			*Register::Ptr() = (1 << channel);
		}
	};
	
	// DMA_CHENS - Channel Enable Set Register
	template
		<
			RegisterAddressType Address
		>
	class ChannelEnableSetRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void ChannelEnable(unsigned char channel)
		{
			*Register::Ptr() = (1 << channel);
		}
		
		static bool ChannelEnabled(unsigned char channel)
		{
			return *Register::Ptr() & (1 << channel);
		}
	};
	
	// DMA_IF - Interrupt Flag Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static bool Done(unsigned char channel)
		{
			return *Register::Ptr() & (1 << channel);
		}
	};
	
	// DMA_IFC - Interrupt Flag Clear Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagClearRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void DoneClear(unsigned char channel)
		{
			*Register::Ptr() = (1 << channel);
		}
	};
	
	// DMA_IEN - Interrupt Enable Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptEnableRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void CompleteInterruptEnable(unsigned char channel)
		{
			*Register::Ptr() |= (1 << channel);
		}
		
		static void CompleteInterruptDisable(unsigned char channel)
		{
			*Register::Ptr() &= ~(1 << channel);
		}
		
		static void ErrorInterruptEnable()
		{
			*Register::Ptr() |= (1 << 30);
		}
		
		static void ErrorInterruptDisable()
		{
			*Register::Ptr() &= ~(1 << 30);
		}
	};
	
	// DMA_CHx_CTRL - Channel Control Register
	template
		<
			RegisterAddressType Address
		>
	class ChannelControlRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType SignalSelect					: 4;
			ValueDataType 								: 12;
			ValueDataType SourceSelect					: 6;
			ValueDataType 								: 10;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetSignalSource(unsigned char channel, unsigned char source, unsigned char signal)
		{
			Register::Ptr()[channel].SignalSelect = signal;
			Register::Ptr()[channel].SourceSelect = source;
		}
	};
	
	// Хранилище дескрипторов каналов
	template
		<
			unsigned char Stride
		>
	class ChannelControlData
	{
	public:
		enum CycleType
		{
			CycleTypeStop								= 0,
			CycleTypeBasic								= 1,
			CycleTypeAutoRequest						= 2,
			CycleTypePingPong							= 3,
			CycleTypeMemoryScatterGatherPrimary			= 4,
			CycleTypeMemoryScatterGatherAlternate		= 5,
			CycleTypePeripheralScatterGatherPrimary		= 6,
			CycleTypePeripheralScatterGatherAlternate	= 7
		};
		
		enum DataSize
		{
			DataSizeByte = 0,
			DataSizeHalfword = 1,
			DataSizeWord		= 2			
		};
		
		enum DataIncrement
		{
			DataIncrementByte 		= 0,
			DataIncrementHalfword 	= 1,
			DataIncrementWord		= 2,
			DataIncrementNone		= 3
		};
		
		struct ChannelControlDataStructure
		{
			void *SrcEnd;
			void *DstEnd;
			
			struct
			{
				ValueDataType CycleCtrl			: 3;
				ValueDataType NextUseburst		: 1;
				ValueDataType NMinus1			: 10;
				ValueDataType RPower			: 4;
				ValueDataType SrcProtCtrl		: 3;
				ValueDataType DstProtCtrl		: 3;
				ValueDataType SrcSize			: 2;
				ValueDataType SrcInc			: 2;
				ValueDataType DstSize			: 2;
				ValueDataType DstInc			: 2;
			} Control;
			
			ValueDataType Reserved;
			
			void Set(void *src, void *dst, unsigned short count, CycleType cycleType = CycleTypeBasic, DataIncrement srcInc = DataIncrementByte, DataIncrement dstInc = DataIncrementByte, DataSize dataSize = DataSizeByte)
			{
				Control.NMinus1 = count - 1;
				if (srcInc == DataIncrementNone)
				{
					SrcEnd = src;
				}
				else
				{
					SrcEnd = (void *)((unsigned int)src + (Control.NMinus1 << srcInc));
				}
				
				if (dstInc == DataIncrementNone)
				{
					DstEnd = dst;
				}
				else
				{
					DstEnd = (void *)((unsigned int)dst + (Control.NMinus1 << dstInc));
				}
				
				Control.CycleCtrl = cycleType;
				Control.RPower = 0;
				Control.SrcSize = dataSize;
				Control.SrcInc = srcInc;
				Control.DstSize = dataSize;
				Control.DstInc = dstInc;
			}
		};
	protected:
		// дескрипторы должны храниться по адресу, заканчивающемуся на 00, поэтому вставляем фиксированный адрес
#ifdef EFM32		
		#pragma location=0x20000000
		__no_init
#endif
			static ChannelControlDataStructure _channelControlData[Stride * 2];
	public:
		static ChannelControlDataStructure *GetDescriptorPtr(unsigned char channel, bool alternate)
		{
			ChannelControlDataStructure *res = 0;
			
			unsigned char i = channel;
			if (alternate)
			{
				i += Stride;
			}
			
			if (i < sizeof(_channelControlData) / sizeof(ChannelControlDataStructure))
			{
				res = &_channelControlData[i];
			}
			
			return res;
		}
	};
	
	
	template<unsigned char CannelsCount>
		ChannelControlData<CannelsCount>::ChannelControlDataStructure ChannelControlData<CannelsCount>::_channelControlData[CannelsCount * 2];
	
	template
		<
			int IdObj
		>
	class StaticParams
	{
	public:
		typedef CallbackListWrapper<16, int> HandlerType;
	public:
		static HandlerType CompleteInterruptHandler;
	};
	
	template <int IdObj>
		StaticParams<IdObj>::HandlerType StaticParams<IdObj>::CompleteInterruptHandler;
	
	template
		<
			RegisterAddressType BaseAddr,
			RegisterAddressType StatusRegAddr,
			RegisterAddressType ConfigurationRegAddr,
			RegisterAddressType ChannelControlDataBasePointerRegAddr,
			RegisterAddressType ChannelAlternateControlDataBasePointerRegAddr,
			RegisterAddressType ChannelWaitOnRequestStatusRegAddr,
			RegisterAddressType ChannelSoftwareRequestRegAddr,
			RegisterAddressType ChannelUseburstSetRegAddr,
			RegisterAddressType ChannelUseburstClearRegAddr,
			RegisterAddressType ChannelRequestMaskSetRegAddr,
			RegisterAddressType ChannelRequestMaskClearRegAddr,
			RegisterAddressType ChannelEnableSetRegAddr,
			RegisterAddressType ChannelEnableClearRegAddr,
			RegisterAddressType ChannelAlternateSetRegAddr,
			RegisterAddressType ChannelAlternateClearRegAddr,
			RegisterAddressType ChannelPrioritySetRegAddr,
			RegisterAddressType ChannelPriorityClearRegAddr,
			RegisterAddressType BusErrorClearRegAddr,
			RegisterAddressType ChannelRequestStatusRegAddr,
			RegisterAddressType ChannelSingleRequestStatusRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr,
			RegisterAddressType InterruptEnableRegAddr,
			RegisterAddressType ControlRegAddr,
			RegisterAddressType RetainDescriptorStateRegAddr,
			RegisterAddressType Channel0LoopRegAddr,
			RegisterAddressType Channel1LoopRegAddr,
			RegisterAddressType Channel0RectangleRegAddr,
			RegisterAddressType ChannelControlRegAddr
		>
	class Dma :
		public ChannelSoftwareRequestRegister<BaseAddr + ChannelSoftwareRequestRegAddr>,
		public ChannelEnableSetRegister<BaseAddr + ChannelEnableSetRegAddr>,
		public InterruptFlagRegister<BaseAddr + InterruptFlagRegAddr>,
		public InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr>,
		public InterruptEnableRegister<BaseAddr + InterruptEnableRegAddr>,
		public ChannelControlRegister<BaseAddr + ChannelControlRegAddr>,
		public ChannelControlData<16>,
		public StaticParams<0>
	{
	protected:
		typedef ConfigurationRegister<BaseAddr + ConfigurationRegAddr> ConfigurationRegister;
		typedef ChannelControlDataBasePointerRegister<BaseAddr + ChannelControlDataBasePointerRegAddr> ChannelControlDataBasePointerRegister;
		typedef InterruptFlagRegister<BaseAddr + InterruptFlagRegAddr> InterruptFlagRegister;
		typedef InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr> InterruptFlagClearRegister;
		typedef StaticParams<0> StaticParams;
		
	public:
		static void Enable(bool enable = true)
		{
			ConfigurationRegister::Enable(enable);
			ValueDataType controlDataAddr = (ValueDataType)_channelControlData;
			memset(_channelControlData, 0, sizeof(_channelControlData));
			ChannelControlDataBasePointerRegister::SetControlDataBasePointer(controlDataAddr);
		}
		
		static void SetupChannel(unsigned char channel, bool alternate, void *src, void *dst, unsigned short count,
									  CycleType cycleType = CycleTypeBasic, DataIncrement srcInc = DataIncrementByte, DataIncrement dstInc = DataIncrementByte, DataSize dataSize = DataSizeByte)
		{
			ChannelControlDataStructure *desc = GetDescriptorPtr(channel, alternate);
			desc->Set(src, dst, count, cycleType, srcInc, dstInc, dataSize);
		}
		
		static void MemCpy(void *dst, void *src, unsigned short count, unsigned char useChannel)
		{
			SetupChannel(useChannel, 0, src, dst, count, Dma::CycleTypeAutoRequest);
			Dma::ChannelEnable(useChannel);
			Dma::ChannelSoftwareRequest(useChannel);
	
			while (!Dma::Done(useChannel))
			{
				;
			}
			
			Dma::DoneClear(useChannel);
		}
		
		static void InterruptHandler()
		{
			for (unsigned char ch = 0; ch < 12; ch ++)
			{
				bool flag = InterruptFlagRegister::Done(ch);
				if (flag)
				{
					InterruptFlagClearRegister::DoneClear(ch);
					StaticParams::CompleteInterruptHandler.Call(ch);
				}
			}
		}
	};
}
}
}

#endif