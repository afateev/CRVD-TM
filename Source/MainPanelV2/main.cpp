
#include "../RbLib/Rblib.h"

#ifdef SD_STORAGE
#include "..\MainPanel\fat_filelib\fat_filelib.h"
#include "..\MainPanel\fat_filelib\fat_format.h"
#endif

#include "Board.h"
#include "Drivers.h"
#include "GUI/GUI.h"
#include "Controllers.h"
#include "KeyboardScanner.h"
#include "SideIndicators.h"

#ifdef SD_STORAGE
#include "Files.h"
#endif

#ifdef USB_STORAGE
#include "FilesUsb.h"
#endif

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
	displayData.InsulationController.RIz = InsulationController::GetRegValue(10);
	displayData.InsulationController.Link = !InsulationController::NoResponse();
	displayData.ActiveDriveController.CosControl = ActiveDriveControllerParams::GetCosControl();
	displayData.ActiveDriveController.FlagRControl = ActiveDriveControllerParams::GetFlagRControl();
	Events::GetLastEventDescription(&displayData.Events.LastEventTime, &displayData.Events.LastEventText, &displayData.Events.LastEventParamText);
	displayData.ActiveDriveController.FlagEngineOn = ActiveDriveControllerParams::GetFlagEngineOn();
	displayData.ActiveDriveController.FlagHasProblem = ActiveDriveControllerParams::HasProblem();
	displayData.ActiveDriveController.Uptime = ActiveDriveControllerParams::GetUpTime();
	displayData.ActiveDriveController.OperatingTime = ActiveDriveControllerParams::GetOperatingTime();
}

void GetDebugOscDisplayData(WindowDebugOsc<Display, display>::DisplayData &data)
{
	data.CacheFileNumber = OscCache.GetCurrentFileNumber();
	data.CurrentOscPos = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegOscCurPos);
	data.LoadedOscPos = OscCache.GetRequestPos();
	
	for (int i = 0; i < WindowDebugOsc<Display, display>::DisplayData::EventCount; i++)
	{
		OscGet::GetEventInfo(i, data.EventData[i].File, data.EventData[i].Pointer);
	}
}

void UsbRun();

static void OnTenKiloHertzTimerTick()
{
	//Drivers::Board::Gpio::B::SetBit(0);
	ModBusState::PacketTimeoutDetectorTick();
	Drivers::Rs485.Tick(Drivers::Board::TenKiloHertzTickFrequency);
	Drivers::Board::UsbConnection::Tick(10000);
	//Drivers::Board::Gpio::B::ClearBit(0);
}

