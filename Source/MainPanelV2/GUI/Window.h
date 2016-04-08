////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		16/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_H
#define WINDOW_H

#include "..\RbLib\Rblib.h"

class WindowBase
{
protected:
	typedef void (OnCloseCallback)();
	OnCloseCallback *_onCloseCallback;
public:
	WindowBase()
	{
		_onCloseCallback = 0;
	}
	
	virtual ~WindowBase()
	{}
	
	virtual bool OnKeyDown(char &key)
	{
		return false;
	}
	
	virtual void OnShow()
	{
	}
	
	virtual void Close()
	{
		if (_onCloseCallback)
			_onCloseCallback();
	}
	
	virtual void SetOnClose(OnCloseCallback *onCloseCallback)
	{
		_onCloseCallback = onCloseCallback;
	}
	
	virtual void OnClick(unsigned int controlId, char key)
	{
	
	}
	
	virtual char *GetEditingValue()
	{
		return "n/a";
	}
};

template<class DisplayType, DisplayType &display>
class Window : public WindowBase
{
public:
	Window()
	{}
	
	virtual ~Window()
	{}
	
	virtual void Draw()
	{
	}
};

#endif