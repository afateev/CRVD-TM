///////////////////////////////////////////////////////////////////////////////
//	Контейнер для элементов управления
//	Вертикальный список
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_CONTROLS_CONTAINERVERTICAL_H
#define GUI_CONTROLS_CONTAINERVERTICAL_H

#include "../Control.h"
#include "Scroll.h"

namespace Rblib
{
namespace Gui
{
namespace Controls
{

template
	<
		int maxItemsCount
	>
class ContainerVertical : public Control
{
public:
	typedef Rblib::Gui::Control Base;
	static const int MaxItemsCount = maxItemsCount;
	typedef Control * ItemType;
	typedef ItemType ContainerType[MaxItemsCount];
protected:
	ContainerType _items;
	Scroll _scroll;

	int _count;
	int _itemHeigth;
	int _firstItem;
	int _selectedItem;

	static const int _scrollWidth = 1;

public:
	ContainerVertical() : _firstItem(0), _selectedItem(0)
	{
		_count = 0;
		_itemHeigth = 20;
		Clear();
	}

	inline unsigned int GetSelectedItem()
	{
		return _selectedItem;
	}

	inline void SetSelectedItem(int item)
	{
		_selectedItem = item;
		Navigation.SetCurrent(_items[_selectedItem]);
	}

	void Clear()
	{
		for (int i = 0; i < _count; i++)
		{
			_items[i]->Navigation.Reset();
		}
		_count = 0;
		_firstItem = 0;
		_selectedItem = 0;
	}

	void InsertItem(int insertIndex, ItemType item)
	{
		if (insertIndex > _count)
		{
			insertIndex = _count;
		}

		if (_count >= MaxItemsCount || !item)
		{
			return;
		}

		for (int i = _count; i > insertIndex; i--)
		{
			_items[i] = _items[i - 1];
		}

		_items[insertIndex] = item;
		_count++;
		UpdateNavigationLinks();
	}

	inline void AddItem(ItemType item)
	{
		InsertItem(_count, item);
	}

	void DeleteItem(int itemIndex)
	{
		if (itemIndex < 0 || itemIndex >= _count)
		{
			return;
		}
		
		_items[itemIndex].Navigation.Reset();
		
		for (int i = itemIndex; i < _count - 1; i++)
		{
			_items[i] = _items[i + 1];
		}
		_count--;
		UpdateNavigationLinks();
	}

	inline int ItemsCount()
	{
		return _count;
	}

	virtual void Draw(Rblib::Gui::Skin *skin, Rblib::Gui::ViewState &viewState)
	{
		Base::Draw(skin, viewState);

		bool scrollVisible = UpdateScroll(viewState.Height());

		Rblib::Gui::ViewState viewStateItems(viewState);
		
		if (scrollVisible)
			viewStateItems.Right -= _scrollWidth + 1;

		int itemsToDraw = ItemsToDraw(viewStateItems.Height());

		for (int i = 0; i < itemsToDraw; i++)
		{
			Rblib::Gui::ViewState viewStateChild = viewStateItems;

			int heigth = _itemHeigth;
			int spaces = 1;
			viewStateChild.Top += (heigth + spaces) * i;
			viewStateChild.Bottom = viewStateChild.Top + heigth;
			_items[i + _firstItem]->Draw(skin, viewStateChild);
		}

		if (scrollVisible)
		{
			Rblib::Gui::ViewState viewStateScroll(viewState);
			viewStateScroll.Left = viewStateItems.Right + 2;
			_scroll.Draw(skin, viewStateScroll);
		}
	}

	virtual bool OnButtonClick(Rblib::Gui::ButtonCode button)
	{
		if (button == Rblib::Gui::KeyUp)
			UpdateSelected(true);
		if (button == Rblib::Gui::KeyDown)
			UpdateSelected(false);

		return Base::OnButtonClick(button);
	}

	void SetItemHeihgt(int itemHeight)
	{
		_itemHeigth = itemHeight;
	}
protected:
	void UpdateNavigationLinks()
	{
		for (int i = 0; i < _count; i++)
		{
			_items[i]->Navigation.Reset();
			
			if (i < _count - 1)
			{
				_items[i]->Navigation.ToDown = _items[i + 1];
			}

			if (i > 0)
			{
				_items[i]->Navigation.ToUp = _items[i - 1];
			}
		}

		if (_count > 0)
		{
			Navigation.SetCurrent(_items[0]);
		}
		else
		{
			Navigation.SetCurrent(0);
		}
	}

	bool UpdateScroll(int viewHeight)
	{
		int visibleItems = VisibleItemsUnbroken(viewHeight);

		if (_selectedItem < _firstItem)
			_firstItem = _selectedItem;
		else if (_selectedItem >= _firstItem + visibleItems)
			_firstItem += _selectedItem - _firstItem - visibleItems + 1;

		_scroll.Set(_count, visibleItems, _firstItem);

		if (_count > visibleItems)
			return true;

		return false;
	}

	void UpdateSelected(bool up)
	{
		if (up)
		{
			if (_selectedItem > 0)
				_selectedItem--;
		}
		else
		{
			if (_selectedItem < _count - 1)
				_selectedItem++;
		}
	}

	inline int VisibleItemsUnbroken(int viewHeight)
	{
		return viewHeight / _itemHeigth;
	}

	inline int VisibleItems(int viewHeight)
	{
		return VisibleItemsUnbroken(viewHeight) + ((viewHeight % _itemHeigth) ? 1 : 0);
	}

	inline int ItemsToDraw(int viewHeight)
	{
		int itemsToDraw = _count - _firstItem;
		int visibleItems = VisibleItems(viewHeight);
		if (visibleItems < itemsToDraw)
			return visibleItems;
		return itemsToDraw;
	}

};

}
}
}

#endif