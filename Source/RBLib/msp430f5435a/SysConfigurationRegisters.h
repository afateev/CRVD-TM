///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ������������ �������
//	MSP430F5435A
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSCONFIGURATIONREGISTERS_MSP430F5435A_H
#define SYSCONFIGURATIONREGISTERS_MSP430F5435A_H

#include "../msp430/f5xxx/SysConfigurationRegisters.h"

namespace Rblib
{
namespace Msp430f5435a
{

// ��� �������� ��������
typedef unsigned short RegisterDataType;
// ��� ������ ��������
typedef unsigned int  RegisterAddresType;

// ������������������ ����� ���������
class SysConfigurationRegisters :
	public Msp430f5xxx::SysResetInterruptVector<RegisterDataType, RegisterAddresType, 0x19E>
{

};

}
}

#endif