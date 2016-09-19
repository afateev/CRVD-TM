////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		16/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_MAIN_H
#define WINDOW_MAIN_H

#include "Window.h"
#include "SevenSegmentDigit.h"
#include <ctime>
#include <cstdio>

class MainWindowDisplayData
{
public:
	struct ActiveDriveControllerDataStruct
	{
		bool IsPrimaryActive;
		float P;
		float CosPhi;
		float Q;
		int CosControl;
		bool FlagRControl;
		bool FlagEngineOn;
		bool FlagHasProblem;
		unsigned long Uptime;
		unsigned long OperatingTime;
	public:
		ActiveDriveControllerDataStruct()
		{
			IsPrimaryActive = true;
			P = 0.0;
			CosPhi = 0.0;
			Q = 0;
			CosControl = 0;
			FlagRControl = false;
			FlagEngineOn = false;
			FlagHasProblem = false;
			Uptime = 0;
			OperatingTime = 0;
		}
	};
	
	struct InsulationControllerDataStruct
	{
		unsigned int RIz;
		bool Link;
	public:
		InsulationControllerDataStruct()
		{
			RIz = 0;
			Link = false;
		}
	};
	
	struct EventsDataStruct
	{
		char *LastEventText;
		char *LastEventParamText;
		time_t LastEventTime;
	public:
		EventsDataStruct()
		{
			LastEventText = 0;
			LastEventParamText = 0;
			LastEventTime = 0;
		}
	};
public:
	ActiveDriveControllerDataStruct ActiveDriveController;
	InsulationControllerDataStruct InsulationController;
	EventsDataStruct Events;
};

template<class DisplayType, DisplayType &display, class DisplayData>
class WindowMain : public Window<DisplayType, display>
{
public:
	typedef Rblib::CallbackWrapper<DisplayData &> GetDisplayDataCallbackType;
public:
	GetDisplayDataCallbackType GetDisplayDataCallback;
protected:
	SevenSegmentDigitBlock<DisplayType, display, 4> _cosFi;
	SevenSegmentDigitBlock<DisplayType, display, 4> _pAct;
	SevenSegmentDigitBlock<DisplayType, display, 4> _pReact;
	
	char str[40];
public:
	WindowMain() : 
		_cosFi(13 * 8, 16 + 1 + 32 + 16 + 8, 32, 64, 2),
		_pAct(4, 16 + 1 + 32 + 16 + 16, 16, 32, 1),
		_pReact((40 - 9) * 8 +8, 16 + 1 + 32 + 16 + 16, 16, 32, 1)
	{
	
	}
	
	virtual void Draw()
	{
		DisplayData displayData;
		GetDisplayDataCallback(displayData);
		
		display.MoveCursorTo(0, 0);
		display.WriteLine("ЦРВД-ТМ");
		
		PrintDateTime(40 - 19, 0 , Rblib::Rtc::GetTime());
		PrintActiveController(0, 16 + 8, displayData.ActiveDriveController.IsPrimaryActive ? "основной регулятор" : "резервный регулятор");
		
		PrintPact(displayData.ActiveDriveController.P, displayData.ActiveDriveController.FlagEngineOn, displayData.ActiveDriveController.FlagHasProblem);
		PrintCosFi(displayData.ActiveDriveController.CosPhi, displayData.ActiveDriveController.FlagEngineOn, displayData.ActiveDriveController.FlagHasProblem);
		PrintPreact(displayData.ActiveDriveController.Q, displayData.ActiveDriveController.FlagEngineOn, displayData.ActiveDriveController.FlagHasProblem);
		
		PrintInsulationResistance(0, 16 + 1 + 32 + 1 + 16 * 6 + 8, displayData.InsulationController.RIz, displayData.InsulationController.Link);
		
		PrintMode(20, 16 + 1 + 32 + 1 + 16 * 6 + 8, displayData.ActiveDriveController.CosControl, displayData.ActiveDriveController.FlagRControl);
		
		PrintLastEvent(0, 16 + 1 + 32 + 1 + 16 * 6 + 32 + 8, displayData.Events.LastEventText, displayData.Events.LastEventTime, displayData.Events.LastEventParamText);
		
		PrintUpTime(0, 16 + 1 + 32 + 1 + 16 * 6 + 32 + 32 + 8, displayData.ActiveDriveController.Uptime);
		PrintOperatingTime(16 + 1 + 32 + 1 + 16 * 6 + 32 + 32 + 8, displayData.ActiveDriveController.OperatingTime);
		for(int i = 0; i < 40; i++)
		{
			display.WriteByte(i, 16, 0xFF);
			display.WriteByte(i, 16 + 1 + 32, 0xFF);
			display.WriteByte(i, 16 + 1 + 32 + 1 + 16 * 6, 0xFF);
			display.WriteByte(i, 16 + 1 + 32 + 1 + 16 * 6 + 32, 0xFF);
			display.WriteByte(i, 16 + 1 + 32 + 1 + 16 * 6 + 32 + 32, 0xFF);
		}
		
		for (int y = 16 + 1 + 32 + 1; y < 16 + 1 + 32 + 1 + 6 * 16; y++)
		{
			display.WriteByte(9, y, 0x08);
			display.WriteByte(40 - 9, y, 0x08);
		}
		
		for (int y = 16 + 1 + 32 + 1 + 6 * 16 + 1; y < 16 + 1 + 32 + 1 + 8 * 16; y++)
		{
			display.WriteByte(20, y, 0x08);
		}
	}
	
	void PrintDateTime(unsigned int x, unsigned int y, time_t dt)
	{
		struct tm * timeinfo;
		timeinfo = localtime ( &dt );
		strftime (str, sizeof(str), "%d.%m.%Y %H:%M:%S", timeinfo);
		display.MoveCursorTo(x, y);
		display.WriteLine(str);
	}
	
