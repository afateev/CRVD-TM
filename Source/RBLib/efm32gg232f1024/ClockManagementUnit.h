#ifndef CMU_EFM32GG232F1024_H
#define CMU_EFM32GG232F1024_H

#include "../efm32/ClockManagementUnit.h"

namespace Rblib
{
namespace Efm32gg232f1024
{
	class ClockManagementUnit :
		public Efm32::ClockManagementUnit::ClockManagementUnit<0x400C8000, 0x000, 0x004, 0x008, 0x00C, 0x010, 0x014, 0x018, 0x01C,
																		   0x020, 0x024, 0x028, 0x02C, 0x030, 0x034, 0x038, 0x03C,
																		   0x040, 0x044, 			   0x050, 0x054, 0x058,
																		   0x060,        0x068, 	   0x070, 		 0x078, 0x07C,
																		   0x080, 0x84>
	{
	public:
	};
}
}

#endif