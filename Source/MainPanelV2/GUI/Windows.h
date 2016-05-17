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
	wndIdGraph,										// ������� (�������������)
	// ���� ��������� ���������� ����������
    wndIdControllerNominalStatorCurrent,			// ����������� ��� �������
    wndIdControllerStartStatorCurrent,
    wndIdControllerNominalRotorCurrent,
    wndIdControllerStartRotorCurrent,
	wndIdControllerRotorCurrentMax,					// I ������ ������������
	wndIdControllerRotorCurrentMin,					// I ������ �����������
    wndIdControllerCosChannel,					    // ����� �� cos
	wndIdControllerAngleSetupDisplay,				// ��������� ���� (����������)
	wndIdControllerEnergizingCurrentSetup,			// ��������� I �����������
	wndIdControllerGlideSetup,						// ��������� �� ����������
    wndIdControllerGlideStart,						// ���� �� ����������
	//wndIdControllerH_ka_w_c0,
	wndIdControllerH_ka_f_I0,
	wndIdControllerH_ka_f_tg,
	wndIdControllerH_ka_f_v,
	wndIdControllerLowStatorVoltageProtection,		        // ������ �� ������� ���������� �������
    wndIdControllerLowStatorVoltageProtectionThreshold,		// ����� ������ �� ������� ���������� �������
    wndIdControllerLowStatorVoltageProtectionTimer,
    wndIdControllerLowStatorCurrentProtection,
	wndIdControllerLowStatorCurrentProtectionThreshold,		// ����� ������ �� ������� ���� �������
    wndIdControllerLowStatorCurrentProtectionTimer,
    wndIdControllerAsyncRotorRotationCount,
    wndIdControllerAsyncCountdownTimer,
    wndIdControllerAsyncProtectionTimer,
    wndIdControllerEnergizingLostTimer,
    wndIdControllerLongStartTimer,
	wndIdControllerShortCircuitProtectionThreshold,			// ����� ������������ ������ �� ��
    wndIdControllerShortCircuitProtectionTimer,
	wndIdControllerOverloadProtectionThreshold,				// ����� ��� ����������
	wndIdControllerRotorCoolingCurrent,						// ��� ��������� ������
	wndIdControllerRotorCoolingTime,						// ����� ��������� ������
	wndIdControllerHeatingTimeOnOverload0_10,				// ����� ������� ��� ���������� 0 - 10 %
	wndIdControllerHeatingTimeOnOverload10_20,				// ����� ������� ��� ���������� 10 - 20 %
	wndIdControllerHeatingTimeOnOverload20,					// ����� ������� ��� ���������� 20 � ����� %
	wndIdControllerWaitAfterDriveOn,
    wndIdControllerWaitAfterStatorCurrenLow,
    wndIdControllerWaitAfterenergizingOn,
    wndIdControllerStartForsingTime,
    wndIdControllerStatorVoltageMaxDisplay,
    wndIdControllerStatorCurrentMaxDisplay,
    wndIdControllerRotorVoltageMaxDisplay,
    wndIdControllerRotorCurrentMaxDisplay,
	
    wndIdControllerUstCosChannel,					    // ����� �� cos
	wndIdControllerUstAngleSetupDisplay,				// ��������� ���� (����������)
	wndIdControllerUstEnergizingCurrentSetup,			// ��������� I �����������
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
	
    wndIdLast		// �� �������, ��� ���� ������������ ���������� ����, ������ ���� � ����� ������������
};


#endif