///////////////////////////////////////////////////////////////////////////////
//	Класс управления флэш памятью
//	MSP430F2618
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYCONTROLLER_MSP430F2618_H
#define FLASHMEMORYCONTROLLER_MSP430F2618_H

#include "..\msp430\FlashMemoryControllerParts.h"
#include "..\msp430\f2xxx\FlashMemoryControllerParts.h"

namespace Rblib
{
namespace Msp430f2618
{
	// Платформозависимый контроллер флэш памяти
	typedef Msp430::FlashMemoryController<Msp430::FlashPointerType, Msp430::FlashRegisterDataType, Msp430::FlashRegisterAddresType,
		0x0128, Msp430f2xxx::FlashMemoryControl1Register<Msp430::FlashRegisterDataType>,
		0x012C, Msp430f2xxx::FlashMemoryControl3Register<Msp430::FlashRegisterDataType>,
		Msp430::FlashMemorySegmentBounds<Msp430::FlashPointerType, 0x1000ul, 256ul,			 64ul>,		// InfoMemory
		Msp430::FlashMemorySegmentBounds<Msp430::FlashPointerType, 0x3100ul, 118272ul, 512ul>		// MainMemory
	> FlashMemoryController;
}
}

#endif