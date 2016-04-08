#ifndef CMU_H
#define CMU_H

#include "Platform.h"

#ifdef EFM32
#include "efm32gg232f1024\ClockManagementUnit.h"
#include "efm32gg990f1024\ClockManagementUnit.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg232f1024::ClockManagementUnit,
			PlatformEfm32gg990f1024, Efm32gg990f1024::ClockManagementUnit
#endif
		>::Result ClockManagementUnitTypes;
	
	// �������� ������ ���������� ������� ����� ������
	template <Platform p>
	class ClockManagementUnitSelector: public PlatformSelector<ClockManagementUnitTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		ClockManagementUnitSelector()
		{
		
		}
	};

	// ����� ���������� ��������� ������� �����-������
	typedef ClockManagementUnitSelector<RBLIB_PALTFORM_AUTODETECT> ClockManagementUnit;
}

#endif