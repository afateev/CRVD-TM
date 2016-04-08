#ifndef ADC_EMF32_H
#define ADC_EMF32_H

#include "Register.h"
#include "../IdObjectBase.h"

namespace Rblib
{
namespace Efm32
{
namespace Adc
{
	// Тип значения регистра
	typedef unsigned int ValueDataType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;
	
	// ADCn_CTRL - Control Register
	template
		<
			RegisterAddressType Address
		>
	class ControlRegister
	{
	public:
		enum WarmUpMode
		{
			WarmUpModeNORMAL				= 0,
			WarmUpModeFASTBG				= 1,
			WarmUpModeKEEPSCANREFWARM		= 2,
			WarmUpModeKEEPADCWARM			= 3
		};
		
		enum LowPassFilterMode
		{
			LowPassFilterModeBYPASS		= 0,
			LowPassFilterModeDECAP		= 1,
			LowPassFilterModeRCFILT		= 2
		};
		
		enum OversampleRate
		{
			OversampleRateX2		= 0,
			OversampleRateX4		= 1,
			OversampleRateX8		= 2,
			OversampleRateX16		= 3,
			OversampleRateX32		= 4,
			OversampleRateX64		= 5,
			OversampleRateX128		= 6,
			OversampleRateX256		= 7,
			OversampleRateX512		= 8,
			OversampleRateX1024		= 9,
			OversampleRateX2048		= 10,
			OversampleRateX4096		= 11
		};
	protected:
		struct RegStruct
		{
			ValueDataType WarmUpMode				: 2;
			ValueDataType 							: 1;
			ValueDataType ConversionTailgating		: 1;
			ValueDataType LowPassFilterMode			: 2;
			ValueDataType 							: 2;
			ValueDataType PrescalerSetting			: 7;
			ValueDataType 							: 1;
			ValueDataType TimeBase					: 5;
			ValueDataType 							: 3;
			ValueDataType 							: 1;
			ValueDataType OversampleRateSelect		: 4;
			ValueDataType 							: 4;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetWarmUpMode(WarmUpMode val)
		{
			Register::Ptr()->WarmUpMode = val;
		}
		
		static void EnableConversionTailgating(bool enable = true)
		{
			Register::Ptr()->ConversionTailgating = enable;
		}
		
		static void SetLowPassFilterMode(LowPassFilterMode val)
		{
			Register::Ptr()->LowPassFilterMode = val;
		}
		
		static void SetPrescaler(unsigned int val)
		{
			
			if (val < 1)
			{
				val = 0;
			}
			else
			{
				val -= 1;
			}
			// Clock division factor of PRESC+1
			// поэтому параметр переданный в функцию уменьшаем на 1, чтобы получить желаемый делитель
			Register::Ptr()->PrescalerSetting = val;
		}
		
		static void SetTimeBase(unsigned int val)
		{
			if (val < 1)
			{
				val = 0;
			}
			else
			{
				val -= 1;
			}
			
			if (val > 0x1F)
			{
				val = 0x1F;
			}
			// ADC warm-up is set to TIMEBASE+1 HFPERCLK clock cycles
			// поэтому параметр переданный в функцию уменьшаем на 1, чтобы получить желаемое количество циклов
			Register::Ptr()->TimeBase = val;
		}
		
		static void SetOversampleRate(OversampleRate val)
		{
			Register::Ptr()->OversampleRateSelect = val;
		}
	};
	
	// ADCn_CMD - Command Register
	template
		<
			RegisterAddressType Address
		>
	class CommandRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType SingleConversionStart		: 1;
			ValueDataType SingleConversionStop		: 1;
			ValueDataType ScanSequenceStart			: 1;
			ValueDataType ScanSequenceStop			: 1;
			ValueDataType				 			: 28;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SingleConversionStart()
		{
			Register::Ptr()->SingleConversionStart = 1;
		}
		
		static void SingleConversionStop()
		{
			Register::Ptr()->SingleConversionStop = 1;
		}
		
