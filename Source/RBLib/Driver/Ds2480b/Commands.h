#ifndef DRIVER_DS2480B_COMMANDS_H
#define DRIVER_DS2480B_COMMANDS_H

#include "../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Driver
{
namespace Ds2480bImplementation
{
	static const unsigned char CommandSetDataMode = 0xE1;
	static const unsigned char CommandSetCommandMode = 0xE3;
	static const unsigned char CommandPulseTermination = 0xF1;
	
	struct CommandReset
	{
		unsigned char Bit0		: 1;
		unsigned char Bit1		: 1;
		unsigned char Speed		: 2;
		unsigned char Bit4		: 1;
		unsigned char Bit5		: 1;
		unsigned char Bit6		: 1;
		unsigned char Bit7		: 1;
	public:
		CommandReset()
		{
			Bit0 = 1;
			Bit1 = 0;
			Speed = 0;
			Bit4 = 0;
			Bit5 = 0;
			Bit6 = 1;
			Bit7 = 1;
		}
	};
	
	struct CommandResetResponse
	{
		unsigned char Presence	: 2;
		unsigned char Bit2		: 1;
		unsigned char Bit3		: 1;
		unsigned char Bit4		: 1;
		unsigned char Bit5		: 1;
		unsigned char Bit6		: 1;
		unsigned char Bit7		: 1;
	public:
		enum PresenceCode
		{
			PresenceCodeShorted,
			PresenceCodePresence,
			PresenceCodeAlarmPresence,
			PresenceCodeNoPresence
		};
	public:
		CommandResetResponse(unsigned char data)
		{
			unsigned char *ptr = (unsigned char *)this;
			*ptr = data;
		}
		
		bool Valid()
		{
			bool res = true;
			res &= Bit2 == 1;
			res &= Bit3 == 1;
			res &= Bit4 == 0;
			res &= Bit6 == 1;
			res &= Bit7 == 1;
			return res;
		}
	};
	
	struct CommandSearchAcceleratorControl
	{
		unsigned char Bit0		: 1;
		unsigned char Bit1		: 1;
		unsigned char Speed		: 2;
		unsigned char On		: 1;
		unsigned char Bit5		: 1;
		unsigned char Bit6		: 1;
		unsigned char Bit7		: 1;
	public:
		CommandSearchAcceleratorControl()
		{
			Bit0 = 1;
			Bit1 = 0;
			Speed = 0;
			On = 0;
			Bit5 = 1;
			Bit6 = 0;
			Bit7 = 1;
		}
	};
}
}
}

#endif