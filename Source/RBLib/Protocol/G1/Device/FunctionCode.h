///////////////////////////////////////////////////////////////////////////////
//	Формат пакета общий для всех устройств
//	Коды функций
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_DEVICE_FUNCTIONCODE_H
#define PROTOCOL_G1_DEVICE_FUNCTIONCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Device
{
	enum FunctionCode
	{
		FunctionCodeContainer				= 0,
		FunctionCodeTextLog					= 1,
		FunctionCodeWirelessModem			= 2,
		FunctionCodeSeismicSensor			= 3,
		FunctionCodeSubscriptionService		= 4,
		FunctionCodeCamera					= 5,
		FunctionCodeStorage					= 6,
		FunctionCodeRadiodozor				= 7,
		FunctionCodeWirebreakSensor			= 8,
		FunctionCodeRelay					= 9,
		FunctionCodeDiagnostics				= 10,
		FunctionCodeActionService			= 11,
		FunctionCodeAlarmLog				= 12,
		FunctionCodeClock					= 13,
		FunctionCodePowerMonitor			= 14,
		FunctionCodeRadiobeacon				= 15,
		FunctionCodeLocation				= 16,
	};
}
}
}
}

#endif