
#ifndef GUI_CONTROLS_IDLIST_H
#define GUI_CONTROLS_IDLIST_H

#include "../Control.h"
#include "Scroll.h"
#include "ButtonWise.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Gui
{
namespace Controls
{

// список для отображения целых чисел (максимум 2^32 элемента)
// данные запрашиваются по мере необходимости, должен быть задан соответствующий callback
// параметр шаблона - количество элементов, которое можно хранить в самом объекте класса
template<unsigned long MaxBufferedItemsCount>
class IdList : public Control
{
protected:

	typedef Rblib::Gui::Control Base;

public:

	typedef Rblib::CallbackWrapper<unsigned long, unsigned long*, unsigned long, unsigned long, unsigned long&> LoadDataCallbackType;
	LoadDataCallbackType LoadDataCallback;

public:

	IdList()
	: _itemsCount(0)
	, _itemFirst(0)
	, _itemSelected(0)
	, _itemsPosition(0)
	, _visibleControlsCount(0)
	, _scrollVisible(false)
	{
		_prefix[0] = 0;
		_postfix[0] = 0;
	}

	void Update(unsigned long id)
	{
		_dataId = id;

		if (!LoadData())
			return;
		UpdateControls();
		UpdateNavigationLinks();
	}

	void SetPrefix(char* prefix)
	{
		strncpy(_prefix, prefix, _maxFixLen);
		_prefix[_maxFixLen] = 0;
	}

	void SetPostfix(char* postfix)
	{
		strncpy(_postfix, postfix, _maxFixLen);
		_postfix[_maxFixLen] = 0;
	}

	virtual void Draw(Rblib::Gui::Skin *skin, Rblib::Gui::ViewState &viewState)
	{
		Rblib::Gui::ViewState viewStateItems(viewState);

		viewStateItems.Right -= (_scrollVisible) ? (_scrollWidth + 1) : 0;

		for (unsigned short i = 0 ; i < _visibleControlsCount; i++)
		{
			Rblib::Gui::ViewState viewStateChild = viewStateItems;

			int heigth = _itemHeigth;
			int spaces = 1;
			viewStateChild.Top += (heigth + spaces) * i;
			viewStateChild.Bottom = viewStateChild.Top + heigth;

			_controls[i].Draw(skin, viewStateChild);
		}

		if (_scrollVisible)
		{
			Rblib::Gui::ViewState viewStateScroll(viewState);
			viewStateScroll.Left = viewStateItems.Right + 2;
			_scroll.Draw(skin, viewStateScroll);
		}
	}

	virtual bool OnButtonClick(Rblib::Gui::ButtonCode button)
	{
		if (button == Rblib::Gui::KeyUp || button == Rblib::Gui::KeyDown)
		{
			bool selectionChanged = UpdatePosition(button == Rblib::Gui::KeyUp);
			UpdateControls();

			if (!selectionChanged) // если селект не должен сдвинуться, выходим
				return true;
		}

		return Base::OnButtonClick(button);
	}

	void SetItemViewType(int viewType)
	{
		for (unsigned char i = 0; i < _controlsCount; i++)
			_controls[i].SetViewType(viewType);
	}

	inline unsigned long GetSelectedId()
	{
		return (_itemsCount > _itemSelected) ? _items[_itemSelected] : 0;
	}

protected:

	bool UpdatePosition(bool up)
	{
		unsigned long selectedPrev = _itemSelected;
		unsigned long firstPrev = _itemFirst;

		UpdateSelected(up);
		UpdateFirst();

		if (!NeedDataUpdate())
			return true;

		if (LoadData() && _itemSelected < _itemsCount + _itemsPosition)
			return true;

		_itemFirst = firstPrev;
		_itemSelected = selectedPrev;

		return false;
	}

	inline void UpdateSelected(bool up)
	{
		if (up == false)
			_itemSelected++;
		else if (_itemSelected > 0)
			_itemSelected--;
	}

	inline void UpdateFirst()
	{
		if (_itemSelected < _itemFirst)
			_itemFirst = _itemSelected;
		else if (_itemSelected >= (_itemFirst + _controlsCount))
			_itemFirst += _itemSelected - _itemFirst - _controlsCount + 1;
	}

	inline bool NeedDataUpdate() const
	{
		return _itemSelected < _itemsPosition || _itemSelected >= _itemsPosition + _itemsCount;
	}

	unsigned long LoadData()
	{
		unsigned long indexDelta = ( MaxBufferedItemsCount - _controlsCount) / 2; //
		unsigned long position = (_itemFirst > indexDelta) ? _itemFirst - indexDelta : 0; // позиция, с которой просим элементы

		unsigned long itemsCount;
		LoadDataCallback.Call(_dataId, _items, position, MaxBufferedItemsCount, itemsCount);

		if (itemsCount) // если успешно, обновляем
		{
			_itemsCount = itemsCount;
			_itemsPosition = position;
		}

		return itemsCount;
	}

	void UpdateControls()
	{
		_visibleControlsCount = 0;

		for (int i = 0; i < _controlsCount; i++)
		{
			if (i + _itemFirst - _itemsPosition >= _itemsCount)
				return;

			int len = sprintf(_strBuffer, "%s%u%s", _prefix, _items[i + _itemFirst - _itemsPosition], _postfix);
			_controls[i].SetString(_strBuffer, len);
			_visibleControlsCount++;
		}

		UpdateScroll();
	}

	inline void UpdateScroll()
	{
		_scrollVisible = _visibleControlsCount < _itemsPosition + _itemsCount;
		_scroll.Set(_itemsPosition + _itemsCount, _visibleControlsCount, _itemFirst);
	}

	void UpdateNavigationLinks()
	{
		for (int i = 0; i < _controlsCount; i++)
		{
			_controls[i].Navigation.Reset();

			if (i < _controlsCount - 1)
				_controls[i].Navigation.ToDown = &_controls[i + 1];

			if (i > 0)
				_controls[i].Navigation.ToUp = &_controls[i - 1];
		}

		if (_controlsCount > 0)
		{
			Navigation.SetCurrent(&_controls[0]);
		}
		else
		{
			Navigation.SetCurrent(0);
		}
	}

protected:

	static const int _itemHeigth = 20;
	static const int _scrollWidth = 1;

	unsigned long _dataId;

	// буфер с порцией данных об элементах списка
	unsigned long _items[MaxBufferedItemsCount];
	unsigned long _itemsCount;

	// абсолютные индексы!
	unsigned long _itemFirst; // индекс первого видимого элемена
	unsigned long _itemSelected; // индекс выделенного элемента
	unsigned long _itemsPosition; // позиция (минимальный индекс) текущей порции элементов

	// короткий список контролов для отображения элементов списка
	static const unsigned char _controlsCount = 9;
	ButtonWise<15> _controls[_controlsCount];
	unsigned char _visibleControlsCount;

	// контролл дла скроллинга
	Scroll _scroll;
	bool _scrollVisible;

	// внутренний буфер для разных нужд
	char _strBuffer[15];

	static const unsigned char _maxFixLen = 4;
	char _prefix[_maxFixLen + 1];
	char _postfix[_maxFixLen + 1];
};

}
}
}

#endif // GUI_CONTROLS_IDLIST_H
