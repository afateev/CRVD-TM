
#include "../RbLib/Rblib.h"
#include "..\MainPanel\fat_filelib\fat_filelib.h"
#include "..\MainPanel\fat_filelib\fat_format.h"
#include "Board.h"
#include "Drivers.h"
#include "GUI/GUI.h"
#include "Controllers.h"

void GetMainWindowDisplayData(MainWindowDisplayData &displayData)
{
	displayData.ActiveDriveController.IsPrimaryActive = ActiveDriveControllerParams::IsPrimaryActive();
	displayData.ActiveDriveController.P = ActiveDriveControllerParams::GetP();
	displayData.ActiveDriveController.CosPhi = ActiveDriveControllerParams::GetCosPhi();
	displayData.ActiveDriveController.Q = ActiveDriveControllerParams::GetQ();
	displayData.InsulationController.RIz = InsulationController::GetRegValue(10);
	displayData.ActiveDriveController.FlagCosControl = ActiveDriveControllerParams::GetFlagCosControl();
	displayData.ActiveDriveController.FlagRControl = ActiveDriveControllerParams::GetFlagRControl();
	Events::GetLastEventDescription(&displayData.Events.LastEventTime, &displayData.Events.LastEventText, &displayData.Events.LastEventParamText);
	displayData.ActiveDriveController.FlagEngineOn = ActiveDriveControllerParams::GetFlagEngineOn();
	displayData.ActiveDriveController.FlagHasProblem = ActiveDriveControllerParams::HasProblem();
	displayData.ActiveDriveController.Uptime = ActiveDriveControllerParams::GetUpTime();
	displayData.ActiveDriveController.OperatingTime = ActiveDriveControllerParams::GetOperatingTime();
}

int main()
{
	Drivers::Init();
	
	display.SelectContext(&Drivers::DrawContext);
			
	display.MoveCursorTo((40 - 18) / 2, 104);
	display.WriteLine("Привет, Мариночка!", 18);
	display.MoveCursorTo((40 - 9) / 2, 120);
	display.WriteLine("Как дела?", 9);
	
	MenuInit();
	DesctopInit();
	ControllerParamsMenuEndEditorsInit();
	
	while(1)
	{
		display.Tick();
		desctop.Draw();
	}
}
