////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		12/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <string.h>
#include "MegaLib\Megalib.h"

class MenuItemListBase
{
public:
	MenuItemListBase *Parent;	
public:	
	MenuItemListBase()
	{
		Parent = 0;
	}
};

class MenuItem
{
public:
	typedef void (OnClickCallback)(MenuItemListBase *sender);
	typedef char * (OnGetValueStringCallback)();
protected:
	char *_text;
	OnClickCallback *_onClick;
	OnGetValueStringCallback *_onGetValueString;
public:
	MenuItemListBase *Child;
public:
	MenuItem(char *text)
	{
		_text = text;
		_onClick = 0;
		_onGetValueString = 0;
		Child = 0;
	}
	
	MenuItem(char *text, OnClickCallback *onClick)
	{
		_text = text;
		_onClick = onClick;
		_onGetValueString = 0;
		Child = 0;
	}
	
	MenuItem(char *text, OnClickCallback *onClick, OnGetValueStringCallback *onGetValueString)
	{
		_text = text;
		_onClick = onClick;
		_onGetValueString = onGetValueString;
		Child = 0;
	}
	
	MenuItem(char *text, MenuItemListBase *child)
	{
		_text = text;
		_onClick = 0;
		_onGetValueString = 0;
		Child = child;
	}
	
	char * GetText()
	{
		return _text;
	}
	
	OnClickCallback *GetCallbackPtr()
	{
		return _onClick;
	}
	
	OnGetValueStringCallback *GetValueStringCallbackPtr()
	{
		return _onGetValueString; 
	}
};

class MenuItemList : public MenuItemListBase
{
public:
	static const unsigned char MaxCount = 42;
protected:
	MenuItem *_list[MaxCount];
	unsigned char _size;
	char *_header;
	signed char _selected;
public:
	MenuItemList(char *header)
	{
		_size = 0;
		_selected = 0;
		_header = header;
	}
	
	inline unsigned char Size()
	{
		return _size;
	}
	
	MenuItem *GetItem(unsigned char i)
	{
		if (i >= MaxCount)
			return 0;
		return _list[i];
	}
	
	void Add(MenuItem *item)
	{
		if (_size >= MaxCount)
			return;
		
		_list[_size] = item;
		if (item->Child)
			item->Child->Parent = this;
		_size++;
	}
	
	inline char *GetHeader()
	{
		return _header;
	}
	
	inline bool Selected(unsigned char i)
	{
		return i == _selected;
	}
	
	inline void Select(unsigned char i)
	{
		if (i > _size)
			return;
		_selected = i;
	}
	
	void SelectNext()
	{
		_selected++;
		if (_selected >= _size)
			_selected = 0;
	}
	
	void SelectPrev()
	{
		_selected--;
		if (_selected < 0)
			_selected = _size - 1;
	}
};

template<class DisplayType, DisplayType &display>
class Menu
{
protected:
	bool _visible;
	MenuItemList *_root;
	MenuItemList *_current;
	Font *_smallFont;
	Font *_bigFont;
	static const unsigned int _maxLineWidth = 40;
	static const unsigned int _maxLineHeight = 32;
	static const unsigned int _maxVisibleItemsCount = MenuItemList::MaxCount;
	static const unsigned int _headerHeight = 16;
	static const unsigned int _viewportWidth = 40;
	static const unsigned int _viewportHeight = 240 - _headerHeight;
	static const unsigned char _scrollerWidth = 1;
	unsigned int _itemsCount;
	unsigned int _visibleItemsCount;
	unsigned int _maxItemHeight;
	unsigned int _totalMenuHeight;
	int _scrollPos;
	bool _showScrollBar;
	unsigned int _scrollBarHeight;
	unsigned int _scrollpos;
	unsigned int _scrollBarPos;
	int _selected;
	
