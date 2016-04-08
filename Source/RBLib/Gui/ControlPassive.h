///////////////////////////////////////////////////////////////////////////////
//	Пассивный элемент интерфейса, не воспринимает действия пользователя
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_CONTROLPASSIVE_H
#define GUI_CONTROLPASSIVE_H

#include "Skin.h"

namespace Rblib
{
namespace Gui
{
	class ControlPassive
	{
	protected:
		int _viewType;
		bool _visible;
	public:
		ControlPassive()
		{
			_viewType = 0;
			_visible = true;
		}

		int ViewType()
		{
			return _viewType;
		}

		void SetViewType(int viewType)
		{
			_viewType = viewType;
		}

		void SetVisible(bool visible)
		{
			_visible = visible;
		}

		bool IsVisible()
		{
			return _visible;
		}

		virtual void Draw(Skin *skin, ViewState &viewState)
		{
			if (skin)
			{
				View * view = skin->GetView(ViewType());
				if (view)
				{
					viewState.Skin = skin;
					//viewState.Focused = false;
					viewState.Visible &= _visible;
					view->Draw(viewState);
				}
			}
		}

		virtual void AnimationTick()
		{

		}
	};
}
}

#endif