///////////////////////////////////////////////////////////////////////////////
//	Коды типов сейсмической информации
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_SEISMICSENSOR_CHANNELINFOTYPECODE_H
#define PROTOCOL_G1_FUNCTION_SEISMICSENSOR_CHANNELINFOTYPECODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace SeismicSensor
{
	enum ChannelInfoTypeCode
	{
		ChannelInfoTypeCodeNoData				= 0,
		ChannelInfoTypeCodeSeismicSignalSource	= 1,
	};
}
}
}
}
}

#endif