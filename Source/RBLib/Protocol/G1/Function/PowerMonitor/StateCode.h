///////////////////////////////////////////////////////////////////////////////
//	���� ��������� �������� �������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_POWERMONITOR_STATECODE_H
#define PROTOCOL_G1_FUNCTION_POWERMONITOR_STATECODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace PowerMonitor
{
	enum StateCode
	{
		StateCodeInvalid								= 0,	// ������������ ��� ���������
		StateCodePowerLo								= 1,	// ��������� ������� ���������� �������
		StateCodeBattery1VoltageMeasuredFlag			= 2,	// ���������� ������� 1 ���� ��������
		StateCodeBattery1Voltage						= 3,	// ���������� ���������� ������� 1
		StateCodeBattery2VoltageMeasuredFlag			= 4,	// ���������� ������� 2 ���� ��������
		StateCodeBattery2Voltage						= 5,	// ���������� ���������� ������� 2
		StateCodeCurrentConsumptionMeasuredFlag			= 6,	// ������������ ��� ��� �������
		StateCodeCurrentConsumption						= 7,	// ���������� �������� ������������� ����
		StateCodeInternalStorageVoltageMeasuredFlag		= 8,	// ���������� �� ���������� ���������� ���� ��������
		StateCodeInternalStorageVoltage					= 9,	// ���������� ���������� �� ���������� ����������
	};
}
}
}
}
}

#endif