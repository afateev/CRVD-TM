///////////////////////////////////////////////////////////////////////////////
//	Классы управления разными частями аппаратных таймеров
//
//	Разные наборы частей разделены по разным пространствам имён по платформам
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_PARTS_H
#define TIMER_PARTS_H

namespace Rblib
{
// Части таймеров специфичные для платформы MSP430
namespace Msp430
{
	/// Структура регистра управления таймером (TxxCTL).
	struct TimerControlRegister
	{
		unsigned short InterruptFlag	: 1;	///< Флаг прерывания
		unsigned short InterruptEnable	: 1;	///< Разрешение прерывания
		unsigned short ClearTimer		: 1;	///< Влаг очистки таймера
		unsigned short					: 1;
		unsigned short Mode				: 2;	///< Режим работы
		unsigned short InputDivider		: 2;	///< Делитель тактового сигнала
		unsigned short ClockSource		: 2;	///< Источник тактирования
		unsigned short 					: 6;
	};

	/// Структура регистра управления таймером TimerB.
	struct TimerBControlRegister
	{
		unsigned short InterruptFlag	: 1;	///< Флаг прерывания
		unsigned short InterruptEnable	: 1;	///< Разрешение прерывания
		unsigned short ClearTimer		: 1;	///< Влаг очистки таймера
		unsigned short					: 1;
		unsigned short Mode				: 2;	///< Режим работы
		unsigned short InputDivider		: 2;	///< Делитель тактового сигнала
		unsigned short ClockSource		: 2;	///< Источник тактирования
		unsigned short 					: 1;
		unsigned short CounterLength	: 2;	///< Разрядность счётчика таймера
		unsigned short GroupingCompare	: 2;	///< Управление группировкой блоков сравнения
		unsigned short 					: 1;
	};

	/// Структура регистра настройки захвата/сравнеия.
	struct TimerCaptureCompareControlRegister
	{
		unsigned short InterruptFlag			: 1;	///< Флаг прерывания
		unsigned short CaptureOverflowFlag		: 1;	///< Переполнения счётчика захвата
		unsigned short Output					: 1;	///< Состояние выхода в режиме OutputMode == 0
		unsigned short Input					: 1;	///< Состояние входа (только для чтения)
		unsigned short InterruptEnable			: 1;	///< Разрешение прерывания
		unsigned short OutputMode				: 3;	///< Режим вывода
		unsigned short EnableCapture			: 1;	///< Влключить захват, инача сравнение
		unsigned short							: 1;
		unsigned short SynchronizedInput		: 1;	///< Вход синхронизирован (только для чтения)
		unsigned short SynchronizeCaptureSource : 1;	///< Синхронизировать вход с таймером
		unsigned short CaptureInputSelect		: 2;	///< Выбор входа для захвата
		unsigned short CaptureMode				: 2;	///< Режим захвата
	};
	
	/// Управление регистром управления.
	/// \param PointerType Тип (размер) значения адресов регистров.
	/// \param ControlRegisterAddr Адрес регистра управления.
	/// \param TimerControlRegisterStructure Структура регистра управления.
	template <class PointerType,
			PointerType ControlRegisterAddr,
			class TimerControlRegisterStructure = TimerControlRegister
		>
	class TimerControl
	{
	protected:
		
