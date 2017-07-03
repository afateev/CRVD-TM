///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами USART
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef USART_H
#define USART_H

#include "Platform.h"

#include "ATmega164PA\Usart.h"

namespace Lib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::Usart
		>::Result UsartTypes;
	
	// Селектор класса для текущей платформы
	template <Platform p>
	class UsartSelector: public PlatformSelector<UsartTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		UsartSelector()
		{
		
		}
	};

	// Класс для текущей платформы
	typedef UsartSelector<RBLIB_PALTFORM_AUTODETECT> Usart;
}

#endif