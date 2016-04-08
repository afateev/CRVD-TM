////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		05/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef AFIOPARTS_STM32_F107_H
#define AFIOPARTS_STM32_F107_H

#include "..\..\RegisterReference.h"

template<RegisterPointerType RegisterAddr>
class AfioRemapRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Spi1						: 1;
		RegisterDataType I2C1						: 1;
		RegisterDataType Usart1						: 1;
		RegisterDataType Usart2						: 1;
		RegisterDataType Usart3						: 2;
		RegisterDataType Timer1						: 2;
		RegisterDataType Timer2						: 2;
		RegisterDataType Timer3						: 2;
		RegisterDataType Timer4						: 1;
		RegisterDataType Can1						: 2;
		RegisterDataType PD							: 1;
		RegisterDataType Timer5Channel4Internal		: 1;
		RegisterDataType Reserved0					: 4;
		RegisterDataType Ethernet					: 1;
		RegisterDataType Can2						: 1;
		RegisterDataType EthernetMiiRmii			: 1;
		RegisterDataType JtagConfig					: 3;
		RegisterDataType Reserved1					: 1;
		RegisterDataType Spi3						: 1;
		RegisterDataType Timer2Trigger1Internal		: 1;
		RegisterDataType EthernetPtpPps				: 1;
		RegisterDataType Reserved2					: 1;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void RemapUsart2(unsigned char value)
	{
		RegRef::_register->Usart2 = value;
	}
	inline static void RemapUsart3(unsigned char value)
	{
		RegRef::_register->Usart3 = value;
	}
	inline static void RemapSpi3(unsigned char value)
	{
		RegRef::_register->Spi3 = value;
	}
};

#endif