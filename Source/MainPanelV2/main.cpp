
#include "../RbLib/Rblib.h"
#include "..\MainPanel\fat_filelib\fat_filelib.h"
#include "..\MainPanel\fat_filelib\fat_format.h"
#include "Board.h"
#include "Drivers.h"
#include "GUI/GUI.h"
#include "Controllers.h"
#include "KeyboardScanner.h"
#include "SideIndicators.h"
#include "Files.h"

typedef KeyboardScanner<Drivers::Board::Gpio::D, 4, 3, 2, 1, 15, 14, 13, 12> keyboardScanner;

SideIndicators<Drivers::StatorDisplay, Drivers::RotorDisplay, ActiveDriveControllerParams> StatorRotorIndicators;

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


void GuiTimerTick()
{
	Drivers::Board::Gpio::A::SetBit(8);
	
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
	
	Drivers::Board::Gpio::A::ClearBit(8);
}

unsigned long FatStateWaitGuardTimeout = 0;

int main()
{
	Drivers::Init();
	
	Drivers::Board::TenKiloHertzTimer::UpdateInterruptHandler = OnTenKiloHertzTimerTick;
	Drivers::Board::KiloHertzTimer::UpdateInterruptHandler = OnKiloHertzTimerTick;
	Drivers::Board::GuiTimer::UpdateInterruptHandler = GuiTimerTick;
	
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
	
	StatorRotorIndicators.Init();
	
	ModBusState::Init();
	
	Flash.Init();
	fl_init();
	
	Events::Init(EventsRead, EventsWrite, EventsSeek);
	ActiveDriveControllerParams::Init(ControllerFilesRead, ControllerFilesWrite);
	
	OscGet::Init(OscFilesRead, OscFilesWrite);
	
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
		ActiveDriveControllerParams::Run();
		/*
		if (ModBus485Slave::MyAddress != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamSlaveAddr>())
		{
			ModBus485Slave::MyAddress = ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamSlaveAddr>();
		}
		
		if (ModBusSlaveState::CurrentBoudrate != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamBoudrate>())
		{
			ModBusSlaveState::ChangeSpeed(Config::CoreFrequency, ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamBoudrate>());
		}
		
		if ((ModBusSlaveState::CurrentParityEnabled != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParity>()) ||
			(ModBusSlaveState::CurrentParityEven != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParityType>()))
		{
			ModBusSlaveState::ChangeParity(ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParity>(),
										   ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParityType>());
		}*/
		
		StatorRotorIndicators.Update();
		
		Flash.Run();
		switch(fatState)
		{
		case FatStateInit:
			{
				if (Flash.Ready())
				{
					if (fl_attach_media(&media_read, media_write) != FAT_INIT_OK)
					{
						/// Если поломалось можно отформатировать
						/*
						fatfs_init(&fs_formatting);
						fs_formatting.disk_io.read_media = media_read;
						fs_formatting.disk_io.write_media = media_write;
						fatfs_format_fat16(&fs_formatting, 0x10000, "123");
						fatState = FatStateError;
						*/
					}
					else
					{
						//fl_listdirectory("/");
                        //FatStateWaitGuardTimeout = 10;
                        //fatState = FatStateWaitGuard;
                        fatState = FatStateReady;
					}
				}
			}
			break;
        // не помогает
        case FatStateWaitGuard:
            {
                if (FatStateWaitGuardTimeout > 0)
                {
                    FatStateWaitGuardTimeout--;
                }
                else
                {
                    fatState = FatStateReady;
                }
                if (!Flash.Ready())
				{
					fatState = FatStateInit;
				}
            }
            break;
		case FatStateError:
		case FatStateReady:
			{
				if (!Flash.Ready())
				{
					fatState = FatStateInit;
				}
			}
			break;
		}
	}
}
