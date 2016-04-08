///////////////////////////////////////////////////////////////////////////////
//	Основной класс приложения
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_H
#define APPLICATION_H

#include "Communication\CommunicationModule.h"
#include "Communication\InterfaceSimple.h"
#include "Protocol\Protocol.h"
#include "Protocol\CommandHandlerBase.h"
#include "EventDispetcher.h"
#include "GlobalVars.h"
#include "SysConfigurationRegisters.h"
#include "SpecialFunctionRegisters.h"
#include "Wdt.h"
#include "OperatingModeWise.h"
#include "ClockSystem.h"

namespace Rblib
{
  
///////////////////////////////////////////////////////////////
///	\brief Класс отключения сторожевого таймера
///	
///	При создании экземпляра этого класса происходит отключение сторожевого таймера
///////////////////////////////////////////////////////////////
class WdtKiller
{
public:
	/// Конструктор по-умолчанию
	/*! Как только буден создан экземпляр класса, т.е. вызван конструктор,
		сразу же произойдет отключение сторожевого таймера.
	*/
	WdtKiller()
	{
#ifdef __ICC430__
		Rblib::Wdt::Stop();
#endif
	}
};

//
// Класс, вызывающий или не вызывающий метод Sleep() сенсора
// в зависимости от значения параметра _sleep
//
/////////////////////////////////////////////////////////////////////////////////////////////
///	\brief Класс, вызывающий или не вызывающий метод Sleep() сенсора
///
///	\tparam SensorClass	Класс пользовательского устройства.
///	\tparam _sleep		Переход в режим сна.
///
///	В зависимости от параметра шаблона _sleep
///	используется специализация SleepInvokerImpl, вызывающая метод SensorClass::Sleep()
///	или не вызывающая его.
///
///	В первом случае класс SensorClass должен реализовывать метод Sleep(void).
///
///	\sa SleepInvokerImpl<SensorClass, false>
///	\sa SleepInvokerImpl<SensorClass, true>
///	\sa Application
/////////////////////////////////////////////////////////////////////////////////////////////
template<class SensorClass, bool _sleep>
class SleepInvokerImpl
{
};

////////////////////////////////////////////////
///	\brief Класс приложения.
///
///	\tparam Params		Параметры приложения
///	\tparam SensorClass	Класс пользовательского устройства
///	\tparam _sleep		Применять логику сна. Если true - то происходит переход в режим сна в конце метода Run. Иначе не выполняется переход в режим сна.
///
///	Основной класс пользовательского приложения.
///	Реализует основную структуру и логику работы пользовательского приложения.
///
////////////////////////////////////////////////
template<class Params, class SensorClass, bool _sleep = false>
class Application
{
protected:
	/// Вариант класса перехода в режим сна
	typedef SleepInvokerImpl<SensorClass, _sleep> SleepInvoker;
  
protected:
	/// Экземпляр объекта отключающего сторожевой таймер
	WdtKiller _wdtKiller;
	/// Экземпляр класса пользовательского устройства
	SensorClass _sensor;

public:
	/// Инициализация приложения
	/*!	Этот метод должен быть вызван в функции main пользовательского приложения до запуска основного цикла.
		В этом методе выполняется проверка не было ли перезагрузки по WatchDog.
		Если была, то увеличивается	счетчик перезагрузок по WatchDog.
		Затем выполняется привязка пользовательского обработчика команд.
		В конце выполняется вызов метода Init пользовательского класса устройства.
	*/
    
    SensorClass& Sensor()
    {
        return _sensor;
    }
    
	void Init()
	{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5340 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5342 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5435A || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5438A || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5659
		// Вариант для определения причины перзагрузки для платформ Msp430f543x
		if (SysConfigurationRegisters::GetLastResetReason() == SysConfigurationRegisters::ResetReasonWdtTimeOut)
			Singletone<GlobalVars>::Instance().IncWatchdogResetCount();
		else
			Singletone<GlobalVars>::Instance().ClearWatchdogResetCount();
#elif RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F2618 || \
	  RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F1611
		// Вариант для определения причины перзагрузки для платформ Msp430f2xxx
		if (SpecialFunctionRegisters::GetWatchdogFlag())
		{
			Singletone<GlobalVars>::Instance().IncWatchdogResetCount();
		}
		else
			Singletone<GlobalVars>::Instance().ClearWatchdogResetCount();
		// Очистить флаг сброса по Wdt
		SpecialFunctionRegisters::ClearWatchdogFlag();
#else
	RBLIB_COMPILE_TIME_WARNING("Watchdog reset counter not supported for selected platform")
#endif
		Protocol::CommandHandlerBase::SetExternalCommandHandler(Protocol::ExternalCommandHandlerFunctor(_sensor));
		_sensor.Init();
	}

	/// Шаг основного цикла приложения
	/*!	Этот метод дожен регулярно вызываться для выполнения основного цикла работы пользовательского устройства.
		\remark Обычно это делается в бесконечном цикле внутри main().
	*/
	void Run()
	{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5340 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5342 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5435A || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5438A || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F2618 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F1611 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5659

		ClockSystem::CheckErrors();
#else
	RBLIB_COMPILE_TIME_WARNING("ClockSystem not supported for selected platform")
#endif
		Singletone<typename SensorClass::CommunicationModule>::Instance().ReadInterfaces();
		Singletone<typename SensorClass::CommunicationModule>::Instance().ProcessPackets();
		// Рассылаем события всем подписчикам
		Singletone<EventDispetcher>::Instance().DispatchMessages();
		// Работа сенсора
		_sensor.Run();
		// Отправка исходящих пакетов
		Singletone<typename SensorClass::CommunicationModule>::Instance().SendPackets();

		// Уснуть, если требуется
		SleepInvoker::Sleep(_sensor);
	}

};

/// \brief Специализации SleepInvokerImpl, не вызывает переход в режим сна
///
///	\sa SleepInvokerImpl<SensorClass, true>
template<class SensorClass>
class SleepInvokerImpl<SensorClass, false>
{
public:
	/// Переход в режим сна
	/*! В данной специализации шаблона реализация метода пустая.
	*/
	static inline void Sleep(SensorClass& sensor)
	{
	}
};

/// \brief Специализации SleepInvokerImpl, вызывает переход в режим сна
///
///	\sa SleepInvokerImpl<SensorClass, false>
template<class SensorClass>
class SleepInvokerImpl<SensorClass, true>
{
public:
	/// Переход в режим сна
	/*!
		\param sensor Объект класса пользовательского устройства (должен быть реализован метод void Sleep(void))

		В данной специализации шаблона вызывается метод sensor.Sleep(), в котором должен быть выполнен переход в режим сна.
	*/
	static inline void Sleep(SensorClass& sensor)
	{
		sensor.Sleep();
	}
};

}

#endif
