
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
#include "OscCache.h"

typedef KeyboardScanner<Drivers::Board::Gpio::D, 4, 3, 2, 1, 15, 14, 13, 12> keyboardScanner;

SideIndicators<Drivers::StatorDisplay, Drivers::RotorDisplay, ActiveDriveControllerParams> StatorRotorIndicators;

typedef OscCacheImplementer<PrimaryController::OscRecordSize, PrimaryController::OscRequestMaxPortionSize> OscCacheType;

OscCacheType OscCache;

void GetMainWindowDisplayData(MainWindowDisplayData &displayData)
{
	displayData.ActiveDriveController.IsPrimaryActive = ActiveDriveControllerParams::IsPrimaryActive();
	displayData.ActiveDriveController.P = ActiveDriveControllerParams::GetP();
	displayData.ActiveDriveController.CosPhi = ActiveDriveControllerParams::GetCosPhi();
	displayData.ActiveDriveController.Q = ActiveDriveControllerParams::GetQ();
	//displayData.InsulationController.RIz = InsulationController::GetRegValue(10);
	displayData.ActiveDriveController.CosControl = ActiveDriveControllerParams::GetCosControl();
	displayData.ActiveDriveController.FlagRControl = ActiveDriveControllerParams::GetFlagRControl();
	Events::GetLastEventDescription(&displayData.Events.LastEventTime, &displayData.Events.LastEventText, &displayData.Events.LastEventParamText);
	displayData.ActiveDriveController.FlagEngineOn = ActiveDriveControllerParams::GetFlagEngineOn();
	displayData.ActiveDriveController.FlagHasProblem = ActiveDriveControllerParams::HasProblem();
	displayData.ActiveDriveController.Uptime = ActiveDriveControllerParams::GetUpTime();
	displayData.ActiveDriveController.OperatingTime = ActiveDriveControllerParams::GetOperatingTime();
}

static void OnTenKiloHertzTimerTick()
{
	//Drivers::Board::Gpio::B::SetBit(0);
	ModBusState::PacketTimeoutDetectorTick();
	Drivers::Rs485.Tick(Drivers::Board::TenKiloHertzTickFrequency);
	//Drivers::Board::Gpio::B::ClearBit(0);
}

static void OnKiloHertzTimerTick()
{
	//Drivers::Board::Gpio::B::SetBit(0);
	
	static unsigned int portScannerCnt = 0;
	static unsigned int oscGetterCnt = 0;
	static unsigned int diagnosticCnt = 0;
	
	if (portScannerCnt >= 0)
	{
		
		ModBusState::Run();
		
		/*
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
		}*/
		
		portScanner::SkipDiagnostic = diagnosticCnt > 0;
		if (diagnosticCnt < 100)
		{
			if (portScanner::RunCountDiagnostic > 0)
			{
				diagnosticCnt++;
			}
		}
		else
		{
			portScanner::RunCountDiagnostic = 0;
			diagnosticCnt = 0;
		}
		
		portScanner::Run();
		
		portScannerCnt = 0;
	}
	
	portScannerCnt++;
	
	display.Tick();
	keyboardScanner::Tick();
	ActiveDriveControllerParams::Tick();
	
	//Drivers::Board::Gpio::B::ClearBit(0);
}

char lastKey = 0;

void OnKeyDownCallback(char key)
{
	lastKey = key;
}


void GuiTimerTick()
{
	Drivers::Board::Gpio::B::SetBit(1);
	
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
	
	Drivers::Board::Gpio::B::ClearBit(1);
}

Rblib::Driver::ModbusSlave ModbusSlave;

unsigned char inputBufferRs485[256];
int _rxRs485Count = 0;
unsigned char outputBufferRs485[256];
int rs485Boudrate = 9600;
bool rs485ParityEnable = false;
bool rx485ParityEven = false;

void OnRs485DataReceived(unsigned char *data, int count)
{
	if (_rxRs485Count > 0)
	{
		return;
	}
	
	if (count > sizeof(inputBufferRs485))
	{
		count = sizeof(inputBufferRs485);
	}
	
	for (int i = 0; i < count; i++)
	{
		inputBufferRs485[i] = data[i];
	}
	
	_rxRs485Count = count;
}

