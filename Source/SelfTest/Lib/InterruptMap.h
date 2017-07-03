///////////////////////////////////////////////////////////////////////////////
//	����� ���������� �������� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTMAP_H
#define INTERRUPTMAP_H

#include "Platform.h"

#ifdef __ICCAVR__
#include "ATmega164PA\InterruptMap.h"
#endif

namespace Lib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef __ICCAVR__
			PlatformATmega164PA, ATmega164PA::InterruptMap
#endif
		>::Result InterruptMapTypes;
	
	// �������� ������ ������� ����������
	template <Platform p>
    class InterruptMapSelector: public PlatformSelector<InterruptMapTypes, p>::Result
	{
	private:
		// ����������� �� ���������
		InterruptMapSelector()
		{
		}
	};

	// ����� ���������� �������� ����������
	typedef InterruptMapSelector<RBLIB_PALTFORM_AUTODETECT> InterruptMap;
}

#endif