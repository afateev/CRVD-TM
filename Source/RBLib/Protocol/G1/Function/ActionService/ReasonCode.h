///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий причин вызова DoAtcion
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_ACTIONSERVICE_REASONCODE_H
#define PROTOCOL_G1_FUNCTION_ACTIONSERVICE_REASONCODE_H

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
	enum ReasonCode
	{
		ReasonCodeUnknown = 0,
		ReasonCodeAlarm,
		ReasonCodeFrequentAlarm,
		ReasonCodeAnomaly
	};
}
}
}
}
}

#endif