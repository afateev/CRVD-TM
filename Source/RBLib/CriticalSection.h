#ifndef CRITICALSECTION_H
#define CRITICALSECTION_H

// IAR
#ifdef __IAR_SYSTEMS_ICC__

#ifdef __ICC430__
#include <intrinsics.h>
namespace Rblib
{
    class CriticalSection
    {
        istate_t mState;
    public:
	    inline void Enter()
		{
			mState = __get_interrupt_state();// Get hold of current interrupt state.        
			__disable_interrupt();// Disable all interrupts.
		}
	    inline void Leave()
		{
			__set_interrupt_state(mState);// Restore the interrupt state.
		}
    };
}
#endif

#ifdef __ICCARM__
#include "intrinsics.h"
namespace Rblib
{
    class CriticalSection
    {
        __istate_t mState;
    public:
	    inline void Enter()
		{
			mState = __get_interrupt_state();// Get hold of current interrupt state.        
			__disable_interrupt();// Disable all interrupts.
		}
	    inline void Leave()
		{
			__set_interrupt_state(mState);// Restore the interrupt state.
		}
    };
}
#endif

#ifdef AVR
#include "intrinsics.h"
namespace Rblib
{
    class CriticalSection
    {
    public:
	    inline void Enter()
		{
		}
	    inline void Leave()
		{
		}
    };
}
#endif

#else
namespace Rblib
{
    class CriticalSection
    {
    public:
	    inline void Enter()
		{
		}
	    inline void Leave()
		{
		}
    };
}
#endif

namespace Rblib
{
	class CriticalBlock : private CriticalSection
	{
	public:
		CriticalBlock()
		{
			CriticalSection::Enter();
		}
	
		~CriticalBlock()
		{
			CriticalSection::Leave();
		}
	};
}
#define CRITICAL_BLOCK Rblib::CriticalBlock __criticalBlockObject__;

#endif //CRITICALSECTION_H

