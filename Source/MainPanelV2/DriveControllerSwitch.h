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
		// Ноги сподтяжкой к питанию
		Port::SetMode(Pin1, Port::ModeInput);
		Port::SetPullUpPullDownMode(Pin1, Port::PullUpPullDownModePullUp);
		Port::SetMode(Pin2, Port::ModeInput);
		Port::SetPullUpPullDownMode(Pin2, Port::PullUpPullDownModePullUp);
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
	
	static void IsPrimaryActive(bool &active)
	{
		active = IsPrimaryActive();
	}
	
	// Резервный выбран
	static bool IsReserveActive()
	{
		bool pin1 = Port::Read(Pin1);
		bool pin2 = Port::Read(Pin2);
		
		return !pin1 && pin2;
	}
	
	static void IsReserveActive(bool &active)
	{
		active = IsReserveActive();
	}
};

#endif 