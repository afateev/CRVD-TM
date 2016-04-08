#ifndef NVIC_EFM32GG990F1024_H
#define NVIC_EFM32GG990F1024_H

#include "../efm32/Nvic.h"

namespace Rblib
{
namespace Efm32gg990f1024
{
	class Nvic :
		public Efm32::Nvic::Nvic<0xE000E000, 0x100, 0x104, 0x180, 0x184, 0x400, 0xF00>
	{
	public:
	};
}
}

#endif