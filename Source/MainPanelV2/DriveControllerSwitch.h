////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		27/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef DRIVECONTROLLERSWITCH_H
#define DRIVECONTROLLERSWITCH_H

template <class Connection>
class DriveControllerSwitch
{
public:
	typedef typename Connection::Port Port;
	static const unsigned char Pin1 = Connection::Pin1;
	static const unsigned char Pin2 = Connection::Pin2;
public:
	static void Init()
	{
		// Ноги споддтяжкой
		Port::SetMode(Pin1, Port::ModeInput);
		Port::SetConfig(Pin1, Port::ConfigInputPull);
		Port::SetMode(Pin2, Port::ModeInput);
		Port::SetConfig(Pin2, Port::ConfigInputPull);
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