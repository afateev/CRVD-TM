///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_H
#define TIMER_H

#include "Platform.h"

#include "ATmega164PA\Timer.h"

namespace Lib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::Timer
		>::Result TimerTypes;
	
	// �������� ������ ��� ������� ���������
	template <Platform p>
	class TimerSelector: public PlatformSelector<TimerTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		TimerSelector()
		{
		
		}
	};

	// ����� ��� ������� ���������
	typedef TimerSelector<RBLIB_PALTFORM_AUTODETECT> Timer;
}

#endif