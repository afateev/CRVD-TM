////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		25/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_CONFIG_SYSTEM_DATETIME_H
#define WINDOW_CONFIG_SYSTEM_DATETIME_H

#include "Window.h"
#include "ControlTextInput.h"
#include "ControlButton.h"

class DateTimeInput : public Control
{
protected:
	typedef Rblib::Font Font;
	ControlTextInput<2> _day;
	ControlTextInput<2> _month;
	ControlTextInput<4> _year;
	ControlTextInput<2> _hours;
	ControlTextInput<2> _minutes;
	ControlTextInput<2> _seconds;
public:
	DateTimeInput()
	{
		SetParent(_parent);
		_day.SetNext(&_month);
		_month.SetNext(&_year);
		_year.SetNext(&_hours);
		_hours.SetNext(&_minutes);
		_minutes.SetNext(&_seconds);
	}
	
	virtual ~DateTimeInput()
	{
	}
	
	virtual void SetParent(WindowBase *parent)
	{
		_day.SetParent(_parent);
		_month.SetParent(_parent);
		_year.SetParent(_parent);
		_hours.SetParent(_parent);
		_minutes.SetParent(_parent);
		_seconds.SetParent(_parent);
	}
	
	virtual void SetText(char *text)
	{
		if (0 == text)
			return;
		if (strlen(text) < 19)
			return;
		_day.SetText(&text[0]);
		_month.SetText(&text[3]);
		_year.SetText(&text[6]);
		_hours.SetText(&text[11]);
		_minutes.SetText(&text[14]);
		_seconds.SetText(&text[17]);
	}
	
	virtual void Draw(Display &display)
	{
		Font *font = display.GetCurrentFont();
		unsigned char charWidth = 1;
		unsigned char charHeight = 16;
		if (font)
		{
			charWidth = font->Width();
			charHeight = font->Height();
		}
		
		_day.SetPos(_x + 0, _y);
		_month.SetPos(_x + 3 * charWidth, _y);
		_year.SetPos(_x + 6 * charWidth, _y);
		_hours.SetPos(_x + 11 * charWidth, _y);
		_minutes.SetPos(_x + 14 * charWidth, _y);
		_seconds.SetPos(_x + 17 * charWidth, _y);
		
		_day.Draw(display);
		display.MoveCursorTo(_x + 2 * charWidth, _y);
		display.WriteLine(".");
		_month.Draw(display);
		display.MoveCursorTo(_x + 5 * charWidth, _y);
		display.WriteLine(".");
		_year.Draw(display);
		display.MoveCursorTo(_x + 10 * charWidth, _y);
		display.WriteLine(".");
		_hours.Draw(display);
		display.MoveCursorTo(_x + 13 * charWidth, _y);
		display.WriteLine(".");
		_minutes.Draw(display);
		display.MoveCursorTo(_x + 16 * charWidth, _y);
		display.WriteLine(".");
		_seconds.Draw(display);
		
	}
	
	virtual bool OnKeyDown(char &key)
	{
		if (!Focused())
			return false;
		
		bool res = false;
		if (!res)
			res |= _day.OnKeyDown(key);
		if (!res)
			res |= _month.OnKeyDown(key);
		if (!res)
			res |= _year.OnKeyDown(key);
		if (!res)
			res |= _hours.OnKeyDown(key);
		if (!res)
			res |= _minutes.OnKeyDown(key);
		if (!res)
			res |= _seconds.OnKeyDown(key);
		return res;
	}
	
	virtual void SetFocus()
	{
		_day.SetFocus();
	}
	
	virtual bool Focused()
	{
		bool res = false;
		res |= _day.Focused();
		res |= _month.Focused();
		res |= _year.Focused();
		res |= _hours.Focused();
		res |= _minutes.Focused();
		res |= _seconds.Focused();
		return res || Control::Focused();
	}
	
	virtual void SetNext(Control *next)
	{
		_seconds.SetNext(next);
	}
	
	virtual time_t GetDateTime()
	{
		time_t curTime = Rtc::GetTime();
		struct tm *timeinfo = localtime(&curTime);
		
		int year = _year.GetIntValue();
		int month = _month.GetIntValue();
		int day = _day.GetIntValue();
		
		int hours = _hours.GetIntValue();
		int minutes = _minutes.GetIntValue();
		int seconds = _seconds.GetIntValue();
		
		timeinfo->tm_year = year - 1900;
		timeinfo->tm_mon = month - 1;
		timeinfo->tm_mday = day;
		timeinfo->tm_hour = hours;
		timeinfo->tm_min = minutes;
		timeinfo->tm_sec = seconds;
		timeinfo->tm_isdst = -1;
		
		time_t res = mktime(timeinfo);
		
		return res;
	}
};

