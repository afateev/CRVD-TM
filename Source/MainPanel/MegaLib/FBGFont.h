////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		30/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef FBGFONT_H
#define FBGFONT_H

#include "Font.h"

class FbgFont : public Font
{
protected:
	unsigned char const* _fontData;
	unsigned short _firstChar;
	unsigned short _lastChar;
	unsigned short _charsCount;
	unsigned char _charHeight;
	unsigned char _charWidth;
public:
	FbgFont(unsigned char const* FontData)
	{
		_fontData = FontData;
		_firstChar = _fontData[2];
		_charsCount = _fontData[3];
		if (_charsCount == 0)
			_charsCount = 0x100;
		
		_lastChar = _firstChar + _charsCount - 1;
		_charHeight = _fontData[4];
		_charWidth = _fontData[5] / 8;
		_charWidth += _fontData[5] % 8 > 0 ? 1 : 0;
	}
	
	virtual Char GetChar(unsigned char charCode) const
	{
		if (charCode < _firstChar)
			return Char();
		if (charCode > _lastChar)
			return Char();
		
		Char ch;
		
		unsigned int offset = charCode - _firstChar;
		offset *= _charHeight * _charWidth;
		offset += 6;
		
		ch.Width = _charWidth;
		ch.Height = _charHeight;
		ch.Data = &_fontData[offset];
		ch.InvertHorByteOrder = true;
		return ch;
	}
	
	virtual unsigned char Width()
	{
		return _charWidth;
	}
	
	virtual unsigned char Height()
	{
		return _charHeight;
	}
};

#endif