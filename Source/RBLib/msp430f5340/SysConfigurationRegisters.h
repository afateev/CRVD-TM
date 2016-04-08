///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ������������ �������
//	MSP430F5340
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSCONFIGURATIONREGISTERS_MSP430F5340_H
#define SYSCONFIGURATIONREGISTERS_MSP430F5340_H

#include "../msp430/f5xxx/SysConfigurationRegisters.h"

namespace Rblib
{
namespace Msp430f5340
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