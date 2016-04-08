///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Радиодозор" устройтсва
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_LOCATION_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_LOCATION_EVENTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Location
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeStateChanged,
		EventCodeLocationChanged,
		EventCodeLocationNotify
	};
}
}
}
}
}

#endif