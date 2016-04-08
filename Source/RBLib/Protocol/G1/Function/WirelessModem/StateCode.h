///////////////////////////////////////////////////////////////////////////////
//	Коды состояний функции "беспроводный модем"
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_WIRELESSMODEM_STATECODE_H
#define PROTOCOL_G1_FUNCTION_WIRELESSMODEM_STATECODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace WirelessModem
{
	enum StateCode
	{
		StateCodeDisabled				= 0,
		StateCodeLocked					= 1,
		StateCodeInitializing			= 2,
		StateCodeSearchigSyncSources	= 3,
		StateCodeSynchronizing			= 4,
		StateCodeSync					= 5,
		StateCodePrepareLinkTable		= 6,
		StateCodePrepareRouteTable		= 7,
		StateCodeReady					= 8,
		StateCodeInfiniteListening		= 9,
		StateCodeCarrierTransmit		= 10,
	};
}
}
}
}
}

#endif