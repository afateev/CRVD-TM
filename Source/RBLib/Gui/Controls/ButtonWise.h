///////////////////////////////////////////////////////////////////////////////
//	Элемент пользовательского интерфейса
//	Кнопка, умеющая хранить отображаемый текст
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_CONTROLS_PASSIVE_BUTTON_WISE_H
#define GUI_CONTROLS_PASSIVE_BUTTON_WISE_H

#include "Button.h"

namespace Rblib
{
namespace Gui
{
namespace Controls
{

template<unsigned int MaxTextBufferLength = 0>
class ButtonWise : public Button
{
public:
	typedef Button Base;
public:

	ButtonWise()
	: _stringLength(0)
	{
	}

	inline bool SetString(char* str, unsigned int len)
	{
		if (len > MaxTextBufferLength)
			return false;
			
		StringId = -1;

		strncpy(_stringBuffer, str, len);
		_stringLength = len;

		return true;
	}

	inline char* String()
	{
		return _stringBuffer;
	}

	inline unsigned int StringLength()
	{
		return _stringLength;
	}

	virtual void Draw(Skin *skin, ViewState &viewState)
	{
		viewState.Text = _stringBuffer;
		viewState.TextLen = _stringLength;

		Base::Draw(skin, viewState);
	}

protected:

	char _stringBuffer[MaxTextBufferLength];
	unsigned int _stringLength;

};

// Если размер буфера 0, то эквивалентна простой Button
template<>
class ButtonWise<0> : public Button
{
};

}
}
}

#endif // GUI_CONTROLS_PASSIVE_BUTTON_WISE_H
