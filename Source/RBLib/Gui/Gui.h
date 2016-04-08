///////////////////////////////////////////////////////////////////////////////
//	Графический интерфейс пользователя
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_H
#define GUI_H

#include "ButtonCode.h"
#include "Control.h"
#include "../Utils/vector_static.h"
#include "../EventDispetcher.h"
#include "Skin.h"
#include "Controls.h"

namespace Rblib
{
namespace Gui
{
	// коды сообщений GUI
	enum EventCode
	{
		EventButtonDown = 0,
		EventButtonUp,
		EventButtonClick
	};
	
	template<class Gdi>
	class Gui
	{
	public:
		static bool IgnoreEvents;
	protected:
		static bool _inited;
		static Skin *_skin;
		static Control *_rootControl;
	public:
		static void Init()
		{
			if (!_inited)
			{
				Singletone<EventDispetcher>::Instance().AddHandler(EventGui, EventDispetcher::EventFunctor(EventHandler), false);
			}
			_inited = true;
		}

		static void SetSkin(Skin *skin)
		{
			_skin = skin;
		}

		static void SetRootControl(Control *control)
		{
			_rootControl = control;
		}
				
		static void Draw()
		{
			if (_rootControl)
			{
				ViewState viewState;
				viewState.Skin = _skin;
				viewState.Left = 0;
				viewState.Right = 319;
				viewState.Top = 0;
				viewState.Bottom = 239;
				viewState.Visible = _rootControl->IsVisible();
				_rootControl->Draw(_skin, viewState);
			}
		}

		static void AnimationTick()
		{
			if (_rootControl)
			{
				_rootControl->AnimationTick();
			}
		}
	protected:
		static View *GetView(int id)
		{
			if (_skin)
			{
				return _skin->GetView(id);
			}

			return 0;
		}

		static void EventHandler(const Rblib::Event &e)
		{
			if (IgnoreEvents)
			{
				return;
			}
			
			switch (e.Code)
			{
			// события GUI
			case EventGui:
				{
					EventParam1Type guiEventCode = e.Param1;
					switch (guiEventCode)
					{
					// клик по кнопке
					case EventButtonClick:
						{
							// код кнопки
							ButtonCode buttonCode = (ButtonCode)e.Param2;
							OnButtonClick(buttonCode);
						}
						break;
					}
				}
				break;
			}
		}

		static void OnButtonClick(ButtonCode button)
		{
			if (_rootControl)
			{
				if (_rootControl->IsEnabled())
				{
					_rootControl->OnButtonClick(button);
				}
			}
		}
	};

	template<class Gdi>
	bool Gui<Gdi>::IgnoreEvents = false;
	
	template<class Gdi>
	bool Gui<Gdi>::_inited = false;

	template<class Gdi>
	Skin *Gui<Gdi>::_skin = 0;

	template<class Gdi>
	Control *Gui<Gdi>::_rootControl = 0;
}
}

#endif