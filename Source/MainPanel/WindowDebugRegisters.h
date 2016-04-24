////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		16/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_DEBUG_REGISTERS_H
#define WINDOW_DEBUG_REGISTERS_H

#include "Window.h"

template<class DisplayType, DisplayType &display, class PrimaryController, class ReserveController, class PrimaryDiagnostic, class ReserveDiagnostic>
class WindowDebugRegisters : public Window<DisplayType, display>
{
public:
	virtual void Draw()
	{
		char str[100];
		unsigned char len;
		
		len = sprintf(str, "Ðåã. ÎÐÂ 0x%2.2X ÐÐÂ 0x%2.2X ÄÎ  0x%2.2X ÄÐ  0x%2.2X ", PrimaryController::GetAddress(), ReserveController::GetAddress(), PrimaryDiagnostic::GetAddress(), ReserveDiagnostic::GetAddress());
		display.MoveCursorTo(0, 0);
		display.WriteLine(str, len);
		
		for (int i = 1; i <= 14; i++)
		{
			len = sprintf(str, " %2.2d", i);
			display.MoveCursorTo(0, 16 * i);
			display.WriteLine(str, len);
			
			if (i == 6 | i == 8)
			{
				len = sprintf(str, "[0x%4.4X]", PrimaryController::GetRegValue(i));
			}
			else
			{
				len = sprintf(str, "[ %5d]", PrimaryController::GetRegValue(i));
			}
			display.MoveCursorTo(5, 16 * i);
			display.WriteLine(str, len);
			
			if (i == 6 | i == 8)
			{
				len = sprintf(str, "[0x%4.4X]", ReserveController::GetRegValue(i));
			}
			else
			{
				len = sprintf(str, "[ %5d]", ReserveController::GetRegValue(i));
			}
			display.MoveCursorTo(14, 16 * i);
			display.WriteLine(str, len);
			
			len = sprintf(str, "[ %5d]", PrimaryDiagnostic::GetRegValue(i));
			display.MoveCursorTo(23, 16 * i);
			display.WriteLine(str, len);
			
			len = sprintf(str, "[ %5d]", ReserveDiagnostic::GetRegValue(i));
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