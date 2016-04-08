///////////////////////////////////////////////////////////////////////////////
//	Коды параметров тревог сейсмического датчика
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ALARMPARAMCODE_H
#define PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ALARMPARAMCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace SeismicSensor
{
	enum AlarmParamCode
	{
		AlarmParamCodeInvalid				= 0,
		AlarmParamCodeHumanDetectedFlag		= 1,
		AlarmParamCodeVehicleDetectedFlag	= 2,
	};
}
}
}
}
}

#endif