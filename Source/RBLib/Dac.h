///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef DAC_H
#define DAC_H

#include "Platform.h"

#ifdef EFM32
#include "efm32gg232f1024\Dac.h"
#include "efm32gg990f1024\Dac.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg232f1024::Dac,
			PlatformEfm32gg990f1024, Efm32gg990f1024::Dac
#endif
		>::Result DacTypes;
	
	// �������� ������
	template <Platform p>
	class DacSelector: public PlatformSelector<DacTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		DacSelector()
		{
		
		}
	};

	// ����� ���������� ���
	typedef DacSelector<RBLIB_PALTFORM_AUTODETECT> Dac;
}

#endif