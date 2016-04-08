///////////////////////////////////////////////////////////////////////////////
//	Коды результата доставки данных
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_RADIOBEACON_DELIVERTRESULT_H
#define PROTOCOL_G1_FUNCTION_RADIOBEACON_DELIVERTRESULT_H

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
	enum DeliveryResult
	{
		DeliveryResultUnknown				= 0,
		DeliveryResultTransmited			= 1,
		DeliveryResultImpossible			= 2,
		DeliveryResultError					= 3
	};
}
}
}
}
}

#endif