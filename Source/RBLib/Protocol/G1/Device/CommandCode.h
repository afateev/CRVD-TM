///////////////////////////////////////////////////////////////////////////////
//	Формат пакета общий для всех устройств
//	Коды команд
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_DEVICE_COMMANDCODE_H
#define PROTOCOL_G1_DEVICE_COMMANDCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Device
{
	enum CommandCode
	{
		CommandGetDeviceInfo		= 0,
		CommandDeviceInfo			= 1,
		CommandGetFunctionInfo		= 2,
		CommandFunctionInfo			= 3,
		CommandToFunction			= 4,
		CommandFromFunction			= 5,
		CommandGetSoftwareVersion	= 6,
		CommandSoftwareVersion		= 7,
	};
}
}
}
}

#endif