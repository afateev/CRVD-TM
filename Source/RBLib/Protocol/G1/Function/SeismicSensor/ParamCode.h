///////////////////////////////////////////////////////////////////////////////
//	Коды параметров сейсмического датчика
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_SEISMICSENSOR_PARAMCODE_H
#define PROTOCOL_G1_FUNCTION_SEISMICSENSOR_PARAMCODE_H

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
	enum ParamCode
	{
		ParamCodeInvalid			= 0,
		ParamCodeThresholdHuman		= 1,
		ParamCodeThresholdVehicle	= 2,
		ParamCodeFrequentAlarmsDensityThreshold	= 3,
		ParamCodeFrequentAlarmsIntervalLenght	= 4,
		ParamCodeHumanEnable		= 5,
		ParamCodeVehicleEnable		= 6
	};
}
}
}
}
}

#endif