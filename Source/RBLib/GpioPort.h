///////////////////////////////////////////////////////////////////////////////
//	Классы управления портом ввода-вывода
//	Т.к. на разных платформах порты ввода ввывода управляются разными регистрами,
//	то функции оправления портами разделены на несколько классов
//
//	GpioPortInputOutputDirection - Класс чтения/записи и задания направления работы "линий" порта
//	GpioPortFunctionSelect - Класс управления функциями "линий" порта
//	GpioPortInterrupt - Класс управления прерываниями порта
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIOPORT_H
#define GPIOPORT_H

namespace Rblib
{
	
	/// Класс чтения/записи и задания направления работы "линий" порта
	template <class PortValueDataType,			// Тип (размер) значения порта ввода-вывода
			class PointerType,					// Тип (размер) значения адресов регистров
			PointerType InputRegisterAddr,		// Адрес регистра для чтения значения
			PointerType OutputRegisterAddr,		// Адрес регистра для записи значения
			PointerType DirectionRegisterAddr	// Адрес регистра для задания направлеия ввода-вывода
		>
	class GpioPortInputOutputDirection
	{
	protected:
		// Адрес регистра чтения данных с порта
		static volatile PortValueDataType * const _inputRegisterAddr;
		// Адрес регистра записи данных в порт
		static volatile PortValueDataType * const _outputRegisterAddr;
		// Адрес регистра настройки направления работы "линий" порта
		static volatile PortValueDataType * const _directionRegisterAddr;
	public:
		/*!	\brief Чтение регистра порта ввода/вывода
			\return Значение регистра порта ввода/вывода, в котором каждый бит отражает состояние каждой линии ввода/вывода порта
		*/
		static inline PortValueDataType Read(void)
		{
			return *_inputRegisterAddr;
		}
		
		/*!	\brief Чтение значения одной линии ввода/вывода порта
			\param pinNumber - номер линии (начиная с 0)
			\return true - на линии "высокий" уровень, false - на линии "низкий уровень"
		*/
		static inline PortValueDataType Read(unsigned char pinNumber)
		{
			return *_inputRegisterAddr & (1 << pinNumber);
		}
		
		/*! \brief Запись регистра порта ввода/вывода
			\param value - новое значение регистра порта
		*/
		static inline void Write(PortValueDataType value)
		{
			*_outputRegisterAddr = value;
		}

		/*!	\brief Установка занчения одной линии в логическую 1
			\param pinNumber - номер линии (начиная с 0)
		*/
		static inline void SetBit(unsigned char pinNumber)
		{
			*_outputRegisterAddr |= (1 << pinNumber);
		}

		/*!	\brief Установка занчения одной линии в логический 0
			\param pinNumber - номер линии (начиная с 0)
		*/
		static inline void ClearBit(unsigned char pinNumber)
		{
			*_outputRegisterAddr &= ~(1 << pinNumber);
		}
		
		/*!	\brief Переключение уровня одной линии
			\param pinNumber - номер линии (начиная с 0)
		*/
		static inline void ToggleBit(unsigned char pinNumber)
		{
			*_outputRegisterAddr ^= (1 << pinNumber);
		}

		/*!	\brief Запись маски направлений ввода/вывода линий порта
			\param mask - маска направлений ввода вывода (0 - линия работает как цифровой вход, 1 - как цифровой выход)
		*/
		static inline void SetDirection(PortValueDataType mask)
		{
			*_directionRegisterAddr = mask;
		}

		/*!	\brief Настроить линию как цифровой выход
			\param pinNumber - номер линии (начиная с 0)
		*/
		static inline void SetOutputPin(unsigned char pinNumber)
		{
			*_directionRegisterAddr |= (1 << pinNumber);
		}

		/*!	\brief Настроить линию как цифровой вход
			\param pinNumber - номер линии (начиная с 0)
		*/
		static inline void SetInputPin(unsigned char pinNumber)
		{
			*_directionRegisterAddr &= ~(1 << pinNumber);
		}
	};

	/// Класс настройки линий порта на работу с периферией или как линий ввода/вывода
	template <class PortValueDataType,					// Тип (размер) значения порта ввода-вывода
			class PointerType,							// Тип (размер) значения адресов регистров
			PointerType FunctionSelectRegisterAddr		// Адрес регистра для настройки специальных функция "ног" порта
		>
	class GpioPortFunctionSelect
	{
	protected:
		// Адрес регистра настройки специальных функции "ног" порта
		static volatile PortValueDataType * const _functionSelectRegisterAddr;
	public:
		/*!	\brief Настроить линию как цифровой вход/выход
			\param pinNumber - номер линии (начиная с 0)
		*/
		static inline void SetFunctionGeneralIo(unsigned char pinNumber)
		{
			*_functionSelectRegisterAddr &= ~(1 << pinNumber);
		}
		