	public:
		/// Режимы работы таймера.
		enum TimerMode
		{
			TimerModeStop		= 0,	///< таймер остановлен
			TimerModeUp			= 1,	///< считать до заданного максимума, затем сброс
			TimerModeContinuous = 2,	///< считать до 0xFFFF, затем сброс
			TimerModeUpDown		= 3		///< считать до заданного максимума, затем обратно до нуля
		};
		// Варианты источников тактирования
		enum TimerClockSource
		{
			TimerClockSourceExternalClock			= 0,	///< Тактирование от внешней "ноги" таймера
			TimerClockSourceAuxClock				= 1,	///< От системного генератора
			TimerClockSourceSubsystemMasterClock	= 2,	///< От системного генератора
			TimerClockSourceInvertedExternalClock	= 3		///< Инвертированное тактирование от "ноги" таймера
		};
		/// Варианты делителей.
		enum TimerDivider
		{
			TimerDiv1 = 0,	///< /1
			TimerDiv2 = 1,	///< /2
			TimerDiv4 = 2,	///< /4
			TimerDiv8 = 3	///< /8
		};
	protected:
		/// Адрес регистра управления.
		static volatile TimerControlRegisterStructure * const _controlRegisterAddr;
	public:
		/// Чтение значения регистра управления.
		static inline unsigned short GetControl()
		{
			return *(unsigned short *)_controlRegisterAddr;
		}
		/// Запись значения регистра управления.
		static inline void SetControl(unsigned short value)
		{
			*(unsigned short *)_controlRegisterAddr = value;
		}
		/// Установить источник тактирования.
		static inline void SetClockSource(TimerClockSource source)
		{
			_controlRegisterAddr->ClockSource = source;
		}
		/// Установить режим работы таймера.
		static inline void SetMode(TimerMode mode)
		{
			_controlRegisterAddr->Mode = mode;
		}
		/// Разрешить прерывания.
		static inline void InterruptEnable()
		{
			_controlRegisterAddr->InterruptEnable = 1;
		}
		/// Запретить прерывания.
		static inline void InterruptDisable()
		{
			_controlRegisterAddr->InterruptEnable = 0;
		}
		/// Сброс таймера.
		static inline void Reset()
		{
			_controlRegisterAddr->ClearTimer = 1;
		}
		/// Установить делитель тактового сигнала.
		static inline void SetDivider(TimerDivider value)
		{
			_controlRegisterAddr->InputDivider = value;
		}
	};

	/// Управление регистром управления TimerB.
	/// \tparam PointerType Тип (размер) значения адресов регистров.
	/// \tparam ControlRegisterAddr Адрес регистра управления.
	/// \tparam TimerControlRegisterStructure Структура регистра управления.
	template <class PointerType,
			PointerType ControlRegisterAddr,
			class TimerControlRegisterStructure = TimerBControlRegister
		>
	class TimerBControl : public TimerControl<PointerType, ControlRegisterAddr, TimerControlRegisterStructure>
	{
	private:
	  
	  typedef TimerControl<PointerType, ControlRegisterAddr, TimerControlRegisterStructure> _TimerControl;
	  
	public:

		/// Рарядность счётчика.
		enum CounterLength
		{
			/// 16 бит.
			CounterLength16Bit = 0,
			/// 12 бит.
			CounterLength12Bit = 1,
			/// 10 бит.
			CounterLength10bit = 2,
			/// 8 бит.
			CounterLength8Bit = 3,
		};

	public:

		/// Установить разрядность счётчика.
		static inline void SetCounterLength(CounterLength counterLength)
		{
			_TimerControl::_controlRegisterAddr->CounterLength = counterLength;
		}

	};

	/// Управление регистром счётчика
	/// \tparam RegisterDataType Тип (размер) значения регистра.
	/// \tparam PointerType Тип (размер) значения адресов регистров.
	/// \tparam PointerType Адрес регистра счётчика.
	template <class RegisterDataType,
			class PointerType,
			PointerType CounterRegisterAddr
		>
	class TimerCounter
	{
	protected:
		/// Адрес регистра счётчика.
		static volatile RegisterDataType * const _counterRegisterAddr;
	public:
		/// Чтение значения счётчика.
		static inline RegisterDataType GetCounter()
		{
			return *_counterRegisterAddr;
		}
		/// Запись значения счётчика.
		static inline void SetCounter(RegisterDataType value)
		{
			*_counterRegisterAddr = value;
		}
	};

	/// Управление регистром вектора прерываний.
	/// \tparam RegisterDataType Тип (размер) значения регистра.
	/// \tparam PointerType Тип (размер) значения адресов регистров.
	/// \tparam InterruptVectorRegisterAddr Адрес регистра вектора прерываний.
	template <class RegisterDataType,
			class PointerType,
			PointerType InterruptVectorRegisterAddr
		>
	class TimerInterruptVector
	{
	protected:
		/// Адрес регистра счётчика.
		static volatile RegisterDataType * const _interruptVectorRegisterAddr;
	public:
		/// Чтение значения вектора прерываний.
		static inline RegisterDataType GetInterruptVector()
		{
			return *_interruptVectorRegisterAddr;
		}
	};

