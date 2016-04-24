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

template<class DisplayType, DisplayType &display, class ActiveDriveControllerParams/*, class InsulationController*/, class Events>
class WindowMain : public Window<DisplayType, display>
{
protected:
	SevenSegmentDigitBlock<DisplayType, display, 4> _cosFi;
	SevenSegmentDigitBlock<DisplayType, display, 4> _pAct;
	SevenSegmentDigitBlock<DisplayType, display, 4> _pReact;
public:
	WindowMain() : 
		_cosFi(13 * 8, 16 + 1 + 32 + 16 + 8, 32, 64, 2),
		_pAct(4, 16 + 1 + 32 + 16 + 16, 16, 32, 1),
		_pReact((40 - 9) * 8 +8, 16 + 1 + 32 + 16 + 16, 16, 32, 1)
	{
	
	}
	
	virtual void Draw()
	{
		display.MoveCursorTo(0, 0);
		display.WriteLine("����-�");
		
		PrintDateTime(40 - 19, 0 , Rtc::GetTime());
		PrintActiveController(0, 16 + 8, ActiveDriveControllerParams::IsPrimaryActive() ? "�������� ���������" : "��������� ���������");
		
		float pAct = ActiveDriveControllerParams::GetP();
		PrintPact(pAct);
		
		float cosFi = ActiveDriveControllerParams::GetCosPhi();
		PrintCosFi(cosFi);
		
		float pReact = ActiveDriveControllerParams::GetQ();
		PrintPreact(pReact);
		
		//unsigned int rIz = InsulationController::GetRegValue(10);
		//PrintInsulationResistance(0, 16 + 1 + 32 + 1 + 16 * 6 + 8, rIz);
		bool flagRControl = ActiveDriveControllerParams::GetFlagRControl();
		PrintMode(20, 16 + 1 + 32 + 1 + 16 * 6 + 8, ActiveDriveControllerParams::GetCosControl(), flagRControl);
		
		char *lastEventText = 0;
		char *lastEventParamText = 0;
		time_t lastEventTime = 0;
		Events::GetLastEventDescription(&lastEventTime, &lastEventText, &lastEventParamText);
		PrintLastEvent(0, 16 + 1 + 32 + 1 + 16 * 6 + 32 + 8, lastEventText, lastEventTime, lastEventParamText);
		
		PrintUpTime(0, 16 + 1 + 32 + 1 + 16 * 6 + 32 + 32 + 8);
		PrintOperatingTime(16 + 1 + 32 + 1 + 16 * 6 + 32 + 32 + 8);
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
		char str[20];
		timeinfo = localtime ( &dt );
		strftime (str, sizeof(str), "%d.%m.%Y %H:%M:%S", timeinfo);
		display.MoveCursorTo(x, y);
		display.WriteLine(str);
	}
	
	void PrintDateTimeShort(unsigned int x, unsigned int y, time_t dt)
	{
		struct tm * timeinfo;
		char str[20];
		timeinfo = localtime ( &dt );
		strftime (str, sizeof(str), "%d.%m.%Y %H:%M", timeinfo);
		display.MoveCursorTo(x, y);
		display.WriteLine(str);
	}
	
	void PrintActiveController(unsigned int x, unsigned int y, char *controllerName)
	{
		if (0 == controllerName)
			return;
		char str[40];
		unsigned char len;
		
		len = sprintf(str, "� ������ %s", controllerName);
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
	}
	
	void PrintPact(float p)
	{
		char str[40];
		unsigned char len;
		
		len = sprintf(str, "P ���.");
		display.MoveCursorTo(2,  16 + 1 + 32 + 4);
		display.WriteLine(str, len);
		if (ActiveDriveControllerParams::GetFlagEngineOn()
            && !ActiveDriveControllerParams::HasProblem())
        {
		    len = sprintf(str, "%5.2f", p);
        }
        else
        {
            len = sprintf(str, "----");
        }
		_pAct.Draw(str, len);
		
		len = sprintf(str, "���");
		display.MoveCursorTo(3,  16 + 1 + 32 + 1 + 16 * 6 - 16 - 4);
		display.WriteLine(str, len);
	}
	
