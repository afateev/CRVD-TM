///////////////////////////////////////////////////////////////////////////////
//	Элемент управления
//	Кнопка
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_CONTROLS_BUTTON_H
#define GUI_CONTROLS_BUTTON_H

#include "../Control.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Gui
{
namespace Controls
{

class Button : public Control
{
public:
	typedef Control Base;
	typedef CallbackWrapper<> OnClickCallbackType;
public:
	int StringId;
	bool WaitFlag;
	OnClickCallbackType OnClickCallback;
public:
	Button()
	{
		StringId = -1;
		WaitFlag = false;
	}

	virtual void Draw(Skin *skin, ViewState &viewState)
	{
		if (skin && StringId >= 0)
		{
			viewState.Text = skin->GetString(StringId, &viewState.TextLen);
		}

		viewState.WaitFlag = WaitFlag;
		
		Base::Draw(skin, viewState);
	}

	virtual bool OnButtonClick(Rblib::Gui::ButtonCode button)
	{
		bool processed = Base::OnButtonClick(button);


		if (!processed)
		{
			switch(button)
			{
			case Rblib::Gui::KeyOk:
				{
					OnClickCallback.Call();
					processed = true;
				}
				break;
			}
		}

		return processed;
	}
};

}
}
}

#endif