///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами ввода-вывода
//	MSP430F2618
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_MSP430F2618_H
#define GPIO_MSP430F2618_H

#include "../GpioPort.h"

namespace Rblib
{
namespace Msp430f2618
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
		PortRegisterAddresType R7,
		PortRegisterAddresType R8
	>
	class PortsFrom1to2:
		public IdObjectBase<IdObj>,
		public GpioPortInputOutputDirection<PortValueDataType, PortRegisterAddresType, R1, R2, R3>,
		public GpioPortInterrupt<PortValueDataType, PortRegisterAddresType, R4, R5, R6>,
		public GpioPortFunctionSelect<PortValueDataType, PortRegisterAddresType, R7>,
		public GpioPortResistor<PortValueDataType, PortRegisterAddresType, R8>
	{
	
	};

	// Шаблон портов Port3, Port4, Port5, Port6, Port7, Port8
	template<unsigned int IdObj,
		PortRegisterAddresType R1,
		PortRegisterAddresType R2,
		PortRegisterAddresType R3,
		PortRegisterAddresType R4,
		PortRegisterAddresType R5
	>
	class PortsFrom3to8: 
		public IdObjectBase<IdObj>,
		public GpioPortInputOutputDirection<PortValueDataType, PortRegisterAddresType, R1, R2, R3>,
		public GpioPortFunctionSelect<PortValueDataType, PortRegisterAddresType, R4>,
		public GpioPortResistor<PortValueDataType, PortRegisterAddresType, R5>
	{
	
	};
	
	// Платформозависимый набор портов
	class Gpio
	{
	public:
		typedef PortsFrom1to2<1, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027> Port1;
		typedef PortsFrom1to2<2, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F> Port2;
		typedef PortsFrom3to8<3, 0x0018, 0x0019, 0x001A, 0x001B, 0x0010> Port3;
		typedef PortsFrom3to8<4, 0x001C, 0x001D, 0x001E, 0x001F, 0x0011> Port4;
		typedef PortsFrom3to8<5, 0x0030, 0x0031, 0x0032, 0x0033, 0x0012> Port5;
		typedef PortsFrom3to8<6, 0x0034, 0x0035, 0x0036, 0x0037, 0x0013> Port6;
		typedef PortsFrom3to8<6, 0x0038, 0x003A, 0x003C, 0x003E, 0x0014> Port7;
		typedef PortsFrom3to8<6, 0x0039, 0x003B, 0x003D, 0x003F, 0x0015> Port8;
	};

	/*! \class Gpio
		\brief Платформозависимый набор портов
		
		Пользователю доступны следующие порты поддерживающие следующие возможности:
		<table>
		<tr>
		<td>
		</td>
		<td>
			\ref Rblib::GpioPortInputOutputDirection "Input Output Direction"
		</td>
		<td>
			\ref Rblib::GpioPortFunctionSelect "Function Select"
		</td>
		<td>
			\ref Rblib::GpioPortInterrupt "Interrupt"
		</td>
		<td>
			\ref Rblib::GpioPortResistor "Resistor"
		</td>
		</tr>
		<tr><td>Port1</td>	<td>	+	</td><td>	+	</td><td>	+	</td><td>	+	</td></tr>
		<tr><td>Port2</td>	<td>	+	</td><td>	+	</td><td>	+	</td><td>	+	</td></tr>
		<tr><td>Port3</td>	<td>	+	</td><td>	+	</td><td>		</td><td>	+	</td></tr>
		<tr><td>Port4</td>	<td>	+	</td><td>	+	</td><td>		</td><td>	+	</td></tr>
		<tr><td>Port5</td>	<td>	+	</td><td>	+	</td><td>		</td><td>	+	</td></tr>
		<tr><td>Port6</td>	<td>	+	</td><td>	+	</td><td>		</td><td>	+	</td></tr>
		<tr><td>Port7</td>	<td>	+	</td><td>	+	</td><td>		</td><td>	+	</td></tr>
		<tr><td>Port8</td>	<td>	+	</td><td>	+	</td><td>		</td><td>	+	</td></tr>
		</table>
	*/
}
}

#endif