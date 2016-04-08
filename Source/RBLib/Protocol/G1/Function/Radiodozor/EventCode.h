///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Радиодозор" устройтсва
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_RADIODOZOR_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_RADIODOZOR_EVENTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Radiodozor
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeModeChanged,
		EventCodeParamsChanged
	};
}
}
}
}
}

#endif