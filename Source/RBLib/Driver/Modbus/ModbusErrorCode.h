#ifndef RBLIB_MODBUSERRORCODE_H
#define RBLIB_MODBUSERRORCODE_H

namespace Rblib
{
namespace Driver
{
namespace Modbus
{
	enum ErrorCode
	{
		ErrorCodeNoError				= 0,
		ErrorCodeInvalidCrc				= -1,
		ErrorCodeIvalidInputFrame		= -2,
	};
}
}
}

#endif