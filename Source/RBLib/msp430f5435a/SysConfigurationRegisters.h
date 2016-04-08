///////////////////////////////////////////////////////////////////////////////
//	Класс управления регистрами конфигурации системы
//	MSP430F5435A
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSCONFIGURATIONREGISTERS_MSP430F5435A_H
#define SYSCONFIGURATIONREGISTERS_MSP430F5435A_H

#include "../msp430/f5xxx/SysConfigurationRegisters.h"

namespace Rblib
{
namespace Msp430f5435a
{

// Тип значения регистра
typedef unsigned short RegisterDataType;
// Тип адреса регистра
typedef unsigned int  RegisterAddresType;

// Платформозависимый набор регистров
class SysConfigurationRegisters :
	public Msp430f5xxx::SysResetInterruptVector<RegisterDataType, RegisterAddresType, 0x19E>
{

};

}
}

#endif