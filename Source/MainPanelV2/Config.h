////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		22/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_H
#define CONFIG_H

class Config
{
public:
	static const unsigned long CoreFrequency = 25000000;		// Частота ядра ("быстрый" внешний кварц)
	static const unsigned long MainComPortClockSourceFrequency = 36000000;	// Частота источника тактирования основного последовательного порта
	static const unsigned long Por485tClockSourceFrequency = 36000000;	// Частота источника тактирования основного последовательного порта 485-го интерфейса
	
	static float GetAdcMax()
	{
		return 32760.0;
	}
	
	static float GetUstMax()
	{
		return 10.0;
	}
	
	static float GetIstMax()
	{
		return 500.0;
	}
	
	static float GetPhiConst()
	{
		return 15.2;
	}
};

#endif