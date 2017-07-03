///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами SPI
//	ATmega164PA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPI_ATMEGA164PA_H
#define SPI_ATMEGA164PA_H

#include "../ATmega/Spi.h"
#include "../IdObjectBase.h"

namespace Lib
{
namespace ATmega164PA
{
	// Платформозависимый набор портов
	class Spi
	{
	public:
		typedef ATmega::Spi::Spi<1, 0x4C, 0x4D, 0x4E> Spi0;
	};
}
}

#endif