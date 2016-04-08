///////////////////////////////////////////////////////////////////////////////
//	Класс управления аппаратными таймерами
//	MSP430F5XXX
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_MSP430F543X_H
#define TIMER_MSP430F543X_H

#include "../../../IdObjectBase.h"
#include "../../TimerParts.h"
#include "../TimerParts.h"

namespace Rblib
{
namespace Msp430f543x
{
	// Тип значения регистров таймера
	typedef unsigned short TimerValueDataType;
	// Тип адреса регистра таймера
	typedef unsigned int  TimerRegisterAddresType;

	/// Шаблон пары регистров управления и значения для захвата/сравнения.
	/// \tparam R1 Адрес CCTL регистра (capture/compare control register).
	/// \tparam R2 Адрес CCR регистра (capture/compare register).
	template<TimerRegisterAddresType R1,
		TimerRegisterAddresType R2
	>
	class TimerCaptureCompare:
		public Msp430::TimerCaptureCompareControl<TimerValueDataType, TimerRegisterAddresType, R1>,
		public Msp430::TimerCaptureCompareRegister<TimerValueDataType, TimerRegisterAddresType, R2>
	{
	
	};

	/// Базовый шаблон таймера.
	/// \tparam IdObj Идентификатор.
	/// \tparam R2 Адрес регистра счётчика (counter register).
	/// \tparam R3 Адрес регистра флагов прерываний (interrupt vector register).
	/// \tparam R4 Адрес регистра расширения (expansion register).
	/// \tparam BaseCaptureCompareRegister Класс управления базовым модуля захвата/сравнения.
	/// \tparam TimerControlImpl Класс управления основным управляющим регистром.
	template<unsigned int IdObj,
		TimerRegisterAddresType R2,
		TimerRegisterAddresType R3,
		TimerRegisterAddresType R4,
		class BaseCaptureCompareRegister,
		class TimerControlImpl
	>
	class TimerBase:
		public IdObjectBase<IdObj>,
		public TimerControlImpl,
		public Msp430::TimerCounter<TimerValueDataType, TimerRegisterAddresType, R2>,
		public Msp430::TimerInterruptVector<TimerValueDataType, TimerRegisterAddresType, R3>,
		public Msp430::TimerExpansionRegister<TimerValueDataType, TimerRegisterAddresType, R4>
	{
	public:

		typedef	TimerValueDataType RegisterValueType;

	public:
		
		/// Установить максимальное значение счётчика.
		static inline void SetMaxCount(TimerValueDataType value)
		{
			BaseCaptureCompareRegister::SetValue(value);
		}
	};

	// Таймеры отличаются друг от друга только количеством регистров захвата/сравнения

	/// Базовый регистр захвата сравениния для таймера A.
	typedef TimerCaptureCompare<0x0342, 0x0352> TimerABaseCaptureCompareReg;

	/// Класс таймера A.
	class TimerA: public TimerBase<1, 0x0350, 0x036E, 0x0360, TimerABaseCaptureCompareReg
									, Msp430::TimerControl<TimerRegisterAddresType, 0x0340> >
	{
	public:
		// Регистры захвата/сравнения
		/// Основонй блок захвата/сравнения (№0).
		typedef TimerABaseCaptureCompareReg			CC0;
		/// Блок захвата/сравнения №1.
		typedef TimerCaptureCompare<0x0344, 0x0354> CC1;
		/// Блок захвата/сравнения №2.
		typedef TimerCaptureCompare<0x0346, 0x0356> CC2;
		/// Блок захвата/сравнения №3.
		typedef TimerCaptureCompare<0x0348, 0x0358> CC3;
		/// Блок захвата/сравнения №4.
		typedef TimerCaptureCompare<0x034A, 0x035A> CC4;
	};

	/// Базовый регистр захвата сравениния для таймера B.
	typedef TimerCaptureCompare<0x0382, 0x0392> TimerBBaseCaptureCompareReg;

	/// Класс таймера B.
	class TimerB: public TimerBase<2, 0x0390, 0x03AE, 0x03A0, TimerBBaseCaptureCompareReg
				, Msp430::TimerBControl<TimerRegisterAddresType, 0x0380> >
	{
	public:
		// Регистры захвата/сравнения
		/// Основонй блок захвата/сравнения (№0).
		typedef TimerBBaseCaptureCompareReg			CC0;
		/// Блок захвата/сравнения №1.
		typedef TimerCaptureCompare<0x0384, 0x0394> CC1;
		/// Блок захвата/сравнения №2.
		typedef TimerCaptureCompare<0x0386, 0x0396> CC2;
	};

	/// Базовый регистр захвата сравениния для таймера C.
	typedef TimerCaptureCompare<0x03C2, 0x03D2> TimerCBaseCaptureCompareReg;

	// Класс таймера C
	class TimerC: public TimerBase<3, 0x03D0, 0x03EE, 0x03E0, TimerCBaseCaptureCompareReg
				, Msp430::TimerBControl<TimerRegisterAddresType, 0x03C0> >
	{
	public:
		// Регистры захвата/сравнения
		/// Основонй блок захвата/сравнения (№0).
		typedef TimerCBaseCaptureCompareReg			CC0;
		/// Блок захвата/сравнения №1.
		typedef TimerCaptureCompare<0x03C4, 0x03D4> CC1;
		/// Блок захвата/сравнения №2.
		typedef TimerCaptureCompare<0x03C6, 0x03D6> CC2;
		/// Блок захвата/сравнения №3.
		typedef TimerCaptureCompare<0x03C8, 0x03D8> CC3;
		/// Блок захвата/сравнения №4.
		typedef TimerCaptureCompare<0x03CA, 0x03DA> CC4;
		/// Блок захвата/сравнения №5.
		typedef TimerCaptureCompare<0x03CC, 0x03DC> CC5;
		/// Блок захвата/сравнения №6.
		typedef TimerCaptureCompare<0x03CE, 0x03DE> CC6;
	};

	/// Платформозависимый набор таймеров.
	class Timer
	{
	public:

		typedef	TimerValueDataType RegisterValueType;

	public:
		/// Таймер TA0.
		typedef TimerA _A; // ТА0
		/// Таймер TA1.
		typedef TimerB _B; // ТА1
		/// Таймер TB0.
		typedef TimerC _C; // ТB0
	};
}
}

#endif
