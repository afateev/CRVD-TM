///////////////////////////////////////////////////////////////////////////////
//	������� ����������������� ����������
//	�������� � �������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_CONTROLS_PASSIVE_IMAGEWITHTEXT_H
#define GUI_CONTROLS_PASSIVE_IMAGEWITHTEXT_H

#include "../ControlPassive.h"
#include "Image.h"
#include "Text.h"

namespace Rblib
{
namespace Gui
{
namespace Controls
{

class ImageWithText : public ControlPassive
{
public:
	typedef ControlPassive Base;
public:
	Image Image;
	Text Text;

	int StringId;
	bool IgnoreStringId;
	int ImageWidth;		// ���� -1, �� ������ ����� ������
public:
	ImageWithText()
	{
		StringId = -1;
		IgnoreStringId = false;
		ImageWidth = -1;
	}

	virtual void Draw(Skin *skin, ViewState &viewState)
	{
		// ��������� ��������� �� ������
		char *text = viewState.Text;
		// ������� ������� �������� ����� �� �����, �������� ����� ��������
		viewState.Text = 0;
		Base::Draw(skin, viewState);
		// ���������������
		viewState.Text = text;
		
		if (skin)
		{
			// ���� �� ������ ������������� �������������� ������
			// �� ������� ������ �� �����
			// ����� ����� ��� ������ ���� ����� ����� viewState
			if (!IgnoreStringId)
			{
				viewState.Text = skin->GetString(StringId, &viewState.TextLen);
			}
		}

		int height = viewState.Bottom - viewState.Top;
		int imageWidth = ImageWidth > -1 ? ImageWidth : height;

		ViewState viewStateImage = viewState;
		viewStateImage.Right = viewStateImage.Left + imageWidth;

		Image.Draw(skin, viewStateImage);

		ViewState viewStateText = viewState;
		viewStateText.Left += imageWidth;
		Text.Draw(skin, viewStateText);
	}
};

}
}
}

#endif