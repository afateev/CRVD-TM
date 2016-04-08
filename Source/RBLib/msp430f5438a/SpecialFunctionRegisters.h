///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ����������� �������
//	MSP430F5438A
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPECIALFUNCTIONREGISTERS_MSP430F5438A_H
#define SPECIALFUNCTIONREGISTERS_MSP430F5438A_H

#include "../msp430/f5xxx/SpecialFunctionRegisters.h"

namespace Rblib
{
namespace Msp430f5438a
{

// ��� �������� ��������
typedef unsigned short RegisterDataType;
// ��� ������ ��������
typedef unsigned int  RegisterAddresType;

// ������������������ ����� ���������
class SpecialFunctionRegisters :
	public Msp430f5xxx::InterruptEnableRegister<RegisterDataType, RegisterAddresType, 0x100>,
	public Msp430f5xxx::InterruptFlagRegister<RegisterDataType, RegisterAddresType, 0x102>,
	public Msp430f5xxx::ResetPinControlRegister<RegisterDataType, RegisterAddresType, 0x104>
{
};

}
}

#endif