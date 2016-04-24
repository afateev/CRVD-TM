////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		07/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef PORTSCANNER_H
#define PORTSCANNER_H

#include "MegaLib\MegaLib.h"

template<class Primary,			// Основной регулятор
		class Reserve,			// Резервный регулятор
		class DiagnosticPrimary,// Основной регулятор диагностика
		class DiagnosticReserve,// Резервный регулятор диагностика
		class TemperatureControl// Плата контроля изолции
		>
class PortScanner
{
protected:
	// Для переключения используется порт D
	typedef Gpio::_D ControlPort;
	// DEC0
	static const unsigned char _contolPin0 = 10;
	// DEC1
	static const unsigned char _contolPin1 = 11;
	
	enum State
	{
		StateInit,
		StateScanPrimary,
		StateScanReserve,
		StateScanDiagnosticPrimary,
		StateScanDiagnosticReserve,
		//StateScanTemperatureControl,
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
			ControlPort::SetOutputPin(_contolPin0);
			// DEC1
			ControlPort::SetOutputPin(_contolPin1);
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
		case StateScanDiagnosticPrimary:
			ControlPort::SetBit(_contolPin0);
			ControlPort::SetBit(_contolPin1);
			if (DiagnosticPrimary::Run())
				Next();
			break;
		case StateScanDiagnosticReserve:
			ControlPort::SetBit(_contolPin0);
			ControlPort::SetBit(_contolPin1);
			if (DiagnosticReserve::Run())
				Next();
			break;
		/*
		case StateScanTemperatureControl:
			ControlPort::SetBit(_contolPin0);
			ControlPort::SetBit(_contolPin1);
			if (TemperatureControl::Run())
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
				_state = StateScanDiagnosticReserve;
				Next();
			}
			else
			{
				_state = StateScanDiagnosticPrimary;
			}
			break;
		case StateScanDiagnosticPrimary:
			_state = StateScanDiagnosticReserve;
			break;
		case StateScanDiagnosticReserve:
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
		case StateScanTemperatureControl:
			_state = StateScanPrimary;
			if (SkipPrimary)
				Next();
			break;*/
		};
	}
};

template<class Primary, class Reserve, class DiagnosticPrimary, class DiagnosticReserve, class TemperatureControl>
PortScanner<Primary, Reserve, DiagnosticPrimary, DiagnosticReserve, TemperatureControl>::State
PortScanner<Primary, Reserve, DiagnosticPrimary, DiagnosticReserve, TemperatureControl>::_state = 
PortScanner<Primary, Reserve, DiagnosticPrimary, DiagnosticReserve, TemperatureControl>::StateInit;

template<class Primary, class Reserve, class DiagnosticPrimary, class DiagnosticReserve, class TemperatureControl>
bool PortScanner<Primary, Reserve, DiagnosticPrimary, DiagnosticReserve, TemperatureControl>::SkipPrimary = false;

template<class Primary, class Reserve, class DiagnosticPrimary, class DiagnosticReserve, class TemperatureControl>
bool PortScanner<Primary, Reserve, DiagnosticPrimary, DiagnosticReserve, TemperatureControl>::SkipReserve = false;

template<class Primary, class Reserve, class DiagnosticPrimary, class DiagnosticReserve, class TemperatureControl>
bool PortScanner<Primary, Reserve, DiagnosticPrimary, DiagnosticReserve, TemperatureControl>::SkipDiagnostic = false;

template<class Primary, class Reserve, class DiagnosticPrimary, class DiagnosticReserve, class TemperatureControl>
unsigned char PortScanner<Primary, Reserve, DiagnosticPrimary, DiagnosticReserve, TemperatureControl>::RunCountPrimary = 0;

template<class Primary, class Reserve, class DiagnosticPrimary, class DiagnosticReserve, class TemperatureControl>
unsigned char PortScanner<Primary, Reserve, DiagnosticPrimary, DiagnosticReserve, TemperatureControl>::RunCountReserve = 0;

template<class Primary, class Reserve, class DiagnosticPrimary, class DiagnosticReserve, class TemperatureControl>
unsigned char PortScanner<Primary, Reserve, DiagnosticPrimary, DiagnosticReserve, TemperatureControl>::RunCountDiagnostic = 0;

#endif