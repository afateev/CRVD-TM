////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		30/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_DISPLAY_H
#define RBLIB_DISPLAY_H

#include "DrawContext.h"
#include <string.h>

namespace Rblib
{
	class Display
	{
	protected:
		unsigned int _cursorCounter;
		static const unsigned int _cursorMaxCount = 500;
		static const unsigned int _cursorVisibleCount = _cursorMaxCount / 2;
	public:
		DrawContext *_currentContext;
	public:
		
		Display()
		{
			_currentContext = 0;
		}
		
		void SelectContext(DrawContext *_newContext)
		{
			_currentContext = _newContext;
		}
		
		Font *SelectFont(Font *font)
		{
			if (0 == _currentContext)
				return 0;
			return _currentContext->SelectFont(font);
		}
		
		Font *GetCurrentFont()
		{
			if (0 == _currentContext)
				return 0;
			return _currentContext->GetCurrentFont();
		}
		
		void WriteChar(char ch)
		{
			if (0 == _currentContext)
				return;
			_currentContext->WriteChar(ch);
		}
		
		void InvertByte(unsigned int x, unsigned int y)
		{
			if (0 == _currentContext)
				return;
			MoveCursorTo(x, y);
			unsigned char tmp = _currentContext->ReadByte();
			_currentContext->WriteByte(~tmp);
		}
		
		void WriteByte(unsigned int x, unsigned int y, unsigned char b)
		{
			if (0 == _currentContext)
				return;
			MoveCursorTo(x, y);
			_currentContext->WriteByte(b);
		}
		
		unsigned char ReadByte(unsigned int x, unsigned int y)
		{
			if (0 == _currentContext)
				return 0;
			MoveCursorTo(x, y);
			return _currentContext->ReadByte();
		}
		
		void DrawPoint(unsigned int x, unsigned int y)
		{
			if (0 == _currentContext)
				return;
			MoveCursorTo(x / 8, y);
			unsigned char tmp = _currentContext->ReadByte();
			tmp |= 1 << (7 - x % 8);
			_currentContext->WriteByte(tmp);
		}
		
		inline static int abs(int a)
		{
			return a < 0 ? -a : a;
		}
		
		void DrawLine(int x1, int y1, int x2, int y2)
		{
			const int deltaX = abs(x2 - x1);
			const int deltaY = abs(y2 - y1);
			const int signX = x1 < x2 ? 1 : -1;
			const int signY = y1 < y2 ? 1 : -1;
			int error = deltaX - deltaY;
			DrawPoint(x2, y2);
			while(x1 != x2 || y1 != y2)
			{
				DrawPoint(x1, y1);
				const int error2 = error * 2;
				if(error2 > -deltaY)
				{
					error -= deltaY;
					x1 += signX;
				}
				if(error2 < deltaX)
				{
					error += deltaX;
					y1 += signY;
				}
			}
		}
		
		void WriteLine(char *str, unsigned int strLen)
		{
			for (int i = 0; i < strLen; i++)
				WriteChar(str[i]);
		}
		
		void WriteLine(char *str)
		{
			unsigned int strLen = strlen(str);
			for (int i = 0; i < strLen; i++)
				WriteChar(str[i]);
		}
		
		void MoveCursorTo(unsigned int x, unsigned int y)
		{
			if (0 == _currentContext)
				return;
			_currentContext->MoveCursorTo(x, y);
		}
		
		void Flush()
		{
			if (0 == _currentContext)
				return;
			_currentContext->Flush();
		}
		
		void Clear()
		{
			if (0 == _currentContext)
				return;
			_currentContext->Clear();
		}
		
		void Tick()
		{
			_cursorCounter++;
			if (_cursorCounter >= _cursorMaxCount)
				_cursorCounter = 0;
		}
		
		void ShowCursor()
		{
			_cursorCounter = 0;
		}
		
		bool CursorVisible()
		{
			return _cursorCounter < _cursorVisibleCount;
		}
	};
}

#endif