	struct ItemDrawingStruct
	{
		int Width;
		int Height;
		bool SmallFont;
		bool TwoLines;
		Font *SelectedFont;
		char *Str1;
		char *Str2;
		int Str1Len;
		int Str2Len;
		char Str1VerticalOffset;
		char Str2VerticalOffset;
		int Str1Offset;
		int Str2Offset;
		int SrcIndex;
		int ActualHeight;
		MenuItem::OnClickCallback * OnClick;
		MenuItem::OnGetValueStringCallback * OnGetValueString;
		MenuItemList *Child;
		
		ItemDrawingStruct() : Width(0), Height(0), SmallFont(false), TwoLines(false),
			SelectedFont(0), Str1(0), Str2(0), Str1Len(0), Str2Len(0),
			Str1VerticalOffset(0), Str2VerticalOffset(0), Str1Offset(0), Str2Offset(0), SrcIndex(-1),
			ActualHeight(0), OnClick(0), OnGetValueString(0), Child(0)
		{}
		
		ItemDrawingStruct(int width, int height, Font *smallFont, Font *bigFont, char *str, int strLen, int srcIndex, MenuItem::OnClickCallback * onClick, MenuItem::OnGetValueStringCallback * onGetValueString, MenuItemList *child)
		{
			Width = width;
			Height = height;
			SelectedFont = bigFont;
			Str1 = str;
			Str1Len = strLen;
			Str2 = 0;
			Str2Len = 0;
			SmallFont = false;
			TwoLines = false;
			Str1VerticalOffset = 0;
			Str2VerticalOffset = 0;
			Str1Offset = 0;
			Str2Offset = 0;
			SrcIndex = srcIndex;
			OnClick = onClick;
			OnGetValueString = onGetValueString;
			Child = child;
			
			bool canTwoLines = true;
			
			if (0 == smallFont)
				return;
			if (0 == bigFont)
				return;
			if (0 == str)
				return;
			
			int realWidth = Str1Len * bigFont->Width();
			int realHeight = bigFont->Height();
						
			if(realHeight > Height)
				canTwoLines = false;
			
			Str1VerticalOffset = (Height - realHeight) / 2;
			Str1Offset = (Width - realWidth) / 2;
			
			if (realWidth > Width || realHeight > Height)
			{
				realWidth = Str1Len * smallFont->Width();
				realHeight = smallFont->Height();
				Str1VerticalOffset = (Height - realHeight) / 2;
				SmallFont = true;
				SelectedFont = smallFont;
				Str1Offset = (Width - realWidth) / 2;
				if (realWidth > Width)
				{
					if (canTwoLines)
					{
						TwoLines = true;
						
						int splitpos = Width;
						int pos = 0;
						while (pos < Width)
						{
							if (Str1[pos] == ' ')
								splitpos = pos;
							pos++;
						}
						Str2 = &Str1[splitpos];
						Str2Len = Str1Len - splitpos;
						Str1Len = splitpos;
						if (Str2Len > Width)
							Str2Len = Width;
						Str1VerticalOffset = Height / 2 - realHeight;
						Str2VerticalOffset = Str1VerticalOffset + realHeight;
						Str1Offset = (Width - Str1Len * smallFont->Width()) / 2;
						Str2Offset = (Width - Str2Len * smallFont->Width()) / 2;
					}
					else
					{
						Str1Offset = 0;
						Str1VerticalOffset = 0;
						Str1Len = Width;
					}
				}
			}
			
			if (OnGetValueString)
			{
				Str1Offset = 0;
				Str2Offset = 0;
			}
			
			ActualHeight = TwoLines ? 2 * realHeight : realHeight;
		}
	};
	
	static ItemDrawingStruct _itemsDrawingArray[_maxVisibleItemsCount];
public:
	Menu(Font *smallFont, Font *bigFont)
	{
		_visible = 0;
		_root = 0;
		_current = 0;
		_smallFont = smallFont;
		_bigFont = bigFont;
		_itemsCount = 0;
	}
	
	void SetRoot(MenuItemList * itemList)
	{
		_root = itemList;
	}
	