		static void ScanSequenceStart()
		{
			Register::Ptr()->ScanSequenceStart = 1;
		}
		
		static void ScanSequenceStop()
		{
			Register::Ptr()->ScanSequenceStop = 1;
		}
	};
	
	// ADCn_STATUS - Status Register
	template
		<
			RegisterAddressType Address
		>
	class StatusRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType SingleConversionActive		: 1;
			ValueDataType ScanConversionActive			: 1;
			ValueDataType 								: 6;
			ValueDataType SingleReferenceWarmedUp		: 1;
			ValueDataType ScanReferenceWarmedUp			: 1;
			ValueDataType 								: 2;
			ValueDataType ADCWarmedUp					: 1;
			ValueDataType 								: 3;
			ValueDataType SingleSampleDataValid			: 1;
			ValueDataType ScanDataValid					: 1;
			ValueDataType 								: 6;
			ValueDataType ScanDataSource				: 3;
			ValueDataType				 				: 5;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static bool IsSingleConversionActive()
		{
			return Register::Ptr()->ScanSequenceStop;
		}
		
		static bool IsScanConversionActive()
		{
			return Register::Ptr()->ScanConversionActive;
		}
		
		static bool IsSingleReferenceWarmedUp()
		{
			return Register::Ptr()->SingleReferenceWarmedUp;
		}
		
		static bool IsScanReferenceWarmedUp()
		{
			return Register::Ptr()->ScanReferenceWarmedUp;
		}
		
		static bool IsADCWarmedUp()
		{
			return Register::Ptr()->ADCWarmedUp;
		}
		
		static bool IsSingleSampleDataValid()
		{
			return Register::Ptr()->SingleSampleDataValid;
		}
		
		static bool IsScanDataValid()
		{
			return Register::Ptr()->ScanDataValid;
		}
		
		static int GetScanDataSource()
		{
			return Register::Ptr()->ScanDataSource;
		}
	};
	
	// ADCn_SINGLECTRL - Single Sample Control Register
	template
		<
			RegisterAddressType Address
		>
	class SingleSampleControlRegister
	{
	public:
		enum Resolution
		{
			Resolution12Bit		= 0,
			Resolution8Bit		= 1,
			Resolution6Bit		= 2
		};
		
		enum Input
		{
			InputCh0		= 0,
			InputCh1		= 1,
			InputCh2		= 2,
			InputCh3		= 3,
			InputCh4		= 4,
			InputCh5		= 5,
			InputCh6		= 6,
			InputCh7		= 7,
			InputTemp		= 8,
			InputVddDiv3	= 9,
			InputVdd		= 10,
			InputVss		= 11,
			InputVrefDiv2	= 12,
			InputDac0Out0	= 13,
			InputDac0Out1	= 14
		};
		
		enum DifferentialInput
		{
			DifferentialInputCh0Ch1		= 0,
			DifferentialInputCh2Ch3		= 1,
			DifferentialInputCh4Ch5		= 2,
			DifferentialInputCh6Ch7		= 3,
			DifferentialInputDiff0		= 4
		};
		
		enum Reference
		{
			Reference1V25			= 0,
			Reference2V25			= 1,
			ReferenceVdd			= 2,
			Reference5VDiff			= 3,
			ReferenceExtSingle		= 4,
			Reference2xExtSingle	= 5,
			Reference2xVdd			= 6
		};
		
		enum AcquisitionTime
		{
			AcquisitionTime1			= 0,
			AcquisitionTime2			= 1,
			AcquisitionTime4			= 2,
			AcquisitionTime8			= 3,
			AcquisitionTime16			= 4,
			AcquisitionTime32			= 5,
			AcquisitionTime64			= 6,
			AcquisitionTime128			= 7,
			AcquisitionTime256			= 8,
		};
	protected:
		struct RegStruct
		{
			ValueDataType RepetitiveMode			: 1;
			ValueDataType DifferentialMode			: 1;
			ValueDataType ResultAdjustment			: 1;
			ValueDataType 							: 1;
			ValueDataType ResolutionSelect			: 2;
			ValueDataType 							: 2;
			ValueDataType InputSelection			: 4;
			ValueDataType 							: 4;
			ValueDataType ReferenceSelection		: 3;
			ValueDataType 							: 1;
			ValueDataType AcquisitionTime			: 4;
			ValueDataType PRSTriggeEnable			: 1;
			ValueDataType 							: 3;
			ValueDataType PRSTriggerSelect			: 4;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableRepetitiveMode(bool enable = true)
		{
			Register::Ptr()->RepetitiveMode = enable;
		}
		
