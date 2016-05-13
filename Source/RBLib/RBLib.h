///////////////////////////////////////////////////////////////////////////////
//	Библиотека RBLib
//	Библиотека реализована с целью удобства использования и адаптации для новых
//	платформ (процессоров, контроллеров).
//	Функции библиотеки разделены на несколько частей:
//		1. Работа со встроенными средствами процессора, такими как таймеры,
//	порты ввода-вывода и т.п. Это платформо зависимая часть.
//		2. Работа с внешними устройствами, такими как Flash, GPS и т.д. Эта часть
//	использует функционал и API первой части, т.е. является независимой от типа
//	процессора.
//		3. Функциональность для работы устройства в составе комплекса "Радиобарьер".
//		4. Иные вспопогательные программные средства.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_H
#define RBLIB_H

// Общие параметры
#include "Common.h"
#include "CriticalSection.h"

#include "Utils\vector_static.h"
#include "Utils\queue_static.h"
#include "Utils\map_static.h"

#include "Utils\vector_dynamic.h"
#include "Utils\queue_dynamic.h"
#include "Utils\map_dynamic.h"

// Основной класс приложения
//#include "Application.h"

// Общие переменные приложения
//#include "GlobalVars.h"

// Система тактирования
#include "ClockSystem.h"
#include "ClockManagementUnit.h"
#include "ResetAndClockControl.h"
// Параллейльная шина
#include "ExternalBusInterface.h"
// Управление портами ввода-вывода
#include "Gpio.h"
// Управление аппаратными таймерами
#include "Timer.h"
// Управление строжевым таймером
#include "Wdt.h"
// Управление АЦП
#include "Adc.h"
// Управление ЦАП
#include "Dac.h"
// Управление флэш памятью
#include "FlashMemoryController.h"
// Управление флэш памятью
#include "FlashMemorySegmented.h"
// Доступ к области флэш памяти для хранения пользовательских данных
#include "Flash.h"
// Управления регистрами специальных функций
#include "SpecialFunctionRegisters.h"
// Управление ригистрами конфигурации системы
#include "SysConfigurationRegisters.h"
// Управление ядром
#include "SystemControlBlock.h"
// Управление энергонезависмой памятью
#include "MemorySystemController.h"
// Управление ситемным таймером ядра
#include "SystemTimer.h"

// Управление прерываниями
#include "InterruptMap.h"
// Nested Vectored Interrupt Controller
#include "Nvic.h"

// Управление событиями
#include "EventDispetcher.h"

// Управление программными таймерами
#include "ProgramTimer.h"

// Реализация i2c интерфейса
//#include "i2c.h"

// Реализация UART интерфейса
#include "uart.h"
#include "usart.h"

// Реализация SPI интерфейса
#include "spi.h"
#include "SpiMaster.h"

// DMA
#include "Dma.h"

// Alternate function I/O
#include "Afio.h"

// USB
#include "Usb.h"

// Драйверы устройств
#include "Driver.h"

#include "ScreenBuffer.h"
#include "DrawContext.h"
#include "FbgFont.h"
#include "Fonts.h"
#include "Display.h"

#include "..\MainPanel\MegaLib\stm32\f107\Rtc.h"


