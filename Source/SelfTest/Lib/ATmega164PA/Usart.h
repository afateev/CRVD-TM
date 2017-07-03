///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами USART
//	ATmega164PA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef USART_ATMEGA164PA_H
#define USART_ATMEGA164PA_H

#include "../ATmega/Usart.h"
#include "../IdObjectBase.h"

namespace Lib
{
namespace ATmega164PA
{
	// Платформозависимый набор портов
	class Usart
	{
	public:
		typedef ATmega::Usart::Usart<1, 0xC6, 0xC0, 0xC1, 0xC2, 0xC4, 0xC5> Usart0;
		typedef ATmega::Usart::Usart<1, 0xCE, 0xC8, 0xC9, 0xCA, 0xCC, 0xCD> Usart1;
	};
}
}

#endif