template<class DisplayType, DisplayType &display>
class WindowConfigSystemDateTime : public Window<DisplayType, display>
{
protected:
	typedef Rblib::Font Font;
	static const unsigned int wndWidth = 40;
	
	static const unsigned int _applyId = 0;
	static const unsigned int _cancelId = 1;
	
	Font *_clockFont;
	//ControlTextInput</*DisplayType, display, */5> _newTime;
	DateTimeInput _newTime;
	ControlButton _apply;
	ControlButton _cancel;
public:
	WindowConfigSystemDateTime(Font *clockFont)
	{
		_newTime.SetParent(this);
		_clockFont = clockFont;
		_apply.SetParent(this);
		_apply.SetId(_applyId);
		_cancel.SetParent(this);
		_cancel.SetId(_cancelId);
	}
	
	virtual void Draw()
	{
		char str[100];
		unsigned char len;
		
		len = sprintf(str, "Настройка системных параметров");
		display.MoveCursorTo((wndWidth - len) / 2, 0);
		display.WriteLine(str, len);
		
		len = sprintf(str, "Дата / время");
		display.MoveCursorTo((wndWidth - len) / 2, 16);
		display.WriteLine(str, len);
		
		display.MoveCursorTo(1, 48);
		display.WriteLine("Текущее значение:");
		
		PrintCurTime(48 + 16, Rtc::GetTime());
		
		display.MoveCursorTo(1, 48 + 16 + 16 + 32);
		display.WriteLine("Новое значение:");
		
		DrawInput(48 + 16 + 16 + 32 + 16);
		
		Font *oldFont = display.SelectFont(_clockFont);
		_apply.Draw(display);
		_cancel.Draw(display);
		display.SelectFont(oldFont);
	}
	
	void PrintCurTime(unsigned int y, time_t dt)
	{
		if (0 == _clockFont)
			return;
		
		struct tm * timeinfo;
		char str[20];
		timeinfo = localtime ( &dt );
		strftime (str, sizeof(str), "%d.%m.%Y %H:%M:%S", timeinfo);
		
		Font *oldFont = display.SelectFont(_clockFont);
		
		unsigned int len = strlen(str);
		unsigned int x = len * _clockFont->Width();
		x = wndWidth - x;
		x /= 2;
		
		display.MoveCursorTo(x, y);
		display.WriteLine(str, len);
		
		display.SelectFont(oldFont);
	}
	
	void DrawInput(unsigned int y)
	{
		if (0 == _clockFont)
			return;
		
		Font *oldFont = display.SelectFont(_clockFont);
		
		_newTime.SetPos(1, y);
		_newTime.Draw(display);
		
		display.SelectFont(oldFont);
	}
	
	virtual bool OnKeyDown(char &key)
	{
		if (_newTime.OnKeyDown(key))
			return true;
		if (_apply.OnKeyDown(key))
			return true;
		if (_cancel.OnKeyDown(key))
			return true;
		if (_newTime.Focused())
		{
			if ('E' == key)
			{
				_cancel.SetFocus();
				return true;
			}
		}
		return false;
	}
	
	virtual void OnShow()
	{
		struct tm * timeinfo;
		char str[20];
		time_t curTime = Rtc::GetTime();
		timeinfo = localtime ( &curTime );
		strftime (str, sizeof(str), "%d.%m.%Y %H:%M:%S", timeinfo);
		
		_newTime.SetFocus();
		_newTime.SetText(str);
		_newTime.SetNext(&_apply);
		_apply.SetNext(&_cancel);
		_cancel.SetNext(&_newTime);
		_apply.SetText("Применить");
		_cancel.SetText("Отмена");
		_apply.SetPos(0, 239 - 32);
		_cancel.SetPos(40 - 14, 239 - 32);
	}
	
	virtual void OnClick(unsigned int controlId, char key)
	{
		if (_applyId == controlId)
		{
			if (13 == key)
			{
				time_t newTime = _newTime.GetDateTime();
				Rtc::SetTime(newTime);
				
				WindowBase::Close();
			}
			if ('E' == key)
			{
				WindowBase::Close();
			}
		}
		if (_cancelId == controlId)
		{
			if (13 == key || 'E' == key)
			{
				WindowBase::Close();
			}
		}
	}
};

#endif