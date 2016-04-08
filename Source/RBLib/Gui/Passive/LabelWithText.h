///////////////////////////////////////////////////////////////////////////////
//	������� ����������������� ����������
//	������� � �������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_CONTROLS_PASSIVE_LABELWITHTEXT_H
#define GUI_CONTROLS_PASSIVE_LABELWITHTEXT_H

#include "../ControlPassive.h"
#include "Text.h"

namespace Rblib
{
namespace Gui
{
namespace Controls
{

class LabelWithText : public ControlPassive
{
public:
	typedef ControlPassive Base;
public:
	Text Text;

	int StringId;
public:
	LabelWithText()
	{
		StringId = -1;
	}

	virtual void Draw(Skin *skin, ViewState &viewState)
	{
		// ��������� ��������� �� ������
		char *text = viewState.Text;
		int textLen = viewState.TextLen;
		
		// ������� ������� �������� ����� �� �����, �������� ����� ��������
		// ������� �������� ����� StringId
		if (skin)
		{
			viewState.Text = skin->GetString(StringId, &viewState.TextLen);
		}

		Base::Draw(skin, viewState);
		
		// ��������� ����� ����� viewState.Text
		ViewState viewStateText = viewState;
		viewStateText.Text = text;
		viewStateText.TextLen = textLen;
		viewStateText.TextHorizontalAlign = ViewState::TextAlingRight;
		Text.Draw(skin, viewStateText);
	}
};

}
}
}

#endif