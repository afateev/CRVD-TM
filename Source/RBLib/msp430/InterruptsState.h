///////////////////////////////////////////////////////////////////////////////
//	 ласс сохранени€ состо€ни€ прерываний
//	
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTSSTATE_H
#define INTERRUPTSSTATE_H

#ifdef __IAR_SYSTEMS_ICC__
#include <intrinsics.h>
#endif

#include "StatusRegister.h"

namespace Rblib
{
namespace Msp430
{

template<unsigned int PlatformId>
class InterrupsState
{
protected:
	static bool _interruptStateSaved;
	static volatile RB_LIB_INTERRUPT_STATE_TYPE RB_LIB_INTERRUPT_STATE_VAR_NAME;
public:
	// —охранить состо€ние прерываний и запретить их
	static inline void SaveInterruptsSateAndDisable()
	{
		if (_interruptStateSaved)
			return;
		RB_LIB_SAVE_INTERRUPTS_STATE_AND_DISABLE
			_interruptStateSaved = true;
	}

	// ¬осстановить сохраненное состо€ние прерываний
	static inline void RestoreInterruptsSate()
	{
		if (!_interruptStateSaved)
			return;
		RB_LIB_RESTORE_INTERRUPT_STATE
			_interruptStateSaved = false;
	}
};

template<unsigned int PlatformId>
bool InterrupsState<PlatformId>::_interruptStateSaved = false;

template<unsigned int PlatformId>
RB_LIB_INTERRUPT_STATE_TYPE InterrupsState<PlatformId>::RB_LIB_INTERRUPT_STATE_VAR_NAME = false;

}
}

#endif