	void PrintDateTimeShort(unsigned int x, unsigned int y, time_t dt)
	{
		struct tm * timeinfo;
		timeinfo = localtime ( &dt );
		strftime (str, sizeof(str), "%d.%m.%Y %H:%M", timeinfo);
		display.MoveCursorTo(x, y);
		display.WriteLine(str);
	}
	
	void PrintActiveController(unsigned int x, unsigned int y, char *controllerName)
	{
		if (0 == controllerName)
			return;
		unsigned char len;
		
		len = sprintf(str, "В работе %s", controllerName);
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
	}
	
	void PrintPact(float p, bool flagEngineOn, bool flagHasProblem)
	{
		unsigned char len;
		len = sprintf(str, "P акт.");
		display.MoveCursorTo(2,  16 + 1 + 32 + 4);
		display.WriteLine(str, len);
		if (flagEngineOn && !flagHasProblem)
        {
		    len = sprintf(str, "%5.2f", p);
        }
        else
        {
            len = sprintf(str, "----");
        }
		_pAct.Draw(str, len);
		
		len = sprintf(str, "МВт");
		display.MoveCursorTo(3,  16 + 1 + 32 + 1 + 16 * 6 - 16 - 4);
		display.WriteLine(str, len);
	}
	
	void PrintPreact(float p, bool flagEngineOn, bool flagHasProblem)
	{
		unsigned char len;
		
		len = sprintf(str, "Q реакт.");
		display.MoveCursorTo(40 - 9 + 1,  16 + 1 + 32 + 4);
		display.WriteLine(str, len);
		
        if (flagEngineOn && !flagHasProblem)
        {
		    len = sprintf(str, "%5.2f", p);
        }
        else
        {
            len = sprintf(str, "----");
        }
		_pReact.Draw(str, len);
		
		len = sprintf(str, "МВАр");
		display.MoveCursorTo(40 - 9 + 3,  16 + 1 + 32 + 1 + 16 * 6 - 16 - 4);
		display.WriteLine(str, len);
	}
	
	void PrintCosFi(float cosFi, bool flagEngineOn, bool flagHasProblem)
	{
		unsigned char len;
		
		len = sprintf(str, " Cos Ф");
		display.MoveCursorTo((40 - len) / 2,  16 + 1 + 32 + 4);
		display.WriteLine(str, len);
		
        if (flagEngineOn && !flagHasProblem)
        {
		    len = sprintf(str, "%5.2f", cosFi);
        }
        else
        {
            len = sprintf(str, "----");
        }
		_cosFi.Draw(str, len);
	}
	
	void PrintInsulationResistance(unsigned int x, unsigned int y, unsigned int r, bool link)
	{
		unsigned char len;
		if (link)
		{
			if (r >= 0xFFFF)
			{
				// Знак бесконечности - символ с кодом 0x01
				len = sprintf(str, "R              %c", 1);
			}
			else
			{
				len = sprintf(str, "R         %5d kОм", r);
			}
		}
		else
		{
			len = sprintf(str, "R             ---");
		}
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		
		len = sprintf(str, "изоляции");
		display.MoveCursorTo(x + 1, y + 3);
		display.WriteLine(str, len);
	}
	
	void PrintMode(unsigned int x, unsigned int y, int mode, bool rFlag)
	{
		unsigned char len;
		
		len = sprintf(str, "  Канал - ток ротора");
		
		if (rFlag)
		{
			len = sprintf(str, "  Ручное управление");
		}
		else
		{
			switch (mode)
			{
			case 1:
				{
					len = sprintf(str, "  Канал - ток ротора");
				}
				break;
			case 2:
				{
					len = sprintf(str, "     Канал - cosФ");
				}
				break;
			case 3:
				{
					len = sprintf(str, "    Канал - Iреакт");
				}
				break;
			case 4:
				{
					len = sprintf(str, "    Канал - Qреакт");
				}
				break;
			}
		}
		
		display.MoveCursorTo(x, y + 3);
		display.WriteLine(str, len);
	}
	
	void PrintLastEvent(unsigned int x, unsigned int y, char *eventDesc, time_t dt, char *eventParam)
	{
		if (0 == eventDesc)
			return;
		if (eventParam)
			y -= 8;
		
		display.MoveCursorTo(x, y);
		display.WriteLine(eventDesc);
		if (eventParam)
		{	
			display.MoveCursorTo(x, y + 16);
			display.WriteLine(eventParam);
		}	
		// непонятное время не будем выводить
		if (dt > 0)
			PrintDateTimeShort(40 - 16, y, dt);
	}
	
	void PrintUpTime(unsigned int x, unsigned int y, unsigned long uptime)
	{
		unsigned char len;
		unsigned long seconds = uptime;
		unsigned long minutes = seconds / 60;
		unsigned long hours = minutes / 60;
		if (hours > 0)
			len = sprintf(str, "В работе %ldч", hours);
		else
		{
			if (minutes)
				len = sprintf(str, "В работе %ldмин", minutes);
			else
			{
				len = sprintf(str, "В работе %ld сек", seconds);
			}
		}
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
	}
	
	void PrintOperatingTime(unsigned int y, unsigned long operatingTime)
	{
		unsigned char len;
		
		unsigned long seconds = operatingTime;
		unsigned long minutes = seconds / 60;
		unsigned long hours = minutes / 60;
		if (hours > 0)
			len = sprintf(str, "Наработка %ldч", hours);
		else
		{
			if (minutes)
				len = sprintf(str, "Наработка %ldмин", minutes);
			else
			{
				len = sprintf(str, "Наработка %ld сек", seconds);
			}
		}
		display.MoveCursorTo(40 - len, y);
		display.WriteLine(str, len);
	}
};

#endif