static void OnKiloHertzTimerTick()
{
	//Drivers::Board::Gpio::B::SetBit(0);
	
	static unsigned int portScannerCnt = 0;
	static unsigned int oscGetterCnt = 0;
	static unsigned int diagnosticCnt = 0;
	
	/*
	if (portScannerCnt >= 0)
	{
		
		ModBusState::Run();
		
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
	*/
	
	PrimaryController::Run();
	ReserveController::Run();
	MainControllerDiagnostic::Run();
	ReservControllerDiagnostic::Run();
	InsulationController::Run();
	ModBusState::Run();
	
	display.Tick();
	keyboardScanner::Tick();
	ActiveDriveControllerParams::Tick();
#ifdef USB_STORAGE
	UsbRun();
#endif
#ifndef SD_STORAGE
	static int statorRototIndecatorsUpdateCounter = 0;
	if (statorRototIndecatorsUpdateCounter < 100)
	{
		statorRototIndecatorsUpdateCounter++;
	}
	else
	{
		statorRototIndecatorsUpdateCounter = 0;
		StatorRotorIndicators.Update();
	}
#endif
	
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
	if (reg >= 200 && reg < 201 )
	{
		// контроль изоляции
		// 10 - й мапится на 200-й
		val = InsulationController::GetRegValue(reg);
	}
	else
	{
		// активный регулятор
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

#ifdef USB_STORAGE
#include "..\RBLib\STM32Cube_FW_F4\Middlewares\Third_Party\FatFs\src\ff_gen_drv.h"
#include "..\RBLib\STM32Cube_FW_F4\Middlewares\Third_Party\FatFs\src\drivers\usbh_diskio.h"

FATFS USBDISKFatFs;           /* File system object for USB disk logical drive */
FIL MyFile;                   /* File object */
char USBDISKPath[4];          /* USB Host logical drive path */
USBH_HandleTypeDef hUSB_Host; /* USB Host handle */

bool filesystemready = false;

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  // Enable HSE Oscillator and activate PLL with HSE as source
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig (&RCC_OscInitStruct);
  
  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

extern HCD_HandleTypeDef hhcd;
void OTG_FS_IRQHandler(void)
{
  HAL_HCD_IRQHandler(&hhcd);
}

char testString[256];

static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{  
	switch(id)
	{ 
	case HOST_USER_SELECT_CONFIGURATION:
		break;

	case HOST_USER_DISCONNECTION:
		{
			fatState = FatStateDisconnectionDetected;
			/*Appli_state = APPLICATION_IDLE;
			BSP_LED_Off(LED3); 
			BSP_LED_Off(LED4);      
			f_mount(NULL, (TCHAR const*)"", 0);  */
		}
		break;

	case HOST_USER_CLASS_ACTIVE:
		{
			fatState = FatStateConnectionDetected;
			/*Appli_state = APPLICATION_START;*/
		}
		break;

	default:
		break;
	}
}

void UsbRun()
{
	// USB Host Background task
	USBH_Process(&hUSB_Host);
}
#endif

void OnOscFileAdded()
{
	wndOscList.OnFileAdded();
}

int main()
{
#ifdef USB_STORAGE
	HAL_Init();
	SystemClock_Config();
#endif
	Drivers::Init();

#ifdef USB_STORAGE	
	Drivers::Board::InterruptMap::SysTickHandler = HAL_IncTick;
	Drivers::Board::InterruptMap::OTG_FS_Handler = OTG_FS_IRQHandler;

	if(FATFS_LinkDriver(&USBH_Driver, USBDISKPath) == 0)
	{
		/*##-2- Init Host Library ################################################*/
		USBH_Init(&hUSB_Host, USBH_UserProcess, 0);
		
		 /*##-3- Add Supported Class ##############################################*/
		USBH_RegisterClass(&hUSB_Host, USBH_MSC_CLASS);
		
		/*##-4- Start Host Process ###############################################*/
		USBH_Start(&hUSB_Host);
		
		fatState = FatStateDisconnected;
	}
#endif
	Drivers::Board::TenKiloHertzTimer::UpdateInterruptHandler = OnTenKiloHertzTimerTick;
	Drivers::Board::KiloHertzTimer::UpdateInterruptHandler = OnKiloHertzTimerTick;
	Drivers::Board::GuiTimer::UpdateInterruptHandler = GuiTimerTick;
	Drivers::Board::ModbusSlaveTimer::UpdateInterruptHandler = ModbusSlaveTimerTick;
	
	PrimaryController::ModbusSelectCallback = Drivers::Board::PortScanerConnection::Select<1>;
	PrimaryController::AllowOscReadCallback.Set(OscCacheType::AllowRead, &OscCache);
	PrimaryController::GetActiveStateCallback = ControllerSwitch::IsPrimaryActive;
	PrimaryController::OnOscReadedCallback.Set(OscCacheType::StoreOscPart, &OscCache);
	PrimaryController::OnOscEventCallback = OnPrimaryOscEvent;
	PrimaryController::GetOscPointerSyncValueCallback = ReserveController::GetOscPointerSyncValue;
	PrimaryController::OscPosUpdatedCallback.Set(OscCacheType::OscPosUpdated, &OscCache);
	PrimaryController::Init();
	
	ReserveController::ModbusSelectCallback = Drivers::Board::PortScanerConnection::Select<2>;
	ReserveController::AllowOscReadCallback.Set(OscCacheType::AllowRead, &OscCache);
	ReserveController::GetActiveStateCallback = ControllerSwitch::IsReserveActive;
	ReserveController::OnOscReadedCallback.Set(OscCacheType::StoreOscPart, &OscCache);
	ReserveController::OnOscEventCallback = OnReserveOscEvent;
	ReserveController::GetOscPointerSyncValueCallback = PrimaryController::GetOscPointerSyncValue;
	ReserveController::OscPosUpdatedCallback.Set(OscCacheType::OscPosUpdated, &OscCache);
	ReserveController::Init();
	
	InsulationController::ModbusSelectCallback = Drivers::Board::PortScanerConnection::Select<3>;
	
	MainControllerDiagnostic::TxEnableCallback = Drivers::Board::DiagnosticRs485TxEnable;
	MainControllerDiagnostic::ModbusSelectCallback = Drivers::Board::PortScanerConnection::Select<4>;
	ReservControllerDiagnostic::TxEnableCallback = Drivers::Board::DiagnosticRs485TxEnable;
	ReservControllerDiagnostic::ModbusSelectCallback = Drivers::Board::PortScanerConnection::Select<4>;
	ControllerDiagnosticTemperature::TxEnableCallback = Drivers::Board::DiagnosticRs485TxEnable;
	
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
	
	Drivers::Rs485.Init(0.0025);
	Drivers::Rs485.OnReadComplete = OnRs485DataReceived;
	
	ModbusSlave.GetSelfAddress = GetModbusAddress;
	ModbusSlave.GetRegisterValue = GetRegisterValue;
	ModbusSlave.SetRegisterValue = SetRegisterValue;
	ModbusSlave.Init();

#ifdef SD_STORAGE
	Flash.Init();
	fl_init();
#endif
	
	Events::Init(EventsRead, EventsWrite, EventsSeek);
	ActiveDriveControllerParams::Init(ControllerFilesRead, ControllerFilesWrite);
	
	OscCache.FileSystemReadyCallback = FileSystemReady;
	OscCache.CreateFileCallback = OscCacheCreateFile;
	OscCache.DeleteFileCallback = OscCacheDeleteFile;
	OscCache.WriteFileCallback = OscCacheWriteFile;
	OscCache.SendRequestCallback = SendOscReadRequest;
	OscCache.AllowOscSkipCallback = OscGet::OscEventPending;
	OscCache.GetOscPendingCallback = GetOscPending;
	
	OscGet::GetOscCacheFileNumber.Set(OscCacheType::GetCurrentFileNumber, &OscCache);
	OscGet::IsDataLoadedCallback.Set(OscCacheType::IsDataLoaded, &OscCache);
	OscGet::AppendOscDataCallback = CopyOscData;
	OscGet::OnOscFileAddedCallback = OnOscFileAdded;
	OscGet::Init(OscFilesRead, OscFilesWrite);
	
	// начали работу
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

#ifdef SD_STORAGE
		StatorRotorIndicators.Update();
#endif

#ifdef SD_STORAGE
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
						Events::GetEventsCount();
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
#endif
#ifdef USB_STORAGE
		switch(fatState)
		{
		case FatStateInit:
			{
			}
			break;
		case FatStateConnectionDetected:
			{
				if (f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) == FR_OK)
				{
					fatState = FatStateConnected;
				}
				else
				{
					fatState = FatStateError;
				}
			}
			break;
		case FatStateConnected:
			{
				fatState = FatStateReady;
			}
			break;
		case FatStateDisconnectionDetected:
			{
				f_mount(NULL, (TCHAR const*)"", 0);
				fatState = FatStateDisconnected;
			}
			break;
		}
#endif
		OscCache.Run();
		OscGet::Run();
		wndOscList.DoLoPiorityWork();
		wndGraph.Run();
		/*
		if (filesystemready)
		{
			bool res = true;
			res &= f_open(&MyFile, "STM32.TXT", FA_READ) == FR_OK;
			if (res)
			{
				unsigned int bytesread = 0;
				res &= f_read(&MyFile, testString, sizeof(testString), &bytesread);
				f_close(&MyFile);
			}
		}
		*/
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
