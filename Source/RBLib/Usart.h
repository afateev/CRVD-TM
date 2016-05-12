#ifndef USART_H
#define USART_H

#include "Platform.h"

#ifdef STM32
#include "stm32\usart.h"
#endif
 
namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef STM32
			PlatformStm32F1, Stm32::F1::Usart,
			PlatformStm32F4, Stm32::F4::Usart
#endif
		>::Result UsartTypes;
	
	// �������� ������ ���������� UART �����������
	template <Platform p>
	class UsartSelector: public PlatformSelector<UsartTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		UsartSelector()
		{
		
		}
	};

	// ����� ���������� UART �����������
	typedef UsartSelector<RBLIB_PALTFORM_AUTODETECT> Usart;
}

#endif