
#ifndef GPIO_MSP430F5XXX_H
#define GPIO_MSP430F5XXX_H

#include "../../GpioPort.h"

namespace Rblib
{
	namespace Msp430f5xxx
	{

		// Тип значения порта
		typedef unsigned char PortValueDataType;

		// Тип адреса регистра порта
		typedef unsigned int  PortRegisterAddresType;

		// Шаблон портов поддерживающих прерывания
		template<unsigned int IdObj,
			PortRegisterAddresType R1, // PxIN
			PortRegisterAddresType R2, // PxOUT
			PortRegisterAddresType R3, // PxDIR
			PortRegisterAddresType R4, // PxSEL
			PortRegisterAddresType R5, // PxIFG
			PortRegisterAddresType R6, // PxIES
			PortRegisterAddresType R7, // PxIE
			PortRegisterAddresType R8  // PxREN
		>
		class PortWithInterruptSupport:
			public IdObjectBase<IdObj>,
			public GpioPortInputOutputDirection<PortValueDataType, PortRegisterAddresType, R1, R2, R3>,
			public GpioPortFunctionSelect<PortValueDataType, PortRegisterAddresType, R4>,
			public GpioPortInterrupt<PortValueDataType, PortRegisterAddresType, R5, R6, R7>,
			public GpioPortResistor<PortValueDataType, PortRegisterAddresType, R8>
		{

		};

		// Шаблон простых портов (без прерываний)
		template<unsigned int IdObj,
			PortRegisterAddresType R1, // PxIN
			PortRegisterAddresType R2, // PxOUT
			PortRegisterAddresType R3, // PxDIR
			PortRegisterAddresType R4, // PxSEL
			PortRegisterAddresType R5  // PxREN
		>
		class PortSimple: 
			public IdObjectBase<IdObj>,
			public GpioPortInputOutputDirection<PortValueDataType, PortRegisterAddresType, R1, R2, R3>,
			public GpioPortFunctionSelect<PortValueDataType, PortRegisterAddresType, R4>,
			public GpioPortResistor<PortValueDataType, PortRegisterAddresType, R5>
		{

		};

		// Шаблон порта PortJ
		template<unsigned int IdObj,
			PortRegisterAddresType R1, // PxIN
			PortRegisterAddresType R2, // PxOUT
			PortRegisterAddresType R3, // PxDIR
			PortRegisterAddresType R4  // PxREN
		>
		class PortJ:
			public IdObjectBase<IdObj>,
			public GpioPortInputOutputDirection<PortValueDataType, PortRegisterAddresType, R1, R2, R3>,
			public GpioPortResistor<PortValueDataType, PortRegisterAddresType, R4>
		{

		};

	}
}

#endif // GPIO_MSP430F5XXX_H