	/// Управление регистром настройки захвата/сравнения.
	/// \tparam RegisterDataType Тип (размер) значения регистра.
	/// \tparam PointerType Тип (размер) значения адресов регистров.
	/// \tparam PointerType Адрес регистра настройки захвата/сравнения.
	template <class RegisterDataType,
			class PointerType,
			PointerType CaptureCompareControlRegisterAddr
		>
	class TimerCaptureCompareControl
	{
	public:
		/// Режимы работы PWM выхода.
		enum TimerPwmMode
		{
			TimerPwmModeOut			= 0,	///< уровни на
			TimerPwmModeSet			= 1,	///< выходе PWM
			TimerPwmModeToggleReset	= 2,	///< зависят от
			TimerPwmModeSetReset	= 3,	///< режима, задданного
			TimerPwmModeToggle		= 4,	///< этими константами,
			TimerPwmModeReset		= 5,	///< и режима работы таймера.
			TimerPwmModeToggleSet	= 6,	///< см. описание
			TimerPwmModeResetSet	= 7		///< контроллера
		};

		/// Режимы захвата.
		enum TimerCaptureMode
		{
			TimerCaptureModeNone = 0, ///< нет захвата
			TimerCaptureModeRising = 1, ///< по переднему фронту
			TimerCaptureModeFalling = 2, ///< по заднему фронту
			TimerCaptureModeBoth = 3 ///< по обоим фронтам
		};

		/// Входные сигналы для режима захвата.
		enum TimerCaptureSignal
		{
			TimerCaptureInputA = 0, ///< CCIxA
			TimerCaptureInputB = 1, ///< CCIxB
			TimerCaptureInputC = 2, ///< GND
			TimerCaptureInputD = 3  ///< Vcc
		};

	protected:
		/// Адрес регистра настрйоки захвата/сравнения.
		static volatile TimerCaptureCompareControlRegister * const _captureCompareControlRegisterAddr;
	public:
		/// Чтение значения регистра управления.
		static inline RegisterDataType GetControl()
		{
			return *(RegisterDataType *)_captureCompareControlRegisterAddr;
		}
		/// Запись значения регистра управления.
		static inline void SetControl(RegisterDataType value)
		{
			*(RegisterDataType *)_captureCompareControlRegisterAddr = value;
		}
		/// Разрешить прерывания.
		static inline void InterruptEnable()
		{
			_captureCompareControlRegisterAddr->InterruptEnable = 1;
		}
		/// Запретить прерывания.
		static inline void InterruptDisable()
		{
			_captureCompareControlRegisterAddr->InterruptEnable = 0;
		}
		/// Установить "высокий" уровень на выходной ноге (применяется в режиме OutputNode == 0).
		static inline void CompareSetOutputHi()
		{
			_captureCompareControlRegisterAddr->Output = 1;
		}
		/// Установить "низкий" уровень на выходной ноге (применяется в режиме OutputNode == 0).
		static inline void CompareSetOutputLow()
		{
			_captureCompareControlRegisterAddr->Output = 0;
		}
		/// Установить режим работы PWM.
		static inline void CompareSetMode(TimerPwmMode mode)
		{
			_captureCompareControlRegisterAddr->OutputMode = mode;
		}
		/// Включить режим сравнения (режим Pwm).
		static inline void CompareEnable()
		{
			_captureCompareControlRegisterAddr->EnableCapture = 0;
		}

		/// Включить режим захвата.
		static inline void CaptureEnable()
		{
			_captureCompareControlRegisterAddr->EnableCapture = 1;
		}

		/// Выбрать режим захвата.
		static inline void CaptureSetMode(TimerCaptureMode mode)
		{
			_captureCompareControlRegisterAddr->CaptureMode = mode;
		}

		/// выбрать сигнал для захвата.
		static inline void CaptureSetSignal(TimerCaptureSignal signal)
		{
			_captureCompareControlRegisterAddr->CaptureInputSelect = signal;
		}

		/// Проверить влаг прерывания.
		static inline bool InterruptOccured()
		{
			return _captureCompareControlRegisterAddr->InterruptFlag;
		}

		/// Сбросить флаг прерывания.
		static inline void ClearInterruptFlag()
		{
			_captureCompareControlRegisterAddr->InterruptFlag = 0;
		}
	};

