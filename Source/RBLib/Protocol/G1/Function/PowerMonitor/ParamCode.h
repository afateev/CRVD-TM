///////////////////////////////////////////////////////////////////////////////
//	���� ���������� �������� �������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_POWERMONITOR_PARAMCODE_H
#define PROTOCOL_G1_FUNCTION_POWERMONITOR_PARAMCODE_H

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
	enum ParamCode
	{
		ParamCodeInvalid									= 0, // ������������ ��� ���������
		ParamCodeBatteryMeasurementMultiplier				= 1, // ��������� ��� ������� ���������� �������
		ParamCodeCurrentConsumptionMeasurementMultiplier	= 2, // ��������� ��� ������� ������������� ����
		ParamCodeInternalStorageMeasurementMultiplier		= 3, // ��������� ��� ������� ���������� �� ���������� ����������
		ParamCodePowerOnThreshold							= 4, // ����� ����������, ��� ������� ����������� ������� "������� ��������" / "������� ���������"
		ParamCodePowerLoThreshold							= 5, // ����� ����������, ��� ������� �������� ������ � ������ ���������� �������
		ParamCodePowerLoTimeout								= 6, // ����� � ��������, � ������� �������� ����������� ������� � ������ ���������� �������
	};
}
}
}
}
}

#endif