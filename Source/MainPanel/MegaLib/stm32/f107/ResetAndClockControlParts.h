////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		26/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RESETANDCLOCKCONTROLPARTS_STM32_F107_H
#define RESETANDCLOCKCONTROLPARTS_STM32_F107_H

#include "..\..\RegisterReference.h"

template<RegisterPointerType RegisterAddr>
class ClockControlRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType HightSpeedInternalClockEnable		: 1;
		RegisterDataType HightSpeedInternalClockReady		: 1;
		RegisterDataType Reserved0							: 1;
		RegisterDataType HightSpeedInternalClockTrim		: 5;
		RegisterDataType HightSpeedInternalClockCalibration	: 8;
		RegisterDataType HightSpeedExternalClockEnable		: 1;
		RegisterDataType HightSpeedExternalClockReady		: 1;
		RegisterDataType HightSpeedExternalClockBypass		: 1;
		RegisterDataType ClockSecuritySystemEnable			: 1;
		RegisterDataType Reserved1							: 4;
		RegisterDataType PllEnable							: 1;
		RegisterDataType PllReady							: 1;
		RegisterDataType Pll2Enable							: 1;
		RegisterDataType Pll2Ready							: 1;
		RegisterDataType Pll3Enable							: 1;
		RegisterDataType Pll3Ready							: 1;
		RegisterDataType Reserved2							: 2;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void ExternalHightSpeedClockEnable()
	{
		RegRef::_register->HightSpeedExternalClockEnable = 1;
	}
	
	inline static bool IsExternalHightSpeedClockReady()
	{
		return RegRef::_register->HightSpeedExternalClockReady;
	}
	
	inline static void PllEnable()
	{
		RegRef::_register->PllEnable = 1;
	}
	
	inline static bool IsPllReady()
	{
		return RegRef::_register->HightSpeedExternalClockReady;
	}
};

template<RegisterPointerType RegisterAddr>
class ClockConfigurationRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType SystemClockSelect			: 2;
		RegisterDataType SystemClockSelectStatus	: 2;
		RegisterDataType AHBPrescaler				: 4;
		RegisterDataType APBLowSpeedPrescaler		: 3;
		RegisterDataType APBHightSpeedPrescaler		: 3;
		RegisterDataType AdcPrescaler				: 2;
		RegisterDataType PllEntryClockSource		: 1;
		RegisterDataType LSBofDivisionFactorPreDiv1	: 1;
		RegisterDataType PllMultiplicationFactor	: 4;
		RegisterDataType UsbOtgFsPrescaler			: 1;
		RegisterDataType Reserved0					: 1;
		RegisterDataType MicrocontrollerClockOutput	: 4;
		RegisterDataType Reserved1					: 4;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void SetClockOutput(unsigned char value)
	{
		RegRef::_register->MicrocontrollerClockOutput = value;
	}
	
	inline static void SetHightSpeedSystemClock()
	{
		RegRef::_register->SystemClockSelect = 1;
	}
	
	inline static void SetPllMultiplicationFactor(unsigned char value)
	{
		RegRef::_register->PllMultiplicationFactor = value;
	}
	
	inline static void SetUsbOtgFsPrescaler(unsigned char value)
	{
		RegRef::_register->UsbOtgFsPrescaler = value;
	}
};

template<RegisterPointerType RegisterAddr>
class AhbPeripheralClockEnableRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType DMA1Enable			: 1;
		RegisterDataType DMA2Enable			: 1;
		RegisterDataType SRAMEnable			: 1;
		RegisterDataType Reserved0			: 1;
		RegisterDataType FlitfEnable		: 1;
		RegisterDataType Reserved1			: 1;
		RegisterDataType CrcEnable			: 1;
		RegisterDataType Reserved2			: 5;
		RegisterDataType UsbEnable			: 1;
		RegisterDataType Reserved3			: 1;
		RegisterDataType EthernetEnable		: 1;
		RegisterDataType EthernetTxEnable	: 1;
		RegisterDataType EthernetRxEnable	: 1;
		RegisterDataType Reserved4			: 15;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void EnableUsb()
	{
		RegRef::_register->UsbEnable = 1;
	}
};

template<RegisterPointerType RegisterAddr>
class Apb2PeripheralClockEnableRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType AlternateFunctionIOClockEnable	: 1;
		RegisterDataType Reserved0	: 1;
		RegisterDataType PortAClockEnable	: 1;
		RegisterDataType PortBClockEnable	: 1;
		RegisterDataType PortCClockEnable	: 1;
		RegisterDataType PortDClockEnable	: 1;
		RegisterDataType PortEClockEnable	: 1;
		RegisterDataType Reserved1	: 2;
		RegisterDataType Adc1InterfaceClockEnable	: 1;
		RegisterDataType Adc2InterfaceClockEnable	: 1;
		RegisterDataType Timer1ClockEnable	: 1;
		RegisterDataType Spi1ClockEnable	: 1;
		RegisterDataType Reserved3	: 1;
		RegisterDataType Usart1ClockEnable	: 1;
		RegisterDataType Reserved4	: 17;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void EnableAlternateFunction(bool enable = true)
	{
		RegRef::_register->AlternateFunctionIOClockEnable = 1;
	}
	inline static void EnableGpioPortA(bool enable = true)
	{
		RegRef::_register->PortAClockEnable = 1;
	}
	inline static void EnableGpioPortB(bool enable = true)
	{
		RegRef::_register->PortBClockEnable = 1;
	}
	inline static void EnableGpioPortC(bool enable = true)
	{
		RegRef::_register->PortCClockEnable = 1;
	}
	inline static void EnableGpioPortD(bool enable = true)
	{
		RegRef::_register->PortDClockEnable = 1;
	}
	inline static void EnableGpioPortE(bool enable = true)
	{
		RegRef::_register->PortEClockEnable = 1;
	}
	inline static void EnableSpi1(bool enable = true)
	{
		RegRef::_register->Spi1ClockEnable = enable;
	}
	inline static void EnableUsart1(bool enable = true)
	{
		RegRef::_register->Usart1ClockEnable = 1;
	}
};

