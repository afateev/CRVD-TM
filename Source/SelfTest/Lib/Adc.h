///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ������� ���
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_H
#define ADC_H

#include "Platform.h"

#include "ATmega164PA\Adc.h"

namespace Lib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::Adc
		>::Result AdcTypes;
	
	// �������� ������ ��� ������� ���������
	template <Platform p>
	class AdcSelector: public PlatformSelector<AdcTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		AdcSelector()
		{
		
		}
	};

	// ����� ��� ������� ���������
	typedef AdcSelector<RBLIB_PALTFORM_AUTODETECT> Adc;
}

#endif