	/// Управление регистром захвата/сравнения.
	/// \tparam Тип (размер) значения регистра.
	/// \tparam Тип (размер) значения адресов регистров.
	/// \tparam Адрес регистра захвата/сравнения.
	template <class RegisterDataType,
			class PointerType,
			PointerType CaptureCompareRegisterAddr
		>
	class TimerCaptureCompareRegister
	{
	protected:
		/// Адрес регистра счётчика.
		static volatile RegisterDataType * const _captureCompareRegisterAddr;
	public:
		/// Чтение значения счётчика.
		static inline RegisterDataType GetValue()
		{
			return *_captureCompareRegisterAddr;
		}
		/// Запись значения счётчика.
		static inline void SetValue(RegisterDataType value)
		{
			*_captureCompareRegisterAddr = value;
		}
	};

	/// Управление регистром расширений
	/// \tparam RegisterDataType Тип (размер) значения регистра.
	/// \tparam PointerType Тип (размер) значения адресов регистров.
	/// \tparam ExpansionRegisterAddr Адрес регистра расширений.
	template <class RegisterDataType,
			class PointerType,
			PointerType ExpansionRegisterAddr
		>
	class TimerExpansionRegister
	{
	public:
		/// Варианты дополнительных делителей.
		enum TimerDividerEx
		{
			TimerDivEx1 = 0,	///< /1
			TimerDivEx2 = 1,	///< /2
			TimerDivEx3 = 2,	///< /3
			TimerDivEx4 = 3,	///< /4
			TimerDivEx5 = 4,	///< /5
			TimerDivEx6 = 5,	///< /6
			TimerDivEx7 = 6,	///< /7
			TimerDivEx8 = 7,	///< /8
		};
	protected:
		/// Адрес регистра счётчика.
		static volatile RegisterDataType * const _expansionRegisterAddr;
	public:
		/// Чтение значения счётчика.
		static inline RegisterDataType GetExpansion()
		{
			return *_expansionRegisterAddr;
		}
		/// Запись значения счётчика.
		static inline void SetExpansion(RegisterDataType value)
		{
			*_expansionRegisterAddr = value;
		}
		/// Установить дополнительный делитель тактового сигнала.
		static inline void SetDividerEx(TimerDividerEx value)
		{
			*_expansionRegisterAddr &= ~0x07;
			*_expansionRegisterAddr |= value;
		}
	};

	// Инстанцирование статических переменных

	// TimerControl

	template<class PointerType, PointerType ControlRegisterAddr, class TimerControlRegisterStructure>
	volatile TimerControlRegisterStructure* const TimerControl<PointerType, ControlRegisterAddr, TimerControlRegisterStructure>
		::_controlRegisterAddr = reinterpret_cast<volatile TimerControlRegisterStructure*>(ControlRegisterAddr);

	// TimerCounter

	template<class RegisterDataType, class PointerType, PointerType CounterRegisterAddr>
	volatile RegisterDataType* const TimerCounter<RegisterDataType, PointerType, CounterRegisterAddr>
		::_counterRegisterAddr = reinterpret_cast<volatile RegisterDataType*>(CounterRegisterAddr);

	// TimerInterruptVector

	template<class RegisterDataType, class PointerType, PointerType InterruptVectorRegisterAddr>
	volatile RegisterDataType* const TimerInterruptVector<RegisterDataType, PointerType, InterruptVectorRegisterAddr>
		::_interruptVectorRegisterAddr = reinterpret_cast<volatile RegisterDataType*>(InterruptVectorRegisterAddr);

	// TimerCaptureCompareControl

	template<class RegisterDataType, class PointerType, PointerType CaptureCompareControlRegisterAddr>
	volatile TimerCaptureCompareControlRegister* const TimerCaptureCompareControl<RegisterDataType, PointerType, CaptureCompareControlRegisterAddr>
		::_captureCompareControlRegisterAddr = reinterpret_cast<volatile  TimerCaptureCompareControlRegister*>(CaptureCompareControlRegisterAddr);

	// TimerCaptureCompareRegister

	template<class RegisterDataType, class PointerType, PointerType CaptureCompareRegisterAddr>
	volatile RegisterDataType* const TimerCaptureCompareRegister<RegisterDataType, PointerType, CaptureCompareRegisterAddr>
		::_captureCompareRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(CaptureCompareRegisterAddr);

	// TimerExpansionRegister

	template<class RegisterDataType, class PointerType, PointerType ExpansionRegisterAddr>
	volatile RegisterDataType* const TimerExpansionRegister<RegisterDataType, PointerType, ExpansionRegisterAddr>
		::_expansionRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(ExpansionRegisterAddr);
}
}

#endif