////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		08/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef SCREENBUFFER_H
#define SCREENBUFFER_H

#include "Screen.h"

template<class BaseScreen, unsigned int Width, unsigned int Height>
class ScreenBuffer : public Screen
{
protected:
	BaseScreen screen;
	Cursor _cursor;
	unsigned char _buffer[Height * Width];
	unsigned char _changed[Width / 8][Height];
public:
	ScreenBuffer()
	{
	
	}
	
	virtual ~ScreenBuffer()
	{
	
	}
	
	virtual void Init(bool secondBuffer)
	{
		screen.Init(secondBuffer);
		_cursor.PosX = 0;
		_cursor.PosY = 0;
		Clear();
	}
	
	virtual unsigned char ReadByte()
	{
		unsigned int addr = _cursor.PosY * Width + _cursor.PosX;
		if (addr >= sizeof(_buffer))
			return 0;
		return _buffer[addr];
	}
	
	virtual void WriteByte(unsigned char value)
	{
		unsigned int addr = _cursor.PosY * Width + _cursor.PosX;
		if (addr >= sizeof(_buffer))
			return;
		_buffer[addr] = value;
	}
	
	virtual void Clear()
	{
		MoveCursorTo(0, 0);
		for (unsigned int y = 0; y < Height; y++)
		{
			for (unsigned int x = 0; x < Width; x++)
			{
				WriteByte(0);
				MoveCursor(1, 0);
			}
			MoveCursor(-Width, 1);
		}
	}
	
	virtual void MoveCursor(int dx, int dy)
	{
		_cursor.PosX += dx;
		_cursor.PosY += dy;
	}
	
	virtual void MoveCursorTo(int x, int y)
	{
		MoveCursor(x - _cursor.PosX, y - _cursor.PosY);
	}
	
	virtual void WriteChar(Char ch)
	{
		for (unsigned char y = 0; y < ch.Height; y++)
		{
			for (unsigned char x = 0; x < ch.Width; x++)
			{
				unsigned char b = ch.GetByte(x, y);
				WriteByte(b);
				MoveCursor(1, 0);
			}
			MoveCursor(-ch.Width, 1);
		}
		
		MoveCursor(ch.Width, -ch.Height);
		
		if (_cursor.PosX >= Width)
		{
			MoveCursor(-_cursor.PosX, ch.Height);
		}
		
		if (_cursor.PosY >= Height)
			MoveCursor(0, -_cursor.PosY);
	}
	
	virtual void Flush()
	{
		screen.MoveCursorTo(0, 0);
		/*
		for (unsigned int y = 0; y < Height; y++)
		{
			for (unsigned int x = 0; x < Width; x++)
			{
				unsigned char b = _buffer[x][y];
				screen.WriteByte(b);
				screen.MoveCursor(1, 0, false);
			}
			MoveCursor(-Width, 1);
		}
		*/
		
		screen.WriteBuffer(_buffer, sizeof(_buffer));
		
		screen.MoveCursorTo(0, 0);
	}
};

#endif