	void Enter(MenuItemListBase * itemList)
	{
		_current = (MenuItemList *)itemList;
		if (_current)
		{
			PrepareDrawStructures(_maxLineWidth, _maxLineHeight);
			if (_maxItemHeight < _maxLineHeight)
			{
				PrepareDrawStructures(_maxLineWidth, _maxItemHeight);
			}
			unsigned int maxItemHeightPrev = _maxItemHeight;
			if (_totalMenuHeight > _viewportHeight)
			{
				PrepareDrawStructures(_maxLineWidth - 1, _maxItemHeight);
				if (_maxItemHeight < maxItemHeightPrev)
				{
					PrepareDrawStructures(_maxLineWidth - 1, _maxItemHeight);
					if (_totalMenuHeight <= _viewportHeight)
					{
						PrepareDrawStructures(_maxLineWidth, _maxItemHeight);
					}
				}
			}
			_showScrollBar = _totalMenuHeight > _viewportHeight;
			_visibleItemsCount = _viewportHeight / (_totalMenuHeight / _itemsCount);
			float visible = _visibleItemsCount;
			float total = _itemsCount;
			float ratio = visible / total;
			total = _viewportHeight;
			visible = ratio * total;
			_scrollBarHeight = (int)visible;
			UpdateScroolPos();
		}
	}
	
	void PrepareDrawStructures(unsigned int maxWidth, unsigned int MaxHeight)
	{
		if (0 == _current)
			return;
		
		_itemsCount = 0;
		_maxItemHeight = 0;
		_totalMenuHeight = 0;
		for (unsigned char i = 0; i < _current->Size(); i ++)
		{
			MenuItem *item = _current->GetItem(i);
			if (item)
			{
				_itemsDrawingArray[i] = ItemDrawingStruct(maxWidth, MaxHeight, _smallFont, _bigFont, item->GetText(), strlen(item->GetText()), i, item->GetCallbackPtr(), item->GetValueStringCallbackPtr(), (MenuItemList *)item->Child);
				_itemsCount++;
				if (_current->Selected(i))
					_selected = i;
				if (_maxItemHeight < _itemsDrawingArray[i].ActualHeight)
					_maxItemHeight = _itemsDrawingArray[i].ActualHeight;
				_totalMenuHeight += _itemsDrawingArray[i].Height;
			}
		}
	}
	
