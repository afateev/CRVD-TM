////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		11/02/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_RESET_OPERATING_TIME_H
#define WINDOW_RESET_OPERATING_TIME_H

#include "Window.h"
#include "ControlTextInput.h"
#include "ControlButton.h"

class ControlYesNoInput : public Control
{
protected:
    bool _boolValue;
public:
    ControlYesNoInput()
	{
		SetParent(_parent);
	}
	
	virtual ~ControlYesNoInput()
	{
	}
	
    virtual void SetText(char *text)
	{
		if (0 == text)
			return;
        
        if (text[1] == 'е' || text[1] == 'Е')
	        _boolValue = 0;
        else
            _boolValue = 1;
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
		
        display.MoveCursorTo(_x + 0, _y);
	   	display.WriteLine(_boolValue ? "да" : "нет");
        if (Focused())
	    {
		    for(unsigned char y = 0 ; y < charHeight; y++)
		    for(unsigned char x = 0 ; x < 3 * charWidth; x++)
		    {
			    display.InvertByte(_x + x, _y + y);
		    }
		}
	}
    
    virtual bool OnKeyDown(char &key)
	{
		if (!Focused())
			return false;
		
		bool res = false;
        if (13 == key || 'S' == key)
	    {
		    if (_next)
			    _next->SetFocus();
		    return true;
	    }
        if (key == '+' || key == '-')
        {
            _boolValue = !_boolValue;
            return true;
        }
		return res;
	}
    
    virtual bool GetBoolValue()
    {
        return _boolValue;
    }
};

template<class DisplayType, DisplayType &display, class ActiveDriveControllerParams>
class WindowResetOperatingTime : public Window<DisplayType, display>
{
protected:
	static const unsigned int wndWidth = 40;
	
	static const unsigned int _applyId = 0;
	static const unsigned int _cancelId = 1;
	
	Font *_bigFont;
	//ControlTextInput</*DisplayType, display, */5> _newTime;
	ControlYesNoInput _yes;
	ControlButton _apply;
	ControlButton _cancel;
public:
	WindowResetOperatingTime(Font *bigFont)
	{
		_yes.SetParent(this);
		_bigFont = bigFont;
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
		
		len = sprintf(str, "Счётчик наработки");
		display.MoveCursorTo((wndWidth - len) / 2, 16);
		display.WriteLine(str, len);
		
		display.MoveCursorTo(1, 48);
		display.WriteLine("Текущее значение:");
		
		PrintCurValue(48 + 16);
		
		display.MoveCursorTo(1, 48 + 16 + 16 + 32);
		display.WriteLine("Обнулить счётчик наработки?");
		
		DrawInput(48 + 16 + 16 + 32 + 16);
		
		Font *oldFont = display.SelectFont(_bigFont);
		_apply.Draw(display);
		_cancel.Draw(display);
		display.SelectFont(oldFont);
	}
	
	void PrintCurValue(unsigned int y)
	{
		if (0 == _bigFont)
			return;
		
		Font *oldFont = display.SelectFont(_bigFont);
		char str[40];
		unsigned char len;
		
		unsigned long seconds = ActiveDriveControllerParams::GetOperatingTime();
		unsigned long minutes = seconds / 60;
		unsigned long hours = minutes / 60;
		if (hours > 0)
			len = sprintf(str, "%ldч", hours);
		else
		{
			if (minutes)
				len = sprintf(str, "%ldмин", minutes);
			else
			{
				len = sprintf(str, "%ld сек", seconds);
			}
		}
		
		display.MoveCursorTo(1, y);
		display.WriteLine(str, len);
		
		display.SelectFont(oldFont);
	}
	
	void DrawInput(unsigned int y)
	{
		if (0 == _bigFont)
			return;
		
		Font *oldFont = display.SelectFont(_bigFont);
		
		_yes.SetPos(1, y);
		_yes.Draw(display);
		
		display.SelectFont(oldFont);
	}
	
	virtual bool OnKeyDown(char &key)
	{
		if (_yes.OnKeyDown(key))
			return true;
		if (_apply.OnKeyDown(key))
			return true;
		if (_cancel.OnKeyDown(key))
			return true;
		if (_yes.Focused())
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
		_yes.SetFocus();
		_yes.SetText("нет");
		_yes.SetNext(&_apply);
		_apply.SetNext(&_cancel);
		_cancel.SetNext(&_yes);
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
				bool yes = _yes.GetBoolValue();
				if (yes)
				{
				    ActiveDriveControllerParams::ResetOperatingTime();
				}
				//WindowBase::Close();
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