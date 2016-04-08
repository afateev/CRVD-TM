///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Сейсмический датчик" устройтсва
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_SEISMICSENSOR_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_SEISMICSENSOR_EVENTCODE_H

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
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeAlarm,
		EventCodeFrequentAlarm,
		EventCodeParamsChanged,
		EventCodeAutotuningComplete
	};
}
}
}
}
}

#endif