template<RegisterPointerType RegisterAddr>
class Apb1PeripheralClockEnableRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Timer2ClockEnable			: 1;
		RegisterDataType Timer3ClockEnable			: 1;
		RegisterDataType Timer4ClockEnable			: 1;
		RegisterDataType Timer5ClockEnable			: 1;
		RegisterDataType Timer6ClockEnable			: 1;
		RegisterDataType Timer7ClockEnable			: 1;
		RegisterDataType Reserved0					: 5;
		RegisterDataType WindowWatchdogClockEnable	: 1;
		RegisterDataType Reserved1					: 2;
		RegisterDataType Spi2ClockEnable			: 1;
		RegisterDataType Spi3ClockEnable			: 1;
		RegisterDataType Reserved2					: 1;
		RegisterDataType Usart2ClockEnable			: 1;
		RegisterDataType Usart3ClockEnable			: 1;
		RegisterDataType Usart4ClockEnable			: 1;
		RegisterDataType Usart5ClockEnable			: 1;
		RegisterDataType I2C1ClockEnable			: 1;
		RegisterDataType I2C2ClockEnable			: 1;
		RegisterDataType Reserved3					: 2;
		RegisterDataType Can1ClockEnable			: 1;
		RegisterDataType Can2ClockEnable			: 1;
		RegisterDataType BackupInterfaceClockEnable	: 1;
		RegisterDataType PowerInterfaceClockEnable	: 1;
		RegisterDataType DacInterfaceClockEnable	: 1;
		RegisterDataType Reserved4					: 2;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void EnableSpi2(bool enable = true)
	{
		RegRef::_register->Spi2ClockEnable = enable;
	}
	inline static void EnableSpi3(bool enable = true)
	{
		RegRef::_register->Spi3ClockEnable = enable;
	}
	inline static void EnableUsart2(bool enable = true)
	{
		RegRef::_register->Usart2ClockEnable = enable;
	}
	inline static void EnableUsart3(bool enable = true)
	{
		RegRef::_register->Usart3ClockEnable = enable;
	}
	inline static void EnableUsart4(bool enable = true)
	{
		RegRef::_register->Usart4ClockEnable = enable;
	}
	inline static void EnableUsart5(bool enable = true)
	{
		RegRef::_register->Usart5ClockEnable = enable;
	}
	inline static void EnableTimer2(bool enable = true)
	{
		RegRef::_register->Timer2ClockEnable = enable;
	}
	inline static void EnableTimer3(bool enable = true)
	{
		RegRef::_register->Timer3ClockEnable = enable;
	}
	inline static void EnableTimer4(bool enable = true)
	{
		RegRef::_register->Timer4ClockEnable = enable;
	}
	inline static void EnableTimer5(bool enable = true)
	{
		RegRef::_register->Timer5ClockEnable = enable;
	}
	inline static void EnableTimer6(bool enable = true)
	{
		RegRef::_register->Timer6ClockEnable = enable;
	}
	inline static void EnableTimer7(bool enable = true)
	{
		RegRef::_register->Timer7ClockEnable = enable;
	}
	inline static void EnableBackupInterface(bool enable = true)
	{
		RegRef::_register->BackupInterfaceClockEnable = enable;
	}
	inline static void EnablePowerInterface(bool enable = true)
	{
		RegRef::_register->PowerInterfaceClockEnable = enable;
	}
};

template<RegisterPointerType RegisterAddr>
class BackupDomainControlRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType ExternalLowSpeedOscillatorEnable	: 1;
		RegisterDataType ExternalLowSpeedOscillatorReady	: 1;
		RegisterDataType ExternalLowSpeedOscillatorBypass	: 1;
		RegisterDataType Reserved0							: 5;
		RegisterDataType RtcClockSourceSelection			: 2;
		RegisterDataType Reserved1							: 5;
		RegisterDataType RtcClockEnable						: 1;
		RegisterDataType BackupDomainSoftwareReset			: 1;
		RegisterDataType Reserved2							: 15;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void RtcEnable()
	{
		RegRef::_register->RtcClockEnable = 1;
	}
	inline static bool IsRtcEnabled()
	{
		return RegRef::_register->RtcClockEnable;
	}
	inline static void RtcClockSourceSelect(unsigned char value)
	{
		RegRef::_register->RtcClockSourceSelection = value;
	}
	inline static void ExternalLowSpeedClockEnable()
	{
		RegRef::_register->ExternalLowSpeedOscillatorEnable = 1;
	}
	
	inline static bool IsExternalLowSpeedClockReady()
	{
		return RegRef::_register->ExternalLowSpeedOscillatorReady;
	}
};

#endif