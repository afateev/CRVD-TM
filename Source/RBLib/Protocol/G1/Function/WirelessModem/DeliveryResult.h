///////////////////////////////////////////////////////////////////////////////
//	Коды результата доставки данных
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_WIRELESSMODEM_DELIVERTRESULT_H
#define PROTOCOL_G1_FUNCTION_WIRELESSMODEM_DELIVERTRESULT_H

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
	enum DeliveryResult
	{
		DeliveryResultUnknown				= 0,
		DeliveryResultDelivered				= 1,
		DeliveryResultImpossible			= 2,
		DeliveryResultError					= 3
	};
}
}
}
}
}

#endif