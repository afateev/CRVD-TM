////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		16/01/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef CONTROL_H
#define CONTROL_H

#include "MegaLib\Megalib.h"

template<class T>
class StaticVariableWrapper
{
public:
	static T Variable;
};

template<class T>
T StaticVariableWrapper<T>::Variable;

//template<class DisplayType, DisplayType &display>
class Control
{
public:
	static const unsigned int InvalidId = 0xFF;
protected:
	WindowBase *_parent;
	unsigned char _id;
	
	typedef StaticVariableWrapper<Control *>_focused;
	Control *_next;
	unsigned short _x;
	unsigned char _y;
public:
	
	Control()
	{
		_parent = 0;
		_id = InvalidId;
		_x = 0;
		_y = 0;
		_next = 0;
	}
	
	
	virtual ~Control()
	{
	
	}
	
	virtual void SetParent(WindowBase *parent)
	{
		_parent = parent;
	}
	
	virtual void SetId(unsigned int id)
	{
		_id = id;
	}
	
	virtual void SetPos(unsigned int x, unsigned int y)
	{
		_x = x;
		_y = y;
	}
	
	virtual void Draw(Display &display)
	{
		
	}
	
	virtual bool OnKeyDown(char &key)
	{
		return false;
	}
	
	virtual void SetFocus()
	{
		_focused::Variable = this;
	}
	
	virtual bool Focused()
	{
		return this == _focused::Variable;
	}
	
	virtual void SetNext(Control *next)
	{
		_next = next;
	}
};
/*
template<class DisplayType, DisplayType &display>
Control<DisplayType, display> *Control<DisplayType, display>::_focused = 0;
*/
#endif