///////////////////////////////////////////////////////////////////////////////
//	Класс управления регистрами специальных функций
//	MSP430F1611
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPECIALFUNCTIONREGISTERS_MSP430F1611_H
#define SPECIALFUNCTIONREGISTERS_MSP430F1611_H

#include "../msp430/x1xxx/SpecialFunctionRegisters.h"

namespace Rblib
{
namespace Msp430f1611
{

// Тип значения регистра
typedef unsigned short RegisterDataType;
// Тип адреса регистра
typedef unsigned int  RegisterAddresType;

// Платформозависимый набор регистров
class SpecialFunctionRegisters :
  public Msp430x1xxx::InterruptEnable1Register<RegisterDataType, RegisterAddresType, 0x0000>,
  public Msp430x1xxx::InterruptEnable2Register<RegisterDataType, RegisterAddresType, 0x0001>,
  public Msp430x1xxx::InterruptFlag1Register<RegisterDataType, RegisterAddresType, 0x0002>,
  public Msp430x1xxx::InterruptFlag2Register<RegisterDataType, RegisterAddresType, 0x0003>,
  public Msp430x1xxx::ModuleEnable1Register<RegisterDataType, RegisterAddresType, 0x0004>,
  public Msp430x1xxx::ModuleEnable2Register<RegisterDataType, RegisterAddresType, 0x0005>
{
};

}
}

#endif
