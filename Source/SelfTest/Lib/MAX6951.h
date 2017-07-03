////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		21/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef MAX6951_H
#define MAX6951_H

template<class Spi, unsigned char Size = 8>
class Max6951
{
protected:
	enum Reg
	{
		regNop = 0x00,
		regDecodeMode	= 0x01,
		regIntensity	= 0x02,
		regScanLimit	= 0x03,
		regConfig		= 0x04,
		regTest			= 0x07,
		regFirst		= 0x60
	};
	
	static unsigned char _decodeFlags;
public:
	static void Init()
	{
		Spi::Init();
		WriteReg(regIntensity, 6);
		WriteReg(regScanLimit, Size - 1);
		Test(false);
		Clear();
	}
	
	static void Enable(bool enable = true)
	{
		WriteReg(regConfig, enable ? 1 : 0);
	}
	
	static void Test(bool enable = true)
	{
		WriteReg(regTest, enable ? 1 : 0);
	}
	
	static void Clear()
	{
		for(unsigned char i = 0; i < 8; i++)
		{
			SetDecode(i, 0);
			WriteReg(regFirst + i, 0);
		}
	}
	
	static void WriteChar(unsigned int pos, char data, bool point = false)
	{
		unsigned char pointMask = point ? 0x80 : 0x00;
		switch (data)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			SetDecode(pos, 1);
			WriteReg(regFirst + pos, (data - 0x30) | pointMask);
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			SetDecode(pos, 1);
			WriteReg(regFirst + pos, (data - 0x41 + 0x0A) | pointMask);
			break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			SetDecode(pos, 1);
			WriteReg(regFirst + pos, (data - 0x61 + 0x0A) | pointMask);
			break;
		case '-':
			SetDecode(pos, 0);
			WriteReg(regFirst + pos, 0x01 | pointMask);
			break;
		default:
			SetDecode(pos, 0);
			WriteReg(regFirst + pos, 0x00 | pointMask);
		}
	}
	
	static void WriteString(char *str, int len)
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
			WriteChar(i, ch, ch == '.' ? 1 : point);
			if (point)
			{
				skip++;
			}
		}
	}
protected:
	static void WriteReg(unsigned char reg, unsigned char data)
	{
		Spi::Select(true);
		Spi::WriteByte(reg);
		Spi::WriteByte(data);
		Spi::Select(false);
	}
	
	static void SetDecode(unsigned char pos, bool decode)
	{
		if (decode)
		{
			_decodeFlags |= (1 << pos);
		}
		else
		{
			_decodeFlags &= ~(1 << pos);
		}
		WriteReg(regDecodeMode, _decodeFlags);
	}
};

template<class Spi, unsigned char Size>
unsigned char Max6951<Spi, Size>::_decodeFlags = 0;

#endif