#ifndef EBI_EFM32GG990F1024_H
#define EBI_EFM32GG990F1024_H

#include "../efm32/ExternalBusInterface.h"

namespace Rblib
{
namespace Efm32gg990f1024
{
	class ExternalBusInterface :
		public Efm32::ExternalBusInterface::ExternalBusInterface<0x40008000, 0x000, 0x004, 0x008, 0x00C, 0x010, 0x014, 0x018, 0x01C,
																		   0x020, 0x024, 0x028, 0x02C, 0x030, 0x034, 0x038, 0x03C,
																		   0x040, 0x044, 0x048, 0x04C, 0x050, 0x054, 0x058, 0x05C,
																		   0x060, 0x064, 0x068, 0x06C, 0x070, 0x074, 0x078, 0x07C,
																		   0x080, 0x084, 0x088, 0x08C, 0x090, 0x094, 0x098, 0x09C,
																		   0x0A0, 0xA4>
	{
	public:
	};
}
}

#endif