////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		17/14/2016
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_DIAGNOSTICSREGULATOR_H
#define WINDOW_DIAGNOSTICSREGULATOR_H

#include "Window.h"

template<class DisplayType, DisplayType &display, class Diagnostic, bool main>
class WindowDiagnosticRegulator : public Window<DisplayType, display>
{
public:
	virtual void Draw()
	{
		char str[100];
		unsigned char len;
		
		//len = sprintf(str, "Рег. ОРВ 0x%2.2X РРВ 0x%2.2X АРВ ---- КИ  0x%2.2X ", primaryControllerAddress, reserveControllerAddress, insulationControllerAddress);
		if (main)
		{
			len = sprintf(str, "Диагностика: основной");
		}
		else
		{
			len = sprintf(str, "Диагностика: резервный");
		}
		display.MoveCursorTo(0, 0);
		display.WriteLine(str, len);
		
		unsigned short errorFlags = Diagnostic::GetRegValue(4);
		
		int x = 1;
		int y = 16 * 2;
		float val = 0;
		bool valError = false;
		static const char *formatString = "%s:     %c%4.2fВ %c";
		//len = sprintf(str, "%c| %c%4.2fВ| %s", '!', '+', 5.12, "Цифровое 1 +5В");
		val = Diagnostic::GetRegValue(7) / 100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 6);
		len = sprintf(str, formatString, "Цифровое 1   +5В", '+', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = Diagnostic::GetRegValue(8) / 100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 5);
		len = sprintf(str, formatString, "Цифровое   +3.3В", '+', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = Diagnostic::GetRegValue(9) / 100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 4);
		len = sprintf(str, formatString, "Аналоговое 1 +5В", '+', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = (signed short)Diagnostic::GetRegValue(10) / -100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 3);
		len = sprintf(str, formatString, "Аналоговое 1 -5В", '-', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = Diagnostic::GetRegValue(11) / 100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 2);
		len = sprintf(str, formatString, "Аналоговое 2 +5В", '+', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = (signed short)Diagnostic::GetRegValue(12) / -100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 1);
		len = sprintf(str, formatString, "Аналоговое 2 -5В", '-', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = Diagnostic::GetRegValue(13) / 100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 0);
		len = sprintf(str, formatString, "Цифровое 2   +5В", '+', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		unsigned short regValue = 0xFFFF;
		/*
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
			
			len = sprintf(str, "[------]");
			//len = sprintf(str, "[ %5d]", xxx::GetRegValue(i));
			display.MoveCursorTo(23, 16 * i);
			display.WriteLine(str, len);
			
			regValue = 0xFFFF;
			InsulationControllerGetRegValueCallback(i, regValue);
			
			if (i >= 1 && i <= 10)
			{
				len = sprintf(str, "[ %5d]", regValue);
			}
			else
			{
				len = sprintf(str, "[------]");
			}
			display.MoveCursorTo(32, 16 * i);
			display.WriteLine(str, len);				
		}*/
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