////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		07/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef PORTSCANNER_H
#define PORTSCANNER_H

//#include "MegaLib\MegaLib.h"

template<
		class Connection,		// конфигурация схемы подключения
		class Primary,			// Основной регулятор
		class Reserve,			// Резервный регулятор
		class Emergency,		// Аварийный регулятор
		class InsulationControl	// Плата контроля изолции
		>
class PortScanner
{
protected:
	// Для переключения используется порт D
	typedef typename Connection::Port ControlPort;
	// DEC0
	static const unsigned char _contolPin0 = Connection::Pin0;
	// DEC1
	static const unsigned char _contolPin1 = Connection::Pin1;
	
	enum State
	{
		StateInit,
		StateScanPrimary,
		StateScanReserve,
		StateScanEmergency,
		StateScanInsulationControl,
	};
	
	static State _state;
public:
	static bool SkipPrimary;
	static bool SkipReserve;
	static bool SkipInsulationControl;
	
	static unsigned char RunCountPrimary;
	static unsigned char RunCountReserve;
	static unsigned char RunCountInsulationControl;	
public:
	static void Run()
	{
		switch (_state)
		{
		case StateInit:
			// DEC0
			ControlPort::SetMode(_contolPin0, ControlPort::ModeOutput);
			ControlPort::SetConfig(_contolPin0, ControlPort::ConfigOutputPushPull);
			// DEC1
			ControlPort::SetMode(_contolPin1, ControlPort::ModeOutput);
			ControlPort::SetConfig(_contolPin1, ControlPort::ConfigOutputPushPull);
			Next();
			break;
		case StateScanPrimary:
			ControlPort::ClearBit(_contolPin0);
			ControlPort::ClearBit(_contolPin1);
			if (Primary::Run())
				Next();
			break;
		case StateScanReserve:
			ControlPort::SetBit(_contolPin0);
			ControlPort::ClearBit(_contolPin1);
			if (Reserve::Run())
				Next();
			break;
		case StateScanEmergency:
			ControlPort::SetBit(_contolPin0);
			ControlPort::SetBit(_contolPin1);
			if (Emergency::Run())
				Next();
			break;
		case StateScanInsulationControl:
			ControlPort::ClearBit(_contolPin0);
			ControlPort::SetBit(_contolPin1);
			if (InsulationControl::Run())
				Next();
			break;
		};
		
	}
protected:	
	static void Next()
	{
		switch (_state)
		{
		case StateInit:
			_state = StateScanPrimary;
			if (SkipPrimary)
				Next();
			break;
		case StateScanPrimary:
			if (RunCountPrimary < 255)
				RunCountPrimary++;
			_state = StateScanReserve;
			if (SkipReserve)
				Next();
			break;
		case StateScanReserve:
			if (RunCountReserve < 255)
				RunCountReserve++;
			_state = StateScanEmergency;
			break;
		case StateScanEmergency:
			_state = StateScanInsulationControl;
			if (SkipInsulationControl)
				Next();
			break;
		case StateScanInsulationControl:
			if (RunCountInsulationControl < 255)
				RunCountInsulationControl++;
			_state = StateScanPrimary;
			if (SkipPrimary)
				Next();
			break;
		};
	}
};

template<class Connection, class Primary, class Reserve, class Emergency, class InsulationControl>
PortScanner<Connection, Primary, Reserve, Emergency, InsulationControl>::State
PortScanner<Connection, Primary, Reserve, Emergency, InsulationControl>::_state = 
PortScanner<Connection, Primary, Reserve, Emergency, InsulationControl>::StateInit;

template<class Connection, class Primary, class Reserve, class Emergency, class InsulationControl>
bool PortScanner<Connection, Primary, Reserve, Emergency, InsulationControl>::SkipPrimary = false;

template<class Connection, class Primary, class Reserve, class Emergency, class InsulationControl>
bool PortScanner<Connection, Primary, Reserve, Emergency, InsulationControl>::SkipReserve = false;

template<class Connection, class Primary, class Reserve, class Emergency, class InsulationControl>
bool PortScanner<Connection, Primary, Reserve, Emergency, InsulationControl>::SkipInsulationControl = false;

template<class Connection, class Primary, class Reserve, class Emergency, class InsulationControl>
unsigned char PortScanner<Connection, Primary, Reserve, Emergency, InsulationControl>::RunCountPrimary = 0;

template<class Connection, class Primary, class Reserve, class Emergency, class InsulationControl>
unsigned char PortScanner<Connection, Primary, Reserve, Emergency, InsulationControl>::RunCountReserve = 0;

template<class Connection, class Primary, class Reserve, class Emergency, class InsulationControl>
unsigned char PortScanner<Connection, Primary, Reserve, Emergency, InsulationControl>::RunCountInsulationControl = 0;

#endif