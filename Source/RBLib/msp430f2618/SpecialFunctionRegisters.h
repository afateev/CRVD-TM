///////////////////////////////////////////////////////////////////////////////
//	Класс управления регистрами специальных функций
//	MSP430F2618
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPECIALFUNCTIONREGISTERS_MSP430F2618_H
#define SPECIALFUNCTIONREGISTERS_MSP430F2618_H

#include "../msp430/f2xxx/SpecialFunctionRegisters.h"

namespace Rblib
{
namespace Msp430f2618
{

// Тип значения регистра
typedef unsigned char RegisterDataType;
// Тип адреса регистра
typedef unsigned int  RegisterAddresType;

// Платформозависимый набор регистров
class SpecialFunctionRegisters :
	public Msp430f2xxx::InterruptEnable1Register<RegisterDataType, RegisterAddresType, 0x0000>,
	public Msp430f2xxx::InterruptEnable2Register<RegisterDataType, RegisterAddresType, 0x0001>,
	public Msp430f2xxx::InterruptFlag1Register<RegisterDataType, RegisterAddresType, 0x0002>,
	public Msp430f2xxx::InterruptFlag2Register<RegisterDataType, RegisterAddresType, 0x0003>,
	public Msp430f2xxx::Usci1InterruptEnableRegister<RegisterDataType, RegisterAddresType, 0x0006>,
	public Msp430f2xxx::Usci1InterruptFlagRegister<RegisterDataType, RegisterAddresType, 0x0007>
{
};

}
}

#endif