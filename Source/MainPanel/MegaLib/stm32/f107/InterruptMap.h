////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		03/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTMAP_STM32_F107_H
#define INTERRUPTMAP_STM32_F107_H

#include "Usart.h"
#include "InterruptController.h"

extern "C"
{

void ISR_BusFault(void)
{
	while (1)
	{}
}

void ISR_Usart2(void)
{
	UsartHardware::_B::InterruptHandler();
}

void ISR_Usart3(void)
{
	UsartHardware::_C::InterruptHandler();
}

void ISR_Timer6(void)
{
	Timer::_G::InterruptHandler();
}

void ISR_Timer7(void)
{
	Timer::_H::InterruptHandler();
}

void ISR_USB(void)
{
	UsbHardware::_A::InterruptHandler();
}

}

#endif