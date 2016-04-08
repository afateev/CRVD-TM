#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "Drivers.h"
#include "DriveController.h"
#include "DriveControllerParams.h"
#include "InsulationControl.h"
#include "DriveControllerSwitch.h"
#include "../MainPanel/MegaLib/Modbus/ModbusStateMachine.h"

class MainComPort
{
};

typedef ModBusMasterStateMachine<Drivers::Board::MainComPort, 256> 		ModBusState;
typedef DriveController<ModBusState, 0x01, 0x02> 	PrimaryController;
typedef DriveController<ModBusState, 0x02, 0x01> 	ReserveController;
typedef InsulationControl<ModBusState, 0x01> 		InsulationController;

DriveControllerInterface<PrimaryController, true>	PrimaryControllerInterface;
DriveControllerInterface<ReserveController, false>	ReserveControllerInterface;

typedef DriveControllerParams<DriveControllerInterfaceBase, Config> ActiveDriveControllerParams;

#endif