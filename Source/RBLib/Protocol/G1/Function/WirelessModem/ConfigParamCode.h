///////////////////////////////////////////////////////////////////////////////
//	Коды параметров сетовой конфигурации
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_WIRELESSMODEM_CONFIGPARAMCODE_H
#define PROTOCOL_G1_FUNCTION_WIRELESSMODEM_CONFIGPARAMCODE_H

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
	enum ConfigParamCode
	{
		ConfigParamCodeInvalid								= 0,
		ConfigParamCodeNetId								= 1,
		ConfigParamCodeNetFrequencyChannel					= 2,
		ConfigParamCodeNetMainTimeSlotCount					= 3,
		ConfigParamCodeNetAdditionalTimeSlotCount			= 4,
		ConfigParamCodeNetSyncPeriod						= 5,
		ConfigParamCodeNetSyncTimeout						= 6,
		ConfigParamCodeNetActivityTableDistributionPeriod	= 7,
		ConfigParamCodeNodeId								= 8,
		ConfigParamCodeNodeSerialNumber						= 9,
		ConfigParamCodeNetAdditionalTimeSlotUsedCount		= 10,
		ConfigParamCodeNetAdditionalTimeSlotStart			= 11,
		ConfigParamCodeNetRelayMode							= 12,
		ConfigParamCodeNetPacketRepeatCount					= 13,
		ConfigParamCodeNetRadioListetingInterval			= 14,
	};
}
}
}
}
}

#endif