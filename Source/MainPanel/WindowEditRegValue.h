////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		22/01/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_EDIT_REG_VALUE_H
#define WINDOW_EDIT_REG_VALUE_H

#include "Window.h"
#include "ControlTextInput.h"
#include "ControlButton.h"

template<unsigned char IntPart, unsigned char FracPart>
class ControlParamInput : public Control
{
protected:
	ControlTextInput<IntPart> _intPart;
	ControlTextInput<(IntPart > 0 ? FracPart : 0)> _fracPart;
    bool _boolValue;
public:
    ControlParamInput()
	{
		SetParent(_parent);
		if (FracPart && IntPart)
            _intPart.SetNext(&_fracPart);
	}
	
	virtual ~ControlParamInput()
	{
	}
	
	virtual void SetParent(WindowBase *parent)
	{
		_intPart.SetParent(_parent);
		_fracPart.SetParent(_parent);
	}
    
    virtual void SetText(char *text)
	{
		if (0 == text)
			return;
        
        if (IsBool())
        {
            if (BoolType() == 0)
			{
				if (text[1] == 'ы' || text[1] == 'Ы')
					_boolValue = 0;
				else
					_boolValue = 1;
			}
			else if (BoolType() == 1)
			{
				if (text[1] == 'd' || text[1] == 'D')
					_boolValue = 0;
				else
					_boolValue = 1;
			}
        }
        
        char tmpInt[IntPart + 1];
        char tmpFrac[FracPart + 1];
        unsigned int pointpos = 0;
        for (int i = 0; i < strlen(text); i++)
        {
            if (text[i] == '.')
            {
                pointpos = i;
                break;
            }
        }
        if (pointpos)
        {
            int skip = IntPart - pointpos;
            if (skip < 0)
                skip = 0;
            int copy = pointpos;
            if (copy > IntPart)
                copy = IntPart;
            for (int i = 0; i < skip; i++)
                tmpInt[i] = ' ';
            for (int i = 0; i < copy; i++)
            {
                tmpInt[skip + i] = text[i];
            }
            
            if (IntPart)
                _intPart.SetText(tmpInt);
            
            copy = strlen(text) - pointpos;
            if (copy > FracPart)
                copy = FracPart;
            for (int i = 0; i < copy; i++)
            {
                tmpFrac[i] = text[pointpos + 1 + i];
            }
            tmpFrac[copy] = 0;
            
            if (FracPart && IntPart)
                _fracPart.SetText(tmpFrac);
        }
        else
        {
            _intPart.SetText(&text[0]);
        }
	}
    
    virtual void Draw(Display &display)
	{
		Font *font = display.GetCurrentFont();
		unsigned char charWidth = 1;
		unsigned char charHeight = 16;
		if (font)
		{
			charWidth = font->Width();
			charHeight = font->Height();
		}
		
		_intPart.SetPos(_x + 0, _y);
		_fracPart.SetPos(_x + (IntPart + 1) * charWidth, _y);
		
		if (IntPart)
        {
            _intPart.Draw(display);
        }
		if (FracPart && IntPart)
        {
            display.MoveCursorTo(_x + IntPart * charWidth, _y);
	    	display.WriteLine(".");
		    _fracPart.Draw(display);
        }
        
        if (IsBool())
        {
            display.MoveCursorTo(_x + 0, _y);
	    	if (BoolType() == 0)
			{
				display.WriteLine(_boolValue ? "вкл" : "выкл");
			}
			else if (BoolType() == 1)
			{
				display.WriteLine(_boolValue ? "Even" : "Odd");
			}
            if (Focused())
		    {
			    for(unsigned char y = 0 ; y < charHeight; y++)
			    for(unsigned char x = 0 ; x < 4 * charWidth; x++)
			    {
				    display.InvertByte(_x + x, _y + y);
			    }
		    }
        }
	}
    
    virtual bool OnKeyDown(char &key)
	{
		if (!Focused())
			return false;
		
		bool res = false;
		if (!res)
			res |= _intPart.OnKeyDown(key);
		if (!res)
		{
			if (key != '+' && key != '-')
			{
				res |= _fracPart.OnKeyDown(key);
			}
			else
			{
				res = false;
			}
		}
        if (!res && IsBool())
        {
            if (13 == key || 'S' == key)
		    {
			    if (_next)
				    _next->SetFocus();
			    return true;
		    }
            if (key == '+' || key == '-')
            {
                _boolValue = !_boolValue;
                return true;
            }
        }
		return res;
	}
    
    virtual bool IsFloat()
    {
        return FracPart && IntPart;
    }
    
    virtual bool IsBool()
    {
        return IntPart == 0;
    }
	
	virtual unsigned char BoolType()
    {
        return (unsigned char)FracPart;
    }
    
