///////////////////////////////////////////////////////////////////////////////
//	Коды причин блокировки
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_STATE_LOCKREASONECODE_H
#define MODEM_STATE_LOCKREASONECODE_H

#include "../../Protocol/G1/Function/WirelessModem/StateCode.h"

namespace Rblib
{
namespace Modem
{
namespace State
{
	enum LockReasoneCode
	{
		LockReasoneUnknown				 = 0,
		LockReasoneTransiverInitError,
		LockReasoneSyncInitError,
		LockReasonePowerUpError,
		LockReasoneTransiverTestError,
		LockReasoneTransiverConfigError,
		LockReasoneNetConfigError,
		LockReasoneSyncStartError,
		LockReasoneSyncError,
		LockReasoneTransiverError,
		LockReasoneActivityTableError,
		LockReasoneRouteTableError,
		LockReasoneTransportError,
	};
}
}
}

#endif