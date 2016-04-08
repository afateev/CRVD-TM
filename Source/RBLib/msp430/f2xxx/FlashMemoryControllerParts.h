///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������� ������� ����������� ���� ������
//	MSP430F2XXX
//
//	������ ������ ������ ��������� �� ������ ������������� ��� �� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYCONTROLLER_PARTS_MSP430F2XXX_H
#define FLASHMEMORYCONTROLLER_PARTS_MSP430F2XXX_H

namespace Rblib
{
// ����� ����������� ��� ��������� MSP430 ���� ��������
namespace Msp430f2xxx
{
	template<class RegisterDataType>
	struct FlashMemoryControl1Register
	{
		RegisterDataType Reserved0						: 1;
		RegisterDataType Erase							: 1;	// ���� ������ �������
		RegisterDataType MassErase						: 1;	// ���� ������ �������� �������
		RegisterDataType EnableEraseInterrupts			: 1;	// ���������� ���������� �� ����� ��������
		RegisterDataType EnableEmergencyInterruptExit	: 1;	// ��������� ���������� ������ ����������� ��� ����������
		RegisterDataType Reserved1						: 1;
		RegisterDataType Write							: 1;	// ���� ���������� ������
		RegisterDataType BlockWrite						: 1;	// ���� ���������� ������� ������
		RegisterDataType Password						: 8;	// ������ ������� � ��������
	};
	
	template<class RegisterDataType>
	struct FlashMemoryControl3Register
	{
		RegisterDataType Busy				: 1;	// ������
		RegisterDataType PasswordViolation	: 1;	// �������� ������
		RegisterDataType AccessViolationInt	: 1;	// ��������� ���������� ��� ������ �������
		RegisterDataType Wait				: 1;	// ������ ������ ��� ������
		RegisterDataType Lock				: 1;	// ������/���������� ������
		RegisterDataType EmergencyExit		: 1;	// �������� ��������� �������� ������
		RegisterDataType LockA				: 1;	// ���������� �������� A
		RegisterDataType Fail				: 1;	// ���� ��� ���������� ��������
		RegisterDataType Password			: 8;	// ������ ������� � ��������
	};
	
	template<class RegisterDataType>
	struct FlashMemoryControl4Register
	{
		RegisterDataType Reserved0				: 4;
		RegisterDataType MarginalRead0			: 1;	// ����� ������ ����� ��� ��������� ��������
		RegisterDataType MarginalRead1			: 1;	// ����� ������ ������ ��� ��������� ��������
		RegisterDataType Reserved1				: 2;
		RegisterDataType Password				: 8;	// ������ ������� � ��������
	};
}
}

#endif