    virtual unsigned int GetIntValue()
    {
        return _intPart.GetIntValue();
    }
    
    virtual float GetFloatValue()
    {
        float intPart = _intPart.GetIntValue();
		float sign = intPart < 0 ? -1.0 : 1.0;
		if (intPart == 0)
		{
			if (_intPart.HasMinusSign())
			{
				sign = -1.0;
			}
		}
        float fracPart = 0;
        if (FracPart && IntPart)
        {
            fracPart = _fracPart.GetIntValue();
            for (int i = 0; i < FracPart; i++)
				fracPart /= 10.0;
        }
        float res = intPart + sign * fracPart;
        return res;
    }
    
    virtual bool GetBoolValue()
    {
        return _boolValue;
    }
    
    virtual void SetFocus()
	{
		if (IntPart)
            _intPart.SetFocus();
        else
            Control::SetFocus();
	}
    
    virtual bool Focused()
	{
		bool res = false;
		res |= _intPart.Focused();
		res |= _fracPart.Focused();
		return res || Control::Focused();
	}
    
	virtual void SetNext(Control *next)
	{
		if (!_fracPart.Dummy())
        {
            _fracPart.SetNext(next);
            return;
        }
        if (!_intPart.Dummy())
        {
            _intPart.SetNext(next);
            return;
        }
        
        Control::SetNext(next);
	}
};

#define CONTROLLER_PARAM_DECL(VarName, VarCaption, VarType, IntPart, FracPart, Format, Divider) MenuItem menuController##VarName (VarCaption, ShowWindow<wndIdController##VarName>, GetEditingValueString<wndIdController##VarName>); \
	WindowEditRegValue<Display, display, VarType, ControlParamInput<IntPart, FracPart>, \
		ActiveDriveControllerParams::GetRegValue_##VarType<ActiveDriveControllerParams::Reg##VarName>, \
		ActiveDriveControllerParams::SetRegValue_##VarType<ActiveDriveControllerParams::Reg##VarName>, \
        Divider> \
			wndController##VarName(&menuBigFont, "Параметры регулятора", VarCaption, Format);

#define CONTROLLER_PARAM_COS_DECL(VarName, VarCaption, VarType, IntPart, FracPart, Format, Divider) MenuItem menuController##VarName (VarCaption, ShowWindow<wndIdController##VarName>, GetEditingValueString<wndIdController##VarName>); \
	WindowEditRegValue<Display, display, VarType, ControlParamInput<IntPart, FracPart>, \
		ActiveDriveControllerParams::GetAngleSetupDisplay, \
		ActiveDriveControllerParams::SetAngleSetupDisplay, \
        Divider> \
			wndController##VarName(&menuBigFont, "Параметры регулятора", VarCaption, Format);


