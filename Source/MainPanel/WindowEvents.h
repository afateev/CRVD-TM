////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		05/02/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_EVENTS_H
#define WINDOW_EVENTS_H

#include "Window.h"

template<class DisplayType, DisplayType &display, class Events, class Event>
class WindowEvents : public Window<DisplayType, display>
{
protected:
	static const unsigned int wndWidth = 40;
	static const unsigned int _headerHeight = 32;
	static const unsigned int _viewportWidth = 40;
	static const unsigned int _viewportHeight = 240 - _headerHeight;
	
	unsigned long pos;
	unsigned long count;
	
	unsigned long _scrollBarHeight;
	long _scrollPos;
	unsigned long _scrollBarPos;
public:
	WindowEvents()
	{
		_scrollPos = 0;
		_scrollBarPos = 0;
		_scrollBarHeight = 0;
	}
	
	virtual void Draw()
	{
		char str[100];
		unsigned char len;
		
		len = sprintf(str, "Журнал событий");
		display.MoveCursorTo((wndWidth - len) / 2, 0);
		display.WriteLine(str, len);
		
		len = sprintf(str, "%ld / %ld", (count - pos), count);
		display.MoveCursorTo((wndWidth - len) / 2, 16);
		display.WriteLine(str, len);
		
		for(int i = 0; i < wndWidth; i++)
		{
			display.WriteByte(i, 0, 0xFF);
			display.WriteByte(i, 16, 0xFF);
			display.WriteByte(i, 31, 0xFF);
		}
		
		for (int i = 0; i < Events::_eventsCountInCache; i++)
		{
			DrawEvent(Event(Events::_eventsCache[Events::_eventsCountInCache - i - 1]), _headerHeight + i * 16);
			
			if (i == (pos - _scrollPos))
			{
				for(int y = _headerHeight + i * 16; y < _headerHeight + i * 16 + 16; y++)
				for(int x = 0; x < wndWidth - 1; x++)
				{
					display.InvertByte(x, y);
				}
			}
		}
		
		DrawScrollBar();
		CheckNewEvents();
	}
	
	virtual void OnShow()
	{
		Events::_newEventsCount = 0;
		pos = 0;
		count = Events::GetEventsCount();
		long newScrollPos = CalcScrollPos(pos);
		Events::Cache(count);
		UpdateScroolPos(newScrollPos);
	}
	
	virtual bool OnKeyDown(char &key)
	{
		if ('+' == key)
		{
			SelectPrev();
			return true;
		}
		
		if ('-' == key)
		{
			SelectNext();
			return true;
		}
		
		if ('E' == key)
		{
			WindowBase::Close();
			return true;
		}
		return false;
	}
protected:
	void DrawEvent(Event e, int y)
	{
		// непонятное время не будем выводить
		if (e.GetDt() > 0)
			PrintDateTimeShort(0, y, e.GetDt());
		
		char * text = e.GetText();
		int textlen = 0;
		if (text)
			textlen = strlen(text);
		
		if (textlen > 40 - 17)
			textlen = 40 - 17;
		
		display.MoveCursorTo(17, y);
		display.WriteLine(text, textlen);
		/*
		// сетка
		for(int i = 0; i < wndWidth; i++)
		{
			display.WriteByte(i, y, 0xFF);
			display.WriteByte(i, y + 16, 0xFF);
		}
		*/
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
	
	void CheckNewEvents()
	{
		unsigned long newEventsCount = Events::_newEventsCount;
		
		if (pos >= newEventsCount)
			pos -= newEventsCount;
		
		count = Events::GetEventsCount();
		long newScrollPos = CalcScrollPos(pos);
		Events::Cache(count - newScrollPos);
		UpdateScroolPos(newScrollPos);
		
		Events::_newEventsCount = 0;
	}
	
	void SelectPrev()
	{
		if (pos == 0)
			return;
		pos--;
		
		count = Events::GetEventsCount();
		long newScrollPos = CalcScrollPos(pos);
		if (newScrollPos != _scrollPos)
			Events::Cache(count - newScrollPos);
		UpdateScroolPos(newScrollPos);
	}
	
	void SelectNext()
	{
		if (0 == count)
			return;
		if (pos >= count - 1)
			return;
		
		pos++;
		count = Events::GetEventsCount();
		long newScrollPos = CalcScrollPos(pos);
		if (newScrollPos != _scrollPos)
			Events::Cache(count - newScrollPos);
		UpdateScroolPos(newScrollPos);
	}
	
	long CalcScrollPos(unsigned long newPos)
	{
		long visibleItemsCount = Events::_eventsCountInCache;
		long scrollPos = _scrollPos;
		
		if (newPos >= scrollPos + visibleItemsCount)
		{
			scrollPos = newPos - visibleItemsCount + 1;
		}
		
		if (newPos < scrollPos)
		{
			scrollPos = newPos;
		}
		
		if (scrollPos < 0)
			scrollPos = 0;
		
		if (scrollPos > count - visibleItemsCount)
			scrollPos = count - visibleItemsCount;
		
		return scrollPos;
	}
	
	void UpdateScroolPos(long newScrollPos)
	{
		long _visibleItemsCount = Events::_eventsCountInCache;
		
		float visible = _visibleItemsCount;
		float total = count;
		float ratio = visible / total;
		total = _viewportHeight;
		visible = ratio * total;
		_scrollBarHeight = (int)visible;
		_scrollPos = newScrollPos;
		
		/*
		if (pos >= _scrollPos + _visibleItemsCount)
		{
			_scrollPos = pos - _visibleItemsCount + 1;
		}
		
		if (pos < _scrollPos)
		{
			_scrollPos = pos;
		}
		
		if (_scrollPos < 0)
			_scrollPos = 0;
		
		if (_scrollPos > count - _visibleItemsCount)
			_scrollPos = count - _visibleItemsCount;
		*/
		float elapsedItems = count - _visibleItemsCount;
		if (elapsedItems < 1)
			elapsedItems = 1;
		float elapsedPixels = _viewportHeight - _scrollBarHeight;
		float pixelsByItem = elapsedPixels / elapsedItems;
		_scrollBarPos = (int)(_scrollPos * pixelsByItem);
	}
	
	void DrawScrollBar()
	{
		for (unsigned int y = 0; y < _viewportHeight; y++)
		{
			unsigned char b = 0x81;
			if (y == 0 || y == _viewportHeight - 1)
				b = 0xFF;
			if (y >= _scrollBarPos && y <= _scrollBarHeight + _scrollBarPos)
			{
				char sb = 0xAA;
				if (y % 2)
					sb = ~sb;
				b |= sb;
			}
			display.WriteByte(wndWidth - 1, y + _headerHeight, b);
		}
	}
};


#endif