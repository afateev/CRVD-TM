#ifndef RTC_H
#define RTC_H

#include "Platform.h"

#ifdef STM32
#include "stm32\Rtc.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef STM32
		PlatformStm32F4, Stm32::F4::Rtc
#endif
		>::Result RtcTypes;
	
	// �������� ������ ���������� ������� ����� ������
	template <Platform p>
	class RtcSelector: public PlatformSelector<RtcTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		RtcSelector()
		{
		
		}
	};

	// ����� ���������� ��������� ������� �����-������
	typedef RtcSelector<RBLIB_PALTFORM_AUTODETECT> Rtc;
}

#endif