////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		17/01/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef CONTROLBUTTON_H
#define CONTROLBUTTON_H

#include "Control.h"

class ControlButton : public Control
{
protected:
	typedef Rblib::Font Font;
	char *_text;
public:
	ControlButton()
	{
		_text = 0;
	}
	
	virtual ~ControlButton()
	{
	}
	
	virtual void SetText(char *text)
	{
		_text = text;
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
		
		unsigned int len = strlen(_text);
		unsigned int width = len * charWidth + 2;
		unsigned int height = charHeight;
		display.MoveCursorTo(_x + 1, _y);
		display.WriteLine(_text);
		
		if (Focused())
		{
			for (unsigned char y = 0; y < height; y++)
			for (unsigned char x = 0; x < width; x++)
			{
				display.InvertByte(_x + x, _y + y);
			}
		}
	}
	
	virtual bool OnKeyDown(char &key)
	{
		if (!Focused())
			return false;
		if (key == 'S')
		{
			if (_next)
				_next->SetFocus();
			return true;
		}
		if (key == 13 || key == 'E')
		{
			if (_parent)
				_parent->OnClick(_id, key);
			return true;
		}
		
		return false;
	}
};

#endif