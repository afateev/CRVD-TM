#ifndef SYSTEMTIMER_H
#define SYSTEMTIMER_H

#include "Platform.h"

#ifdef EFM32
#include "efm32gg\SystemTimer.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg::SystemTimer,
			PlatformEfm32gg990f1024, Efm32gg::SystemTimer
#endif
		>::Result SystemTimerTypes;
	
	// �������� ������
	template <Platform p>
    class SystemTimerSelector: public PlatformSelector<SystemTimerTypes, p>::Result
	{
	private:
		// ����������� �� ���������
		SystemTimerSelector()
		{
		}
	};

	// �����
	typedef SystemTimerSelector<RBLIB_PALTFORM_AUTODETECT> SystemTimer;
}

#endif