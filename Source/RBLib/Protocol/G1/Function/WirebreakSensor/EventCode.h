///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Обрывной датчик" устройтсва
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_EVENTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace WirebreakSensor
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeWireStateChanged,
	};
}
}
}
}
}

#endif