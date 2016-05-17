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
public:
	GetRegValueCallbackType GetRegValueCallback;
public:
	virtual void Draw()
	{
		char str[100];
		unsigned char len;
		
		//len = sprintf(str, "���. ��� 0x%2.2X ��� 0x%2.2X ��� ---- ��  0x%2.2X ", primaryControllerAddress, reserveControllerAddress, insulationControllerAddress);
		if (main)
		{
			len = sprintf(str, "�����������: ��������");
		}
		else
		{
			len = sprintf(str, "�����������: ���������");
		}
		display.MoveCursorTo(0, 0);
		display.WriteLine(str, len);
		
		unsigned short errorFlags = GetRegValue(4);
		
		int x = 1;
		int y = 16 * 2;
		float val = 0;
		bool valError = false;
		static const char *formatString = "%s:     %c%4.2f� %c";
		//len = sprintf(str, "%c| %c%4.2f�| %s", '!', '+', 5.12, "�������� 1 +5�");
		val = GetRegValue(7) / 100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 6);
		len = sprintf(str, formatString, "�������� 1   +5�", '+', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = GetRegValue(8) / 100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 5);
		len = sprintf(str, formatString, "��������   +3.3�", '+', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = GetRegValue(9) / 100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 4);
		len = sprintf(str, formatString, "���������� 1 +5�", '+', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = (signed short)GetRegValue(10) / -100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 3);
		len = sprintf(str, formatString, "���������� 1 -5�", '-', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = GetRegValue(11) / 100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 2);
		len = sprintf(str, formatString, "���������� 2 +5�", '+', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = (signed short)GetRegValue(12) / -100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 1);
		len = sprintf(str, formatString, "���������� 2 -5�", '-', val, valError ? '!' : ' ');
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		y += 16;
		
		val = GetRegValue(13) / 100.0;
		if (val < 0) val = -val;
		valError = errorFlags & (1 << 0);
		len = sprintf(str, formatString, "�������� 2   +5�", '+', val, valError ? '!' : ' ');
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