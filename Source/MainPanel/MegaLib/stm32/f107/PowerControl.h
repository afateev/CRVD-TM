////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		18/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef POWERCONTROL_STM32_F107_H
#define POWERCONTROL_STM32_F107_H

#include "PowerControlParts.h"

const unsigned int PowerControlBase = 0x40007000;

class PowerControl :
	public PowerControlRegister<PowerControlBase + 0x00>
{

};

#endif