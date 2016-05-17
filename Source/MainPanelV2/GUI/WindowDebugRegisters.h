////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		16/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_DEBUG_REGISTERS_H
#define WINDOW_DEBUG_REGISTERS_H

#include "Window.h"

template<class DisplayType, DisplayType &display>
class WindowDebugRegisters : public Window<DisplayType, display>
{
public:
	typedef Rblib::CallbackWrapper<unsigned char &> GetAddressCallbackType;
	typedef Rblib::CallbackWrapper<int, unsigned short &> GetRegValueCallbackType;
public:
	GetAddressCallbackType PrimaryControllerGetAddressCallback;
	GetRegValueCallbackType PrimaryControllerGetRegValueCallback;
	GetAddressCallbackType ReserveControllerGetAddressCallback;
	GetRegValueCallbackType ReserveControllerGetRegValueCallback;
	GetAddressCallbackType PrimaryDiagnosticGetAddressCallback;
	GetRegValueCallbackType PrimaryDiagnosticGetRegValueCallback;
	GetAddressCallbackType ReserveDiagnosticGetAddressCallback;
	GetRegValueCallbackType ReserveDiagnosticGetRegValueCallback;
public:
	virtual void Draw()
	{
		char str[100];
		unsigned char len;
		
		unsigned char primaryControllerAddress = 0xFF;
		PrimaryControllerGetAddressCallback(primaryControllerAddress);
		unsigned char reserveControllerAddress = 0xFF;
		ReserveControllerGetAddressCallback(reserveControllerAddress);
		unsigned char primaryDiagnosticControllerAddress = 0xFF;
		PrimaryDiagnosticGetAddressCallback(primaryDiagnosticControllerAddress);
		unsigned char reserveDiagnosticControllerAddress = 0xFF;
		ReserveDiagnosticGetAddressCallback(reserveDiagnosticControllerAddress);
		
		len = sprintf(str, "Ðåã. ÎÐÂ 0x%2.2X ÐÐÂ 0x%2.2X ÄÎ  0x%2.2X ÄÐ  0x%2.2X ", primaryControllerAddress, reserveControllerAddress, primaryDiagnosticControllerAddress, reserveDiagnosticControllerAddress);
		display.MoveCursorTo(0, 0);
		display.WriteLine(str, len);
		
		unsigned short regValue = 0xFFFF;
		
		for (int i = 1; i <= 14; i++)
		{
			len = sprintf(str, " %2.2d", i);
			display.MoveCursorTo(0, 16 * i);
			display.WriteLine(str, len);
			
			regValue = 0xFFFF;
			PrimaryControllerGetRegValueCallback(i, regValue);
			
			if (i == 6 | i == 8)
			{
				len = sprintf(str, "[0x%4.4X]", regValue);
			}
			else
			{
				len = sprintf(str, "[ %5d]", regValue);
			}
			display.MoveCursorTo(5, 16 * i);
			display.WriteLine(str, len);
			
			regValue = 0xFFFF;
			ReserveControllerGetRegValueCallback(i, regValue);
			
			if (i == 6 | i == 8)
			{
				len = sprintf(str, "[0x%4.4X]", regValue);
			}
			else
			{
				len = sprintf(str, "[ %5d]", regValue);
			}
			display.MoveCursorTo(14, 16 * i);
			display.WriteLine(str, len);
			
			regValue = 0xFFFF;
			PrimaryDiagnosticGetRegValueCallback(i, regValue);
			len = sprintf(str, "[ %5d]", regValue);
			display.MoveCursorTo(23, 16 * i);
			display.WriteLine(str, len);
			
			regValue = 0xFFFF;
			ReserveDiagnosticGetRegValueCallback(i, regValue);
			len = sprintf(str, "[ %5d]", regValue);
			display.MoveCursorTo(32, 16 * i);
			display.WriteLine(str, len);				
		}
	}
	
	virtual bool OnKeyDown(char &key)
	{
		if ('E' == key)
		{
			WindowBase::Close();
			return true;
		}
		return false;
	}
};

#endif