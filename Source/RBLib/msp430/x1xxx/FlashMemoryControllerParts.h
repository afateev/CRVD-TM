///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������� ������� ����������� ���� ������
//	MSP430X1XXX
//
//	������ ������ ������ ��������� �� ������ ������������� ��� �� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYCONTROLLER_PARTS_MSP430X1XXX_H
#define FLASHMEMORYCONTROLLER_PARTS_MSP430X1XXX_H

namespace Rblib
{

namespace Msp430x1xxx
{
	template<class RegisterDataType>
	struct FlashMemoryControl1Register
	{
		RegisterDataType Reserved0						: 1;
		RegisterDataType Erase							: 1;	// ���� ������ �������
		RegisterDataType MassErase						: 1;	// ���� ������ �������� �������
		RegisterDataType Reserved1						: 3;
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
		RegisterDataType Reserved0		: 2;	// ���� ��� ���������� ��������
		RegisterDataType Password			: 8;	// ������ ������� � ��������
	};
	
}
}

#endif
