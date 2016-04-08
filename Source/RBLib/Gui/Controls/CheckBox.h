
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
// ������� �������
// ��������� ������ �� id
//
class CheckBoxBase : public Control
{
public:

	typedef Control Base;

public:

	CheckBoxBase()
	: _checked(false)
	, _stringId(-1) // �� ��������� ����� ���������� ������ �� ������
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
			if (_stringId >= 0) // ��������� ������ �� id
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
// ����������� �������, ������� �������� ������� ������ �� ������ � �������� ����. �������
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
			if (_stringId < 0) // ����������� ������� ������
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
// ������� � 0-� ������ ������ ���� ���� ��� �������
//
template<>
class CheckBox<0> : public CheckBoxBase
{
};

}
}
}

#endif // GUI_CONTROLS_CHECKBOX_H
