////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		16/01/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef CONTROLCHARINPUT_H
#define CONTROLCHARINPUT_H

#include "Control.h"

//template<class DisplayType, DisplayType &display>
class ControlCharInput : public Control//<DisplayType, display>
{
protected:
	//typedef Control<DisplayType, display> _base;
	char _text;
public:
	ControlCharInput()
	{
		_text = 0;
	}
	
	virtual ~ControlCharInput()
	{
	}
	
	virtual void SetText(char text)
	{
		_text = text;
	}
	
	virtual char GetText()
	{
		return _text;
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
		
		display.MoveCursorTo(_x, _y);
		display.WriteLine(&_text, 1);
		
		// типа курсор
		if (display.CursorVisible() && Focused())
		{
			for (unsigned char y = 0; y < 3; y++)
			for (unsigned char x = 0; x < charWidth; x++)
			{
				display.InvertByte(_x + x, _y + charHeight - 1 - y - 2 * charWidth);
			}
		}
	}
	
	virtual bool OnKeyDown(char &key)
	{
		if (!Focused())
			return false;
		if (key >= '0' && key <= '9')
		{
			_text = key;
			if (_next)
				_next->SetFocus();
			return true;
		}
		
		if (key == '+')
		{
			_text = ' ';
			if (_next)
				_next->SetFocus();
			return true;
		}
		
		if (key == '-')
		{
			_text = '-';
			if (_next)
				_next->SetFocus();
			return true;
		}
		return false;
	}
};

#endif