///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ��������
//	MSP430F5435A
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef WDT_MSP430F5435A_H
#define WDT_MSP430F5435A_H

#include "../msp430/f5xxx/Wdt.h"
#include "../msp430/Wdt.h"

namespace Rblib
{
namespace Msp430f5435a
{
	// ��� �������� ��������
	typedef unsigned short WdtRegisterDataType;
	// ��� ������ ��������
	typedef unsigned int  WdtRegisterAddresType;
	
	// ������������������ ���������� ������
	class Wdt : public Msp430::Wdt< Msp430f5xxx::WtdControlRegister<WdtRegisterDataType, WdtRegisterAddresType, 0x015C> >
	{
	public:
	};
}
}

#endif