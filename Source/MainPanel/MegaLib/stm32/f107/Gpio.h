////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		26/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_STM32_F107_H
#define GPIO_STM32_F107_H

#include "GpioParts.h"

template<unsigned int PortBaseAddr, unsigned int ConfigRegisterLow, unsigned int ConfigRegisterHi, unsigned int InuptRegister, unsigned int OutputRegister, unsigned int BitSetResetRegister>
class GpioPort:
	public GpioConfigRegister<PortBaseAddr + ConfigRegisterLow, PortBaseAddr + ConfigRegisterHi>,
	public GpioInputDataRegister<unsigned int, unsigned int, PortBaseAddr + InuptRegister>,
	public GpioOutputDataRegister<unsigned int, unsigned int, PortBaseAddr + OutputRegister>,
	public GpioBitSetResetRegister<PortBaseAddr + BitSetResetRegister>
{

};

class Gpio
{
protected:
	static const unsigned int PortABase = 0x40010800;
	static const unsigned int PortBBase = 0x40010C00;
	static const unsigned int PortCBase = 0x40011000;
	static const unsigned int PortDBase = 0x40011400;
	static const unsigned int PortEBase = 0x40011800;
	static const unsigned int PortFBase = 0x40011C00;
	static const unsigned int PortGBase = 0x40012000;
	
public:
	typedef GpioPort<PortABase, 0x00, 0x04, 0x08, 0x0C, 0x10> _A;
	typedef GpioPort<PortBBase, 0x00, 0x04,  0x08, 0x0C, 0x10> _B;
	typedef GpioPort<PortCBase, 0x00, 0x04,  0x08, 0x0C, 0x10> _C;
	typedef GpioPort<PortDBase, 0x00, 0x04,  0x08, 0x0C, 0x10> _D;
	typedef GpioPort<PortEBase, 0x00, 0x04,  0x08, 0x0C, 0x10> _E;
	typedef GpioPort<PortFBase, 0x00, 0x04,  0x08, 0x0C, 0x10> _F;
	typedef GpioPort<PortGBase, 0x00, 0x04,  0x08, 0x0C, 0x10> _G;
};

#endif