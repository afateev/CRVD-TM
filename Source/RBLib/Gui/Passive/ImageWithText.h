///////////////////////////////////////////////////////////////////////////////
//	Элемент пользовательского интерфейса
//	Картинка с текстом
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
	int ImageWidth;		// Если -1, то ширина равна высоте
public:
	ImageWithText()
	{
		StringId = -1;
		IgnoreStringId = false;
		ImageWidth = -1;
	}

	virtual void Draw(Skin *skin, ViewState &viewState)
	{
		// сохраняем указатель на строку
		char *text = viewState.Text;
		// базовым методом рисовать текст не будем, нарисуем позже отдельно
		viewState.Text = 0;
		Base::Draw(skin, viewState);
		// восстанавливаем
		viewState.Text = text;
		
		if (skin)
		{
			// если не задано игнорирование идентификатора строки
			// то получем строку из скина
			// иначе текст уже должен быть задан через viewState
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