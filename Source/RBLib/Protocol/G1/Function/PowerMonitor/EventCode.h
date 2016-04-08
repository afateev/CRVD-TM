///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Монитор питания"
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_POWERMONITOR_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_POWERMONITOR_EVENTCODE_H

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
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodePowerStateChanged,
		EventCodeParamsChanged,
	};
}
}
}
}
}

#endif