		static void EnableDifferentialMode(bool enable = true)
		{
			Register::Ptr()->DifferentialMode = enable;
		}
		
		static void EnableLeftAdjustment(bool enable = true)
		{
			Register::Ptr()->ResultAdjustment = enable;
		}
		
		static void SetResolution(Resolution val)
		{
			Register::Ptr()->ResolutionSelect = val;
		}
		
		static void SetInput(Input val)
		{
			Register::Ptr()->InputSelection = val;
		}
		
		static void SetDifferentialInput(DifferentialInput val)
		{
			Register::Ptr()->InputSelection = val;
		}
		
		static void SetReference(Reference val)
		{
			Register::Ptr()->ReferenceSelection = val;
		}
		
		static void SetAcquisitionTime(AcquisitionTime val)
		{
			Register::Ptr()->AcquisitionTime = val;
		}
	};
	
	// ADCn_SCANCTRL - Scan Control Register
	template
		<
			RegisterAddressType Address
		>
	class ScanControlRegister
	{
	public:
		enum ScanResolution
		{
			ScanResolution12Bit			= 0,
			ScanResolution8Bit			= 1,
			ScanResolution6Bit			= 2,
			ScanResolutionOversampling	= 3
		};
		
		enum ScanMask
		{
			ScanMaskCh0		= 1,
			ScanMaskCh1		= 2,
			ScanMaskCh2		= 4,
			ScanMaskCh3		= 8,
			ScanMaskCh4		= 16,
			ScanMaskCh5		= 32,
			ScanMaskCh6		= 64,
			ScanMaskCh7		= 128
		};
		
		enum DifferentialScanMask
		{
			DifferentialScanMaskCh0Ch1		= 1,
			DifferentialScanMaskCh2Ch3		= 2,
			DifferentialScanMaskCh4Ch5		= 4,
			DifferentialScanMaskCh6Ch7		= 8
		};
		
		enum ScanReference
		{
			ScanReference1V25			= 0,
			ScanReference2V25			= 1,
			ScanReferenceVdd			= 2,
			ScanReference5VDiff			= 3,
			ScanReferenceExtSingle		= 4,
			ScanReference2xExtSingle	= 5,
			ScanReference2xVdd			= 6
		};
		
		enum ScanAcquisitionTime
		{
			ScanAcquisitionTime1			= 0,
			ScanAcquisitionTime2			= 1,
			ScanAcquisitionTime4			= 2,
			ScanAcquisitionTime8			= 3,
			ScanAcquisitionTime16			= 4,
			ScanAcquisitionTime32			= 5,
			ScanAcquisitionTime64			= 6,
			ScanAcquisitionTime128			= 7,
			ScanAcquisitionTime256			= 8,
		};
	protected:
		struct RegStruct
		{
			ValueDataType RepetitiveMode			: 1;
			ValueDataType DifferentialMode			: 1;
			ValueDataType ResultAdjustment			: 1;
			ValueDataType 							: 1;
			ValueDataType ResolutionSelect			: 2;
			ValueDataType 							: 2;
			ValueDataType InputSelection			: 4;
			ValueDataType 							: 4;
			ValueDataType ReferenceSelection		: 3;
			ValueDataType 							: 1;
			ValueDataType AcquisitionTime			: 4;
			ValueDataType PRSTriggeEnable			: 1;
			ValueDataType 							: 3;
			ValueDataType PRSTriggerSelect			: 4;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableScanRepetitiveMode(bool enable = true)
		{
			Register::Ptr()->RepetitiveMode = enable;
		}
		
