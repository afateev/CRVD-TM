///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Диагностика" устройтсва
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_DIAGNOSTICS_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_DIAGNOSTICS_EVENTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Diagnostics
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeDeviceStateChanged,
	};
}
}
}
}
}

#endif