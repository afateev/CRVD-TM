#ifndef TIMER_STM32_F4_H
#define TIMER_STM32_F4_H

#include "Register.h"
#include "../../IdObjectBase.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Stm32
{
namespace F4
{
namespace TimerImplementation
{
	// TIM1&TIM8 control register 1 (TIMx_CR1)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIM1_8Control1Register
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x00;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType CEN			: 1;
			Core::RegisterValueType UDIS		: 1;
			Core::RegisterValueType URS			: 1;
			Core::RegisterValueType OPM			: 1;
			Core::RegisterValueType DIR			: 1;
			Core::RegisterValueType CMS			: 2;
			Core::RegisterValueType ARPE		: 1;
			Core::RegisterValueType CKD			: 2;
			Core::RegisterValueType 			: 22;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Start()
		{
			Register::Ptr()->CEN = 1;
		}
		
		static void Stop()
		{
			Register::Ptr()->CEN = 1;
		}
	};
	
	// TIMx control register 1 (TIMx_CR1)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIMControl1Register
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x00;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType CEN			: 1;
			Core::RegisterValueType UDIS		: 1;
			Core::RegisterValueType URS			: 1;
			Core::RegisterValueType OPM			: 1;
			Core::RegisterValueType DIR			: 1;
			Core::RegisterValueType CMS			: 2;
			Core::RegisterValueType ARPE		: 1;
			Core::RegisterValueType CKD			: 2;
			Core::RegisterValueType 			: 22;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Start()
		{
			Register::Ptr()->CEN = 1;
		}
		
		static void Stop()
		{
			Register::Ptr()->CEN = 1;
		}
	};
	
	// TIM1&TIM8 DMA/interrupt enable register (TIMx_DIER)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIM1_8DmaInterruptEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x0C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType UIE			: 1;
			Core::RegisterValueType CC1IE		: 1;
			Core::RegisterValueType CC2IE		: 1;
			Core::RegisterValueType CC3IE		: 1;
			Core::RegisterValueType CC4IE		: 1;
			Core::RegisterValueType COMIE		: 1;
			Core::RegisterValueType TIE			: 1;
			Core::RegisterValueType BIE			: 1;
			Core::RegisterValueType UDE			: 1;
			Core::RegisterValueType CC1DE		: 1;
			Core::RegisterValueType CC2DE		: 1;
			Core::RegisterValueType CC3DE		: 1;
			Core::RegisterValueType CC4DE		: 1;
			Core::RegisterValueType COMDE		: 1;
			Core::RegisterValueType TDE			: 1;
			Core::RegisterValueType 			: 17;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableUpdateInterrupt(bool enable = true)
		{
			Register::Ptr()->UIE = enable;
		}
		
		static bool IsEnabledUpdateInterrupt(bool enable = true)
		{
			return Register::Ptr()->UIE;
		}
	};
	
	// TIMx DMA/Interrupt enable register (TIMx_DIER)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIMDmaInterruptEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x0C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType UIE			: 1;
			Core::RegisterValueType CC1IE		: 1;
			Core::RegisterValueType CC2IE		: 1;
			Core::RegisterValueType CC3IE		: 1;
			Core::RegisterValueType CC4IE		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType TIE			: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType UDE			: 1;
			Core::RegisterValueType CC1DE		: 1;
			Core::RegisterValueType CC2DE		: 1;
			Core::RegisterValueType CC3DE		: 1;
			Core::RegisterValueType CC4DE		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType TDE			: 1;
			Core::RegisterValueType 			: 17;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableUpdateInterrupt(bool enable = true)
		{
			Register::Ptr()->UIE = enable;
		}
		
		static bool IsEnabledUpdateInterrupt(bool enable = true)
		{
			return Register::Ptr()->UIE;
		}
	};
	
	// TIM1&TIM8 status register (TIMx_SR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIM1_8StatusRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x10;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType UIF			: 1;
			Core::RegisterValueType CC1IF		: 1;
			Core::RegisterValueType CC2IF		: 1;
			Core::RegisterValueType CC3IF		: 1;
			Core::RegisterValueType CC4IF		: 1;
			Core::RegisterValueType COMIF		: 1;
			Core::RegisterValueType TIF			: 1;
			Core::RegisterValueType BIF			: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType CC1OF		: 1;
			Core::RegisterValueType CC2OF		: 1;
			Core::RegisterValueType CC3OF		: 1;
			Core::RegisterValueType CC4OF		: 1;
			Core::RegisterValueType 			: 19;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static bool GetUpdateInterruptFlag()
		{
			return Register::Ptr()->UIF;
		}
		
		static void ClearUpdateInterruptFlag()
		{
			Register::Ptr()->UIF = 0;
		}
	};
	
	// TIMx status register (TIMx_SR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIMStatusRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x10;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType UIF			: 1;
			Core::RegisterValueType CC1IF		: 1;
			Core::RegisterValueType CC2IF		: 1;
			Core::RegisterValueType CC3IF		: 1;
			Core::RegisterValueType CC4IF		: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType TIF			: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType 			: 1;
			Core::RegisterValueType CC1OF		: 1;
			Core::RegisterValueType CC2OF		: 1;
			Core::RegisterValueType CC3OF		: 1;
			Core::RegisterValueType CC4OF		: 1;
			Core::RegisterValueType 			: 19;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static bool GetUpdateInterruptFlag()
		{
			return Register::Ptr()->UIF;
		}
		
		static void ClearUpdateInterruptFlag()
		{
			Register::Ptr()->UIF = 0;
		}
	};
	
	// TIMx capture/compare mode register 1 (TIMx_CCMR1)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIMCaptureCompareMode1Register
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x18;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		union RegStruct
		{
			struct
			{
				Core::RegisterValueType CC1S		: 2;
				Core::RegisterValueType OC1FE		: 1;
				Core::RegisterValueType OC1PE		: 1;
				Core::RegisterValueType OC1M		: 3;
				Core::RegisterValueType OC1CE		: 1;
				Core::RegisterValueType CC2S		: 2;
				Core::RegisterValueType OC2FE		: 1;
				Core::RegisterValueType OC2PE		: 1;
				Core::RegisterValueType OC2M		: 3;
				Core::RegisterValueType OC2CE		: 1;
				Core::RegisterValueType 			: 16;
			} OutputMode;
			
			struct
			{
				Core::RegisterValueType CC1S		: 2;
				Core::RegisterValueType IC1PSC		: 2;
				Core::RegisterValueType IC1F		: 4;
				Core::RegisterValueType CC2S		: 2;
				Core::RegisterValueType IC2PSC		: 2;
				Core::RegisterValueType IC2F		: 4;
				Core::RegisterValueType 			: 16;
			} InputMode;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void CC1SetConfig(Core::RegisterValueType val)
		{
			Register::Ptr()->OutputMode.CC1S = val;
		}
		
		static void CC1SetMode(Core::RegisterValueType val)
		{
			Register::Ptr()->OutputMode.OC1M = val;
		}
		
		static void CC2SetConfig(Core::RegisterValueType val)
		{
			Register::Ptr()->OutputMode.CC2S = val;
		}
		
		static void CC2SetMode(Core::RegisterValueType val)
		{
			Register::Ptr()->OutputMode.OC2M = val;
		}
	};
	
	// TIMx capture/compare mode register 2 (TIMx_CCMR2)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIMCaptureCompareMode2Register
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x1C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		union RegStruct
		{
			struct
			{
				Core::RegisterValueType CC3S		: 2;
				Core::RegisterValueType OC3FE		: 1;
				Core::RegisterValueType OC3PE		: 1;
				Core::RegisterValueType OC3M		: 3;
				Core::RegisterValueType OC3CE		: 1;
				Core::RegisterValueType CC4S		: 2;
				Core::RegisterValueType OC4FE		: 1;
				Core::RegisterValueType OC4PE		: 1;
				Core::RegisterValueType OC4M		: 3;
				Core::RegisterValueType OC4CE		: 1;
				Core::RegisterValueType 			: 16;
			} OutputMode;
			
			struct
			{
				Core::RegisterValueType CC3S		: 2;
				Core::RegisterValueType IC3PSC		: 2;
				Core::RegisterValueType IC3F		: 4;
				Core::RegisterValueType CC4S		: 2;
				Core::RegisterValueType IC4PSC		: 2;
				Core::RegisterValueType IC4F		: 4;
				Core::RegisterValueType 			: 16;
			} InputMode;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void CC3SetConfig(Core::RegisterValueType val)
		{
			Register::Ptr()->OutputMode.CC3S = val;
		}
		
		static void CC3SetMode(Core::RegisterValueType val)
		{
			Register::Ptr()->OutputMode.OC3M = val;
		}
		
		static void CC4SetConfig(Core::RegisterValueType val)
		{
			Register::Ptr()->OutputMode.CC4S = val;
		}
		
		static void CC4SetMode(Core::RegisterValueType val)
		{
			Register::Ptr()->OutputMode.OC4M = val;
		}
	};
	
	// TIMx capture/compare enable register (TIMx_CCER)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIMCaptureCompareEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x20;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType CC1E		: 1;
			Core::RegisterValueType CC1P		: 1;
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType CC2E		: 1;
			Core::RegisterValueType CC2P		: 1;
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType CC3E		: 1;
			Core::RegisterValueType CC3P		: 1;
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType CC4E		: 1;
			Core::RegisterValueType CC4P		: 1;
			Core::RegisterValueType 			: 2;
			Core::RegisterValueType 			: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void CC1Enable(bool enable = true)
		{
			Register::Ptr()->CC1E = enable;
		}
		
		static void CC2Enable(bool enable = true)
		{
			Register::Ptr()->CC2E = enable;
		}
		
		static void CC3Enable(bool enable = true)
		{
			Register::Ptr()->CC3E = enable;
		}
		
		static void CC4Enable(bool enable = true)
		{
			Register::Ptr()->CC4E = enable;
		}
	};
	
	// TIM1&TIM8 counter (TIMx_CNT)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIM1_8CounterRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x24;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType CounterValue		: 16;
			Core::RegisterValueType 					: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static Core::RegisterValueType GetCounter()
		{
			return Register::Ptr()->CounterValue;
		}
		
		static void SetCounter(Core::RegisterValueType val)
		{
			Register::Ptr()->CounterValue = val;
		}
	};
	
	// TIMx counter (TIMx_CNT)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIMCounterRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x24;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType CounterValue		: 32;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static Core::RegisterValueType GetCounter()
		{
			return Register::Ptr()->CounterValue;
		}
		
		static void SetCounter(Core::RegisterValueType val)
		{
			Register::Ptr()->CounterValue = val;
		}
	};
	
	// TIM1&TIM8 prescaler (TIMx_PSC)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIM1_8PrescalerRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x28;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType PrescalerValue		: 16;
			Core::RegisterValueType 					: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static Core::RegisterValueType GetPrescaler()
		{
			return Register::Ptr()->PrescalerValue + 1;
		}
		
		static void SetPrescaler(Core::RegisterValueType val)
		{
			Register::Ptr()->PrescalerValue = val - 1;
		}
	};
	
	// TIMx prescaler (TIMx_PSC)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIMPrescalerRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x28;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType PrescalerValue		: 16;
			Core::RegisterValueType 					: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static Core::RegisterValueType GetPrescaler()
		{
			return Register::Ptr()->PrescalerValue + 1;
		}
		
		static void SetPrescaler(Core::RegisterValueType val)
		{
			Register::Ptr()->PrescalerValue = val - 1;
		}
	};
	
	// TIM1&TIM8 auto-reload register (TIMx_ARR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIM1_8AutoReloadRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x2C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType AutoReloadValue		: 16;
			Core::RegisterValueType 					: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetMaxCount(Core::RegisterValueType val)
		{
			Register::Ptr()->AutoReloadValue = val;
		}
	};
	
	// TIMx auto-reload register (TIMx_ARR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class TIMAutoReloadRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x2C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType AutoReloadValue		: 32;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetMaxCount(Core::RegisterValueType val)
		{
			Register::Ptr()->AutoReloadValue = val;
		}
	};
	
	template
		<
			int IdObj
		>
	class StaticParams
	{
	public:
		typedef CallbackWrapper<> UpdateHandlerType;
	public:
		static UpdateHandlerType UpdateInterruptHandler;
	};
	
	template <int IdObj>
		StaticParams<IdObj>::UpdateHandlerType StaticParams<IdObj>::UpdateInterruptHandler;
	
	template
		<
			unsigned int IdObj,
			Core::RegisterAddressType BaseAddress
		>
	class Timer1_8 :
		public IdObjectBase<IdObj>,
		public TIM1_8Control1Register<BaseAddress>,
		public TIM1_8DmaInterruptEnableRegister<BaseAddress>,
		public TIM1_8StatusRegister<BaseAddress>,
		public TIM1_8CounterRegister<BaseAddress>,
		public TIM1_8PrescalerRegister<BaseAddress>,
		public TIM1_8AutoReloadRegister<BaseAddress>,
		public StaticParams<IdObj>
	{
	public:
		typedef TIM1_8StatusRegister<BaseAddress> StatusRegister;
		typedef StaticParams<IdObj> StaticParams;
	public:
		static void InterruptHandler_BRK()
		{
		}
		
		static void InterruptHandler_UP()
		{
			StatusRegister::ClearUpdateInterruptFlag();
			StaticParams::UpdateInterruptHandler();
		}
		
		static void InterruptHandler_TRG_COM()
		{
		}
		
		static void InterruptHandler_CC()
		{
		}
	};
	
	template
		<
			unsigned int IdObj,
			Core::RegisterAddressType BaseAddress
		>
	class TimerFrom2to5 :
		public IdObjectBase<IdObj>,
		public TIMControl1Register<BaseAddress>,
		public TIMDmaInterruptEnableRegister<BaseAddress>,
		public TIMStatusRegister<BaseAddress>,
		public TIMCaptureCompareMode1Register<BaseAddress>,
		public TIMCaptureCompareMode2Register<BaseAddress>,
		public TIMCaptureCompareEnableRegister<BaseAddress>,
		public TIMCounterRegister<BaseAddress>,
		public TIMPrescalerRegister<BaseAddress>,
		public TIMAutoReloadRegister<BaseAddress>,
		public StaticParams<IdObj>
	{
	public:
		typedef TIMStatusRegister<BaseAddress> StatusRegister;
		typedef StaticParams<IdObj> StaticParams;
	public:
		static void InterruptHandler()
		{
			if (StatusRegister::GetUpdateInterruptFlag())
			{
				StatusRegister::ClearUpdateInterruptFlag();
				StaticParams::UpdateInterruptHandler();
			}
		}
	};
	
	class Timer
	{
	public:
		typedef Timer1_8<1, 0x40010000> Timer1;
		typedef TimerFrom2to5<2, 0x40000000> Timer2;
		typedef TimerFrom2to5<3, 0x40000400> Timer3;
		typedef TimerFrom2to5<4, 0x40000800> Timer4;
		typedef TimerFrom2to5<5, 0x40000C00> Timer5;
		typedef Timer1_8<8, 0x40010400> Timer8;
	};
}

typedef TimerImplementation::Timer Timer;
	
}
}
}

#endif