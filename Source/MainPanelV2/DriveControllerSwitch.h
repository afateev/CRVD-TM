////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		27/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef DRIVECONTROLLERSWITCH_H
#define DRIVECONTROLLERSWITCH_H

template <class Port, unsigned char Pin1, unsigned char Pin2>
class DriveControllerSwitch
{
public:
	static void Init()
	{
		// Ноги споддтяжкой
		Port::SetInputWithPullUpDownPin(Pin1);
		Port::SetInputWithPullUpDownPin(Pin2);
		// к питанию
		Port::SetBit(Pin1);
		Port::SetBit(Pin2);
	}
	
	/*	Ниже функции проверки какой регулятор выбран
		Могут обе вернуть false, значит непонятно какой выбран, наверное Atmega не работает
	*/
	
	// Основной выбран
	static bool IsPrimaryActive()
	{
		bool pin1 = Port::Read(Pin1);
		bool pin2 = Port::Read(Pin2);
		
		return pin1 && !pin2;
	}
	
	// Резервный выбран
	static bool IsReserveActive()
	{
		bool pin1 = Port::Read(Pin1);
		bool pin2 = Port::Read(Pin2);
		
		return !pin1 && pin2;
	}
};

#endif 