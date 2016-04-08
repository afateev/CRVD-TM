#ifndef RBLIB_MODBUSCOMMON_H
#define RBLIB_MODBUSCOMMON_H

namespace Rblib
{
namespace Driver
{
namespace Modbus
{
	unsigned short WordFromArray(unsigned char *byteArray)
	{
		return byteArray[0] << 8 | (byteArray[1]);
	}
	
	void WordToArray(unsigned short value, unsigned char *byteArray)
	{
		byteArray[0] = (value >> 8) & 0xFF;
		byteArray[1] = value & 0xFF;
	}
}
}
}

#endif