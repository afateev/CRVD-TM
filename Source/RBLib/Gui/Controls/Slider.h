///////////////////////////////////////////////////////////////////////////////
//	Элемент управления
//	Бегунок
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_CONTROLS_SLIDER_H
#define GUI_CONTROLS_SLIDER_H

#include "../Control.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Gui
{
namespace Controls
{

class Slider : public Control
{
public:
	typedef Control Base;
	typedef CallbackWrapper<float> OnClickCallbackType;
public:
	float Min;
	float Max;
	float Step;
	float Value;
	bool NowSetting;
	char *StringFormat;
	OnClickCallbackType OnClickCallback;
public:
	Slider()
	{
		Min = 0.0;
		Max = 1.0;
		Step = 0.1;
		Value = 0.0;
		NowSetting = false;
		StringFormat = "%5.1f";
	}

	virtual void Draw(Skin *skin, ViewState &viewState)
	{
		char tmp[32];
		viewState.FloatingValue = Value / (Max - Min);
		viewState.WaitFlag = NowSetting;
		viewState.Text = tmp;
		viewState.TextLen = sprintf(tmp, StringFormat, Value);
		Base::Draw(skin, viewState);
	}

	virtual bool OnButtonClick(Rblib::Gui::ButtonCode button)
	{
		bool processed = Base::OnButtonClick(button);


		if (!processed)
		{
			switch(button)
			{
			case Rblib::Gui::KeyLeft:
				{
					StepLeft();
				}
				break;
			case Rblib::Gui::KeyRight:
				{
					StepRight();
				}
				break;
			case Rblib::Gui::KeyOk:
				{
					OnClickCallback.Call(Value);
					processed = true;
				}
				break;
			}
		}

		return processed;
	}
protected:
	void StepLeft()
	{
		Value -= Step;
		Validate();
	}

	void StepRight()
	{
		Value += Step;
		Validate();
	}

	void Validate()
	{
		if (Value > Max)
		{
			Value = Max;
		}

		if (Value < Min)
		{
			Value = Min;
		}
	}
};

}
}
}

#endif