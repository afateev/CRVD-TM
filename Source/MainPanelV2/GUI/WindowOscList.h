////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		24/01/2013
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_OSCLIST_H
#define WINDOW_OSCLIST_H

#include "Window.h"

template<class DisplayType, DisplayType &display, class OscList, class Osc, class DesctopType, DesctopType *desctop, class WindowGraphType, WindowGraphType *windowGraph, int windowGraphId>
class WindowOscList : public Window<DisplayType, display>
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
	bool _onUpdatePosPending;
	bool _onUpdatePosGetOscCount;
public:
	WindowOscList()
	{
		_scrollPos = 0;
		_scrollBarPos = 0;
		_scrollBarHeight = 0;
		_onUpdatePosPending = false;
		_onUpdatePosGetOscCount = false;
	}
	
	virtual void Draw()
	{
		char str[100];
		unsigned char len;
		
		len = sprintf(str, "Журнал осциллограмм");
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
		
		short progress = OscList::GetProgress();
		
		if (progress != 100)
		{
			len = sprintf(str, "Выполняется сохранение осциллограммы");
			display.MoveCursorTo((wndWidth - len) / 2, 32);
			display.WriteLine(str, len);
			
			len = sprintf(str, "%s", OscList::GetLastFileName());
			display.MoveCursorTo((wndWidth - len) / 2, 32 + 16);
			display.WriteLine(str, len);
			
			len = sprintf(str, "Завершено %d%%", progress);
			display.MoveCursorTo((wndWidth - len) / 2, 32 + 16 + 16);
			display.WriteLine(str, len);
			return;
		}
		
		for (int i = 0; i < OscList::_oscCountInCache; i++)
		{
			DrawOsc(Osc(OscList::_oscCache[OscList::_oscCountInCache - i - 1]), _headerHeight + i * 16);
			
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
	}
	
	virtual void OnShow()
	{
		OscList::_newOscCount = 0;
		pos = 0;
		OnUpdatePos(true);
	}
	
	virtual bool OnKeyDown(char &key)
	{
		bool blocked = OscList::GetProgress() != 100;
		
		if ('+' == key && !blocked)
		{
			SelectPrev();
			return true;
		}
		
		if ('-' == key && !blocked)
		{
			SelectNext();
			return true;
		}
		
		if ('E' == key)
		{
			WindowBase::Close();
			return true;
		}
		
		if (13 == key && !blocked)
		{
			if (OscList::_oscCountInCache > 0)
			{
				if (windowGraph)
				{
					windowGraph->OscFileName = OscList::_oscCache[OscList::_oscCountInCache - (pos - _scrollPos) - 1].FileName;
				}
				
				if (desctop)
				{
					desctop->ShowWindow(windowGraphId);
				}
			}
			return true;
		}
		return false;
	}
	
	void OnUpdatePos(bool getOscCount)
	{
		_onUpdatePosPending = true;
		_onUpdatePosGetOscCount = getOscCount;
	}
	
	void DoLoPiorityWork()
	{
		if (_onUpdatePosPending)
		{
			_onUpdatePosPending = false;
			
			// OnShow
			if (_onUpdatePosGetOscCount)
			{
				count = OscList::GetOscCount();
			}
			long newScrollPos = CalcScrollPos(pos);
			if (newScrollPos != _scrollPos || _onUpdatePosGetOscCount)
			{
				OscList::Cache(count - newScrollPos, count);
			}
			UpdateScroolPos(newScrollPos);
		}
	}
protected:
	void DrawOsc(Osc o, int y)
	{
		char * text = o.FileName;
		int textlen = 0;
		if (text)
			textlen = strlen(text);
		
		if (textlen > 40 - 1)
			textlen = 40 - 1;
		
		display.MoveCursorTo(0, y);
		display.WriteLine(text, textlen);
		
		// непонятное время не будем выводить
		/*
		if (o.GetDt() > 0)
			PrintDateTimeShort(0, y, o.GetDt());
		
		char * text = o.GetText();
		int textlen = 0;
		if (text)
			textlen = strlen(text);
		
		if (textlen > 40 - 17)
			textlen = 40 - 17;
		
		display.MoveCursorTo(17, y);
		display.WriteLine(text, textlen);
		*/
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
	
	void SelectPrev()
	{
		if (pos == 0)
			return;
		pos--;
		
		OnUpdatePos(false);
	}
	
	void SelectNext()
	{
		if (0 == count)
			return;
		if (pos >= count - 1)
			return;
		
		pos++;
		OnUpdatePos(false);
	}
	
	long CalcScrollPos(unsigned long newPos)
	{
		long visibleItemsCount = OscList::_oscCountInCache;
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
		long _visibleItemsCount = OscList::_oscCountInCache;
		
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