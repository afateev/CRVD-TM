///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Служба подписки" устройтсва
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_EVENTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace SubscriptionService
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeSubscriptionAdded,
		EventCodeSubscriptionRemoved,
		EventCodeSubscriberAdded,
		EventCodeSubscriberRemoved,
		EventCodeSubscriptionsListChanged,
		EventCodeSubscribersListChanged,
		EventCodeKeepAlive
	};
}
}
}
}
}

#endif