		/*!	\brief Настроить линию на работу с периферией
			\param pinNumber - номер линии (начиная с 0)
		*/
		static inline void SetFunctionPeripheral(unsigned char pinNumber)
		{
			*_functionSelectRegisterAddr |= (1 << pinNumber);
		}
	};

	/// Класс управления прерываниями порта
	template <class PortValueDataType,					// Тип (размер) значения порта ввода-вывода
			class PointerType,							// Тип (размер) значения адресов регистров
			PointerType InterruptFlagRegisterAddr,		// Адрес регистра флагов прерываний
			PointerType InterruptEdgeSelectAddr,		// Адрес регистра настройки прерываний по уровню
			PointerType InterruptEnableRegisterAddr		// Адрес регистра разрешения прерываний
		>
	class GpioPortInterrupt
	{
	protected:
		// Адрес регистра флагов прерываний
		static volatile PortValueDataType * const _interruptFlagRegisterAddr;
		// Адрес регистра настройки прерываний по уровню
		static volatile PortValueDataType * const _interruptEdgeSelectRegisterAddr;
		// Адрес регистра разрешения прерываний
		static volatile PortValueDataType * const _interruptEnableRegisterAddr;
	public:
		/*!	\brief Чтение регистра флагов прерываний
			\return Текущий набор флагов прерываний порта
		*/
		static inline PortValueDataType GetInterruptFlags(void)
		{
			return *_interruptFlagRegisterAddr;
		}
		
		/*!	\brief Чтение флага прерывания
			\param bitNumber - номер линии (начиная с 0)
			\return true - если выставлен флаг прерывания
		*/
		static inline PortValueDataType GetInterruptFlag(unsigned char bitNumber)
		{
			return *_interruptFlagRegisterAddr & (1 << bitNumber);
		}
		
		/*!	\brief Запись регистра флагов прерываний
			\param value - новое значение регистра флагов прерываний
		*/
		static inline void SetInterruptFlags(PortValueDataType value)
		{
			*_interruptFlagRegisterAddr = value;
		}

		/*!	\brief Установка флага прерывания
			\param bitNumber - номер линии (начиная с 0)
		*/
		static inline void SetInterruptFlag(unsigned char bitNumber)
		{
			*_interruptFlagRegisterAddr |= (1 << bitNumber);
		}

		/*!	\brief Очистка флага прерывания
			\param bitNumber - номер линии (начиная с 0)
		*/
		static inline void ClearInterruptFlag(unsigned char bitNumber)
		{
			*_interruptFlagRegisterAddr &= ~(1 << bitNumber);
		}

		/*!	\brief Чтение регистра настройки прерываний по уровню
			\return Значение регистра настройки прерываний по уровню
		*/
		static inline PortValueDataType GetInterruptEdgeSelect(void)
		{
			return *_interruptEdgeSelectRegisterAddr;
		}

		/*!	\brief Чтение текущего значения условия прерывания по уровню
			\param bitNumber - номер линии (начиная с 0)
			\return Режим прерывания по уровню
		*/
		static inline PortValueDataType GetInterruptEdgeSelect(unsigned char bitNumber)
		{
			return *_interruptEdgeSelectRegisterAddr & (1 << bitNumber);
		}

		/*!	\brief Чтение текущего значения условия прерывания по уровню
			\param value - новое значение регистра условий прерываний по уровню
		*/
		static inline void SetInterruptEdgeSelect(PortValueDataType value)
		{
			*_interruptEdgeSelectRegisterAddr = value;
		}

		/*!	\brief Установка прерывания по спаду фронта
			\param bitNumber - номер линии (начиная с 0)
		*/
		static inline void SetInterruptEdgeHiToLow(unsigned char bitNumber)
		{
			*_interruptEdgeSelectRegisterAddr |= (1 << bitNumber);
		}

		/*!	\brief Установка прерывания по возрастанию фронта
			\param bitNumber - номер линии (начиная с 0)
		*/
		static inline void SetInterruptEdgeLowToHi(unsigned char bitNumber)
		{
			*_interruptEdgeSelectRegisterAddr &= ~(1 << bitNumber);
		}

		/*!	\brief Чтение регистра разрешения прерываний
			\return Текущий набор флагов разрешения прерываний порта
		*/
		// Чтение регистра разрешения прерываний
		static inline PortValueDataType GetInterruptEnable(void)
		{
			return *_interruptEnableRegisterAddr;
		}

		/*!	\brief Чтение бита разрешения прерывания по изменению уровня на линии
			\param bitNumber - номер линии (начиная с 0)
			\return true - если прерывание разрешено
		*/
		static inline PortValueDataType GetInterruptEnable(unsigned char bitNumber)
		{
			return *_interruptEnableRegisterAddr & (1 << bitNumber);
		}

		/*!	\brief Запись регистра разрешения прерываний
			\param value - новое значение регистра разрешения прерываний
		*/
		static inline void SetInterruptEnable(PortValueDataType value)
		{
			*_interruptEnableRegisterAddr = value;
		}
		
