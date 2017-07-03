///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ������� SPI
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPI_H
#define SPI_H

#include "Platform.h"

#include "ATmega164PA\Spi.h"

namespace Lib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::Spi
		>::Result SpiTypes;
	
	// �������� ������ ��� ������� ���������
	template <Platform p>
	class SpiSelector: public PlatformSelector<SpiTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		SpiSelector()
		{
		
		}
	};

	// ����� ��� ������� ���������
	typedef SpiSelector<RBLIB_PALTFORM_AUTODETECT> Spi;
}

#endif