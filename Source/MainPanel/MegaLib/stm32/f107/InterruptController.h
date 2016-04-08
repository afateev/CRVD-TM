////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		06/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTCONTROLLER_H
#define INTERRUPTCONTROLLER_H

#include "..\..\RegisterReference.h"

enum InterruptId
{
	InterruptIdUsart2		= 38,
	InterruptIdUsart3		= 39,
	InterruptIdTimer6		= 54,
	InterruptIdTimer7		= 55,
	InterruptIdUsb			= 67
};

class InterruptController
{
	
protected:
	typedef RegisterReference<unsigned int, 0xE000E100 + 0x00> InterruptEnable0;
	typedef RegisterReference<unsigned int, 0xE000E100 + 0x04> InterruptEnable1;
	typedef RegisterReference<unsigned int, 0xE000E100 + 0x08> InterruptEnable2;
public:
	static void InterruptEnable(unsigned short id)
	{
		if (id < 32)
		{
			*InterruptEnable0::_register |= 1 << id;
			return;
		}
		if (id < 64)
		{
			*InterruptEnable1::_register |= 1 << (id - 32);
			return;
		}
		if (id < 96)
		{
			*InterruptEnable2::_register |= 1 << (id - 64);
			return;
		}
	}
	
	static void InterruptEnableAll()
	{
		*InterruptEnable0::_register = 0xFFFFFFFF;
		*InterruptEnable1::_register = 0xFFFFFFFF;
		*InterruptEnable2::_register = 0xFFFFFFFF;
	}
};

#endif