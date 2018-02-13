#ifndef BUTTON_H
#define BUTTON_H

#include "../../RbLib/Rblib.h"

class Button
{
public:
	enum State
	{
		StateUnknown,
		StateWaitPressed,
		StatePressed,
		StateWaitRelease
	};
public:
	static const unsigned int PressedCounterMaxValue = 0xFFFFFFFF - 1;
	Rblib::CallbackWrapper<bool &> GetState;
	Rblib::CallbackWrapper<> OnClick;
	Rblib::CallbackWrapper<> OnLongClick;
protected:
	State _state;
	unsigned int _pressedCounter;
public:
	Button()
	{
		Reset();
	}
	
	void Reset()
	{
		_state = StateUnknown;
		_pressedCounter = 0;
	}
	
	void Tick(unsigned int tickFrequency)
	{
		bool pressed = false;
		GetState(pressed);
		
		switch (_state)
		{
		case StateUnknown:
			{
				_state = StateWaitPressed;
			}
			break;
		case StateWaitPressed:
			{
				// отжатие после короткого нажатия
				if (_pressedCounter > tickFrequency * 0.1)
				{
					_state = StateWaitRelease;
					OnClick();
				}
				
				_pressedCounter = 0;

				if (pressed)
				{
					_state = StatePressed;
				}
			}
			break;
		case StatePressed:
			{
				if (_pressedCounter < PressedCounterMaxValue)
				{
					_pressedCounter++;
				}
				
				if (pressed)
				{
					// длинное нажатие
					if (_pressedCounter > tickFrequency * 2)
					{
						_state = StateWaitRelease;
						OnLongClick();
					}
				}
				else
				{
					_state = StateWaitPressed;
				}
			}
			break;
		case StateWaitRelease:
			{
				if (!pressed)
				{
					_state = StateWaitPressed;
					// чтобы не сработало отжатие после короткого нажатия
					_pressedCounter = 0;
				}
			}
			break;
		}
	}
};

#endif