///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Пеленг" устройтсва
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_PELENG_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_PELENG_EVENTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Peleng
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodePelengEnabled,
		EventCodePelengDisabled
	};
}
}
}
}
}

#endif