#ifndef SPIMASTER_H
#define SPIMASTER_H

#include "Platform.h"

#ifdef EFM32
#include "efm32gg232f1024\SpiMaster.h"
#include "efm32gg990f1024\SpiMaster.h"
#endif
 
namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg232f1024::SpiMaster,
			PlatformEfm32gg990f1024, Efm32gg990f1024::SpiMaster
#endif
		>::Result SpiMasterTypes;
	
	// �������� ������ ���������� SPI �����������
	template <Platform p>
	class SpiMasterSelector: public PlatformSelector<SpiMasterTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		SpiMasterSelector()
		{
		
		}
	};

	// ����� ���������� SPI �����������
	typedef SpiMasterSelector<RBLIB_PALTFORM_AUTODETECT> SpiMaster;
}

#endif //SPIMASTER_H
