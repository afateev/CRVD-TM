////////////////////////////////////
// Управление статусным регистром
// MSP430
////////////////////////////////////

#ifndef STATUS_REGISTER_MSP430_H
#define STATUS_REGISTER_MSP430_H

namespace Rblib
{
namespace Msp430
{
	enum StatusReisterBits
	{
		StatusRegisterScg0 = 0x0040, //SCG0
		StatusRegisterScg1 = 0x0080, // SCG1
		StatusRegisterOscOff = 0x0020, // OSCOFF
		StatusRegisterCpuOff = 0x0010, // CPUOFF
	};
}
}

// IAR
#ifdef __IAR_SYSTEMS_ICC__

	#include <intrinsics.h>

	#define RBLIB_BIS_SR(srBits) __bis_SR_register(srBits)
	#define RBLIB_BIC_SR(srBits) __bic_SR_register(srBits)

	#define RBLIB_BIS_SR_ON_EXIT(srBits) __bis_SR_register_on_exit(srBits)
	#define RBLIB_BIC_SR_ON_EXIT(srBits) __bic_SR_register_on_exit(srBits)

	// Тип переменной для сохранения состояния прерываний
	#define RB_LIB_INTERRUPT_STATE_TYPE istate_t
	// Имя переменной для сохранения состояния прерываний
	#define RB_LIB_INTERRUPT_STATE_VAR_NAME _interruptState
	// Сохранить состояние прерываний и выключить
	#define RB_LIB_SAVE_INTERRUPTS_STATE_AND_DISABLE \
		RB_LIB_INTERRUPT_STATE_VAR_NAME = __get_interrupt_state();\
		__disable_interrupt();
	// Восттановить состояние прерываний
	#define RB_LIB_RESTORE_INTERRUPT_STATE \
		__set_interrupt_state(RB_LIB_INTERRUPT_STATE_VAR_NAME);

// другие компилляторы
#else

	#define RBLIB_BIS_SR(srBits)
	#define RBLIB_BIC_SR(srBits)

	#define RBLIB_BIS_SR_ON_EXIT(srBits)
	#define RBLIB_BIC_SR_ON_EXIT(srBits)

	// Тип переменной для сохранения состояния прерываний
	#define RB_LIB_INTERRUPT_STATE_TYPE unsigned char
	// Имя переменной для сохранения состояния прерываний
	#define RB_LIB_INTERRUPT_STATE_VAR_NAME _interruptState
	// Сохранить состояние прерываний и выключить
	#define RB_LIB_SAVE_INTERRUPTS_STATE_AND_DISABLE
	// Восттановить состояние прерываний
	#define RB_LIB_RESTORE_INTERRUPT_STATE
#endif

#endif // STATIS_REGISTER_MSP430_H