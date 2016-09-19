////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		17/14/2016
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_DIAGNOSTICSREGULATOR_H
#define WINDOW_DIAGNOSTICSREGULATOR_H

#include "Window.h"

template<class DisplayType, DisplayType &display, bool main>
class WindowDiagnosticRegulator : public Window<DisplayType, display>
{
public:
	typedef Rblib::CallbackWrapper<int, unsigned short &> GetRegValueCallbackType;
	typedef Rblib::CallbackWrapper<bool &> GetTimeoutFlagCallbackType;
public:
	GetRegValueCallbackType GetRegValueCallback;
	GetTimeoutFlagCallbackType GetTimeoutFlagCallback;
public:
	virtual void Draw()
	{
		char str[100];
		char valStr[10];
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
		
		bool timeoutFlag = false;
		GetTimeoutFlagCallback(timeoutFlag);
		
		unsigned short errorFlags = GetRegValue(4);
		
		if (timeoutFlag)
		{
			sprintf(valStr, "---");
		}
		
		int x = 1;
		int y = 16 * 2;
		float val = 0;
		bool valError = false;
		static const char *formatString = "%s:     %s %c";
		static const char *valFormatString = "%+4.2fВ";
		//len = sprintf(str, "%c| %c%4.2fВ| %s", '!', '+', 5.12, "Цифровое 1 +5В");
		val = GetRegValue(7) / 100.0;
		valError = errorFlags & (1 << 6);
		if (!timeoutFlag)
		{
			sprintf(valStr, valFormatString, val);
		}
		len = sprintf(str, formatString, "Цифровое 1   +5В", valStr, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = GetRegValue(8) / 100.0;
		valError = errorFlags & (1 << 5);
		if (!timeoutFlag)
		{
			sprintf(valStr, valFormatString, val);
		}
		len = sprintf(str, formatString, "Цифровое   +3.3В", valStr, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = GetRegValue(9) / 100.0;
		valError = errorFlags & (1 << 4);
		if (!timeoutFlag)
		{
			sprintf(valStr, valFormatString, val);
		}
		len = sprintf(str, formatString, "Аналоговое 1 +5В", valStr, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = (signed short)GetRegValue(10) / 100.0;
		valError = errorFlags & (1 << 3);
		if (!timeoutFlag)
		{
			sprintf(valStr, valFormatString, val);
		}
		len = sprintf(str, formatString, "Аналоговое 1 -5В", valStr, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = GetRegValue(11) / 100.0;
		valError = errorFlags & (1 << 2);
		if (!timeoutFlag)
		{
			sprintf(valStr, valFormatString, val);
		}
		len = sprintf(str, formatString, "Аналоговое 2 +5В", valStr, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = (signed short)GetRegValue(12) / 100.0;
		valError = errorFlags & (1 << 1);
		if (!timeoutFlag)
		{
			sprintf(valStr, valFormatString, val);
		}
		len = sprintf(str, formatString, "Аналоговое 2 -5В", valStr, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = GetRegValue(13) / 100.0;
		valError = errorFlags & (1 << 0);
		if (!timeoutFlag)
		{
			sprintf(valStr, valFormatString, val);
		}
		len = sprintf(str, formatString, "Цифровое 2   +5В", valStr, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
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
protected:
	unsigned short GetRegValue(int reg)
	{
		unsigned short res = 0xFFFF;
		GetRegValueCallback(reg, res);
		
		return res;
	}
};

#endif