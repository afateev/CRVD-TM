///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Служба действий" устройства
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_ACTIONSERVICE_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_ACTIONSERVICE_EVENTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace ActionService
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeSlaveListChanged
	};
}
}
}
}
}

#endif