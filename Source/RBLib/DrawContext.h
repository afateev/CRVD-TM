////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		30/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_DRAWCONTEXT_H
#define RBLIB_DRAWCONTEXT_H

#include "Screen.h"
#include "Font.h"

namespace Rblib
{
	class DrawContext
	{
	protected:
		Screen *_screen;
		Font *_font;
	public:
		DrawContext()
		{
			_screen = 0;
			_font = 0;
		}
		
		virtual ~DrawContext()
		{
		
		}
		
		virtual void Init(Screen *screen)
		{
			_screen = screen;
			if (0 == _screen)
				return;
			
			_screen->Init(false);
			_screen->Clear();
		}
		
		virtual void SecondBufferEnable(bool enable)
		{
			if (0 == _screen)
				return;
			
			_screen->Init(enable);
		}
		
		virtual Font *SelectFont(Font *font)
		{
			Font *oldFont = _font;
			_font = font;
			return oldFont;
		}
		
		virtual Font *GetCurrentFont()
		{
			return _font;
		}
		
		virtual void WriteChar(char chCode)
		{
			if (0 == _screen)
				return;
			if (0 == _font)
				return;
			Char ch = _font->GetChar(chCode);
			/*
			for (unsigned char y = 0; y < ch.Height; y++)
			{
				for (unsigned char x = 0; x < ch.Width; x++)
				{
					unsigned char b = ch.GetByte(x, y);
					_screen->WriteByte(b);
					_screen->MoveCursor(1, 0);
				}
				_screen->MoveCursor(-ch.Width, 1);
			}
			
			_screen->MoveCursor(ch.Width, -ch.Height);
			*/
			_screen->WriteChar(ch);
		}
		
		virtual unsigned char ReadByte()
		{
			if (0 == _screen)
				return 0;
			return _screen->ReadByte();
		}
		
		virtual void WriteByte(unsigned char value)
		{
			if (0 == _screen)
				return;
			_screen->WriteByte(value);
		}
		
		virtual void MoveCursorTo(unsigned int x, unsigned int y)
		{
			if (0 == _screen)
				return;
			_screen->MoveCursorTo(x, y);
		}
		
		virtual void Flush()
		{
			if (0 == _screen)
				return;
			_screen->Flush();
		}
		
		virtual void Clear()
		{
			if (0 == _screen)
				return;
			_screen->Clear();
		}
	};
}

#endif