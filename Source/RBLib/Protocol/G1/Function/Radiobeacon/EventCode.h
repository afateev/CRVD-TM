///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Радиомаяк" устройтсва
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_RADIOBEACON_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_RADIOBEACON_EVENTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Radiobeacon
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeEventDataUpdated,
	};
}
}
}
}
}

#endif