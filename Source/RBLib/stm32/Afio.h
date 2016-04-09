#ifndef RBLIB_STM32_AFIO_H
#define RBLIB_STM32_AFIO_H

#include "Register.h"
#include "../IdObjectBase.h"

namespace Rblib
{
namespace Stm32
{
namespace AfioImplementation
{
	static const unsigned int BaseAddress = 0x40010000;
	
	// AF remap and debug I/O configuration register (AFIO_MAPR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class ConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x04;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType SPI1_REMAP				: 1;
			Core::RegisterValueType I2C1_REMAP				: 1;
			Core::RegisterValueType USART1_REMAP			: 1;
			Core::RegisterValueType USART2_REMAP			: 1;
			Core::RegisterValueType USART3_REMAP			: 2;
			Core::RegisterValueType TIM1_REMAP				: 2;
			Core::RegisterValueType TIM2_REMAP				: 2;
			Core::RegisterValueType TIM3_REMAP				: 2;
			Core::RegisterValueType TIM4_REMAP				: 1;
			Core::RegisterValueType CAN1_REMAP				: 2;
			Core::RegisterValueType PD01_REMAP				: 1;
			Core::RegisterValueType TIM5CH4_IREMAP			: 1;
			Core::RegisterValueType 						: 4;
			Core::RegisterValueType ETH_REMAP				: 1;
			Core::RegisterValueType CAN2_REMAP				: 1;
			Core::RegisterValueType MII_RMII_SEL			: 1;
			Core::RegisterValueType SWJ_CFG					: 3;
			Core::RegisterValueType 						: 1;
			Core::RegisterValueType SPI3_REMAP				: 1;
			Core::RegisterValueType TIM2ITR1_IREMAP			: 1;
			Core::RegisterValueType PTP_PPS_REMAP			: 1;
			Core::RegisterValueType 						: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void RemapSpi1(unsigned char selector)
		{
			Register::Ptr()->SPI1_REMAP = selector;
		}
		
		static void RemapI2C1(unsigned char selector)
		{
			Register::Ptr()->I2C1_REMAP = selector;
		}
		
		static void RemapUsart1(unsigned char selector)
		{
			Register::Ptr()->USART1_REMAP = selector;
		}
		
		static void RemapUsart2(unsigned char selector)
		{
			Register::Ptr()->USART2_REMAP = selector;
		}
		
		static void RemapUsart3(unsigned char selector)
		{
			Register::Ptr()->USART3_REMAP = selector;
		}
		
		static void RemapSpi3(unsigned char selector)
		{
			Register::Ptr()->SPI3_REMAP = selector;
		}
	};
	
	template
		<
			unsigned int IdObj
		>
	class Afio :
		public IdObjectBase<IdObj>,
		public ConfigurationRegister<BaseAddress>
	{
	};
}

typedef AfioImplementation::Afio<1> Afio;

}
}

#endif