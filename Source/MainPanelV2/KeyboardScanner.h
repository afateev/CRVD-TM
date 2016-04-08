////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		11/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef KEYBOARDSCANNER_H
#define KEYBOARDSCANNER_H

template <class Port, unsigned char Pin1, unsigned char Pin2, unsigned char Pin3, unsigned char Pin4, unsigned char PinA, unsigned char PinB, unsigned char PinC, unsigned char PinD>
class KeyboardScanner
{
protected:
	static unsigned char _row;
	static unsigned char _col;
	static const unsigned char _rows[4];
	static const unsigned char _cols[4];
	static bool _prevState[4][4];
	static const char _keyCodes[4][4];
	typedef void (CallbackType)(char key);
	static CallbackType *_onKeyDown;
	static CallbackType *_onKeyUp;
	static unsigned int _repeateCount;
	static const unsigned int RepeatTimeout = 500;
public:
	
	static inline void Init()
	{
		for (unsigned char i = 0 ; i < 4; i ++)
		{
			Port::SetMode(_cols[i], Port::ModeInput);
			Port::SetConfig(_cols[i], Port::ConfigInputPull);
			Port::SetBit(_cols[i]);
			Port::SetMode(_rows[i], Port::ModeInput);
			Port::SetConfig(_rows[i], Port::ConfigInputFloating);
			Port::ClearBit(_rows[i]);
		}
	}
	
	static inline void Tick()
	{
		static unsigned int pause = 0;
		
		Port::SetMode(_rows[_row], Port::ModeOutput);
		Port::SetConfig(_rows[_row], Port::ConfigOutputPushPull);
		bool curState = Port::Read(_cols[_col]);
		
		if (_prevState[_row][_col] != curState)
		{
			if (curState)
			{
				OnKeyUp(_row, _col);
			}
			else
			{
				OnKeyDown(_row, _col);
			}
			_repeateCount = 0;
			pause = 0;
		}
		else
		{
			if (!curState)
			{
				if (_repeateCount < RepeatTimeout)
					_repeateCount++;
				else
				{
					if (pause > 0)
						pause--;
					else
					{
						OnKeyDown(_row, _col);
						pause = 10;
					}
				}
			}
		}
		_prevState[_row][_col] = curState;
		
		Port::SetMode(_rows[_row], Port::ModeInput);
		Port::SetConfig(_rows[_row], Port::ConfigInputFloating);

		_col++;
		if (_col >= 4)
		{
			_col = 0;
			_row++;
			if (_row >= 4)
				_row = 0;
		}
	}
	
	static inline void OnKeyDown(unsigned char row, unsigned char col)
	{
		OnKeyDown(_keyCodes[row][col]);
	}
	
	static inline void OnKeyUp(unsigned char row, unsigned char col)
	{
		OnKeyUp(_keyCodes[row][col]);
	}
	
	static inline void OnKeyDown(char key)
	{
		if (_onKeyDown)
			_onKeyDown(key);
	}
	
	static inline void OnKeyUp(char key)
	{
		if (_onKeyUp)
			_onKeyUp(key);
	}
	
	static inline void SetOnKeyDown(CallbackType *callback)
	{
		_onKeyDown = callback;
	}
	
	static inline void SetOnKeyUp(CallbackType *callback)
	{
		_onKeyUp = callback;
	}
};

template <class Port, unsigned char Pin1, unsigned char Pin2, unsigned char Pin3, unsigned char Pin4, unsigned char PinA, unsigned char PinB, unsigned char PinC, unsigned char PinD>
unsigned char KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::_row = 0;

template <class Port, unsigned char Pin1, unsigned char Pin2, unsigned char Pin3, unsigned char Pin4, unsigned char PinA, unsigned char PinB, unsigned char PinC, unsigned char PinD>
unsigned char KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::_col = 0;

template <class Port, unsigned char Pin1, unsigned char Pin2, unsigned char Pin3, unsigned char Pin4, unsigned char PinA, unsigned char PinB, unsigned char PinC, unsigned char PinD>
const unsigned char KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::_rows[4] = {Pin1, Pin2, Pin3, Pin4};

template <class Port, unsigned char Pin1, unsigned char Pin2, unsigned char Pin3, unsigned char Pin4, unsigned char PinA, unsigned char PinB, unsigned char PinC, unsigned char PinD>
const unsigned char KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::_cols[4] = {PinA, PinB, PinC, PinD};

template <class Port, unsigned char Pin1, unsigned char Pin2, unsigned char Pin3, unsigned char Pin4, unsigned char PinA, unsigned char PinB, unsigned char PinC, unsigned char PinD>
bool KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::_prevState[4][4] = {{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};

template <class Port, unsigned char Pin1, unsigned char Pin2, unsigned char Pin3, unsigned char Pin4, unsigned char PinA, unsigned char PinB, unsigned char PinC, unsigned char PinD>
const char KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::_keyCodes[4][4] = {{'1','+','2','3'},{'4','-','5','6'},{'7','F','8','9'},{'E','S','0',13}};

template <class Port, unsigned char Pin1, unsigned char Pin2, unsigned char Pin3, unsigned char Pin4, unsigned char PinA, unsigned char PinB, unsigned char PinC, unsigned char PinD>
KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::CallbackType *
KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::_onKeyDown = 0;

template <class Port, unsigned char Pin1, unsigned char Pin2, unsigned char Pin3, unsigned char Pin4, unsigned char PinA, unsigned char PinB, unsigned char PinC, unsigned char PinD>
KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::CallbackType *
KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::_onKeyUp = 0;

template <class Port, unsigned char Pin1, unsigned char Pin2, unsigned char Pin3, unsigned char Pin4, unsigned char PinA, unsigned char PinB, unsigned char PinC, unsigned char PinD>
unsigned int KeyboardScanner<Port, Pin1, Pin2, Pin3, Pin4, PinA, PinB, PinC, PinD>::_repeateCount = 0;

#endif