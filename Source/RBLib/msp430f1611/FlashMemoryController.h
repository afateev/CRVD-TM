///////////////////////////////////////////////////////////////////////////////
//	Класс управления флэш памятью
//	MSP430F1611
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYCONTROLLER_MSP430F1611_H
#define FLASHMEMORYCONTROLLER_MSP430F1611_H

#include <string.h>

#include "..\msp430\FlashMemoryControllerParts.h"
#include "..\msp430\x1xxx\FlashMemoryControllerParts.h"

namespace Rblib
{
namespace Msp430f1611
{
	// Платформозависимый контроллер флэш памяти
  typedef Msp430::FlashMemoryController<Msp430::FlashPointerType, Msp430::FlashRegisterDataType, Msp430::FlashRegisterAddresType,
  0x0128, Msp430x1xxx::FlashMemoryControl1Register<Msp430::FlashRegisterDataType>,
    0x012C, Msp430x1xxx::FlashMemoryControl3Register<Msp430::FlashRegisterDataType>,
    Msp430::FlashMemorySegmentBounds<Msp430::FlashPointerType, 0x1000ul, 256ul, 64ul>,		// InfoMemory
    Msp430::FlashMemorySegmentBounds<Msp430::FlashPointerType, 0x4000ul, 48640ul, 512ul>		// MainMemory
  > FlashMemoryController;

}
}

#endif
