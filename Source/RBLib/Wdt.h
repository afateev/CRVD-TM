///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ��������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef WDT_H
#define WDT_H

#include "Platform.h"

#ifdef MSP430
#include "msp430f1611\Wdt.h"
#include "msp430f2618\Wdt.h"
#include "msp430f5340\Wdt.h"
//#include "msp430f5342\Wdt.h"
#include "msp430f5435a\Wdt.h"
#include "msp430f5438a\Wdt.h"
#include "msp430f5659\Wdt.h"
#endif

#ifdef EFM32
#include "efm32gg232f1024\Wdt.h"
#include "efm32gg990f1024\Wdt.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f1611, Msp430f1611::Wdt,
			PlatformMsp430f2618, Msp430f2618::Wdt,
      PlatformMsp430f5340, Msp430f5340::Wdt,
	  //PlatformMsp430f5342, Msp430f5342::Wdt,
			PlatformMsp430f5435a, Msp430f5435a::Wdt,
			PlatformMsp430f5438a, Msp430f5438a::Wdt,
			PlatformMsp430f5659, Msp430f5659::Wdt
#endif
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg232f1024::Wdt,
			PlatformEfm32gg990f1024, Efm32gg990f1024::Wdt
#endif
		>::Result WdtTypes;
	
	// �������� ������ ���������� ���������� ��������
	template <Platform p>
	class WdtSelector: public PlatformSelector<WdtTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		WdtSelector()
		{
		
		}
	};

	// ����� ���������� ���������� ��������
	typedef WdtSelector<RBLIB_PALTFORM_AUTODETECT> Wdt;
}

#endif
