////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		30/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_FONT_H
#define RBLIB_FONT_H

namespace Rblib
{
	class Char
	{
	public:
		unsigned char Width;
		unsigned char Height;
		unsigned char const*Data;
		bool InvertHorByteOrder;
	public:
		Char()
		{
			Width = 0;
			Height = 0;
			Data = 0;
			InvertHorByteOrder = false;
		}
		
		unsigned char GetByte(unsigned char x, unsigned char y)
		{
			/*
			unsigned char value = Data[y * Width + x];
			unsigned char ret = 0;
			for (int i = 0; i < 8; i++)
				if ((value & (unsigned char)(1 << i)) != 0) ret += (unsigned char)(1 << (7 - i));
			
			return ret;
			*/
			unsigned char xOffset = InvertHorByteOrder ? Width - 1 - x : x;
			return Data[y * Width + xOffset];
		}
	};
	
	class Font
	{
	public:
	
	public:
		virtual Char GetChar(unsigned char charCode) const
		{
			return Char();
		}
		
		virtual unsigned char Width()
		{
			return 0;
		}
		
		virtual unsigned char Height()
		{
			return 0;
		}
	};
}

#endif