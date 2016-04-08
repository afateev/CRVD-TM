///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами ввода-вывода
//	EFM32GG
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_EFM32GG_H
#define GPIO_EFM32GG_H

#include "../efm32/Gpio.h"
#include "../IdObjectBase.h"

namespace Rblib
{
namespace Efm32gg
{
	// Платформозависимый набор портов
	class Gpio : public Efm32::Gpio::InterruptHandlerImpl<0x40006000, 0x100, 0x104, 0x108, 0x10C, 0x110, 0x114, 0x118, 0x11C>
	{
	public:
		typedef Efm32::Gpio::Port<1, 0x40006000, 0x000, 0x004, 0x008, 0x00C, 0x010, 0x014, 0x018, 0x01C, 0x020, 0x100, 0x104, 0x108, 0x10C, 0x110, 0x114, 0x118, 0x11C, 0> A;
		typedef Efm32::Gpio::Port<2, 0x40006000, 0x024, 0x028, 0x02C, 0x030, 0x034, 0x038, 0x03C, 0x040, 0x044, 0x100, 0x104, 0x108, 0x10C, 0x110, 0x114, 0x118, 0x11C, 1> B;
		typedef Efm32::Gpio::Port<3, 0x40006000, 0x048, 0x04C, 0x050, 0x054, 0x058, 0x05C, 0x060, 0x064, 0x068, 0x100, 0x104, 0x108, 0x10C, 0x110, 0x114, 0x118, 0x11C, 2> C;
		typedef Efm32::Gpio::Port<4, 0x40006000, 0x06C, 0x070, 0x074, 0x078, 0x07C, 0x080, 0x084, 0x088, 0x08C, 0x100, 0x104, 0x108, 0x10C, 0x110, 0x114, 0x118, 0x11C, 3> D;
		typedef Efm32::Gpio::Port<5, 0x40006000, 0x090, 0x094, 0x098, 0x09C, 0x0A0, 0x0A4, 0x0A8, 0x0AC, 0x0B0, 0x100, 0x104, 0x108, 0x10C, 0x110, 0x114, 0x118, 0x11C, 4> E;
		typedef Efm32::Gpio::Port<6, 0x40006000, 0x0B4, 0x0B8, 0x0BC, 0x0C0, 0x0C4, 0x0C8, 0x0CC, 0x0D0, 0x0D4, 0x100, 0x104, 0x108, 0x10C, 0x110, 0x114, 0x118, 0x11C, 5> F;
	public:
		static void InterruptHandler(bool odd)
		{
			for (unsigned char line = odd ? 1 : 0; line < 16; line += 2)
			{
				bool flag = ExternalInterruptFlag(line);
				if (flag)
				{
					ExternalInterruptFlagClear(line);
					Port p = ExternalInterruptGetSource(line);
					switch(p)
					{
					case PortA:
						{
							A::InterruptHandler(line);
						}
						break;
					case PortB:
						{
							B::InterruptHandler(line);
						}
						break;
					case PortC:
						{
							C::InterruptHandler(line);
						}
						break;
					case PortD:
						{
							D::InterruptHandler(line);
						}
						break;
					case PortE:
						{
							E::InterruptHandler(line);
						}
						break;
					case PortF:
						{
							F::InterruptHandler(line);
						}
						break;
					}
				}
			}
		}
	};
}
}

#endif