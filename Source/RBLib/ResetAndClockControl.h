#ifndef RCC_H
#define RCC_H

#include "Platform.h"

#ifdef STM32
#include "stm32\ResetAndClockControl.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef STM32
			PlatformStm32, Stm32::ResetAndClockControl
#endif
		>::Result ResetAndClockControlTypes;
	
	// �������� ������ ���������� ������� ����� ������
	template <Platform p>
	class ResetAndClockControlSelector: public PlatformSelector<ResetAndClockControlTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		ResetAndClockControlSelector()
		{
		
		}
	};

	// ����� ���������� ��������� ������� �����-������
	typedef ResetAndClockControlSelector<RBLIB_PALTFORM_AUTODETECT> ResetAndClockControl;
}

#endif