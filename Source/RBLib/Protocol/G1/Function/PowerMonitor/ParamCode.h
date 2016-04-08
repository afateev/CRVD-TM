///////////////////////////////////////////////////////////////////////////////
//	Коды параметров монитора питания
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
		ParamCodeInvalid									= 0, // недопустимый код параметра
		ParamCodeBatteryMeasurementMultiplier				= 1, // множиетль для расчета напряжения батареи
		ParamCodeCurrentConsumptionMeasurementMultiplier	= 2, // множитель для расчета потребляемого тока
		ParamCodeInternalStorageMeasurementMultiplier		= 3, // множитель для расчета напряжения на внутреннем накопителе
		ParamCodePowerOnThreshold							= 4, // порог напряжения, при котором принимается решение "питание включено" / "питание отключено"
		ParamCodePowerLoThreshold							= 5, // порог напряжения, при котором выдается сигнал о низком напряжении питания
		ParamCodePowerLoTimeout								= 6, // время в секундах, в течении которого принимается решение о низком напряжении питания
	};
}
}
}
}
}

#endif