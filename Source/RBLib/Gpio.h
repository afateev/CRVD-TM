///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ������� �����-������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_H
#define GPIO_H

#include "Platform.h"

#ifdef MSP430
#include "msp430f1611\Gpio.h"
#include "msp430f2618\Gpio.h"
#include "msp430f5340\Gpio.h"
//#include "msp430f5342\Gpio.h"
#include "msp430f5435a\Gpio.h"
#include "msp430f5438a\Gpio.h"
#include "msp430f5659\Gpio.h"
#endif

#ifdef EFM32
#include "efm32gg\Gpio.h"
#endif

#ifdef STM32
#include "stm32\Gpio.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f1611, Msp430f1611::Gpio,
			PlatformMsp430f2618, Msp430f2618::Gpio,
			PlatformMsp430f5340, Msp430f5340::Gpio,
			//PlatformMsp430f5342, Msp430f5342::Gpio,
			PlatformMsp430f5435a, Msp430f5435a::Gpio,
			PlatformMsp430f5438a, Msp430f5438a::Gpio,
			PlatformMsp430f5659, Msp430f5659::Gpio
#endif
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg::Gpio,
			PlatformEfm32gg990f1024, Efm32gg::Gpio
#endif
#ifdef STM32
        PlatformStm32, Stm32::Gpio
#endif
		>::Result GpioTypes;
	
	// �������� ������ ���������� ������� ����� ������
	template <Platform p>
	class GpioSelector: public PlatformSelector<GpioTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		GpioSelector()
		{
		
		}
	};

	// ����� ���������� ��������� ������� �����-������
	typedef GpioSelector<RBLIB_PALTFORM_AUTODETECT> Gpio;

/*!
\page PageGpio Gpio - �������� ����� �����/������
������ ���������������� ����� ����� ��������� ������ �����/������. ������ ���� �������� ��������� (������ 8) ������� (�����).
<br>������ �� ������� ����� ������������� ��������������� � �������� ����� ��� ������.
<br>������ � ������ ����� �����/������ ����� ����� �������������� � �������������� �������.
<br>��������� ����� ������������ ������� ����������. ��� ������� �� ������� ������ ������ ����� ������������� ��������� ���������� � ���������������� ��� ���,
����� ��� �������������� �� ������������ ��� ���������� ������ �������� �������.
<br>������ ������ ����� ����� ���������� ������������� ���������, ������� ����� �������� � ��������� �������� � ������� ���� � "�����".
<br>����������� ������� ������ ������������ ���������� ������������� ��������, ������� ������ ��������� ��������� ����� ����� ��� ����� �����/������ ��� ��� ����� ������������� ������.
<br>��� ���������� �������������� ���� ��������� �������� ������ �����/������ ������������ ����� Gpio.
*/

/*!
\class Rblib::Gpio
\brief ����� ���������� ��������� ������� �����-������
\extends Msp430f1611::Gpio
\extends Msp430f2618::Gpio
\extends Msp430f5435a::Gpio
\extends Msp430f5438a::Gpio

� ����������� �� ��������� ������ ���� ������������ ��������� �������, ����������� ���������� ��������:
- \ref Rblib::GpioPortInputOutputDirection "GpioPortInputOutputDirection" - ������, ������ � ��������� ����������� ������ "�����" �����
	- \ref Rblib::GpioPortInputOutputDirection::Read() "Read()"
	- \ref Rblib::GpioPortInputOutputDirection::Read(unsigned char pinNumber) "Read(pinNumber)"
	- \ref Rblib::GpioPortInputOutputDirection::Write(PortValueDataType value) " Write(value)"
	- \ref Rblib::GpioPortInputOutputDirection::SetBit(unsigned char pinNumber) "SetBit(pinNumber)"
	- \ref Rblib::GpioPortInputOutputDirection::ClearBit(unsigned char pinNumber) "ClearBit(pinNumber)"
	- \ref Rblib::GpioPortInputOutputDirection::ToggleBit(unsigned char pinNumber) "ToggleBit(pinNumber)"
	- \ref Rblib::GpioPortInputOutputDirection::SetDirection(PortValueDataType mask) "SetDirection(mask)"
	- \ref Rblib::GpioPortInputOutputDirection::SetOutputPin(unsigned char pinNumber) "SetOutputPin(pinNumber)"
	- \ref Rblib::GpioPortInputOutputDirection::SetInputPin(unsigned char pinNumber) "SetInputPin(pinNumber)"
- \ref Rblib::GpioPortFunctionSelect "GpioPortFunctionSelect" - ��������� ����� ����� �� ������ � ���������� ��� ��� ����� �����/������
	- \ref Rblib::GpioPortFunctionSelect::SetFunctionGeneralIo(unsigned char pinNumber) "SetFunctionGeneralIo(pinNumber)"
	- \ref Rblib::GpioPortFunctionSelect::SetFunctionPeripheral(unsigned char pinNumber) "SetFunctionPeripheral(pinNumber)"
- \ref Rblib::GpioPortInterrupt "GpioPortInterrupt" - ���������� ������������ �����
	- \ref Rblib::GpioPortInterrupt::GetInterruptFlags(void) "GetInterruptFlags()"
	- \ref Rblib::GpioPortInterrupt::GetInterruptFlag(unsigned char bitNumber) "GetInterruptFlag(bitNumber)"
	- \ref Rblib::GpioPortInterrupt::SetInterruptFlags(PortValueDataType value) "SetInterruptFlags(value)"
	- \ref Rblib::GpioPortInterrupt::SetInterruptFlag(unsigned char bitNumber) "SetInterruptFlag(bitNumber)"
	- \ref Rblib::GpioPortInterrupt::ClearInterruptFlag(unsigned char bitNumber) "ClearInterruptFlag(bitNumber)"
	- \ref Rblib::GpioPortInterrupt::GetInterruptEdgeSelect(void) "GetInterruptEdgeSelect()"
	- \ref Rblib::GpioPortInterrupt::GetInterruptEdgeSelect(unsigned char bitNumber) "GetInterruptEdgeSelect(bitNumber)"
	- \ref Rblib::GpioPortInterrupt::SetInterruptEdgeSelect(PortValueDataType value) "SetInterruptEdgeSelect(value)"
	- \ref Rblib::GpioPortInterrupt::SetInterruptEdgeHiToLow(unsigned char bitNumber) "SetInterruptEdgeHiToLow(bitNumber)"
	- \ref Rblib::GpioPortInterrupt::SetInterruptEdgeLowToHi(unsigned char bitNumber) "SetInterruptEdgeLowToHi(bitNumber)"
	- \ref Rblib::GpioPortInterrupt::GetInterruptEnable(void) "GetInterruptEnable()"
	- \ref Rblib::GpioPortInterrupt::GetInterruptEnable(unsigned char bitNumber) "GetInterruptEnable(bitNumber)"
	- \ref Rblib::GpioPortInterrupt::SetInterruptEnable(PortValueDataType value) "SetInterruptEnable(value)"
	- \ref Rblib::GpioPortInterrupt::InterruptEnable(unsigned char bitNumber) "InterruptEnable(bitNumber)"
	- \ref Rblib::GpioPortInterrupt::InterruptDisable(unsigned char bitNumber) "InterruptDisable(bitNumber)"
- \ref Rblib::GpioPortResistor "GpioPortResistor" - ���������� �������������� �����������
	- \ref Rblib::GpioPortResistor::ResistorEnable(unsigned char pinNumber) "ResistorEnable(pinNumber)"
	- \ref Rblib::GpioPortResistor::ResistorDisable(unsigned char pinNumber) "ResistorDisable(pinNumber)"
*/

}

#endif