///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами ввода-вывода
//	MSP430F1611
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_MSP430F1611_H
#define GPIO_MSP430F1611_H

#include "../GpioPort.h"
#include "../IdObjectBase.h"

namespace Rblib
{
namespace Msp430f1611
{
	// Тип значения порта
	typedef unsigned char PortValueDataType;
	// Тип адреса регистра порта
	typedef unsigned int  PortRegisterAddresType;
	
	// Шаблон портов Port1, Port2
	template<unsigned int IdObj,
		PortRegisterAddresType R1,
		PortRegisterAddresType R2,
		PortRegisterAddresType R3,
		PortRegisterAddresType R4,
		PortRegisterAddresType R5,
		PortRegisterAddresType R6,
		PortRegisterAddresType R7
	>
	class PortsFrom1to2:
		public IdObjectBase<IdObj>,
		public GpioPortInputOutputDirection<PortValueDataType, PortRegisterAddresType, R1, R2, R3>,
		public GpioPortInterrupt<PortValueDataType, PortRegisterAddresType, R4, R5, R6>,
		public GpioPortFunctionSelect<PortValueDataType, PortRegisterAddresType, R7>
	{
	
	};

	// Шаблон портов Port3, Port4, Port5, Port6
	template<unsigned int IdObj,
		PortRegisterAddresType R1,
		PortRegisterAddresType R2,
		PortRegisterAddresType R3,
		PortRegisterAddresType R4
	>
	class PortsFrom3to6: 
		public IdObjectBase<IdObj>,
		public GpioPortInputOutputDirection<PortValueDataType, PortRegisterAddresType, R1, R2, R3>,
		public GpioPortFunctionSelect<PortValueDataType, PortRegisterAddresType, R4>
	{
	
	};

	// Платформозависимый набор портов
	class Gpio
	{
	public:
		typedef PortsFrom1to2<1, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026> Port1;
		typedef PortsFrom1to2<2, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E> Port2;
		typedef PortsFrom3to6<3, 0x0018, 0x0019, 0x001A, 0x001B> Port3;
		typedef PortsFrom3to6<4, 0x001C, 0x001D, 0x001E, 0x001F> Port4;
		typedef PortsFrom3to6<5, 0x0030, 0x0031, 0x0032, 0x0033> Port5;
		typedef PortsFrom3to6<6, 0x0034, 0x0035, 0x0036, 0x0037> Port6;
	};
}
}

#endif