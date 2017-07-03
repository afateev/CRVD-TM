///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ������� USART
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef USART_H
#define USART_H

#include "Platform.h"

#include "ATmega164PA\Usart.h"

namespace Lib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::Usart
		>::Result UsartTypes;
	
	// �������� ������ ��� ������� ���������
	template <Platform p>
	class UsartSelector: public PlatformSelector<UsartTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		UsartSelector()
		{
		
		}
	};

	// ����� ��� ������� ���������
	typedef UsartSelector<RBLIB_PALTFORM_AUTODETECT> Usart;
}

#endif