	void PrintPreact(float p)
	{
		char str[40];
		unsigned char len;
		
		len = sprintf(str, "Q �����.");
		display.MoveCursorTo(40 - 9 + 1,  16 + 1 + 32 + 4);
		display.WriteLine(str, len);
		
        if (ActiveDriveControllerParams::GetFlagEngineOn()
            && !ActiveDriveControllerParams::HasProblem())
        {
		    len = sprintf(str, "%5.2f", p);
        }
        else
        {
            len = sprintf(str, "----");
        }
		_pReact.Draw(str, len);
		
		len = sprintf(str, "����");
		display.MoveCursorTo(40 - 9 + 3,  16 + 1 + 32 + 1 + 16 * 6 - 16 - 4);
		display.WriteLine(str, len);
	}
	
	void PrintCosFi(float cosFi)
	{
		char str[40];
		unsigned char len;
		
		len = sprintf(str, " Cos �");
		display.MoveCursorTo((40 - len) / 2,  16 + 1 + 32 + 4);
		display.WriteLine(str, len);
		
        if (ActiveDriveControllerParams::GetFlagEngineOn()
            && !ActiveDriveControllerParams::HasProblem())
        {
		    len = sprintf(str, "%5.2f", cosFi);
        }
        else
        {
            len = sprintf(str, "----");
        }
		_cosFi.Draw(str, len);
	}
	
	void PrintInsulationResistance(unsigned int x, unsigned int y, unsigned int r)
	{
		char str[40];
		unsigned char len;
		if (r >= 0xFFFF)
		{
			// ���� ������������� - ������ � ����� 0x01
			len = sprintf(str, "R              %c", 1);
		}
		else
		{
			len = sprintf(str, "R         %5d k��", r);
		}
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		
		len = sprintf(str, "��������");
		display.MoveCursorTo(x + 1, y + 3);
		display.WriteLine(str, len);
	}
	
	void PrintMode(unsigned int x, unsigned int y, int mode, bool rFlag)
	{
		char str[40];
		unsigned char len;
		
		len = sprintf(str, "  ����� - ��� ������");
		
		if (rFlag)
		{
			len = sprintf(str, "  ������ ����������");
		}
		else
		{
			switch (ActiveDriveControllerParams::GetCosControl())
			{
			case 1:
				{
					len = sprintf(str, "  ����� - ��� ������");
				}
				break;
			case 2:
				{
					len = sprintf(str, "     ����� - cos�");
				}
				break;
			case 3:
				{
					len = sprintf(str, "    ����� - I�����");
				}
				break;
			case 4:
				{
					len = sprintf(str, "    ����� - Q�����");
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
		display.MoveCursorTo(x, y + 16);
		display.WriteLine(eventParam);
		// ���������� ����� �� ����� ��������
		if (dt > 0)
			PrintDateTimeShort(40 - 16, y, dt);
	}
	
	void PrintUpTime(unsigned int x, unsigned int y)
	{
		char str[40];
		unsigned char len;
		unsigned long seconds = ActiveDriveControllerParams::GetUpTime();
		unsigned long minutes = seconds / 60;
		unsigned long hours = minutes / 60;
		if (hours > 0)
			len = sprintf(str, "� ������ %ld�", hours);
		else
		{
			if (minutes)
				len = sprintf(str, "� ������ %ld���", minutes);
			else
			{
				len = sprintf(str, "� ������ %ld ���", seconds);
			}
		}
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
	}
	
	void PrintOperatingTime(unsigned int y)
	{
		char str[40];
		unsigned char len;
		
		unsigned long seconds = ActiveDriveControllerParams::GetOperatingTime();
		unsigned long minutes = seconds / 60;
		unsigned long hours = minutes / 60;
		if (hours > 0)
			len = sprintf(str, "��������� %ld�", hours);
		else
		{
			if (minutes)
				len = sprintf(str, "��������� %ld���", minutes);
			else
			{
				len = sprintf(str, "��������� %ld ���", seconds);
			}
		}
		display.MoveCursorTo(40 - len, y);
		display.WriteLine(str, len);
	}
};

#endif