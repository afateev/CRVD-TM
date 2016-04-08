///////////////////////////////////////////////////////////////////////////////
//	Ёлемент управлени€ - соствна€ часть графического интерфейса пользовател€
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_CONTROL_H
#define GUI_CONTROL_H

#include "Skin.h"
#include "ButtonCode.h"

namespace Rblib
{
namespace Gui
{
	class Control
	{
	public:
		class NavigationInfo
		{
		public:
			Control * ToLeft;
			Control * ToRight;
			Control * ToUp;
			Control * ToDown;
		protected:
			Control *_current;
		public:
			NavigationInfo()
			{
				Reset();
			}

			void Reset()
			{
				_current = 0;
				ToLeft = 0;
				ToRight = 0;
				ToUp = 0;
				ToDown = 0;
			}

			Control *GetCurrent()
			{
				return _current;
			}

			void SetCurrent(Control *control)
			{
				if (_current != control)
				{
					if (_current)
					{
						_current->SetFocus(false);
					}

					if (control)
					{
						control->SetFocus(true);
					}
				}
				_current = control;
			}

			void CurrentSetFocus(bool focused)
			{
				if (_current)
				{
					_current->SetFocus(focused);
				}
			}
		};
	protected:
		int _viewType;
		bool _focused;
		bool _enabled;
		bool _visible;
	public:
		NavigationInfo Navigation;
	public:
		Control()
		{
			_viewType = 0;
			_focused = false;
			_enabled = true;
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

		void SetFocus(bool focused)
		{
			_focused = focused;
		}

		void SetEnabled(bool enabled)
		{
			_enabled = enabled;
		}

		bool IsEnabled()
		{
			return _enabled;
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
					viewState.Focused = _focused;
					viewState.Enabled = _enabled;
					viewState.Visible &= _visible;
					view->Draw(viewState);
				}
			}
		}

		virtual bool OnButtonClick(ButtonCode button)
		{
			bool processed = false;

			Control *current = Navigation.GetCurrent();
			if (current)
			{
				switch(button)
				{
				case Rblib::Gui::KeyLeft:
					{
						if (current->Navigation.ToLeft)
						{
							Navigation.SetCurrent(current->Navigation.ToLeft);
							processed = true;
						}
					}
					break;
				case Rblib::Gui::KeyRight:
					{
						if (current->Navigation.ToRight)
						{
							Navigation.SetCurrent(current->Navigation.ToRight);
							processed = true;
						}
					}
					break;
				case Rblib::Gui::KeyUp:
					{
						if (current->Navigation.ToUp)
						{
							Navigation.SetCurrent(current->Navigation.ToUp);
							processed = true;
						}
					}
					break;
				case Rblib::Gui::KeyDown:
					{
						if (current->Navigation.ToDown)
						{
							Navigation.SetCurrent(current->Navigation.ToDown);
							processed = true;
						}
					}
					break;
				}
				
				if (!processed && current->IsEnabled())
				{
					processed = current->OnButtonClick(button);
				}
			}

			return processed;
		}

		virtual void AnimationTick()
		{

		}
	};
}
}

#endif