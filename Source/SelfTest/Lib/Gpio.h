///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ������� �����-������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_H
#define GPIO_H

#include "Platform.h"

#include "ATmega164PA\Gpio.h"

namespace Lib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::Gpio
		>::Result GpioTypes;
	
	// �������� ������ ��� ������� ���������
	template <Platform p>
	class GpioSelector: public PlatformSelector<GpioTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		GpioSelector()
		{
		
		}
	};

	// ����� ��� ������� ���������
	typedef GpioSelector<RBLIB_PALTFORM_AUTODETECT> Gpio;
}

#endif