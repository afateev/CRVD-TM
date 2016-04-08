///////////////////////////////////////////////////////////////////////////////
//	����� SPI ����������
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
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f5340, Msp430f5340::Spi,
			PlatformMsp430f5659, Msp430f5659::Spi

#endif
		>::Result SpiTypes;
	
	// �������� ������ ���������� SPI �����������
	template <Platform p>
	class SpiSelector: public PlatformSelector<SpiTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		SpiSelector()
		{
		
		}
	};

	// ����� ���������� SPI �����������
	typedef SpiSelector<RBLIB_PALTFORM_AUTODETECT> Spi;
}

#endif //SPI_H
