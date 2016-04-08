///////////////////////////////////////////////////////////////////////////////
//	Коды состояний монитора питания
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
		StateCodeInvalid								= 0,	// недопустимый код состояния
		StateCodePowerLo								= 1,	// индикация низкого няпряжения питания
		StateCodeBattery1VoltageMeasuredFlag			= 2,	// напряжение батареи 1 было измерено
		StateCodeBattery1Voltage						= 3,	// измеренное напряжение батареи 1
		StateCodeBattery2VoltageMeasuredFlag			= 4,	// напряжение батареи 2 было измерено
		StateCodeBattery2Voltage						= 5,	// измеренное напряжение батареи 2
		StateCodeCurrentConsumptionMeasuredFlag			= 6,	// потребляемый ток был измерен
		StateCodeCurrentConsumption						= 7,	// измеренное значение потребляемого тока
		StateCodeInternalStorageVoltageMeasuredFlag		= 8,	// напряжение на внутреннем накопителе было измерено
		StateCodeInternalStorageVoltage					= 9,	// измеренное напряжение на внутреннем накопителе
	};
}
}
}
}
}

#endif