		static void EnableScanDifferentialMode(bool enable = true)
		{
			Register::Ptr()->DifferentialMode = enable;
		}
		
		static void EnableScanLeftAdjustment(bool enable = true)
		{
			Register::Ptr()->ResultAdjustment = enable;
		}
		
		static void SetScanResolution(ScanResolution val)
		{
			Register::Ptr()->ResolutionSelect = val;
		}
		
		static void SetScanMaskBit(ScanMask bit)
		{
			Register::Ptr()->InputSelection |= bit;
		}
		
		static void ClearScanMaskBit(ScanMask bit)
		{
			Register::Ptr()->InputSelection &= ~bit;
		}
		
		static void SetDifferentialScanMaskBit(DifferentialScanMask bit)
		{
			Register::Ptr()->InputSelection |= bit;
		}
		
		static void ClearDifferentialScanMaskBit(DifferentialScanMask bit)
		{
			Register::Ptr()->InputSelection &= ~bit;
		}
		
		static void SetScanReference(ScanReference val)
		{
			Register::Ptr()->ReferenceSelection = val;
		}
		
		static void SetScanAcquisitionTime(ScanAcquisitionTime val)
		{
			Register::Ptr()->AcquisitionTime = val;
		}
	};
	
	// ADCn_IEN - Interrupt Enable Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptEnableRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType SingleConversionComplete		: 1;
			ValueDataType ScanConversionComplete		: 1;
			ValueDataType 								: 6;
			ValueDataType SingleResultOverflow			: 1;
			ValueDataType ScanResultOverflow			: 1;
			ValueDataType				 				: 22;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SingleConversionCompleteInterruptEnable(bool enable = true)
		{
			Register::Ptr()->SingleConversionComplete = enable;
		}
		
		static void ScanConversionCompleteInterruptEnable(bool enable = true)
		{
			Register::Ptr()->ScanConversionComplete = enable;
		}
		
		static void SingleResultOverflowInterruptEnable(bool enable = true)
		{
			Register::Ptr()->SingleResultOverflow = enable;
		}
		
		static void ScanResultOverflowInterruptEnable(bool enable = true)
		{
			Register::Ptr()->ScanResultOverflow = enable;
		}
	};
	
	// ADCn_IF - Interrupt Flag Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType SingleConversionComplete		: 1;
			ValueDataType ScanConversionComplete		: 1;
			ValueDataType 								: 6;
			ValueDataType SingleResultOverflow			: 1;
			ValueDataType ScanResultOverflow			: 1;
			ValueDataType				 				: 22;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static bool SingleConversionCompleteInterruptFlag()
		{
			return Register::Ptr()->SingleConversionComplete;
		}
		
		static bool ScanConversionCompleteInterruptFlag()
		{
			return Register::Ptr()->ScanConversionComplete;
		}
		
		static bool SingleResultOverflowInterruptFlag()
		{
			return Register::Ptr()->SingleResultOverflow;
		}
		
		static bool ScanResultOverflowInterruptFlag()
		{
			return Register::Ptr()->ScanResultOverflow;
		}
	};
	
	// ADCn_IFS - Interrupt Flag Set Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagSetRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType SingleConversionComplete		: 1;
			ValueDataType ScanConversionComplete		: 1;
			ValueDataType 								: 6;
			ValueDataType SingleResultOverflow			: 1;
			ValueDataType ScanResultOverflow			: 1;
			ValueDataType				 				: 22;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SingleConversionCompleteInterruptFlagSet()
		{
			Register::Ptr()->SingleConversionComplete = 1;
		}
		
		static void ScanConversionCompleteInterruptFlagSet()
		{
			Register::Ptr()->ScanConversionComplete = 1;
		}
		
		static void SingleResultOverflowInterruptFlagSet()
		{
			Register::Ptr()->SingleResultOverflow = 1;
		}
		
		static void ScanResultOverflowInterruptFlagSet()
		{
			Register::Ptr()->ScanResultOverflow = 1;
		}
	};
	
