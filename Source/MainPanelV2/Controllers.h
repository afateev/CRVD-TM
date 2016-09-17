#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "Drivers.h"
#include "DriveController.h"
#include "DriveControllerParams.h"
#include "InsulationControl.h"
#include "ControllerDiagnostic.h"
#include "DriveControllerSwitch.h"
#include "PortScanner.h"
#include "DriveEventsGenerator.h"
#include "OscFileFormat.h"
#include "../MainPanel/MegaLib/Modbus/ModbusStateMachine.h"

class MainComPort
{
};

typedef ModBusMasterStateMachine<Drivers::Board::MainComPort, 256> 		ModBusState;
typedef DriveController<ModBusState, 0x01, 0x02, OscFileFormat::OscRecordSize> 	PrimaryController;
typedef DriveController<ModBusState, 0x02, 0x01, OscFileFormat::OscRecordSize> 	ReserveController;
typedef ControllerDiagnostic<ModBusState, 0x01, 1, 17>	MainControllerDiagnostic;
typedef ControllerDiagnostic<ModBusState, 0x02, 1, 17>	ReservControllerDiagnostic;
typedef ControllerDiagnostic<ModBusState, 0x03, 1, 17>	ControllerDiagnosticTemperature;
typedef InsulationControl<ModBusState, 0x01, 1, 10> 	InsulationController;

DriveControllerInterface<PrimaryController, true>	PrimaryControllerInterface;
DriveControllerInterface<ReserveController, false>	ReserveControllerInterface;

typedef DriveControllerParams<DriveControllerInterfaceBase, Config> ActiveDriveControllerParams;

typedef DriveControllerSwitch<Drivers::Board::ControllerSwitchConnection> ControllerSwitch;

//����� ��������
class Empty
{
public:
	static bool Run()
	{
		return true;
	}
};

void SendOscReadRequest(unsigned int pos, bool &result)
{
	result |= PrimaryController::OscRequest(pos);
	result |= ReserveController::OscRequest(pos);
}

//typedef PortScanner<Drivers::Board::PortScanerConnection, PrimaryController, ReserveController, MainControllerDiagnostic, ReservControllerDiagnostic, ControllerDiagnosticTemperature> portScanner;

typedef DriveEventsGenerator<ActiveDriveControllerParams, Events> DriveEvets;

#endif