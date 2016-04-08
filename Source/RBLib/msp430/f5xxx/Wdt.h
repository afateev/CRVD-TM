///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ��������
//	MSP430F5XXX
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef WDT_MSP430F5XXX_H
#define WDT_MSP430F5XXX_H

#include <string.h>

namespace Rblib
{
namespace Msp430f5xxx
{
	// ����� ���������� ���������� ��������
	template <class RegisterDataType,		// ��� (������) �������� ��������
			class PointerType,				// ��� (������) �������� ������� ���������
			PointerType ControlRegisterAddr	// ����� �������� ����������
		>
	class WtdControlRegister
	{
	protected:
		// ����� �������� ���������� ���������� ��������
		static volatile RegisterDataType * const _wdtControlRegisterAddr;

		// ��������� �������� ���������� ���������� ��������
		struct Struct
		{
			unsigned short Interval		: 3;	// �������� ������������
			unsigned short Clear		: 1;	// ���� ������ �������
			unsigned short Mode			: 1;	// ����� �������
			unsigned short ClockSource	: 2;	// �������� ������������
			unsigned short Hold			: 1;	// ���� ��������� �������
			unsigned short Password		: 8;	// ������ ������� � ��������
		};
	public:
		// ���������� ��������� ������������ WDT
		enum WdtIntrvals
		{
			WdtInterval2G		= 0,
			WdtInterval128M		= 1,
			WdtInterval8192k	= 2,
			WdtInterval512k		= 3,
			WdtInterval32k		= 4,
			WdtInterval8192		= 5,
			WdtInterval512		= 6,
			WdtInterval64		= 7
		};
		enum WdtClockSource
		{
			WdtClockSourceSubsystemMasterClock	= 0,
			WdtClockSourceAuxClock				= 1,
			WdtClockSourceVeryLowPowerClock		= 2,
			WdtClockSourceXClock				= 3
		};
	protected:
		// ������ �������� ��������
		static inline void GetControl(Struct *value)
		{
			 memcpy(value, (const void*)_wdtControlRegisterAddr, sizeof(RegisterDataType));
		}
		// ������ �������� ��������
		static inline void SetControl(Struct *value)
		{
			// ���������� ������������ ������ �� ������
			value->Password = 0x5A;
			// �.�. � ������� ����� ������ ������ ����� ������
			// ������� ��� �����
			RegisterDataType tmp;
			// �������� ���� �������� ���������
			memcpy(&tmp, value, sizeof(RegisterDataType));
			// ���������� � �������
			*_wdtControlRegisterAddr = tmp;
		}
    static inline void SetControlRaw(RegisterDataType value)
    {
      *_wdtControlRegisterAddr = value;
    }
	};
	
	// ��������������� ����������� ����������
	template<class RegisterDataType, class PointerType, PointerType ControlRegisterAddr>
	volatile  RegisterDataType* const WtdControlRegister<RegisterDataType, PointerType, ControlRegisterAddr>
		::_wdtControlRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(ControlRegisterAddr);
}
}

#endif