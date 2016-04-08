////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		30/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_DRIVER_SCREENRA8835_H
#define RBLIB_DRIVER_SCREENRA8835_H

#include "..\Screen.h"
#include "RA8835.h"
namespace Rblib
{
namespace Driver
{
	template<class DataBus>
	class ScreenRA8835 : public Screen
	{
	public:
		typedef	RA8835<DataBus> DisplayType;
		DisplayType display;
		
		Cursor _cursor;
	public:
		ScreenRA8835()
		{
		
		}
		
		virtual ~ScreenRA8835()
		{
		
		}
		
		virtual void Init(bool secondBuffer)
		{
			display.Init(secondBuffer);
			_cursor.PosX = 0;
			_cursor.PosY = 0;
		}
		
		virtual void WriteByte(unsigned char value)
		{
			display.WriteByte(value);
		}
		
		virtual void WriteBuffer(unsigned char *buf, unsigned int len)
		{
			display.WriteBuffer(buf, len);
		}
		
		virtual void Clear()
		{
			display.Clear();
		}
		
		virtual void MoveCursor(int dx, int dy)
		{
			MoveCursor(dx, dy, true);
		}
		
		virtual void MoveCursor(int dx, int dy, bool setAddr)
		{
			unsigned int displayWidth = 40;
			
			_cursor.PosX += dx;
			_cursor.PosY += dy;
			
			unsigned int addr = 0;
			addr += _cursor.PosY * displayWidth;
			addr += _cursor.PosX;
			
			if (setAddr)
				display.SetCursorAddress(addr);
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
					MoveCursor(1, 0, false);
				}
				MoveCursor(-ch.Width, 1);
			}
			
			MoveCursor(ch.Width, -ch.Height);
			
			
			if (_cursor.PosX >= display.Width)
			{
				MoveCursor(-_cursor.PosX, ch.Height);
			}
			
			if (_cursor.PosY >= display.Height)
				MoveCursor(0, -_cursor.PosY);
		}
	};
}
}

#endif