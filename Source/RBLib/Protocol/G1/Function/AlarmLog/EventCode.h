///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "лог тревог"
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_ALARMLOG_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_ALARMLOG_EVENTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace AlarmLog
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeAnomaly
	};
}
}
}
}
}

#endif