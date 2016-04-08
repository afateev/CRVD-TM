///////////////////////////////////////////////////////////////////////////////
//	Класс управления сторожевым таймером
//	MSP430F5340
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef WDT_MSP430F5340_H
#define WDT_MSP430F5340_H

#include "../msp430/f5xxx/Wdt.h"
#include "../msp430/Wdt.h"

namespace Rblib
{
namespace Msp430f5340
{
	// Тип значения регистра
	typedef unsigned short WdtRegisterDataType;
	// Тип адреса регистра
	typedef unsigned int  WdtRegisterAddresType;
	
	// Платформозависимый сторожевой таймер
	class Wdt : public Msp430::Wdt< Msp430f5xxx::WtdControlRegister<WdtRegisterDataType, WdtRegisterAddresType, 0x015C> >
	{
	public:
	};
}
}

#endif