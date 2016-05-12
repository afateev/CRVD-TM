#ifndef USART_H
#define USART_H

#include "Platform.h"

#ifdef STM32
#include "stm32\usart.h"
#endif
 
namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef STM32
			PlatformStm32F1, Stm32::F1::Usart,
			PlatformStm32F4, Stm32::F4::Usart
#endif
		>::Result UsartTypes;
	
	// Селектор класса управления UART интерфейсом
	template <Platform p>
	class UsartSelector: public PlatformSelector<UsartTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		UsartSelector()
		{
		
		}
	};

	// Класс управления UART интерфейсом
	typedef UsartSelector<RBLIB_PALTFORM_AUTODETECT> Usart;
}

#endif