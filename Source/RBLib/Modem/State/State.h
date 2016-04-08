///////////////////////////////////////////////////////////////////////////////
//	Состояния модема
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_STATE_STATE_H
#define MODEM_STATE_STATE_H

#include "../../Protocol/G1/Function/WirelessModem/StateCode.h"

namespace Rblib
{
namespace Modem
{
namespace State
{
	// коды должны совпадать с кодами, утвержденными в протокле
	enum State
	{
		StateDisabled 				= Rblib::Protocol::G1::Function::WirelessModem::StateCodeDisabled,
		StateLocked 				= Rblib::Protocol::G1::Function::WirelessModem::StateCodeLocked,
		StateInitializing 			= Rblib::Protocol::G1::Function::WirelessModem::StateCodeInitializing,
		StateSearchigSyncSources 	= Rblib::Protocol::G1::Function::WirelessModem::StateCodeSearchigSyncSources,
		StateSynchronizing			= Rblib::Protocol::G1::Function::WirelessModem::StateCodeSynchronizing,
		StateSync 					= Rblib::Protocol::G1::Function::WirelessModem::StateCodeSync,
		StatePrepareLinkTable 		= Rblib::Protocol::G1::Function::WirelessModem::StateCodePrepareLinkTable,
		StatePrepareRouteTable 		= Rblib::Protocol::G1::Function::WirelessModem::StateCodePrepareRouteTable,
		StateReady 					= Rblib::Protocol::G1::Function::WirelessModem::StateCodeReady,
		StateInfiniteListening	 	= Rblib::Protocol::G1::Function::WirelessModem::StateCodeInfiniteListening,
		StateCarrierTransmit	 	= Rblib::Protocol::G1::Function::WirelessModem::StateCodeCarrierTransmit,
	};
}
}
}

#endif