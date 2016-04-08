////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		16/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef DESCTOP_H
#define DESCTOP_H

#include "..\RbLib\Rblib.h"
#include "Windows.h"

Rblib::FbgFont EnicompLogoChar(Rblib::EnicompLogo);

template<class DisplayType, DisplayType &display>
class Desctop
{
public:
	typedef Rblib::Font Font;
	typedef Window<DisplayType, display> WindowType;
protected:
	static const unsigned char _maxWindowsCount = wndIdLast;
	WindowType * _windows[_maxWindowsCount];
	int _visibleWindow;
public:
	Desctop()
	{
		_visibleWindow = -1;
		for (int i = 0; i < _maxWindowsCount; i++)
			_windows[i] = 0;
	}
	
	void RegisterWindow(WindowType *wnd, int id)
	{
		if (id < 0 || id >= _maxWindowsCount)
			return;
		_windows[id] = wnd;
	}
	
	WindowType *FindWindow(int id)
	{
		if (id < -1 || id >= _maxWindowsCount)
			return 0;
		return _windows[id];
	}
	
	void ShowWindow(int id)
	{
		if (id < -1 || id >= _maxWindowsCount)
			return;
		
		_visibleWindow = id;
		OnShowWindow();
	}
	
	void HideWindow()
	{
		_visibleWindow = -1;
	}
	
	bool IsWindowVisible(int wnd)
	{
		return _visibleWindow == wnd;
	}
	
	void Draw()
	{
		display.Clear();
		
		if (_visibleWindow >= 0)
		{
			if (_windows[_visibleWindow] != 0)
			{
				_windows[_visibleWindow]->Draw();
			}
		}
		else
		{
			Font *old = display.SelectFont(&EnicompLogoChar);
			
			display.MoveCursorTo((40 - 16) / 2, (240 - 78) / 2 - 16);
			display.WriteLine("\0", 1);
			
			display.SelectFont(old);
			
			char str[100];
			unsigned char len;
			len = sprintf(str, "enicomp.ru");
			display.MoveCursorTo((40 - len) / 2, (240 - 78) / 2 - 16 + 78);
			display.WriteLine(str, len);
		}
		
		display.Flush();
	}
	
	bool OnKeyDown(char &key)
	{
		if (_visibleWindow >= 0)
		{
			if (_windows[_visibleWindow] != 0)
			{
				return _windows[_visibleWindow]->OnKeyDown(key);
			}
		}
		
		return false;
	}
	
	void OnShowWindow()
	{
		if (_visibleWindow >= 0)
		{
			if (_windows[_visibleWindow] != 0)
			{
				_windows[_visibleWindow]->OnShow();
			}
		}
	}
};

#endif