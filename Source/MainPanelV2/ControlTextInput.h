////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		16/01/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef CONTROLTEXTINPUT_H
#define CONTROLTEXTINPUT_H

#include "ControlCharInput.h"

template<char len>
class ControlTextInput : public Control//<DisplayType, display>
{
protected:
	//typedef Control<DisplayType, display> _base;
	typedef Rblib::Font Font;
	ControlCharInput/*<DisplayType, display>*/ _input[len];
public:
	ControlTextInput()
	{
		SetParent(_parent);
		for(short i = 0 ; i < len - 1; i++)
		{
			_input[i].SetNext(&_input[i + 1]);
		}
	}
	virtual ~ControlTextInput()
	{
	
	}
	
	virtual void SetParent(WindowBase *parent)
	{
		_parent = parent;
		for(short i = 0; i < len - 1; i++)
		{
			_input[i].SetParent(parent);
		}
	}
	
	virtual void SetText(char *text)
	{
		if (0 == text)
			return;
		unsigned int strLen = strlen(text);
		if (strLen > len)
			strLen = len;
		for(unsigned char i = 0 ; i < strLen; i++)
		{
			_input[i].SetText(text[i]);
		}
	}
	
	virtual unsigned int GetText(char* dst)
	{
		if (0 == dst)
			return 0;
		
		for(unsigned char i = 0 ; i < len; i++)
		{
			dst[i] = _input[i].GetText();
		}
		
		return len;
	}
	
	virtual int GetIntValue()
	{
		char tmp[50];
		
		unsigned int strLen = GetText(tmp);
		tmp[strLen] = 0;
		
		int tmpVal;
		
		int res = sscanf(tmp, "%d", &tmpVal);
		if (res == 1)
			return tmpVal;
		else
			return 0;
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
		for(unsigned char i = 0 ; i< len; i++)
		{
			_input[i].SetPos(_x + charWidth * i, _y);
			_input[i].Draw(display);
		}
		if (Focused())
		{
			for(unsigned char y = 0 ; y < charHeight; y++)
			for(unsigned char x = 0 ; x < len * charWidth; x++)
			{
				display.InvertByte(_x + x, _y + y);
			}
		}
	}
	
	virtual bool OnKeyDown(char &key)
	{
		if (!Focused())
			return false;
		for(unsigned char i = 0 ; i< len; i++)
		{
			if (_input[i].OnKeyDown(key))
				return true;
		}
		if (13 == key || 'S' == key)
		{
			if (_next)
				_next->SetFocus();
			return true;
		}
		return false;
	}
	
	virtual void SetFocus()
	{
		_input[0].SetFocus();
	}
	
	virtual bool Focused()
	{
		bool res = false;
		for(unsigned char i = 0 ; i< len; i++)
		{
			res |= _input[i].Focused();
		}
		return res || Control::Focused();
	}
	
	virtual void SetNext(Control *next)
	{
		_next = next;
		if (len > 0)
			_input[len - 1].SetNext(next);
	}
	
	virtual bool Dummy()
	{
		return false;
	}
	
	virtual bool HasMinusSign()
	{
		char tmp[50];
		
		unsigned int strLen = GetText(tmp);
	    for (unsigned int i = 0; i < strLen; i++)
		{
			if (tmp[i] == '-')
			{
				return true;
			}
		}
		
		return false;
	}
};

template<>
class ControlTextInput<0> : public Control
{
public:
	/*
	ControlTextInput()
	{
		SetParent(_parent);
	}
	virtual ~ControlTextInput()
	{
	
	}
	
	virtual void SetParent(WindowBase *parent)
	{
		_parent = parent;
	}
	*/
	virtual void SetText(char *text)
	{
	}
	/*
	virtual unsigned int GetText(char* dst)
	{
		return 0;
	}
	*/
	virtual int GetIntValue()
	{
		return 0;
	}
	/*
	virtual void Draw(Display &display)
	{
	}
	
	virtual bool OnKeyDown(char &key)
	{
		return false;
	}
	
	virtual void SetFocus()
	{
		_input[0].SetFocus();
	}
	
	virtual bool Focused()
	{
		bool res = false;
		for(unsigned char i = 0 ; i< len; i++)
		{
			res |= _input[i].Focused();
		}
		return res || Control::Focused();
	}
	
	virtual void SetNext(Control *next)
	{
		_next = next;
		if (len > 0)
			_input[len - 1].SetNext(next);
	}
	*/
	
	virtual bool Dummy()
	{
		return true;
	}
	
	virtual bool HasMinusSign()
	{
		return false;
	}
};

#endif