#ifndef DMA_H
#define DMA_H

#include "Platform.h"

#ifdef EFM32
#include "efm32gg232f1024\Dma.h"
#include "efm32gg990f1024\Dma.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg232f1024::Dma,
			PlatformEfm32gg990f1024, Efm32gg990f1024::Dma
#endif
		>::Result DmaTypes;
	
	// �������� ������ ������� ����������
	template <Platform p>
    class DmaSelector: public PlatformSelector<DmaTypes, p>::Result
	{
	private:
		// ����������� �� ���������
		DmaSelector()
		{
		}
	};

	// ����� ���������� �������� ����������
	typedef DmaSelector<RBLIB_PALTFORM_AUTODETECT> Dma;
}

#endif