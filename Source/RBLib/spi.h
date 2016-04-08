///////////////////////////////////////////////////////////////////////////////
//	Класс SPI интерфейса
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPI_H
#define SPI_H

#include "Platform.h"

#ifdef MSP430
#include "msp430f5340\spi.h"
#include "msp430F5659\spi.h"
#endif
 
namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f5340, Msp430f5340::Spi,
			PlatformMsp430f5659, Msp430f5659::Spi

#endif
		>::Result SpiTypes;
	
	// Селектор класса управления SPI интерфейсом
	template <Platform p>
	class SpiSelector: public PlatformSelector<SpiTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		SpiSelector()
		{
		
		}
	};

	// Класс управления SPI интерфейсом
	typedef SpiSelector<RBLIB_PALTFORM_AUTODETECT> Spi;
}

#endif //SPI_H
