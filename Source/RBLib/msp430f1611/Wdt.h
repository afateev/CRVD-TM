///////////////////////////////////////////////////////////////////////////////
//	Класс управления сторожевым таймером
//	MSP430F1611
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef WDT_MSP430F1611_H
#define WDT_MSP430F1611_H

#include "../msp430/x1xxx/Wdt.h"
#include "../msp430/Wdt.h"

namespace Rblib
{
	namespace Msp430f1611
	{
		// Тип значения регистра
		typedef unsigned short WdtRegisterDataType;
		// Тип адреса регистра
		typedef unsigned int  WdtRegisterAddresType;
	
		// Платформозависимый сторожевой таймер
		class Wdt : public Msp430::Wdt< Msp430x1xxx::WdtControlRegister<WdtRegisterDataType, WdtRegisterAddresType, 0x0120> >
		{
			public:
		};
	}
}

#endif