#define CONTROLLER_PARAM_INIT(VarName) menuListControllerSetup.Add(&menuController##VarName); \
	wndController##VarName.SetOnClose(OnWindowClose); \
	desctop.RegisterWindow(&wndController##VarName, wndIdController##VarName);

#define CONTROLLER_PARAM_UST_DECL(VarName, VarCaption, VarType, IntPart, FracPart, Format, Divider) MenuItem menuControllerUst##VarName (VarCaption, ShowWindow<wndIdControllerUst##VarName>, GetEditingValueString<wndIdControllerUst##VarName>); \
	WindowEditRegValue<Display, display, VarType, ControlParamInput<IntPart, FracPart>, \
		ActiveDriveControllerParams::GetRegValue_##VarType<ActiveDriveControllerParams::Reg##VarName>, \
		ActiveDriveControllerParams::SetRegValue_##VarType<ActiveDriveControllerParams::Reg##VarName>, \
        Divider> \
			wndControllerUst##VarName(&menuBigFont, "Уставки регулирования", VarCaption, Format);

#define CONTROLLER_PARAM_UST_INIT(VarName) menuListControllerUstSetup.Add(&menuController##VarName); \
	wndController##VarName.SetOnClose(OnWindowClose); \
	desctop.RegisterWindow(&wndController##VarName, wndIdController##VarName);

#define INDICATION_PARAM_DECL(VarName, VarCaption, VarType, IntPart, FracPart, Format, Divider) MenuItem menuIndication##VarName (VarCaption, ShowWindow<wndIdIndication##VarName>, GetEditingValueString<wndIdIndication##VarName>); \
	WindowEditRegValue<Display, display, VarType, ControlParamInput<IntPart, FracPart>, \
		ActiveDriveControllerParams::GetIndicationParamValue_##VarType<ActiveDriveControllerParams::IndicationParam##VarName>, \
		ActiveDriveControllerParams::SetIndicationParamValue_##VarType<ActiveDriveControllerParams::IndicationParam##VarName>, \
        Divider> \
			wndIndication##VarName(&menuBigFont, "Параметры индикации", VarCaption, Format);

#define INDICATION_PARAM_INIT(VarName) menuListIndicationSetup.Add(&menuIndication##VarName); \
	wndIndication##VarName.SetOnClose(OnWindowClose); \
	desctop.RegisterWindow(&wndIndication##VarName, wndIdIndication##VarName);

#define MODBUS_PARAM_DECL(VarName, VarCaption, VarType, IntPart, FracPart, Format, Divider) MenuItem menuModbus##VarName (VarCaption, ShowWindow<wndIdModbus##VarName>, GetEditingValueString<wndIdModbus##VarName>); \
	WindowEditRegValue<Display, display, VarType, ControlParamInput<IntPart, FracPart>, \
		ActiveDriveControllerParams::GetModbusParamValue_##VarType<ActiveDriveControllerParams::ModbusParam##VarName>, \
		ActiveDriveControllerParams::SetModbusParamValue_##VarType<ActiveDriveControllerParams::ModbusParam##VarName>, \
        Divider> \
			wndModbus##VarName(&menuBigFont, "Modbus", VarCaption, Format);

#define MODBUS_PARAM_INIT(VarName) menuListModbusSetup.Add(&menuModbus##VarName); \
	wndModbus##VarName.SetOnClose(OnWindowClose); \
	desctop.RegisterWindow(&wndModbus##VarName, wndIdModbus##VarName);


template<class DisplayType, DisplayType &display,
class ValueType, class EditorType,
ValueType (Getter)(),
void (Setter)(ValueType),
int Divider
>
class WindowEditRegValue : public Window<DisplayType, display>
{
protected:
	static const unsigned int wndWidth = 40;
	static const unsigned int _applyId = 0;
	static const unsigned int _cancelId = 1;
	
	Font *_bigFont;
	EditorType _newValue;
	ControlButton _apply;
	ControlButton _cancel;
	const char *_header;
	const char *_subHeader;
	const char *_formatString;
public:
	WindowEditRegValue(Font *bigFont, const char *header, const char *subHeader, const char *formatString)
	{
		_bigFont = bigFont;
		_header = header;
		_subHeader = subHeader;
		_formatString = formatString;
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
		
		len = sprintf(str, _header);
		display.MoveCursorTo((wndWidth - len) / 2, 0);
		display.WriteLine(str, len);
		
		len = sprintf(str, _subHeader);
		display.MoveCursorTo((wndWidth - len) / 2, 16);
		display.WriteLine(str, len);
		
		display.MoveCursorTo(1, 48);
		display.WriteLine("Текущее значение:");
		
        Font *oldFont = display.SelectFont(_bigFont);
        len = GetCurrentValueString(str);
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
	
	virtual void OnShow()
	{
        char str[100];
        GetCurrentValueString(str);
        
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
	
	virtual bool OnKeyDown(char &key)
	{
		if (_newValue.OnKeyDown(key))
			return true;
		if (_apply.OnKeyDown(key))
			return true;
		if (_cancel.OnKeyDown(key))
			return true;
		if (_newValue.Focused())
		{
			if ('E' == key)
			{
				_cancel.SetFocus();
				return true;
			}
		}
		return false;
	}
	
	virtual void OnClick(unsigned int controlId, char key)
	{
		if (_applyId == controlId)
		{
			if (13 == key)
			{
				if (_newValue.IsFloat())
                {
                    float newValue = _newValue.GetFloatValue();
                    Setter((ValueType)(newValue * Divider));
                }
                else
                {
                    if (_newValue.IsBool())
                    {
                        Setter(_newValue.GetBoolValue() ? 1 : 0);
                    }
                    else
                    {
                        int newValue = _newValue.GetIntValue();
                        Setter(newValue * Divider);
                    }
                }
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
	
	virtual char *GetEditingValue()
	{
		static char tmp[10];
		GetCurrentValueString(tmp);
		return tmp;
	}
protected:
	unsigned char GetCurrentValueString(char *dts)
	{
		const char *formatString = GetFormatString();
		ValueType value = Getter();
        value /= Divider;
		unsigned char len = sprintf(dts, formatString, value);
		if (_newValue.IsBool())
        {
            if (_newValue.BoolType() == 0)
			{
				len = sprintf(dts, formatString, value == 0 ? "выкл" : "вкл");
			}
			else if (_newValue.BoolType() == 1)
			{
				len = sprintf(dts, formatString, value == 0 ? "Odd" : "Even");
			}
        }
        return len;
	}
	
	const char *GetFormatString()
	{
		return _formatString;
	}
};

#endif