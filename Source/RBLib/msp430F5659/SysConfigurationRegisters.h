///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ������������ �������
//	MSP430F5659
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSCONFIGURATIONREGISTERS_MSP430F5659_H
#define SYSCONFIGURATIONREGISTERS_MSP430F5659_H

#include "../msp430/f5xxx/SysConfigurationRegisters.h"

namespace Rblib
{
namespace Msp430f5659
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
