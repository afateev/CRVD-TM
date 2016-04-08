
#ifndef GUI_CONTROLS_SCROLL_H
#define GUI_CONTROLS_SCROLL_H

#include "../Control.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Gui
{
namespace Controls
{

class Scroll : public Control
{
public:

	typedef Control Base;

public:

	Scroll()
	: _max(1)
	, _visible(1)
	, _current(0)
	{
	}

	inline void SetMax(int max)
	{
		if (max == 0)
			return;
		_max = max;
	}

	inline void SetVisible(int visible)
	{
		_visible = visible;
	}

	inline void SetCurrent(int current)
	{
		_current = current;
	}

	inline void Set(int max, int visible, int current)
	{
		_max = max;
		_visible = visible;
		_current = current;
	}

	virtual void Draw(Skin *skin, ViewState &viewState)
	{
		ViewState viewStateScroll = viewState;

		float stepHeight = (float)viewState.Height() / _max;
		int scrollHeight = (int)(stepHeight * _visible);
		int scrollTop = (int)(stepHeight * _current);
	
		viewStateScroll.Top = viewStateScroll.Top + scrollTop;
		viewStateScroll.Bottom = viewStateScroll.Top + scrollHeight;
		
		SetFocus(true); // чтобы рисовалось €рким цветом

		Base::Draw(skin, viewStateScroll);
	}

protected:

	int _max;
	int _visible;
	int _current;
};

}
}
}

#endif // GUI_CONTROLS_SCROLL_H
