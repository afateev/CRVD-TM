///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ����������� �������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPECIALFUNCTIONREGISTERS_H
#define SPECIALFUNCTIONREGISTERS_H

#include "Platform.h"

#ifdef MSP430
#include "msp430f1611\SpecialFunctionRegisters.h"
#include "msp430f2618\SpecialFunctionRegisters.h"
#include "msp430f5340\SpecialFunctionRegisters.h"
//#include "msp430f5342\SpecialFunctionRegisters.h"
#include "msp430f5435a\SpecialFunctionRegisters.h"
#include "msp430f5438a\SpecialFunctionRegisters.h"
#include "msp430f5659\SpecialFunctionRegisters.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f1611, Msp430f1611::SpecialFunctionRegisters,
			PlatformMsp430f2618, Msp430f2618::SpecialFunctionRegisters,
      PlatformMsp430f5340, Msp430f5340::SpecialFunctionRegisters,
			//PlatformMsp430f5342, Msp430f5340::SpecialFunctionRegisters,
			PlatformMsp430f5435a, Msp430f5435a::SpecialFunctionRegisters,
			PlatformMsp430f5438a, Msp430f5438a::SpecialFunctionRegisters,
			PlatformMsp430f5659, Msp430f5659::SpecialFunctionRegisters
#endif
		>::Result SpecialFunctionRegistersTypes;
	
	// �������� ������ ���������� ���������� ����������� �������
	template <Platform p>
	class SpecialFunctionRegistersSelector: public PlatformSelector<SpecialFunctionRegistersTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		SpecialFunctionRegistersSelector()
		{
		
		}
	};

	// ����� ���������� ���������� ����������� �������
	typedef SpecialFunctionRegistersSelector<RBLIB_PALTFORM_AUTODETECT> SpecialFunctionRegisters;
}

#endif