		/*!	\brief Разрешить прерывание по изменению уровня на линии
			\param bitNumber - номер линии (начиная с 0)
		*/
		static inline void InterruptEnable(unsigned char bitNumber)
		{
			*_interruptEnableRegisterAddr |= (1 << bitNumber);
		}

		/*!	\brief Запретить прерывание по изменению уровня на линии
			\param bitNumber - номер линии (начиная с 0)
		*/
		static inline void InterruptDisable(unsigned char bitNumber)
		{
			*_interruptEnableRegisterAddr &= ~(1 << bitNumber);
		}
	};

	/// Класс управления подтягивающими резисторами
	template <class PortValueDataType,					// Тип (размер) значения порта ввода-вывода
			class PointerType,							// Тип (размер) значения адресов регистров
			PointerType ResistorRegisterAddr			// Адрес регистра для настройки подтягивающих резисторов
		>
	class GpioPortResistor
	{
	protected:
		// Адрес регистра настройки специальных функции "ног" порта
		static volatile PortValueDataType * const _resistorRegisterAddr;
	public:
		/*!	\brief Включить подтягивающий резистор
			\param pinNumber - номер линии (начиная с 0)
		*/
		static inline void ResistorEnable(unsigned char pinNumber)
		{
			*_resistorRegisterAddr |= (1 << pinNumber);
		}
		
		/*!	\brief Выключить подтягивающий резистор
			\param pinNumber - номер линии (начиная с 0)
		*/
		static inline void ResistorDisable(unsigned char pinNumber)
		{
			*_resistorRegisterAddr &= ~(1 << pinNumber);
		}
	};


	// Инстанцирование статических переменных

	// GpioPortInputOutputDirection

	template<class PortValueDataType, class PointerType, PointerType InputRegisterAddr, PointerType OutputRegisterAddr, PointerType DirectionRegisterAddr>
	volatile PortValueDataType* const GpioPortInputOutputDirection<PortValueDataType, PointerType, InputRegisterAddr, OutputRegisterAddr, DirectionRegisterAddr>
		::_inputRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(InputRegisterAddr);

	template<class PortValueDataType, class PointerType, PointerType InputRegisterAddr, PointerType OutputRegisterAddr, PointerType DirectionRegisterAddr>
	volatile PortValueDataType* const GpioPortInputOutputDirection<PortValueDataType, PointerType, InputRegisterAddr, OutputRegisterAddr, DirectionRegisterAddr>
		::_outputRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(OutputRegisterAddr);

	template<class PortValueDataType, class PointerType, PointerType InputRegisterAddr, PointerType OutputRegisterAddr, PointerType DirectionRegisterAddr>
	volatile PortValueDataType* const GpioPortInputOutputDirection<PortValueDataType, PointerType, InputRegisterAddr, OutputRegisterAddr, DirectionRegisterAddr>
		::_directionRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(DirectionRegisterAddr);

	// GpioPortFunctionSelect

	template<class PortValueDataType, class PointerType, PointerType FunctionSelectRegisterAddr>
	volatile PortValueDataType* const GpioPortFunctionSelect<PortValueDataType, PointerType, FunctionSelectRegisterAddr>
		::_functionSelectRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(FunctionSelectRegisterAddr);

	// GpioPortInterrupt

	template<class PortValueDataType, class PointerType, PointerType InterruptFlagRegisterAddr, PointerType InterruptEdgeSelectAddr, PointerType InterruptEnableRegisterAddr>
	volatile PortValueDataType* const GpioPortInterrupt<PortValueDataType, PointerType, InterruptFlagRegisterAddr, InterruptEdgeSelectAddr, InterruptEnableRegisterAddr>
		::_interruptFlagRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(InterruptFlagRegisterAddr);

	template<class PortValueDataType, class PointerType, PointerType InterruptFlagRegisterAddr, PointerType InterruptEdgeSelectAddr, PointerType InterruptEnableRegisterAddr>
	volatile PortValueDataType* const GpioPortInterrupt<PortValueDataType, PointerType, InterruptFlagRegisterAddr, InterruptEdgeSelectAddr, InterruptEnableRegisterAddr>
		::_interruptEdgeSelectRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(InterruptEdgeSelectAddr);

	template<class PortValueDataType, class PointerType, PointerType InterruptFlagRegisterAddr, PointerType InterruptEdgeSelectAddr, PointerType InterruptEnableRegisterAddr>
	volatile PortValueDataType* const GpioPortInterrupt<PortValueDataType, PointerType, InterruptFlagRegisterAddr, InterruptEdgeSelectAddr, InterruptEnableRegisterAddr>
		::_interruptEnableRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(InterruptEnableRegisterAddr);

	// GpioPortResistor

	template<class PortValueDataType, class PointerType, PointerType ResistorRegisterAddr>
	volatile PortValueDataType* const GpioPortResistor<PortValueDataType, PointerType, ResistorRegisterAddr>
		::_resistorRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(ResistorRegisterAddr);
}

#endif