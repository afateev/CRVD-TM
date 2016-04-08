////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		22/01/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_CONTROLLER_NOMINAL_STATOR_CURRENT_H
#define WINDOW_CONTROLLER_NOMINAL_STATOR_CURRENT_H

#include "Window.h"
#include "ControlTextInput.h"
#include "ControlButton.h"

template<class DisplayType, DisplayType &display, class ActiveDriveControllerParams>
class WindowControllerNominalStatorCurrent : public Window<DisplayType, display>
{
protected:
	static const unsigned int wndWidth = 40;
	
	static const unsigned int _applyId = 0;
	static const unsigned int _cancelId = 1;
	
	Font *_bigFont;
    ControlTextInput<3> _newValue;
	ControlButton _apply;
	ControlButton _cancel;
public:
	WindowControllerNominalStatorCurrent(Font *bigFont)
	{
		_bigFont = bigFont;
        _newValue.SetParent(this);
		_apply.SetParent(this);
		_apply.SetId(_applyId);
		_cancel.SetParent(this);
		_cancel.SetId(_cancelId);
	}
	
	virtual void Draw()
	{
		char str[100];
		unsigned char len;
		
		len = sprintf(str, "Параметры регулятора");
		display.MoveCursorTo((wndWidth - len) / 2, 0);
		display.WriteLine(str, len);
		
		len = sprintf(str, "Номинальный ток статора");
		display.MoveCursorTo((wndWidth - len) / 2, 16);
		display.WriteLine(str, len);
		
		display.MoveCursorTo(1, 48);
		display.WriteLine("Текущее значение:");
		
		int curValue = ActiveDriveControllerParams::GetNominalStatorCurrent();
        
        Font *oldFont = display.SelectFont(_bigFont);
        len = sprintf(str, "%3.3d", curValue);
		display.MoveCursorTo(1, 48 + 16);
		display.WriteLine(str, len);
        display.SelectFont(oldFont);
		
		display.MoveCursorTo(1, 48 + 16 + 16 + 32);
		display.WriteLine("Новое значение:");
		
        oldFont = display.SelectFont(_bigFont);
		_newValue.SetPos(1, 48 + 16 + 16 + 32 + 16);
        _newValue.Draw(display);
		display.SelectFont(oldFont);
        
		oldFont = display.SelectFont(_bigFont);
		_apply.Draw(display);
		_cancel.Draw(display);
		display.SelectFont(oldFont);
	}
    
    virtual bool OnKeyDown(char &key)
	{
		if (_newValue.OnKeyDown(key))
			return true;
		if (_apply.OnKeyDown(key))
			return true;
		if (_cancel.OnKeyDown(key))
			return true;
		return false;
	}
    
    virtual void OnShow()
	{
        char str[100];
        int curValue = ActiveDriveControllerParams::GetNominalStatorCurrent();
        sprintf(str, "%3.3d", curValue);
        
        _newValue.SetFocus();
        _newValue.SetText(str);
		_newValue.SetNext(&_apply);
		_apply.SetNext(&_cancel);
		_cancel.SetNext(&_newValue);
		_apply.SetText("Применить");
		_cancel.SetText("Отмена");
		_apply.SetPos(0, 239 - 32);
		_cancel.SetPos(40 - 14, 239 - 32);
    }
    
    virtual void OnClick(unsigned int controlId, char key)
	{
		if (_applyId == controlId)
		{
			if (13 == key)
			{
				int newValue = _newValue.GetIntValue();
                ActiveDriveControllerParams::SetNominalStatorCurrent(newValue);
				//WindowBase::Close();
			}
			if ('E' == key)
			{
				WindowBase::Close();
			}
		}
		if (_cancelId == controlId)
		{
			if (13 == key || 'E' == key)
			{
				WindowBase::Close();
			}
		}
	}
};

#endif
