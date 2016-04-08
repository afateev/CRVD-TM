////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		20/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef SOFTWARE_SPI_H
#define SOFTWARE_SPI_H

template<class SckPort, unsigned char SckPin, class MosiPort, unsigned char MosiPin, class MisoPort, unsigned char MisoPin, class CsPort, unsigned char CsPin, bool CsPolarity, bool SckPolarity>
class SoftwareSpi
{
public:
	static void Init()
	{
		SckPort::SetOutputPin(SckPin);
		MosiPort::SetOutputPin(MosiPin);
		MisoPort::SetInputPin(MisoPin);
		CsPort::SetOutputPin(CsPin);
		Select(false);
	}
	
	static void Select(bool select)
	{
		Clock(false);
		if (select)
		{
			if (CsPolarity)
				CsPort::SetBit(CsPin);
			else
				CsPort::ClearBit(CsPin);
		}
		else
		{
			if (CsPolarity)
				CsPort::ClearBit(CsPin);
			else
				CsPort::SetBit(CsPin);
		}
	}
	
	static void Clock(bool up)
	{
		if (up)
		{
			if (SckPolarity)
				SckPort::SetBit(SckPin);
			else
				SckPort::ClearBit(SckPin);
		}
		else
		{
			if (SckPolarity)
				SckPort::ClearBit(SckPin);
			else
				SckPort::SetBit(SckPin);
		}
	}
	
	static unsigned char WriteByte(unsigned char data)
	{
		unsigned char res = 0;
		Clock(false);
		for (unsigned char i = 0; i < 8; i++)
		{
			bool bit = (data >> (7 - i)) & 0x01;
			if (bit)
				MosiPort::SetBit(MosiPin);
			else
				MosiPort::ClearBit(MosiPin);
			Clock(true);
			bool input = MisoPort::Read(MisoPin);
			res <<= 1;
			res |= (unsigned char)(input ? 1 : 0);
			Clock(false);
				
		}
		Clock(false);
		return res;
	}
};

#endif