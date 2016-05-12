
#include "../RbLib/Rblib.h"

typedef Rblib::ResetAndClockControl Rcc;
typedef Rblib::FlashMemoryController FlashMemoryController;

int main()
{
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
	
	while(1)
	{
	}
}
