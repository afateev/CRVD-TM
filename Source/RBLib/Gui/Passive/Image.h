///////////////////////////////////////////////////////////////////////////////
//	Ёлемент пользовательского интерфейса
//	»зображение
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_CONTROLS_PASSIVE_IMAGE_H
#define GUI_CONTROLS_PASSIVE_IMAGE_H

#include "../ControlPassive.h"

namespace Rblib
{
namespace Gui
{
namespace Controls
{

class Image : public ControlPassive
{
public:
	typedef ControlPassive Base;
public:
	int ImageId;
public:
	Image()
	{
		ImageId = -1;
	}

	virtual void Draw(Skin *skin, ViewState &viewState)
	{
		if (skin)
		{
			viewState.ImageDump = skin->GetImageDump(ImageId);
		}

		Base::Draw(skin, viewState);
	}
};

}
}
}

#endif