/*
// Список поддерживаемых протоколов
#include "Protocol\Protocol.h"

// Реализация очереди пакетов
#include "Protocol\PacketQueue.h"

// Модуль обмена данными
#include "Communication\CommunicationModule.h"

// Реализация простого интерфейса
#include "Communication\InterfaceSimple.h"

// Базовые обработчики команд
#include "Protocol\CommandHandlerBase.h"

// Программный эмулятор uart интерфейса 
#include "Utils\UartEmulator.h"

// Одиночка
#include "Utils\Singletone.h"

// Стандартная реализация обработчика запроса версии
#include "StdDevice\VersionReporter.h"
// Стандартная реализация контейнера географических координат
#include "StdDevice\CoordinatesStorage.h"
// Стандартная реализация контейнера географических координат направления
#include "StdDevice\CoordinatesDirectionStorage.h"
// Драйвер батареи
#include "StdDevice\Battary.h"
// Класс отсылки уведомлений об оставшемся времени работы батареи
#include "StdDevice\BattaryReporter.h"
// Стандартная реализация обработчика запроса системной информации
#include "StdDevice\SysInfoReporter.h"
// Стандартная реализация работы с радиосетью по протоколу G1
//#include "StdDevice\G1\Network.h"

// Режимы энергопотребления
#include "OperatingModeWise.h"

// Управление питанием.
#include "PowerManagmentModule.h"

// Графический интерфейс
//#include "Gdi\Gdi.h"
// Графический интерфейс пользователя
//#include "Gui\Gui.h"

// Модем
//#include "Modem.h"

// Носители информации
//#include "Media.h"
*/
/*!
\mainpage Содержание
- \subpage PageAbout
- \subpage PageFunctions "Основные функции библиотеки:"
	- \ref PagePlatforms
		- \ref PageGpio
		- \ref PageFlash
		- \ref ClockSystem
		- \ref PowerManagmentModule
	- \ref PagePeriphery
	- \ref PageEvents
	- \ref PageSkRb
		- \ref PageFirTwig
		- \ref PageStdDev
			- \ref PageStdDevCoordinatesStorage

\page PageAbout О библиотеке
Библиотека реализована с целью удобства использования и адаптации типовых программных решений для новых платформ (процессоров, контроллеров).
Функции библиотеки разделены на несколько частей:
	- Работа со встроенными средствами процессора, такими как таймеры, порты ввода-вывода и т.п. Это платформо зависимая часть.
	- Работа с внешними устройствами, такими как Flash, GPS и т.д. Эта часть использует функционал и API первой части, т.е. является независимой от типа
процессора.
	- Функциональность для работы устройства в составе комплекса "Радиобарьер".
	- Иные вспопогательные программные средства.

\page PageFunctions Основные функции библиотеки
	- \subpage PagePlatforms, такими как таймеры, порты ввода-вывода и т.п. Это платформо зависимая часть.
	- \subpage PagePeriphery, такими как Flash, GPS и т.д. Эта часть использует функционал и API первой части, т.е. является независимой от типа
процессора.
	- \subpage PageSkRb.
	- Иные вспопогательные программные средства.

\page PagePlatforms Работа со встроенными средствами процессора
Библиотека поддерживает следующие аппаратные части различных платформ:
<table>
<tr align="center"><td>Аппаратная часть</td>				<td>Msp430f1611</td>	<td>Msp430f2618</td>		<td>Msp430f5435a</td>		<td>Msp430f5438a</td>		</tr>
<tr align="center"><td>\ref PageGpio "Gpio"</td>			<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>Timer</td>							<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>Wdt</td>								<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>Uart</td>							<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>I2C</td>								<td></td>				<td>+ (только master)</td>	<td>+ (только master)</td>	<td>+ (только master)</td>	</tr>
<tr align="center"><td>ADC</td>								<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>\ref PowerManagmentModule "PMM"</td>	<td></td>				<td></td>					<td>-+</td>					<td>-+</td>					</tr>
<tr align="center"><td>\ref ClockSystem "ClockSystem"</td>	<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>\ref PageFlash "Flash"</td>			<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
</table>

Работа с перечисленными аппаратными частями описана в следующих разделах:
- \subpage PageGpio
- \subpage PageFlash
- \subpage ClockSystem
- \subpage PowerManagmentModule

\page PagePeriphery Работа с внешними устройствами
Пока ни одного периферийного устройства не поддерживается.

\page PageEvents Система событий
В библиотеке реализована система событий, позволяющая пользователю не заботиться об обработчиках прерываний (они определены в библиотеке).
Каждому прерыванию поставлено в соответствие событие с некоторым кодом. Используя код события, пользователь может связывать с ним свои обработчики.
При возникновении события вызов обработчиков может осуществляться как непосредственно из прерывания, так и отложенно из основного цикла.
\nСписок событий определён в перечислении Rblib::EventCode.
\nУправление обработчика событий осучествляется с помощью диспетчера событий, реализованного в классе ::Rblib::EventDispetcher.


\page PageSkRb Обеспечение работы устройства в составе комплекса "Радиобарьер"
- \subpage PageStdDev

\page PageStdDev Стандартные функции

Каждое устройство, работающее в составе СК "Радиобарьер", должно поддержиывать ряд стандартных функций.

Некоторые из них реализованы на уровне библиотеки:
- \subpage PageStdDevCoordinatesStorage

\page PageStdDevCoordinatesStorage Хранение и выдача географических координат
Устройство должно принимать команды установки координат и сохранять координаты в энергонезависимой памяти.\n
Обрабатываются только те пары координат, для которых поле номера устройства в пакете совпадают с его собственным номером.

Устройство должно принимать команды запроса координат и отправлять в ответ координаты, ранее сохраненные в памяти.\n
Запрос обрабатывается только, если в поле данных команды указан номер устройства, совпадающий с его собственным.\n
Ответ отправляется только если в памяти есть координаты.

Данный функционал реализуется классом Rblib::StdDevice::CoordinatesStorage.

\page PageFirTwig Протокол FirTwig2

*/

#endif
