///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Реле" устройтсва
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_RELAY_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_RELAY_EVENTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Relay
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeRelayStateChanged,
		EventCodeParamsChanged,
	};
}
}
}
}
}

#endif