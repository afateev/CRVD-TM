#ifndef SYSTEMCONTROLBLOCK_H
#define SYSTEMCONTROLBLOCK_H

#include "Platform.h"

#ifdef EFM32
#include "efm32gg\SystemControlBlock.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg::SystemControlBlock,
			PlatformEfm32gg990f1024, Efm32gg::SystemControlBlock
#endif
		>::Result SystemControlBlockTypes;
	
	// �������� ������
	template <Platform p>
    class SystemControlBlockSelector: public PlatformSelector<SystemControlBlockTypes, p>::Result
	{
	private:
		// ����������� �� ���������
		SystemControlBlockSelector()
		{
		}
	};

	// �����
	typedef SystemControlBlockSelector<RBLIB_PALTFORM_AUTODETECT> SystemControlBlock;
}

#endif