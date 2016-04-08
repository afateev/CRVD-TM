////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		30/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_SCREEN_H
#define RBLIB_SCREEN_H

#include "Font.h"
namespace Rblib
{
	class Cursor
	{
	public:
		unsigned int PosX;
		unsigned int PosY;
	};
	
	class Screen
	{
	public:
		Screen()
		{
		
		}
		
		virtual ~Screen()
		{
		
		}
		
		virtual void Init(bool secondBuffer)
		{
			
		}
		
		virtual unsigned char ReadByte()
		{
			return 0;
		}
		
		virtual void WriteByte(unsigned char value)
		{
		
		}
		
		virtual void WriteBuffer(unsigned char *buf, unsigned int len)
		{
		
		}
		
		virtual void Clear()
		{
			
		}
		
		virtual void MoveCursor(int dx, int dy)
		{
		
		}
		
		virtual void MoveCursorTo(int x, int y)
		{
		
		}
		
		virtual void WriteChar(Char ch)
		{
		
		}
		
		virtual void Flush()
		{
		
		}
	};
}

#endif