	// ADCn_IFC - Interrupt Flag Clear Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagClearRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType SingleConversionComplete		: 1;
			ValueDataType ScanConversionComplete		: 1;
			ValueDataType 								: 6;
			ValueDataType SingleResultOverflow			: 1;
			ValueDataType ScanResultOverflow			: 1;
			ValueDataType				 				: 22;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SingleConversionCompleteInterruptFlagClear()
		{
			Register::Ptr()->SingleConversionComplete = 1;
		}
		
		static void ScanConversionCompleteInterruptFlagClear()
		{
			Register::Ptr()->ScanConversionComplete = 1;
		}
		
		static void SingleResultOverflowInterruptFlagClear()
		{
			Register::Ptr()->SingleResultOverflow = 1;
		}
		
		static void ScanResultOverflowInterruptFlagClear()
		{
			Register::Ptr()->ScanResultOverflow = 1;
		}
	};
	
	// ADCn_SINGLEDATA - Single Conversion Result Data
	template
		<
			RegisterAddressType Address
		>
	class SingleConversionResultDataRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static ValueDataType GetSingleConversionResultData()
		{
			return *Register::Ptr();
		}
	};
	
	// ADCn_SCANDATA - Scan Conversion Result Data
	template
		<
			RegisterAddressType Address
		>
	class ScanConversionResultDataRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static ValueDataType GetScanConversionResultData()
		{
			return *Register::Ptr();
		}
	};
	
	template
		<
			unsigned int IdObj,
			RegisterAddressType BaseAddr,
			RegisterAddressType ControlRegAddr,
			RegisterAddressType CommandRegAddr,
			RegisterAddressType StatusRegAddr,
			RegisterAddressType SingleSampleControlRegAddr,
			RegisterAddressType ScanControlRegAddr,
			RegisterAddressType InterruptEnableRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr,
			RegisterAddressType SingleConversionResultDataRegAddr,
			RegisterAddressType ScanConversionResultDataRegAddr,
			RegisterAddressType SingleConversionResultDataPeekRegAddr,
			RegisterAddressType ScanSequenceResultDataPeekRegAddr,
			RegisterAddressType CalibrationRegAddr,
			RegisterAddressType BiasProgrammingRegAddr
		>
	class Adc :
		public IdObjectBase<IdObj>,
		public ControlRegister<BaseAddr + ControlRegAddr>,
		public CommandRegister<BaseAddr + CommandRegAddr>,
		public StatusRegister<BaseAddr + StatusRegAddr>,
		public SingleSampleControlRegister<BaseAddr + SingleSampleControlRegAddr>,
		public ScanControlRegister<BaseAddr + ScanControlRegAddr>,
		public InterruptEnableRegister<BaseAddr + InterruptEnableRegAddr>,
		public InterruptFlagRegister<BaseAddr + InterruptFlagRegAddr>,
		public InterruptFlagSetRegister<BaseAddr + InterruptFlagSetRegAddr>,
		public InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr>,
		public SingleConversionResultDataRegister<BaseAddr + SingleConversionResultDataRegAddr>,
		public ScanConversionResultDataRegister<BaseAddr + ScanConversionResultDataRegAddr>
	{
	public:
		typedef CommandRegister<BaseAddr + CommandRegAddr> CommandRegister;
		typedef StatusRegister<BaseAddr + StatusRegAddr> StatusRegister;
		typedef SingleSampleControlRegister<BaseAddr + SingleSampleControlRegAddr> SingleSampleControlRegister;
		typedef SingleConversionResultDataRegister<BaseAddr + SingleConversionResultDataRegAddr> SingleConversionResultDataRegister;
	public:
		static ValueDataType GetSample(typename SingleSampleControlRegister::Input input)
		{
			SingleSampleControlRegister::SetInput(input);
			CommandRegister::SingleConversionStart();
			
			while (!StatusRegister::IsSingleSampleDataValid())
			{
				;
			}
			
			return SingleConversionResultDataRegister::GetSingleConversionResultData();
		}
	};
}
}
}

#endif