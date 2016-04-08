////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		15/02/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef DESCTOP_STACK_H
#define DESCTOP_STACK_H

#include "Menu.h"

template<unsigned int Size>
class DesctopStack
{
protected:
	struct Element
	{
		MenuItemListBase *Menu;
		Element() : Menu(0)
		{}
		
		Element(MenuItemListBase *menu) : Menu(menu)
		{}
	};
	
	Element _data[Size];
	unsigned int _len;
public:
	DesctopStack()
	{
		_len = 0;
	}
	
	void Push(MenuItemListBase *menu)
	{
		if (_len >= Size)
			return;
		_data[_len] = Element(menu);
		_len++;
	}
	
	bool Pop(MenuItemListBase **menu)
	{
		if(_len == 0)
			return false;
		_len--;
		if (menu)
		{
			*menu = _data[_len].Menu;
		}
		return true;
	}
};

#endif