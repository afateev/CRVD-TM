///////////////////////////////////////////////////////////////////////////////
//	Класс управления флэш памятью
//	MSP430F5342
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYCONTROLLER_MSP430F5342_H
#define FLASHMEMORYCONTROLLER_MSP430F5342_H

#include "..\msp430\FlashMemoryControllerParts.h"
#include "..\msp430\f5xxx\FlashMemoryControllerParts.h"

namespace Rblib
{
namespace Msp430f5342
{
	// Платформозависимый контроллер флэш памяти
	typedef Msp430::FlashMemoryController<Msp430::FlashPointerType, Msp430::FlashRegisterDataType, Msp430::FlashRegisterAddresType,
		0x0140, Msp430f5xxx::FlashMemoryControl1Register<Msp430::FlashRegisterDataType>,
		0x0144, Msp430f5xxx::FlashMemoryControl3Register<Msp430::FlashRegisterDataType>,
		Msp430::FlashMemorySegmentBounds<Msp430::FlashPointerType, 0x1800ul, 512ul,			 128ul>,	// InfoMemory
		Msp430::FlashMemorySegmentBounds<Msp430::FlashPointerType, 0x4400ul, 64ul * 1024ul, 512ul>		// MainMemory
	> FlashMemoryController;
}
}

#endif