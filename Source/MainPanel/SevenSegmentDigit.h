
////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		18/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef SEVENSEGMENTDIGIT_H
#define SEVENSEGMENTDIGIT_H

#include "MegaLib\Megalib.h"

template<class DisplayType, DisplayType &display>
class SevenSegmentDigit
{
protected:
	int _x;
	int _y;
	int _width;
	int _height;
	int _lineWidth;
	struct RefPoint
	{
		int X;
		int Y;
	};
	RefPoint _refPoint[7];
public:
	SevenSegmentDigit()
	{
		Init(0, 0, 8, 16, 1);
	}
	SevenSegmentDigit(int x, int y, int width, int height, int lineWidth)
	{
		Init(x, y, width, height, lineWidth);
	}
	
	void Init(int x, int y, int width, int height, int lineWidth) 
	{
		_x = x;
		_y = y;
		_width = width;
		_height = height;
		_lineWidth = lineWidth;
		
		_refPoint[0].X = _x + _lineWidth;
		_refPoint[0].Y = _y + _lineWidth;
		_refPoint[1].X = _x + _width - _lineWidth  - 3 * _lineWidth - 1;
		_refPoint[1].Y = _y + _lineWidth;
		
		_refPoint[2].X = _x + _lineWidth;
		_refPoint[2].Y = _y + _height / 2;
		_refPoint[3].X = _x + _width - _lineWidth  - 3 * _lineWidth - 1;
		_refPoint[3].Y = _y + _height / 2;
		
		_refPoint[4].X = _x + _lineWidth;
		_refPoint[4].Y = _y + _height - _lineWidth;
		_refPoint[5].X = _x + _width - _lineWidth  - 3 * _lineWidth - 1;
		_refPoint[5].Y = _y + _height - _lineWidth;
		
		_refPoint[6].X = _x + _width + _lineWidth - 3 * _lineWidth;
		_refPoint[6].Y = _y + _height - _lineWidth;
	}
	
	void Draw(char digit, bool point)
	{
		switch (digit)
		{
		case '1':
			DrawVerLine(1, 3);
			DrawVerLine(3, 5);
			break;
		case '2':
			DrawHorLine(0, 1);
			DrawHorLine(2, 3);
			DrawHorLine(4, 5);
			DrawVerLine(1, 3);
			DrawVerLine(2, 4);
			break;
		case '3':
			DrawHorLine(0, 1);
			DrawHorLine(2, 3);
			DrawHorLine(4, 5);
			DrawVerLine(1, 3);
			DrawVerLine(3, 5);
			break;
		case '4':
			DrawHorLine(2, 3);
			DrawVerLine(0, 2);
			DrawVerLine(1, 3);
			DrawVerLine(3, 5);
			break;
		case '5':
			DrawHorLine(0, 1);
			DrawHorLine(2, 3);
			DrawHorLine(4, 5);
			DrawVerLine(0, 2);
			DrawVerLine(3, 5);
			break;
		case '6':
			DrawHorLine(0, 1);
			DrawHorLine(2, 3);
			DrawHorLine(4, 5);
			DrawVerLine(0, 2);
			DrawVerLine(2, 4);
			DrawVerLine(3, 5);
			break;
		case '7':
			DrawHorLine(0, 1);
			DrawVerLine(1, 3);
			DrawVerLine(3, 5);
			break;
		case '8':
			DrawHorLine(0, 1);
			DrawHorLine(2, 3);
			DrawHorLine(4, 5);
			DrawVerLine(0, 2);
			DrawVerLine(1, 3);
			DrawVerLine(2, 4);
			DrawVerLine(3, 5);
			break;
		case '9':
			DrawHorLine(0, 1);
			DrawHorLine(2, 3);
			DrawHorLine(4, 5);
			DrawVerLine(0, 2);
			DrawVerLine(1, 3);
			DrawVerLine(3, 5);
			break;
		case '0':
			DrawHorLine(0, 1);
			DrawHorLine(4, 5);
			DrawVerLine(0, 2);
			DrawVerLine(1, 3);
			DrawVerLine(2, 4);
			DrawVerLine(3, 5);
			break;
		case '-':
			DrawHorLine(2, 3);
			break;
		}
		if(point)
			DrawPoint();
	}
	
protected:
	
	void DrawHorLine(unsigned char ref1, unsigned char ref2)
	{
		for (int y = - _lineWidth; y <= _lineWidth; y++)
		{
			int offset = (y < 0 ? -y : y) + 1;
			for (int x = _refPoint[ref1].X + offset; x <= _refPoint[ref2].X - offset; x++)
			{
				display.DrawPoint(x, _refPoint[ref1].Y + y);
			}
		}
	}
	void DrawVerLine(unsigned char ref1, unsigned char ref2)
	{
		for (int x = - _lineWidth; x <= _lineWidth; x++)
		{
			int offset = (x < 0 ? -x : x) + 1;
			for (int y = _refPoint[ref1].Y + offset; y <= _refPoint[ref2].Y - offset; y++)
			{
				display.DrawPoint(_refPoint[ref1].X + x, y);
			}
		}
	}
	void DrawPoint()
	{
		for (int y = - _lineWidth; y <= _lineWidth; y++)
		for (int x = - _lineWidth; x <= _lineWidth; x++)
		{
			display.DrawPoint(_refPoint[6].X + x, _refPoint[6].Y + y);
		}
	}
};

template<class DisplayType, DisplayType &display, unsigned char Size>
class SevenSegmentDigitBlock
{
protected:
	SevenSegmentDigit<DisplayType, display> _digit[Size];
public:
	SevenSegmentDigitBlock(int x, int y, int width, int height, int lineWidth)
	{
		for (int i = 0; i < Size; i++)
		{
			_digit[i].Init(x + i * width, y, width, height, lineWidth);
		}
	}
	
	void Draw(char *str, int len)
	{
		int count = len < Size ? len : Size;
		int skip = 0;
		for (int i = 0; i < count; i++)
		{
			bool point = false;
			if (str[i + 1 + skip] == '.')
			{
				point = true;
			}
			char ch = str[i + skip];
			_digit[i].Draw(ch , ch == '.' ? 1 : point);
			if (point)
			{
				skip++;
			}
		}
	}
};

#endif