void ModbusSlaveTimerTick()
{
	Drivers::Board::Gpio::B::SetBit(0);
	
	if (_rxRs485Count)
	{
		int writeCount = ModbusSlave.ProcessFrame(inputBufferRs485, _rxRs485Count, outputBufferRs485);
		if (writeCount > 0)
		{
			Drivers::Rs485.Write(outputBufferRs485, writeCount);
		}
		_rxRs485Count = 0;
	}
	
	bool paramsChanged = false;
	paramsChanged |= rs485Boudrate != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamBoudrate>();
	paramsChanged |= rs485ParityEnable != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParity>();
	paramsChanged |= rx485ParityEven != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParityType>();
	
	if (paramsChanged)
	{
		rs485Boudrate = ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamBoudrate>();
		rs485ParityEnable = ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParity>();
		rx485ParityEven = ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParityType>();
		
		Drivers::Board::Rs485Init(rs485Boudrate, rs485ParityEnable, rx485ParityEven);
	}
	
	Drivers::Board::Gpio::B::ClearBit(0);
}

void GetModbusAddress(unsigned char &address)
{
	address = ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamSlaveAddr>();
}

void GetRegisterValue(unsigned short reg, unsigned short &val)
{
	/*
	if (reg >= 200 && reg < 201 )
	{
		// �������� ��������
		// 10 - � ������� �� 200-�
		val = InsulationController::GetRegValue(reg);
	}
	else*/
	{
		// �������� ���������
		val = ActiveDriveControllerParams::GetRegValue(reg);
	}
}

void SetRegisterValue(unsigned short reg, unsigned short val, bool &result)
{
	result = false;
}

unsigned long FatStateWaitGuardTimeout = 0;

int putchar(int c)
{
  return c;
}

void OnPrimaryOscEvent(PrimaryController::OscType oscType, unsigned int pointer)
{
	OscFileFormat::OscType t;
	switch(oscType)
	{
	case PrimaryController::OscEngineStart:
		{
			t = OscFileFormat::OscTypeEngineStart;
		}
		break;
	case PrimaryController::OscEngineStop:
		{
			t = OscFileFormat::OscTypeEngineStop;
		}
		break;
	case PrimaryController::OscEngineEvent:
		{
			t = OscFileFormat::OscTypeEngineEvent;
		}
		break;
	case PrimaryController::OscChechoutStart:
		{
			t = OscFileFormat::OscTypeCheckoutStart;
		}
		break;
	case PrimaryController::OscChechoutStop:
		{
			t = OscFileFormat::OscTypeCheckoutStop;
		}
		break;
	}
	OscGet::OnOscEvent(t, pointer);
}

void OnReserveOscEvent(ReserveController::OscType oscType, unsigned int pointer)
{
	OscFileFormat::OscType t;
	switch(oscType)
	{
	case ReserveController::OscEngineStart:
		{
			t = OscFileFormat::OscTypeEngineStart;
		}
		break;
	case ReserveController::OscEngineStop:
		{
			t = OscFileFormat::OscTypeEngineStop;
		}
		break;
	case ReserveController::OscEngineEvent:
		{
			t = OscFileFormat::OscTypeEngineEvent;
		}
		break;
	case ReserveController::OscChechoutStart:
		{
			t = OscFileFormat::OscTypeCheckoutStart;
		}
		break;
	case ReserveController::OscChechoutStop:
		{
			t = OscFileFormat::OscTypeCheckoutStop;
		}
		break;
	}
	OscGet::OnOscEvent(t, pointer);
}

