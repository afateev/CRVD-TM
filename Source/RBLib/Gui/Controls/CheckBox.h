
#ifndef GUI_CONTROLS_CHECKBOX_H
#define GUI_CONTROLS_CHECKBOX_H

#include "../Control.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Gui
{
namespace Controls
{
//
// базовый контрол
// загружает строку по id
//
class CheckBoxBase : public Control
{
public:

	typedef Control Base;

public:

	CheckBoxBase()
	: _checked(false)
	, _stringId(-1) // по умолчанию будет выводиться строка из буфера
	{
	}

	inline bool Checked()
	{
		return _checked;
	}

	inline void SetChecked(bool checked)
	{
		_checked = checked;
	}
	
	inline void SetChecked()
	{
		_checked = true;
	}

	inline void SetUnchecked()
	{
		_checked = false;
	}
	
	inline void SetStringId(int stringId)
	{
		_stringId = stringId;
	}

	inline int StringId()
	{
		return _stringId;
	}

	virtual void Draw(Skin *skin, ViewState &viewState)
	{
		if (skin)
		{
			if (_stringId >= 0) // загружаем строку по id
				viewState.Text = skin->GetString(_stringId, &viewState.TextLen); 
		}

		viewState.Checked = _checked;

		Base::Draw(skin, viewState);
	}

	virtual bool OnButtonClick(Rblib::Gui::ButtonCode button)
	{
		bool processed = Base::OnButtonClick(button);

		if (processed || button != Rblib::Gui::KeyCenter)
			return processed;

		_checked = !_checked;

		return false;
	}

protected:

	signed int _stringId;
	bool _checked;
};

//
// расширенный чекбокс, умеющий рисовать готовую строку из буфера с заданной макс. длинной
//
template<unsigned int MaxTextBufferLength>
class CheckBox : public CheckBoxBase
{

	typedef CheckBoxBase Base;

public:

	inline bool SetString(char* str, unsigned int len)
	{
	  	if (len > MaxTextBufferLength)
			return false;

		_stringId = -1;

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
		if (skin)
		{
			if (_stringId < 0) // подставляем готовую строку
			{
				viewState.Text = _stringBuffer;
				viewState.TextLen = _stringLength;
			}
		}

		Base::Draw(skin, viewState);
	}
	
protected:
  
  char _stringBuffer[MaxTextBufferLength];
  unsigned int _stringLength;

};

// 
// чекбокс с 0-й длиной буфера ведёт себя как базовый
//
template<>
class CheckBox<0> : public CheckBoxBase
{
};

}
}
}

#endif // GUI_CONTROLS_CHECKBOX_H
