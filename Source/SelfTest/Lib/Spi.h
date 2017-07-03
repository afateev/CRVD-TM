///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами SPI
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPI_H
#define SPI_H

#include "Platform.h"

#include "ATmega164PA\Spi.h"

namespace Lib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::Spi
		>::Result SpiTypes;
	
	// Селектор класса для текущей платформы
	template <Platform p>
	class SpiSelector: public PlatformSelector<SpiTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		SpiSelector()
		{
		
		}
	};

	// Класс для текущей платформы
	typedef SpiSelector<RBLIB_PALTFORM_AUTODETECT> Spi;
}

#endif