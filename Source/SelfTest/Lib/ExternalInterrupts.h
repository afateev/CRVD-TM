///////////////////////////////////////////////////////////////////////////////
//	����� ���������� �������� ������������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef EXTERNALINTERRUPTS_H
#define EXTERNALINTERRUPTS_H

#include "Platform.h"

#include "ATmega164PA\ExternalInterrupts.h"

namespace Lib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::ExternalInterrupts
		>::Result ExternalInterruptsTypes;
	
	// �������� ������ ��� ������� ���������
	template <Platform p>
	class ExternalInterruptsSelector: public PlatformSelector<ExternalInterruptsTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		ExternalInterruptsSelector()
		{
		
		}
	};

	// ����� ��� ������� ���������
	typedef ExternalInterruptsSelector<RBLIB_PALTFORM_AUTODETECT> ExternalInterrupts;
}

#endif