///////////////////////////////////////////////////////////////////////////////
//	������ SPI
//	MSP430
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPI_MSP430_H
#define SPI_MSP430_H

#include "../IdObjectBase.h"

namespace Rblib
{
namespace Msp430
{
	// ��� �������� ��������
	typedef unsigned char SpiRegisterDataType;
	// ��� �������� �������� ������
	typedef unsigned short SpiAddressRegisterDataType;
	// ��� ������ ��������
	typedef unsigned int  SpiRegisterAddressType;
	
	////////////////////////////////////////////////////
	// ����� ��� ���� �������� ����� SPI
	////////////////////////////////////////////////////

	template<
		unsigned int Id,
		class Hardware
		>
	class Spi :
		public IdObjectBase<Id>,
		public Hardware
	{
	public:
		// �������������
		static void Init(typename Hardware::ClockSource clockSource, unsigned int prescaller, bool msbFirst, bool clockPolarity, bool clockPhase)
		{
			// �������������� ���������� �����
			Hardware::Init(clockSource, prescaller, msbFirst, clockPolarity, clockPhase);
		}
	};

	///////////////////////////////////////////////////////////
	// ������ ���������� ����� SPI ����� ��� ���� ��������
	//////////////////////////////////////////////////////////
	template<
		unsigned char HardwareId,
		SpiRegisterAddressType Control0RegisterAddr,
		SpiRegisterAddressType Control1RegisterAddr,
		SpiRegisterAddressType BaudRateControl0RegisterAddr,
		SpiRegisterAddressType BaudRateControl1RegisterAddr,
		class StatusRegister,
		SpiRegisterAddressType RxBufferRegisterAddr,
		SpiRegisterAddressType TxBufferRegisterAddr,
		class InterruptControl,
		class MOSIPort, unsigned char MOSIPin,
		class MISOPort, unsigned char MISOPin,
		class CLKPort, unsigned char CLKPin
		>
	class SpiHardware :
		public UsciHardwareSpi<HardwareId,
			UsciControl< SpiRegisterDataType, SpiRegisterAddressType,
				// ������
				Control0RegisterAddr, Control1RegisterAddr,
				// ���������
				UsciControlRegister0Spi<SpiRegisterDataType>, UsciControlRegister1Spi<SpiRegisterDataType>
			>,
			UsciBaudRateControl< SpiRegisterDataType, SpiRegisterAddressType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>,
			StatusRegister,
			UsciBuffers<SpiRegisterDataType, SpiRegisterAddressType, RxBufferRegisterAddr, TxBufferRegisterAddr>,
			InterruptControl,
			GpioPeripheralFunctionControl<MOSIPort, MOSIPin, MISOPort, MISOPin, CLKPort, CLKPin>
		>
	{
	
	};
}
}

#endif