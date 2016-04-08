///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами ввода-вывода
//	MSP430F5435A
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_MSP430F5435A_H
#define GPIO_MSP430F5435A_H

#include "../GpioPort.h"

namespace Rblib
{
namespace Msp430f5435a
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
		public GpioPortFunctionSelect<PortValueDataType, PortRegisterAddresType, R4>,
		public GpioPortInterrupt<PortValueDataType, PortRegisterAddresType, R5, R6, R7>,
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

	// Шаблон порта PortJ
	template<unsigned int IdObj,
		PortRegisterAddresType R1,
		PortRegisterAddresType R2,
		PortRegisterAddresType R3,
		PortRegisterAddresType R4
	>
	class PortsJ:
		public IdObjectBase<IdObj>,
		public GpioPortInputOutputDirection<PortValueDataType, PortRegisterAddresType, R1, R2, R3>,
		public GpioPortResistor<PortValueDataType, PortRegisterAddresType, R4>
	{
	
	};

	// Платформозависимый набор портов
	class Gpio
	{
	public:
		typedef PortsFrom1to2<1, 0x0200, 0x0202, 0x0204, 0x020A, 0x021C, 0x0218, 0x021A, 0x0206> Port1;
		typedef PortsFrom1to2<2, 0x0201, 0x0203, 0x0205, 0x020B, 0x021D, 0x0219, 0x021B, 0x0207> Port2;
		typedef PortsFrom3to8<3, 0x0220, 0x0222, 0x0224, 0x022A, 0x0226> Port3;
		typedef PortsFrom3to8<4, 0x0221, 0x0223, 0x0225, 0x022B, 0x0227> Port4;
		typedef PortsFrom3to8<5, 0x0240, 0x0242, 0x0244, 0x024A, 0x0246> Port5;
		typedef PortsFrom3to8<6, 0x0241, 0x0243, 0x0245, 0x024B, 0x0247> Port6;
		typedef PortsFrom3to8<7, 0x0260, 0x0262, 0x0264, 0x026A, 0x0266> Port7;
		typedef PortsFrom3to8<8, 0x0261, 0x0263, 0x0265, 0x026B, 0x0267> Port8;
		typedef PortsJ		<12, 0x0320, 0x0322, 0x0324, 0x0326>		 PortJ;
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
		<tr><td>PortJ</td>	<td>	+	</td><td>		</td><td>		</td><td>	+	</td></tr>
		</table>
	*/
}
}

#endif