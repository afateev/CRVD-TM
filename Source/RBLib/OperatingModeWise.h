
#ifndef OPERATING_MODE_WISE_H
#define OPERATING_MODE_WISE_H

#ifdef __ICC430__

#include "./OperatingMode.h"
#include "./system/OperatingModeWakeUp.h"

namespace Rblib
{
/// ����������� ����� ���������� �������� ���.
/// \details �������� �������� ������ � ������ ������� �� �����������, 
/// ���������� �������������� ������������ ���������� ��� ������ �� ����������.
class OperatingModeWise : public OperatingMode
{
public:

	/// �������� ���������� � ������������� ����������� ��� ������ �� ����������, ������������� ����.
	/// \param mode ����� ���, � ������� ����� ����������.
	static inline void WakeUp(unsigned short mode = OperatingMode::Active)
	{
		System::OperatingModeWakeUp::WakeUp(mode);
	}

	/// �������� ������ �� �����������.
	static inline void WakeUpCancel()
	{
		System::OperatingModeWakeUp::WakeUpCancel();
	}
};

}

#endif // __ICC430__

#endif // OPERATING_MODE_WISE_H