	void Draw()
	{
		if (!Visible())
			return;
		
		Font *oldFont;
		
		display.Clear();
		
		
		char str[100];
		unsigned char len;
		
		unsigned int line = 0;
		if (_current)
		{
			len = sprintf(str, _current->GetHeader());
			display.MoveCursorTo((_maxLineWidth - len) / 2, line);
			display.WriteLine(str, len);
			line += _headerHeight;
			
			for (unsigned int x = 0; x < _maxLineWidth; x++)
			{
				display.InvertByte(x, 0);
				display.InvertByte(x, _headerHeight - 1);
			}
			
			unsigned int n = _itemsCount < _visibleItemsCount ? _itemsCount : _visibleItemsCount;
			for (unsigned char i = 0 + _scrollPos; i < n + _scrollPos; i ++)
			{
				oldFont = display.SelectFont(_itemsDrawingArray[i].SelectedFont);
				
				int line1y = line  + _itemsDrawingArray[i].Str1VerticalOffset;
				display.MoveCursorTo(_itemsDrawingArray[i].Str1Offset, line1y);
				display.WriteLine(_itemsDrawingArray[i].Str1, _itemsDrawingArray[i].Str1Len);
				if (_itemsDrawingArray[i].OnGetValueString)
				{
					char * value = _itemsDrawingArray[i].OnGetValueString();
					unsigned char valueLen = strlen(value);
					unsigned char charWidth = 1;
					if (_itemsDrawingArray[i].SelectedFont)
						charWidth = _itemsDrawingArray[i].SelectedFont->Width();
					int valueX = _maxLineWidth - valueLen * charWidth;
					if (_showScrollBar)
						valueX -= 1;
					display.MoveCursorTo(valueX, line1y);
					display.WriteLine(value, valueLen);
				}
					
				
				if (_itemsDrawingArray[i].TwoLines)
				{
					display.MoveCursorTo(_itemsDrawingArray[i].Str2Offset, line  + _itemsDrawingArray[i].Str2VerticalOffset);
					display.WriteLine(_itemsDrawingArray[i].Str2, _itemsDrawingArray[i].Str2Len);
				}
				
				display.SelectFont(oldFont);
				
				if (i == _selected)
				{
					for (unsigned int y = 0; y < _itemsDrawingArray[i].Height; y++)
					for (unsigned int x = 0; x < _itemsDrawingArray[i].Width; x++)
						display.InvertByte(x, y + line);
				}
				
				line += _itemsDrawingArray[i].Height;
			}
			if (_showScrollBar)
				DrawScrollBar();
		}
		
		display.Flush();
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
			display.WriteByte(_maxLineWidth - 1, y + _headerHeight, b);
		}
	}
	
	bool Visible()
	{
		return _visible;
	}
	
	void Show()
	{
		if (0 == _root)
			return;
		_visible = true;
		Enter(_root);
	}
	
	void Show(MenuItemListBase *menu)
	{
		if (0 != menu)
			_visible = true;
		Enter(menu);
	}
	
	void Hide()
	{
		_visible = false;
		Enter(0);
	}
	
	void SelectNext()
	{
		_selected++;
		if (_selected >= _itemsCount)
			_selected = 0;
		if (_current)
			_current->Select(_itemsDrawingArray[_selected].SrcIndex);
		UpdateScroolPos();
	}
	
	void SelectPrev()
	{
		_selected--;
		if (_selected < 0)
			_selected = _itemsCount - 1;
		if (_current)
			_current->Select(_itemsDrawingArray[_selected].SrcIndex);
		UpdateScroolPos();
	}
	
	void UpdateScroolPos()
	{
		if (!_showScrollBar)
		{
			_scrollPos = 0;
			return;
		}
		if (_selected >= _scrollPos + _visibleItemsCount)
		{
			_scrollPos = _selected - _visibleItemsCount + 1;
		}
		
		if (_selected < _scrollPos)
		{
			_scrollPos = _selected;
		}
		
		if (_scrollPos < 0)
			_scrollPos = 0;
		
		if (_scrollPos > _itemsCount - _visibleItemsCount)
			_scrollPos = _itemsCount - _visibleItemsCount;
		
		float elapsedItems = _itemsCount - _visibleItemsCount;
		float elapsedPixels = _viewportHeight - _scrollBarHeight;
		float pixelsByItem = elapsedPixels / elapsedItems;
		_scrollBarPos = (int)(_scrollPos * pixelsByItem);
	}
	
	void OnKeyDown(char key)
	{
		static const char KeyUp = '+';
		static const char KeyDown = '-';
		static const char KeyEnter = 13;
		static const char KeyEsc = 'E';
		switch(key)
		{
		case KeyEnter:
			if (Visible())
			{
				if (_itemsDrawingArray[_selected].OnClick)
				{
					MenuItemListBase *current = _current;
					Hide();
					_itemsDrawingArray[_selected].OnClick(current);
				}
				if (_itemsDrawingArray[_selected].Child)
					Enter(_itemsDrawingArray[_selected].Child);
			}
			break;
		case KeyEsc:
			if (Visible())
			{
				bool goParent = false;
				if (_current)
				{
					if (_current->Parent)
					{
						Enter(_current->Parent);
						goParent = true;
					}
				}
				if (!goParent)
					Hide();
			}
			break;
		case KeyDown:
			if (Visible())
				SelectNext();
			break;
		case KeyUp:
			if (Visible())
				SelectPrev();
			break;
		}
	}
};

template<class DisplayType, DisplayType &display>
typename Menu<DisplayType, display>::ItemDrawingStruct
Menu<DisplayType, display>::_itemsDrawingArray[Menu<DisplayType, display>::_maxVisibleItemsCount];

#endif