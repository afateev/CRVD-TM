///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ����������� ���������
//	MSP430X1XXX, MSP430X2XXX
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_MSP430X1XXX_MSP430X2XXX_H
#define TIMER_MSP430X1XXX_MSP430X2XXX_H

#include "../../IdObjectBase.h"
#include "../TimerParts.h"

namespace Rblib
{
namespace Msp430x12xxx
{
	// ��� �������� ��������� �������
	typedef unsigned short TimerValueDataType;
	// ��� ������ �������� �������
	typedef unsigned int  TimerRegisterAddresType;

	// ������ ���� ��������� ���������� � �������� ��� �������/���������
	template<TimerRegisterAddresType R1,
		TimerRegisterAddresType R2
	>
	class TimerCaptureCompare:
		public Msp430::TimerCaptureCompareControl<TimerValueDataType, TimerRegisterAddresType, R1>,
		public Msp430::TimerCaptureCompareRegister<TimerValueDataType, TimerRegisterAddresType, R2>
	{
	
	};

	// ������� ������ �������
	template<unsigned int IdObj,
		TimerRegisterAddresType R2,
		TimerRegisterAddresType R3,
		class BaseCaptureCompareRegister,
		class TimerControlImpl
	>
	class TimerBase:
		public IdObjectBase<IdObj>,
		public TimerControlImpl,
		public Msp430::TimerCounter<TimerValueDataType, TimerRegisterAddresType, R2>,
		public Msp430::TimerInterruptVector<TimerValueDataType, TimerRegisterAddresType, R3>
	{
	public:

		typedef	TimerValueDataType RegisterValueType;

	public:
		// ���������� ������������ �������� ��������
		static inline void SetMaxCount(TimerValueDataType value)
		{
			BaseCaptureCompareRegister::SetValue(value);
		}
	};

	// ������� ���������� ���� �� ����� ������ ����������� ��������� �������/���������

	// ������� ������� ������� ���������� ��� ������� A
	typedef TimerCaptureCompare<0x0162, 0x0172> TimerABaseCaptureCompareReg;

	// ����� ������� A
	class TimerA: public TimerBase<1, 0x0170, 0x012E, TimerABaseCaptureCompareReg
									, Msp430::TimerControl<TimerRegisterAddresType, 0x0160> >
	{
	public:
		// �������� �������/���������
		typedef TimerABaseCaptureCompareReg			CC0;
		typedef TimerCaptureCompare<0x0164, 0x0174> CC1;
		typedef TimerCaptureCompare<0x0166, 0x0176> CC2;
	};
	
	
	// ������� ������� ������� ���������� ��� ������� B
	typedef TimerCaptureCompare<0x0182, 0x0192> TimerBBaseCaptureCompareReg;

	// ����� ������� B
	class TimerB: public TimerBase<2, 0x0190, 0x011E, TimerBBaseCaptureCompareReg
				, Msp430::TimerBControl<TimerRegisterAddresType, 0x0180> >
	{
	public:
		// �������� �������/���������
		typedef TimerBBaseCaptureCompareReg			CC0;
		typedef TimerCaptureCompare<0x0184, 0x0194> CC1;
		typedef TimerCaptureCompare<0x0186, 0x0196> CC2;
		typedef TimerCaptureCompare<0x0188, 0x0198> CC3;
		typedef TimerCaptureCompare<0x018A, 0x019A> CC4;
		typedef TimerCaptureCompare<0x018C, 0x019C> CC5;
		typedef TimerCaptureCompare<0x018E, 0x019E> CC6;
	};
}
}

#endif // TIMER_MSP430X1XXX_MSP430X2XXX_H
