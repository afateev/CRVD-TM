
#include "../RbLib/Rblib.h"
#include "..\MainPanel\fat_filelib\fat_filelib.h"
#include "..\MainPanel\fat_filelib\fat_format.h"
#include "Board.h"
#include "Drivers.h"
#include "GUI/GUI.h"
#include "Controllers.h"
#include "KeyboardScanner.h"

typedef KeyboardScanner<Drivers::Board::Gpio::D, 4, 3, 2, 1, 15, 14, 13, 12> keyboardScanner;

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

static void OnTenKiloHertzTimerTick()
{
	ModBusState::PacketTimeoutDetectorTick();
}

static void OnKiloHertzTimerTick()
{
	static unsigned int portScannerCnt = 0;
	static unsigned int oscGetterCnt = 0;
	static unsigned int insulationControlCnt = 0;
	
	if (portScannerCnt >= 10)
	{
		ModBusState::Run();
		
		if (ControllerSwitch::IsPrimaryActive() || true)
		{
			portScanner::SkipPrimary = false;
			if (!portScanner::SkipReserve)
			{
				portScanner::SkipReserve = portScanner::RunCountReserve > 7;
			}
			else
			{
				portScanner::RunCountReserve = 0;
			}
		}
		
		if (ControllerSwitch::IsReserveActive())
		{
			portScanner::SkipPrimary = true;
			portScanner::SkipReserve = false;
			
			if (!portScanner::SkipPrimary)
			{
				portScanner::SkipPrimary = portScanner::RunCountPrimary > 7;
			}
			else
			{
				portScanner::RunCountPrimary = 0;
			}
		}
		
		if (insulationControlCnt >= 1000)
		{
			portScanner::SkipInsulationControl = false; 
			insulationControlCnt = 0;
		}
		
		insulationControlCnt++;
		
		if (!portScanner::SkipInsulationControl)
		{
			portScanner::SkipInsulationControl = portScanner::RunCountInsulationControl > 0;
		}
		else
		{
			portScanner::RunCountInsulationControl = 0;
		}
		
		portScanner::Run();
		
		portScannerCnt = 0;
	}
	
	portScannerCnt++;
	
	display.Tick();
	keyboardScanner::Tick();
	ActiveDriveControllerParams::Tick();
}

char lastKey = 0;

void OnKeyDownCallback(char key)
{
	lastKey = key;
}

int main()
{
	Drivers::Init();
	
	Drivers::Board::TenKiloHertzTimer::UpdateInterruptHandler = OnTenKiloHertzTimerTick;
	Drivers::Board::KiloHertzTimer::UpdateInterruptHandler = OnKiloHertzTimerTick;
	
	display.SelectContext(&Drivers::DrawContext);
			
	display.MoveCursorTo((40 - 18) / 2, 104);
	display.WriteLine("Привет, Мариночка!", 18);
	display.MoveCursorTo((40 - 9) / 2, 120);
	display.WriteLine("Как дела?", 9);
	
	MenuInit();
	DesctopInit();
	ControllerParamsMenuEndEditorsInit();
	
	ActiveDriveControllerParams::SetActiveController(&PrimaryControllerInterface);
	ControllerSwitch::Init();
	
	keyboardScanner::Init();
	keyboardScanner::SetOnKeyDown(OnKeyDownCallback);
	
	ModBusState::Init();
	
	// начали работу
	Event e(Rtc::GetTime(), EventProgramStart);
	Events::Push(e);
	
	while(1)
	{
		if (ControllerSwitch::IsPrimaryActive())
		{
			ActiveDriveControllerParams::SetActiveController(&PrimaryControllerInterface);
		}
		else
		{
			PrimaryControllerInterface.SetDoOnlyLowerRegsRequest(false);
		}
		
		if (ControllerSwitch::IsReserveActive())
		{
			ActiveDriveControllerParams::SetActiveController(&ReserveControllerInterface);
		}
		else
		{
			ReserveControllerInterface.SetDoOnlyLowerRegsRequest(false);
		}
		
		DriveEvets::Run();
		
		if (lastKey)
		{
			if (mainMenu.Visible())
			{
				mainMenu.OnKeyDown(lastKey);
				if ('E' == lastKey)
				{
					if (!mainMenu.Visible())
						desctop.ShowWindow(wndIdMain);
				}
			}
			else
			{
				if (!desctop.OnKeyDown(lastKey))
				{
					if (desctop.IsWindowVisible(wndIdMain))
					{
						if (13 == lastKey)
							mainMenu.Show();
					}
					else
					{
						mainMenu.OnKeyDown(lastKey);
					}
				}
			}
			lastKey = 0;
		}
		
		if (mainMenu.Visible())
		{
			mainMenu.Draw();
		}
		else
		{
			//DrawDebugRegistersScreen();
			//DrawMainScreen();
			desctop.Draw();
		}
	}
}