int main()
{
	Drivers::Init();
	
	Drivers::Board::TenKiloHertzTimer::UpdateInterruptHandler = OnTenKiloHertzTimerTick;
	Drivers::Board::KiloHertzTimer::UpdateInterruptHandler = OnKiloHertzTimerTick;
	Drivers::Board::GuiTimer::UpdateInterruptHandler = GuiTimerTick;
	Drivers::Board::ModbusSlaveTimer::UpdateInterruptHandler = ModbusSlaveTimerTick;
	
	PrimaryController::AllowOscReadCallback.Set(OscCacheType::AllowRead, &OscCache);
	PrimaryController::OnOscReadedCallback.Set(OscCacheType::StoreOscPart, &OscCache);
	PrimaryController::OnOscEventCallback = OnPrimaryOscEvent;
	PrimaryController::Init();
	
	ReserveController::AllowOscReadCallback.Set(OscCacheType::AllowRead, &OscCache);
	ReserveController::OnOscReadedCallback.Set(OscCacheType::StoreOscPart, &OscCache);
	ReserveController::OnOscEventCallback = OnReserveOscEvent;
	ReserveController::Init();
	
	MainControllerDiagnostic::TxEnableCallback = Drivers::Board::DiagnosticRs485TxEnable;
	ReservControllerDiagnostic::TxEnableCallback = Drivers::Board::DiagnosticRs485TxEnable;
	ControllerDiagnosticTemperature::TxEnableCallback = Drivers::Board::DiagnosticRs485TxEnable;
	
	display.SelectContext(&Drivers::DrawContext);
			
	display.MoveCursorTo((40 - 18) / 2, 104);
	display.WriteLine("������, ���������!", 18);
	display.MoveCursorTo((40 - 9) / 2, 120);
	display.WriteLine("��� ����?", 9);
	
	MenuInit();
	DesctopInit();
	ControllerParamsMenuEndEditorsInit();
	
	ActiveDriveControllerParams::SetActiveController(&PrimaryControllerInterface);
	ControllerSwitch::Init();
	
	keyboardScanner::Init();
	keyboardScanner::SetOnKeyDown(OnKeyDownCallback);
	
	StatorRotorIndicators.Init();
	
	ModBusState::Init();
	
	Drivers::Rs485.Init(0.0025);
	Drivers::Rs485.OnReadComplete = OnRs485DataReceived;
	
	ModbusSlave.GetSelfAddress = GetModbusAddress;
	ModbusSlave.GetRegisterValue = GetRegisterValue;
	ModbusSlave.SetRegisterValue = SetRegisterValue;
	ModbusSlave.Init();
	
	Flash.Init();
	fl_init();
	
	Events::Init(EventsRead, EventsWrite, EventsSeek);
	ActiveDriveControllerParams::Init(ControllerFilesRead, ControllerFilesWrite);
	
	OscCache.FileSystemReadyCallback = FileSystemReady;
	OscCache.CreateFileCallback = OscCacheCreateFile;
	OscCache.DeleteFileCallback = OscCacheDeleteFile;
	OscCache.WriteFileCallback = OscCacheWriteFile;
	
	OscGet::GetOscCacheFileNumber.Set(OscCacheType::GetCurrentFileNumber, &OscCache);
	OscGet::IsDataLoadedCallback.Set(OscCacheType::IsDataLoaded, &OscCache);
	OscGet::AppendOscDataCallback = CopyOscData;
	OscGet::Init(OscFilesRead, OscFilesWrite);
	
	// ������ ������
	Event e(Rblib::Rtc::GetTime(), EventProgramStart);
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
		Events::Run();
		wndEvents.DoLoPiorityWork();
		ActiveDriveControllerParams::Run();
		
		StatorRotorIndicators.Update();
		
		Flash.Run();
		switch(fatState)
		{
		case FatStateInit:
			{
				eventsFileSize = 0;

				if (Flash.Ready())
				{
					if (fl_attach_media(&media_read, media_write) != FAT_INIT_OK)
					{
						/// ���� ���������� ����� ���������������
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
						Events::GetEventsCount();
					}
				}
			}
			break;
        // �� ��������
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
		OscCache.Run();
		OscGet::Run();
		wndOscList.DoLoPiorityWork();
		wndGraph.Run();
		/*
		for (unsigned short i = 0; i < 100; i++)
		{
			if (OscGet::Run())
			{
				break;
			}
		}*/
	}
}
