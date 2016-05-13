#ifndef RBLIB_USB_H
#define RBLIB_USB_H

#include "Platform.h"

#ifdef STM32
#include "stm32\Usb.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef STM32
			PlatformStm32F4, Stm32::F4::Usb
#endif
		>::Result UsbTypes;
	
	// Селектор класса
	template <Platform p>
	class UsbSelector: public PlatformSelector<UsbTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		UsbSelector()
		{
		
		}
	};

	// Класс
	typedef UsbSelector<RBLIB_PALTFORM_AUTODETECT> Usb;
}

#endif