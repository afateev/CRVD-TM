////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		16/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_DEBUG_REGISTERS_H
#define WINDOW_DEBUG_REGISTERS_H

#include "Window.h"

template<class DisplayType, DisplayType &display, class PrimaryController, class ReserveController, class InsulationController>
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
	GetAddressCallbackType InsulationControllerGetAddressCallback;
	GetRegValueCallbackType InsulationControllerGetRegValueCallback;
public:
	virtual void Draw()
	{
		char str[100];
		unsigned char len;
		
		unsigned char primaryControllerAddress = 0xFF;
		PrimaryControllerGetAddressCallback(primaryControllerAddress);
		unsigned char reserveControllerAddress = 0xFF;
		ReserveControllerGetAddressCallback(reserveControllerAddress);
		unsigned char insulationControllerAddress = 0xFF;
		InsulationControllerGetAddressCallback(insulationControllerAddress);
		
		len = sprintf(str, "Ðåã. ÎÐÂ 0x%2.2X ÐÐÂ 0x%2.2X ÀÐÂ ---- ÊÈ  0x%2.2X ", primaryControllerAddress, reserveControllerAddress, insulationControllerAddress);
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