/// \file
///	\brief Коды событий и класс события.
///	


#ifndef EVENTS_H
#define EVENTS_H

/// Пространство имёт библиотекаи RBlib.
namespace Rblib
{
	/// Тип кодов событий.
	typedef unsigned char EventCodeType;
	/// Тип параметра 1.
	typedef unsigned char EventParam1Type;
	/// Тип параметра 2.
	typedef unsigned char EventParam2Type;
	/// Тип параметра 3.
	typedef unsigned char EventParam3Type;
	/// Тип параметра 4.
	typedef unsigned char EventParam4Type;

	/// \brief Коды событий.
	typedef enum
	{
		/// Просто событие, ничего не значащее (для тестов).
		EventEmty = 0,
#ifndef __ICCARM__
		/// Прерывание порта по изменению "ноги".\n
		/// Param1 - Номер порта.\n
		/// Param2 - Номер "ноги".\n
		/// Param3 - Состояние "ноги".
		EventGpioPinChanged = 1,
		/// Переполнение таймера.\n
		/// Param1 - Номер таймера.
		EventTimerOverflow = 2,
		/// Счётчик таймера достиг регистра сравнения.\n
		/// Param1 - Номер таймера.\n
		/// Param2 - Номер регистра сравнения.
		EventTimerCompare = 3,		
		/// Прерывание от Uart.\n
		/// Param1 - Номер интерфейса.
		EventUart = 4,
		/// Обнаружены ошибки источников тактирования.
		EventClockErrorsSet = 5,
		/// Все флаги ошибок источников тактирования сбросились.
		EventClockErrorsClear = 6,
#endif
		/// Ошибка шины I2C
		/// Param1 - Номер интерфейса.
		EventI2CError = 7,
        /// I2C, завершена асинхронная операция чтения.
        /// Param1 - Номер интерфейса.
        /// Param2 - код ошибки.
        EventI2CAsynchWriteComplete = 8,
        /// I2C, завершена асинхронная операция чтения.
        /// Param1 - Номер интерфейса.
        /// Param2 - код ошибки.
        EventI2CAsynchReadComplete = 9,
		/// Уведомление Uart о начале приема данных
		/// Param1 - Номер интерфейса.
		EventUartReceiveStartNotify = 10,
		/// Уведомление Uart о начале отправки данных
		/// Param1 - Номер интерфейса.
		EventUartTransmitStartNotify = 11,
		/// События графического интерфейса пользователя
		/// Param1 - код GUI события
		/// Param2..3 - параметры GUI события
		EventGui = 12,
        /// Событие фильтра событий
        /// Param1 - параметр переданный пользователем
        EventFilterEvent = 13,
		/// Начало пользовательских событий.
		EventUser = 14
	} EventCode;

	/// Событие.
	class Event
	{
	public:	
		/// Код события.
		EventCode Code;
		/// Параметр события зависящий от кода.
		EventParam1Type Param1;
		/// Параметр события зависящий от кода.
		EventParam2Type Param2;
		/// Параметр события зависящий от кода.
		EventParam3Type Param3;
		/// Параметр события зависящий от кода.
		EventParam4Type Param4;
	private:
	public:

		/// Конструткор по умолчанию
		Event()
		{}
        
		/// Конструткор по коду события.
		Event(EventCode code):
			Code(code), Param1(0), Param2(0), Param3(0), Param4(0)
		{}
		/// Конструктор по коду + 1 параметр.
		Event(EventCode code, EventParam1Type param1):
			Code(code), Param1(param1), Param2(0), Param3(0), Param4(0)
		{}
		/// Конструткор по коду + 2 параметра.
		Event(EventCode code, EventParam1Type param1, EventParam2Type param2):
			Code(code), Param1(param1), Param2(param2), Param3(0), Param4(0)
		{}
		/// Конструткор по коду + 3 параметра.
		Event(EventCode code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3):
			Code(code), Param1(param1), Param2(param2), Param3(param3), Param4(0)
		{}
		/// Конструткор по коду + 4 параметра.
		Event(EventCode code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3, EventParam4Type param4):
			Code(code), Param1(param1), Param2(param2), Param3(param3), Param4(param4)
		{}
		
		/// Конструктор для кода события в виде целого числа.
		/// \details Удобно для пользовательских событий.
		Event(unsigned int code):
			Code((EventCode)code), Param1(0), Param2(0), Param3(0), Param4(0)
		{}
		/// Конструктор по коду + 1 параметр
		Event(unsigned int code, EventParam1Type param1):
			Code((EventCode)code), Param1(param1), Param2(0), Param3(0), Param4(0)
		{}
		/// Конструткор по коду + 2 параметра.
		Event(unsigned int code, EventParam1Type param1, EventParam2Type param2):
			Code((EventCode)code), Param1(param1), Param2(param2), Param3(0), Param4(0)
		{}
		/// Конструткор по коду + 3 параметра.
		Event(unsigned int code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3):
			Code((EventCode)code), Param1(param1), Param2(param2), Param3(param3), Param4(0)
		{}
		/// Конструткор по коду + 4 параметра.
		Event(unsigned int code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3, EventParam4Type param4):
			Code((EventCode)code), Param1(param1), Param2(param2), Param3(param3), Param4(param4)
		{}

		/// Конструткор по коду события.
		void Set(EventCode code)
		{
			Code = code;
			Param1 = 0;
			Param2 = 0;
			Param3 = 0;
			Param4 = 0;
		}

		/// Конструктор по коду + 1 параметр.
		void Set(EventCode code, EventParam1Type param1)
		{
			Code = code;
			Param1 = param1;
			Param2 = 0;
			Param3 = 0;
			Param4 = 0;
		}

		/// Конструткор по коду + 2 параметра.
		void Set(EventCode code, EventParam1Type param1, EventParam2Type param2)
		{
			Code = code;
			Param1 = param1;
			Param2 = param2;
			Param3 = 0;
			Param4 = 0;
		}

		/// Конструткор по коду + 3 параметра.
		void Set(EventCode code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3)
		{
			Code = code;
			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = 0;
		}
		
		/// Конструткор по коду + 4 параметра.
		void Set(EventCode code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3, EventParam4Type param4)
		{
			Code = code;
			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = param4;
		}

        bool operator == (const Event& e) const
        {
            return (Code == e.Code && Param1 == e.Param1 && Param2 == e.Param2 && Param3 == e.Param3 && Param4 == e.Param4);
        }

        bool operator != (const Event& e) const
        {
            return (Code != e.Code || Param1 != e.Param1 || Param2 != e.Param2 || Param3 != e.Param3 || Param4 != e.Param4);
        }

	};

	// Шаблон обработчика событий.
	template<class T>
	class EventHandlerClass
	{
		T *handler;
	public:
  		EventHandlerClass(T *object) : handler(object)
		{
		}
  		/// Оператор обработки событий.
		void operator()(const Event &e)
		{
			handler->EventHandler(e);
		}
	};
}

#endif // EVENTS
