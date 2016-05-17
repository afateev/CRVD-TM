////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		16/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOWS_H
#define WINDOWS_H

#include "Window.h"
#include "WindowMain.h"
#include "WindowDebugRegisters.h"
#include "WindowEvents.h"
#include "WindowConfigSystemDateTime.h"
#include "WindowControllerNominalStatorCurrent.h"
#include "WindowEditRegValue.h"
#include "WindowResetOperatingTime.h"
#include "WindowOscList.h"
#include "WindowGraph.h"
#include "WindowDiagnosticRegulator.h"

enum WindowId
{
	wndIdDesctop = -1,
	wndIdMain = 0,
	wndIdDebugRegisters,
	wndIdEvents,
	wndIdConfigSystemDateTime,
	wndIdResetOperatingTime,
	wndIdOscList,
	wndIdGraph,										// графики (осциллограммы)
	// окна настройки параметров регулятора
    wndIdControllerNominalStatorCurrent,			// Номинальный ток статора
    wndIdControllerStartStatorCurrent,
    wndIdControllerNominalRotorCurrent,
    wndIdControllerStartRotorCurrent,
	wndIdControllerRotorCurrentMax,					// I ротора максимальное
	wndIdControllerRotorCurrentMin,					// I ротора минимальное
    wndIdControllerCosChannel,					    // канал по cos
	wndIdControllerAngleSetupDisplay,				// Установка угла (отображеие)
	wndIdControllerEnergizingCurrentSetup,			// Установка I возбуждения
	wndIdControllerGlideSetup,						// Установка по скольжению
    wndIdControllerGlideStart,						// Пуск по скольжению
	//wndIdControllerH_ka_w_c0,
	wndIdControllerH_ka_f_I0,
	wndIdControllerH_ka_f_tg,
	wndIdControllerH_ka_f_v,
	wndIdControllerLowStatorVoltageProtection,		        // защита от низкого напряжения статора
    wndIdControllerLowStatorVoltageProtectionThreshold,		// порог защиты от низкого напряжения статора
    wndIdControllerLowStatorVoltageProtectionTimer,
    wndIdControllerLowStatorCurrentProtection,
	wndIdControllerLowStatorCurrentProtectionThreshold,		// порог защиты от низкого тока статора
    wndIdControllerLowStatorCurrentProtectionTimer,
    wndIdControllerAsyncRotorRotationCount,
    wndIdControllerAsyncCountdownTimer,
    wndIdControllerAsyncProtectionTimer,
    wndIdControllerEnergizingLostTimer,
    wndIdControllerLongStartTimer,
	wndIdControllerShortCircuitProtectionThreshold,			// порог срабатывания защиты от КЗ
    wndIdControllerShortCircuitProtectionTimer,
	wndIdControllerOverloadProtectionThreshold,				// порог для перегрузки
	wndIdControllerRotorCoolingCurrent,						// ток остывания ротора
	wndIdControllerRotorCoolingTime,						// время остывания ротора
	wndIdControllerHeatingTimeOnOverload0_10,				// время нагрева при перегрузке 0 - 10 %
	wndIdControllerHeatingTimeOnOverload10_20,				// время нагрева при перегрузке 10 - 20 %
	wndIdControllerHeatingTimeOnOverload20,					// время нагрева при перегрузке 20 и более %
	wndIdControllerWaitAfterDriveOn,
    wndIdControllerWaitAfterStatorCurrenLow,
    wndIdControllerWaitAfterenergizingOn,
    wndIdControllerStartForsingTime,
    wndIdControllerStatorVoltageMaxDisplay,
    wndIdControllerStatorCurrentMaxDisplay,
    wndIdControllerRotorVoltageMaxDisplay,
    wndIdControllerRotorCurrentMaxDisplay,
	
    wndIdControllerUstCosChannel,					    // канал по cos
	wndIdControllerUstAngleSetupDisplay,				// Установка угла (отображеие)
	wndIdControllerUstEnergizingCurrentSetup,			// Установка I возбуждения
	wndIdControllerEnergizingReactCurrentSetup,
	wndIdControllerEnergizingReactPowerSetup,
    
	wndIdIndicationStatorVoltageMinDisplay,
    wndIdIndicationStatorCurrentMinDisplay,
    wndIdIndicationRotorVoltageMinDisplay,
    wndIdIndicationRotorCurrentMinDisplay,
	
	wndIdModbusSlaveAddr,
	wndIdModbusBoudrate,
	wndIdModbusParity,
	wndIdModbusParityType,
	
	wndIdDiagnosticMain,
	wndIdDiagnosticReserv,
	wndIdDiagnosticTemperature,
	wndIdDiagnosticAnalogOutputs,
	wndIdDiagnosticLinkStatus,
	
    wndIdLast		// не удалять, это типа максимальное количество окон, должно быть в конце перечисления
};


#endif