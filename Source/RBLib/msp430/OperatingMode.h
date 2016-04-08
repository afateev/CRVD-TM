/// \file
/// \brief Управление режимами сна процессоров семества MSP430.

#ifndef OPERATING_MODE_MSP430_H
#define OPERATING_MODE_MSP430_H

#include "./StatusRegister.h"

namespace Rblib
{
namespace Msp430
{

	/// Методы для управления режимами сна процессоров семейства MSP430.
	class OperatingMode
	{
	public:

		/// Флаги SR регистра отвечающие за сон.
		enum Flags
		{
			/// SCG0.
			FlagScg0 = 0x0040,
			/// SCG1.
			FlagScg1 = 0x0080,
			/// OSCOFF.
			FlagOscOff = 0x0020, 
			/// CPUOFF.
			FlagCpuOff = 0x0010,
			/// SCG0 | SCG1 | OSCOFF | CPUOFF
			FlagAll = 0x00F0
		};

		/// Режимы сна. Комбинации различных флагов.
		enum Modes
		{
			/// Полностью активный режим.
			Active = 0,
			/// LPM0. CPU Off, MCLK Off, SMCLK On, DCO On, ACLK On.
			SleepLevel0 = FlagCpuOff,
			/// LPM1. CPU Off, MCLK Off, SMCLK On, DCO Off, ACLK On.
			SleepLevel1 = FlagCpuOff | FlagScg0,
			/// LPM2. CPU Off, MCLK Off, SMCLK Off, DCO Off, ACLK On.
			SleepLevel2 = FlagCpuOff | FlagScg1,
			/// LPM3. CPU Off, MCLK Off, SMCLK Off, DCO Off, ACLK On.
			SleepLevel3 = FlagCpuOff | FlagScg1 | FlagScg0,
			/// LPM4. Ядро и все источники тактового сигнала остановлены.
			SleepLevel4 = FlagCpuOff | FlagScg1 | FlagScg0 | FlagOscOff,
		};

	protected:

		enum Constants
		{
			_dependentFlagsMask = FlagScg0 | FlagScg1
		};

	public:

		/// Установить флаги в регистра SR.
		/// \param flags Набор флагов. Биты, соответствующие флагам, которые надо установить, должны быть установлены в 1.
		static inline void SetFlags(unsigned short flags)
		{
			RBLIB_BIS_SR(flags);
		}

		/// Сбросить флаги в регистра SR.
		/// \param flags Набор флагов. Биты, соответствующие флагам, которые надо сбросить, должны быть установлены в 1.
		static inline void ClearFlags(unsigned short flags)
		{
			RBLIB_BIC_SR(flags);
		}

		/// Включить ядро процессора.
		/// \details Сбрасывает флаг CPUOFF.
		static inline void EnableCpu()
		{
			RBLIB_BIC_SR(FlagCpuOff);
		}

		/// Выключить ядро процессора.
		/// \details Устанавлива флаг CPUOFF.
		static inline void DisableCpu()
		{
			RBLIB_BIS_SR(FlagCpuOff);
		}

		/// Установить режим сна.
		/// \details Выставляет в регистре SR набор флагов, соответствующий заданному режиму сна.
		/// \param mode Режим сна.
		static inline void Sleep(unsigned short mode = SleepLevel3)
		{
			if ((mode & _dependentFlagsMask) == _dependentFlagsMask)
				RBLIB_BIS_SR(mode);
			else
			{
				RBLIB_BIS_SR(_dependentFlagsMask);
				RBLIB_BIC_SR((~mode) & _dependentFlagsMask);
				RBLIB_BIS_SR(mode);
			}
		}

		// 
		// Эти методы можно использовать, если компиллятором 
		// разрешено встраивание функций (function inlining).
		// В противном случает надо использовать макросы (см. ниже).
		// {{
		//
		static inline void SetFlagsOnExit(unsigned short flags)
		{
			RBLIB_BIS_SR_ON_EXIT(flags);
		}

		static inline void ClearFlagsOnExit(unsigned short flags)
		{
			RBLIB_BIC_SR_ON_EXIT(flags);
		}

		static inline void WakeUp(unsigned short mode = Active)
		{
			RBLIB_BIC_SR_ON_EXIT((~mode) & FlagAll);
			RBLIB_BIS_SR_ON_EXIT(mode);
		}
		//
		// }}
		//

	protected:

		static inline void SetSleepMode(unsigned short mode)
		{
			if ((mode & _dependentFlagsMask) == _dependentFlagsMask)
				RBLIB_BIS_SR(mode);
			else
			{
				RBLIB_BIS_SR(_dependentFlagsMask);
				RBLIB_BIC_SR((~mode) & _dependentFlagsMask);
				RBLIB_BIS_SR(mode);
			}
		}

	};

}
}

//
// Макросы изменения режима энергопотребления при выходе из прерывания
//

/// Установить флаги в SR регистре при выходе из прерывания.
#define RBLIB_OPERATING_MODE_MSP430_SET_FLAGS_ON_EXIT(flags) RBLIB_BIS_SR_ON_EXIT(flags)
/// Сбросить флаги в SR регистре при выходе из прерывания.
#define RBLIB_OPERATING_MODE_MSP430_CLR_FLAGS_ON_EXIT(flags) RBLIB_BIC_SR_ON_EXIT(flags)
/// Проснуться (переключиться) в другой режим сна при выходе из прерывания.
#define RBLIB_OPERATING_MODE_MSP430_WAKE_UP(mode) RBLIB_BIC_SR_ON_EXIT((~mode) & Rblib::Msp430::OperatingMode::FlagAll); \
													RBLIB_BIS_SR_ON_EXIT(mode)

#endif // OPERATING_MODE_MSP430_H
