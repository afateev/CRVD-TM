///////////////////////////////////////////////////////////////////////////////
//	Класс управления внешними прерываниями
//	ATmega164PA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef EXTERNALINTERRUPTS_ATMEGA164PA_H
#define EXTERNALINTERRUPTS_ATMEGA164PA_H

#include "../ATmega/ExternalInterrupts.h"
#include "../IdObjectBase.h"

namespace Lib
{
namespace ATmega164PA
{
	// Платформозависимый класс
	typedef ATmega::ExternalInterrupts::ExternalInterrupts<1, 0x69, 0x3D, 0x3C, 0x68, 0x3B, 0x73, 0x6D, 0x6C, 0x6B> ExternalInterrupts;
}
}

#endif