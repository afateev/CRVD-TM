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
		class DiagnosticMain,	// Диагностика основного
		class DiagnosticReserv,	// Диагностика резервного
		class DiagnosticTemperature	// Температура
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
		StateScanDiagnosticMain,
		StateScanDiagnosticReserv,
		//StateScanDiagnosticTemperature,
	};
	
	static State _state;
public:
	static bool SkipPrimary;
	static bool SkipReserve;
	static bool SkipDiagnostic;
	
	static unsigned char RunCountPrimary;
	static unsigned char RunCountReserve;
	static unsigned char RunCountDiagnostic;	
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
		case StateScanDiagnosticMain:
			ControlPort::SetBit(_contolPin0);
			ControlPort::SetBit(_contolPin1);
			if (DiagnosticMain::Run())
				Next();
			break;
		case StateScanDiagnosticReserv:
			ControlPort::SetBit(_contolPin0);
			ControlPort::SetBit(_contolPin1);
			if (DiagnosticReserv::Run())
				Next();
			break;
		/*
		case StateScanDiagnosticTemperature:
			ControlPort::SetBit(_contolPin0);
			ControlPort::SetBit(_contolPin1);
			if (DiagnosticTemperature::Run())
				Next();
			break;*/
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
			if (SkipDiagnostic)
			{
				//_state = StateScanTemperatureControl;
				_state = StateScanDiagnosticReserv;
				Next();
			}
			else
			{
				_state = StateScanDiagnosticMain;
			}
			break;
		case StateScanDiagnosticMain:
			_state = StateScanDiagnosticReserv;
			break;
		case StateScanDiagnosticReserv:
			//_state = StateScanTemperatureControl;
			if (RunCountDiagnostic < 255)
			{
				RunCountDiagnostic++;
			}
			_state = StateScanPrimary;
			if (SkipPrimary)
				Next();
			break;
		/*
		case StateScanDiagnosticTemperature:
			_state = StateScanPrimary;
			if (SkipPrimary)
				Next();
			break;*/
		};
	}
};

template<class Connection, class Primary, class Reserve, class DiagnosticMain, class DiagnosticReserv, class DiagnosticTemperature>
PortScanner<Connection, Primary, Reserve, DiagnosticMain, DiagnosticReserv, DiagnosticTemperature>::State
PortScanner<Connection, Primary, Reserve, DiagnosticMain, DiagnosticReserv, DiagnosticTemperature>::_state = 
PortScanner<Connection, Primary, Reserve, DiagnosticMain, DiagnosticReserv, DiagnosticTemperature>::StateInit;

template<class Connection, class Primary, class Reserve, class DiagnosticMain, class DiagnosticReserv, class DiagnosticTemperature>
bool PortScanner<Connection, Primary, Reserve, DiagnosticMain, DiagnosticReserv, DiagnosticTemperature>::SkipPrimary = false;

template<class Connection, class Primary, class Reserve, class DiagnosticMain, class DiagnosticReserv, class DiagnosticTemperature>
bool PortScanner<Connection, Primary, Reserve, DiagnosticMain, DiagnosticReserv, DiagnosticTemperature>::SkipReserve = false;

template<class Connection, class Primary, class Reserve, class DiagnosticMain, class DiagnosticReserv, class DiagnosticTemperature>
bool PortScanner<Connection, Primary, Reserve, DiagnosticMain, DiagnosticReserv, DiagnosticTemperature>::SkipDiagnostic = false;

template<class Connection, class Primary, class Reserve, class DiagnosticMain, class DiagnosticReserv, class DiagnosticTemperature>
unsigned char PortScanner<Connection, Primary, Reserve, DiagnosticMain, DiagnosticReserv, DiagnosticTemperature>::RunCountPrimary = 0;

template<class Connection, class Primary, class Reserve, class DiagnosticMain, class DiagnosticReserv, class DiagnosticTemperature>
unsigned char PortScanner<Connection, Primary, Reserve, DiagnosticMain, DiagnosticReserv, DiagnosticTemperature>::RunCountReserve = 0;

template<class Connection, class Primary, class Reserve, class DiagnosticMain, class DiagnosticReserv, class DiagnosticTemperature>
unsigned char PortScanner<Connection, Primary, Reserve, DiagnosticMain, DiagnosticReserv, DiagnosticTemperature>::RunCountDiagnostic = 0;

#endif