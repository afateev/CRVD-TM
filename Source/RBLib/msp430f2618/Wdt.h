///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ��������
//	MSP430F2618
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef WDT_MSP430F2618_H
#define WDT_MSP430F2618_H

#include "../msp430/f2xxx/Wdt.h"
#include "../msp430/Wdt.h"

namespace Rblib
{
namespace Msp430f2618
{
	// ��� �������� ��������
	typedef unsigned short WdtRegisterDataType;
	// ��� ������ ��������
	typedef unsigned int  WdtRegisterAddresType;
	
	// ������������������ ���������� ������
	class Wdt : public Msp430::Wdt< Msp430f2xxx::WdtControlRegister<WdtRegisterDataType, WdtRegisterAddresType, 0x0120> >
	{
	public:
	};
}
}

#endif