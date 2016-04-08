///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ��������
//	MSP430
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef WDT_MSP430_H
#define WDT_MSP430_H

#include <string.h>

namespace Rblib
{
namespace Msp430
{
	// ����� ���������� ���������� ��������
	template <class WdtControlRegister			// ��������� ������������ ��������, � ������ ������� � ���
		>
	class Wdt : public WdtControlRegister
	{
	public:
		typedef typename WdtControlRegister::WdtIntrvals	WdtIntrvals;
		typedef typename WdtControlRegister::WdtClockSource WdtClockSource;
		typedef typename WdtControlRegister::Struct			Struct;
		typedef WdtControlRegister							Control;
	public:
		// ���������� ������
		static inline void Stop()
		{
			// ��������� �������� ��������
			Struct tmp;
			// ������ ��� ���
			Control::GetControl(&tmp);
			// �������� ��� ����
			tmp.Hold = 1;
			// ���������� �������
			Control::SetControl(&tmp);
		}
		// ��������� ������
		static inline void Start()
		{
			// ��������� �������� ��������
			Struct tmp;
			// ������ ��� ���
			Control::GetControl(&tmp);
			// �������� ��� ����
			tmp.Hold = 0;
			// ���������� �������
			Control::SetControl(&tmp);
		}
		// ����� �������
		static inline void Clear()
		{
			// ��������� �������� ��������
			Struct tmp;
			// ������ ��� ���
			Control::GetControl(&tmp);
			// �������� ��� ����
			tmp.Clear = 1;
			// ���������� �������
			Control::SetControl(&tmp);
		}
		// ���������� �������� ������������ �������
		static inline void SetInterval(WdtIntrvals value)
		{
			// ��������� �������� ��������
			Struct tmp;
			// ������ ��� ���
			Control::GetControl(&tmp);
			// �������� ��� ����
			tmp.Interval = value;
			// ���� ��������� �������, ����� ����� ���������
			tmp.Clear = 1;
			// ���������� �������
			Control::SetControl(&tmp);
		}
		// ���������� �������� ������������
		static inline void SetClockSource(WdtClockSource value)
		{
			// ��������� �������� ��������
			Struct tmp;
			// ������ ��� ���
			Control::GetControl(&tmp);
			// �������� ��� ����
			tmp.ClockSource = value;
			// ���������� �������
			Control::SetControl(&tmp);
		}
		static inline void GeneratePUC()
		{
			Control::SetControlRaw(0);
		}
	};
}
}

#endif