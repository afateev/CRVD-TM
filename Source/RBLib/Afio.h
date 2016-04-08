#ifndef RBLIB_AFIO_H
#define RBLIB_AFIO_H

#include "Platform.h"

#ifdef STM32
#include "stm32\Afio.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef STM32
			PlatformStm32, Stm32::Afio
#endif
		>::Result AfioTypes;
	
	// �������� ������
	template <Platform p>
	class AfioSelector: public PlatformSelector<AfioTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		AfioSelector()
		{
		
		}
	};

	// �����
	typedef AfioSelector<RBLIB_PALTFORM_AUTODETECT> Afio;
}

#endif