#ifndef NVIC_H
#define NVIC_H

#include "Platform.h"

#ifdef EFM32
#include "efm32gg232f1024\Nvic.h"
#include "efm32gg990f1024\Nvic.h"
#endif

#ifdef STM32
#include "stm32\Nvic.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg232f1024::Nvic,
			PlatformEfm32gg990f1024, Efm32gg990f1024::Nvic
#endif
#ifdef STM32
			PlatformStm32, Stm32::Nvic
#endif
		>::Result NvicTypes;
	
	// �������� ������ ������� ����������
	template <Platform p>
    class NvicSelector: public PlatformSelector<NvicTypes, p>::Result
	{
	private:
		// ����������� �� ���������
		NvicSelector()
		{
		}
	};

	// ����� ���������� �������� ����������
	typedef NvicSelector<RBLIB_PALTFORM_AUTODETECT> Nvic;
}

#endif