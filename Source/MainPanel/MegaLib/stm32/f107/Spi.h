////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		05/02/2013
////////////////////////////////////////////////////////////////////////////////

#ifndef SPI_STM32_F107_H
#define SPI_STM32_F107_H

#include "SpiParts.h"

template<unsigned int BaseAddr>
class SpiHardwareBase:
	public SpiControl1Register	<BaseAddr + 0x00>,
	public SpiStatusRegister	<BaseAddr + 0x08>,
	public SpiDataRegister		<BaseAddr + 0x0C>
	//public UsartStatusRegister		<BaseAddr + 0x00>,
	//public UsartDataRegister		<BaseAddr + 0x04>,
	//public UsartBaudRateRegister	<BaseAddr + 0x08>,
	//public UsartControl1Register	<BaseAddr + 0x0C>
{
};

class Spi
{
public:
	typedef SpiHardwareBase<0x40013000> _A;
	typedef SpiHardwareBase<0x40003800> _B;
	typedef SpiHardwareBase<0x40003C00> _C;
};

template<class Hardware, class CsPort, unsigned char CsPin, bool CsPolarity, bool SckPolarity>
class SpiWrapper
{
public:
	static void Init()
	{
		Hardware::SetDivider(Hardware::Div32);
		Hardware::Enable(true, true, SckPolarity);
		CsPort::SetOutputPin(CsPin);
		Select(false);
	}
	
	static void Select(bool select)
	{
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
	
	static unsigned char WriteByte(unsigned char data)
	{
		unsigned char res = 0;
		
		Hardware::WriteData(data);
		
		int counter = 1000;
		while (counter > 0 || Hardware::IsBusy())
		{
			counter--;
			if (!Hardware::IsBusy())
			{
				break;
			}
		}
		if (Hardware::IsRxNotEmpty())
		{
			res = Hardware::ReadData();
		}

		return res;
	}
};

#endif