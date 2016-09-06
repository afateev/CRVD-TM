
#include "../RbLib/Rblib.h"
#include "stm32f4xx_hal.h"
/* FatFs includes component */
#include "..\RBLib\STM32Cube_FW_F4\Middlewares\Third_Party\FatFs\src\ff_gen_drv.h"
#include "..\RBLib\STM32Cube_FW_F4\Middlewares\Third_Party\FatFs\src\drivers\usbh_diskio.h"

typedef Rblib::ResetAndClockControl Rcc;
typedef Rblib::FlashMemoryController FlashMemoryController;
typedef Rblib::Nvic Nvic;
typedef Rblib::Gpio Gpio;
typedef Rblib::Usb::OtgFs Usb;
typedef Rblib::InterruptMap InterruptMap;

FATFS USBDISKFatFs;           /* File system object for USB disk logical drive */
FIL MyFile;                   /* File object */
char USBDISKPath[4];          /* USB Host logical drive path */
USBH_HandleTypeDef hUSB_Host; /* USB Host handle */

//uint32_t SystemCoreClock = 168000000;

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
			filesystemready = false;
			f_mount(NULL, (TCHAR const*)"", 0);
			/*Appli_state = APPLICATION_IDLE;
			BSP_LED_Off(LED3); 
			BSP_LED_Off(LED4);      
			f_mount(NULL, (TCHAR const*)"", 0);  */
		}
		break;

	case HOST_USER_CLASS_ACTIVE:
		{
			filesystemready = f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) == FR_OK;
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
			/*Appli_state = APPLICATION_START;*/
		}
		break;

	default:
		break;
	}
}

int main()
{
	HAL_Init();
	SystemClock_Config();
	
	Rcc::HSEEnable();
	while(!Rcc::HSIReady())
	{
		;
	}
	
	// PLL от HSE
	Rcc::SetPLLSource(Rcc::PLLSourceHSE);
	
	// VCO input frequency = PLL input clock frequency / PLLM
	// It is recommended to select a frequency of 2 MHz
	// VCO input frequency = 8 / 4 = 2 MHz
	Rcc::SetPLLM(4);
	
	// VCO output frequency = VCO input frequency * PLLN with 50 <= PLLN <= 432
	// The software has to set these bits correctly to ensure that the VCO output frequency is between 100 and 432 MHz
	// VCO output frequency = 2 * 168 = 336 MHz
	Rcc::SetPLLN(168);
	
	// PLL output clock frequency = VCO frequency / PLLP
	// PLL output clock frequency = 336 / 2 = 168 MHz
	Rcc::SetPLLP(2);
	
	// USB OTG FS clock frequency = VCO frequency / PLLQ with 2 <= PLLQ <= 15
	// USB OTG FS clock frequency = 336 / 7 = 48 MHz
	Rcc::SetPLLQ(7);
	
	// ¬ключаем
	Rcc::PLLEnable();
	
	while(!Rcc::PLLReady())
	{
		;
	}
	
	// max 36 MHz
	// 168 / 8 = 21 MHz
	Rcc::SetAPB1Prescaler(Rcc::APB1PrescalerAHBDiv8);
	
	// max 72 MHz
	// 168 / 8 = 42 MHz
	Rcc::SetAPB2Prescaler(Rcc::APB2PrescalerAHBDiv4);
	
	// 5 wait states, if 150 MHz < SYSCLK <= 168 MHz
	FlashMemoryController::SetLatency(5);
	
	// ядро от PLL
	Rcc::SetSystemClock(Rcc::SystemClockPLL);
	
	SystemCoreClock = 168000000;
	
	Rcc::EnableClockPortA();
	Rcc::EnableClockPortC();
	Rcc::EnableClockOTGFS();
	
	// MCO1
	Gpio::A::SetMode(8, Gpio::A::ModeAlternate);
	Gpio::A::SetAlternateFunctionNumber(8, 0);
	Gpio::A::SetOutputSpeed(8, Gpio::A::SpeedVeryHight);
	Rcc::SetClockOutput1Prescaler(Rcc::McoPrescalerDiv4);
	Rcc::SetClockOutput1(Rcc::ClockOutput1Pll);
	
	// MCO2
	Gpio::C::SetMode(9, Gpio::C::ModeAlternate);
	Gpio::C::SetAlternateFunctionNumber(9, 0);
	Gpio::C::SetOutputSpeed(9, Gpio::C::SpeedVeryHight);
	Rcc::SetClockOutput2Prescaler(Rcc::McoPrescalerDiv4);
	Rcc::SetClockOutput2(Rcc::ClockOutput2SysClk);
	
	Nvic::InterruptEnable(Nvic::InterruptVector_OTG_FS);
	//Usb::Init();

	
	InterruptMap::SysTickHandler = HAL_IncTick;
	InterruptMap::OTG_FS_Handler = OTG_FS_IRQHandler;
	
	if(FATFS_LinkDriver(&USBH_Driver, USBDISKPath) == 0)
	{
		/*##-2- Init Host Library ################################################*/
		USBH_Init(&hUSB_Host, USBH_UserProcess, 0);
		
		 /*##-3- Add Supported Class ##############################################*/
		USBH_RegisterClass(&hUSB_Host, USBH_MSC_CLASS);
		
		/*##-4- Start Host Process ###############################################*/
		USBH_Start(&hUSB_Host);
		
		/*##-5- Run Application (Blocking mode) ##################################*/
		while (1)
		{
			/* USB Host Background task */
			USBH_Process(&hUSB_Host);

			// Mass Storage Application State Machine
			/*
			switch(Appli_state)
			{
			case APPLICATION_START:
				{
					MSC_Application();
					Appli_state = APPLICATION_IDLE;
				}
				break;

			case APPLICATION_IDLE:
			default:
				break;      
			}*/
		}
	}
	
	while(1)
	{
	}
}
