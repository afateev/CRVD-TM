///////////////////////////////////////////////////////////////////////////////
//	���������� RBLib
//	���������� ����������� � ����� �������� ������������� � ��������� ��� �����
//	�������� (�����������, ������������).
//	������� ���������� ��������� �� ��������� ������:
//		1. ������ �� ����������� ���������� ����������, ������ ��� �������,
//	����� �����-������ � �.�. ��� ��������� ��������� �����.
//		2. ������ � �������� ������������, ������ ��� Flash, GPS � �.�. ��� �����
//	���������� ���������� � API ������ �����, �.�. �������� ����������� �� ����
//	����������.
//		3. ���������������� ��� ������ ���������� � ������� ��������� "�����������".
//		4. ���� ��������������� ����������� ��������.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_H
#define RBLIB_H

// ����� ���������
#include "Common.h"
#include "CriticalSection.h"

#include "Utils\vector_static.h"
#include "Utils\queue_static.h"
#include "Utils\map_static.h"

#include "Utils\vector_dynamic.h"
#include "Utils\queue_dynamic.h"
#include "Utils\map_dynamic.h"

// �������� ����� ����������
//#include "Application.h"

// ����� ���������� ����������
//#include "GlobalVars.h"

// ������� ������������
#include "ClockSystem.h"
#include "ClockManagementUnit.h"
#include "ResetAndClockControl.h"
// ������������� ����
#include "ExternalBusInterface.h"
// ���������� ������� �����-������
#include "Gpio.h"
// ���������� ����������� ���������
#include "Timer.h"
// ���������� ��������� ��������
#include "Wdt.h"
// ���������� ���
#include "Adc.h"
// ���������� ���
#include "Dac.h"
// ���������� ���� �������
#include "FlashMemoryController.h"
// ���������� ���� �������
#include "FlashMemorySegmented.h"
// ������ � ������� ���� ������ ��� �������� ���������������� ������
#include "Flash.h"
// ���������� ���������� ����������� �������
#include "SpecialFunctionRegisters.h"
// ���������� ���������� ������������ �������
#include "SysConfigurationRegisters.h"
// ���������� �����
#include "SystemControlBlock.h"
// ���������� ���������������� �������
#include "MemorySystemController.h"
// ���������� �������� �������� ����
#include "SystemTimer.h"

// ���������� ������������
#include "InterruptMap.h"
// Nested Vectored Interrupt Controller
#include "Nvic.h"

// ���������� ���������
#include "EventDispetcher.h"

// ���������� ������������ ���������
#include "ProgramTimer.h"

// ���������� i2c ����������
//#include "i2c.h"

// ���������� UART ����������
#include "uart.h"
#include "usart.h"

// ���������� SPI ����������
#include "spi.h"
#include "SpiMaster.h"

// DMA
#include "Dma.h"

// Alternate function I/O
#include "Afio.h"

// USB
#include "Usb.h"

// �������� ���������
#include "Driver.h"

#include "ScreenBuffer.h"
#include "DrawContext.h"
#include "FbgFont.h"
#include "Fonts.h"
#include "Display.h"

#include "..\MainPanel\MegaLib\stm32\f107\Rtc.h"


/*
// ������ �������������� ����������
#include "Protocol\Protocol.h"

// ���������� ������� �������
#include "Protocol\PacketQueue.h"

// ������ ������ �������
#include "Communication\CommunicationModule.h"

// ���������� �������� ����������
#include "Communication\InterfaceSimple.h"

// ������� ����������� ������
#include "Protocol\CommandHandlerBase.h"

// ����������� �������� uart ���������� 
#include "Utils\UartEmulator.h"

// ��������
#include "Utils\Singletone.h"

// ����������� ���������� ����������� ������� ������
#include "StdDevice\VersionReporter.h"
// ����������� ���������� ���������� �������������� ���������
#include "StdDevice\CoordinatesStorage.h"
// ����������� ���������� ���������� �������������� ��������� �����������
#include "StdDevice\CoordinatesDirectionStorage.h"
// ������� �������
#include "StdDevice\Battary.h"
// ����� ������� ����������� �� ���������� ������� ������ �������
#include "StdDevice\BattaryReporter.h"
// ����������� ���������� ����������� ������� ��������� ����������
#include "StdDevice\SysInfoReporter.h"
// ����������� ���������� ������ � ���������� �� ��������� G1
//#include "StdDevice\G1\Network.h"

// ������ �����������������
#include "OperatingModeWise.h"

// ���������� ��������.
#include "PowerManagmentModule.h"

// ����������� ���������
//#include "Gdi\Gdi.h"
// ����������� ��������� ������������
//#include "Gui\Gui.h"

// �����
//#include "Modem.h"

// �������� ����������
//#include "Media.h"
*/
/*!
\mainpage ����������
- \subpage PageAbout
- \subpage PageFunctions "�������� ������� ����������:"
	- \ref PagePlatforms
		- \ref PageGpio
		- \ref PageFlash
		- \ref ClockSystem
		- \ref PowerManagmentModule
	- \ref PagePeriphery
	- \ref PageEvents
	- \ref PageSkRb
		- \ref PageFirTwig
		- \ref PageStdDev
			- \ref PageStdDevCoordinatesStorage

\page PageAbout � ����������
���������� ����������� � ����� �������� ������������� � ��������� ������� ����������� ������� ��� ����� �������� (�����������, ������������).
������� ���������� ��������� �� ��������� ������:
	- ������ �� ����������� ���������� ����������, ������ ��� �������, ����� �����-������ � �.�. ��� ��������� ��������� �����.
	- ������ � �������� ������������, ������ ��� Flash, GPS � �.�. ��� ����� ���������� ���������� � API ������ �����, �.�. �������� ����������� �� ����
����������.
	- ���������������� ��� ������ ���������� � ������� ��������� "�����������".
	- ���� ��������������� ����������� ��������.

\page PageFunctions �������� ������� ����������
	- \subpage PagePlatforms, ������ ��� �������, ����� �����-������ � �.�. ��� ��������� ��������� �����.
	- \subpage PagePeriphery, ������ ��� Flash, GPS � �.�. ��� ����� ���������� ���������� � API ������ �����, �.�. �������� ����������� �� ����
����������.
	- \subpage PageSkRb.
	- ���� ��������������� ����������� ��������.

\page PagePlatforms ������ �� ����������� ���������� ����������
���������� ������������ ��������� ���������� ����� ��������� ��������:
<table>
<tr align="center"><td>���������� �����</td>				<td>Msp430f1611</td>	<td>Msp430f2618</td>		<td>Msp430f5435a</td>		<td>Msp430f5438a</td>		</tr>
<tr align="center"><td>\ref PageGpio "Gpio"</td>			<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>Timer</td>							<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>Wdt</td>								<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>Uart</td>							<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>I2C</td>								<td></td>				<td>+ (������ master)</td>	<td>+ (������ master)</td>	<td>+ (������ master)</td>	</tr>
<tr align="center"><td>ADC</td>								<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>\ref PowerManagmentModule "PMM"</td>	<td></td>				<td></td>					<td>-+</td>					<td>-+</td>					</tr>
<tr align="center"><td>\ref ClockSystem "ClockSystem"</td>	<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
<tr align="center"><td>\ref PageFlash "Flash"</td>			<td></td>				<td>+</td>					<td>+</td>					<td>+</td>					</tr>
</table>

������ � �������������� ����������� ������� ������� � ��������� ��������:
- \subpage PageGpio
- \subpage PageFlash
- \subpage ClockSystem
- \subpage PowerManagmentModule

\page PagePeriphery ������ � �������� ������������
���� �� ������ ������������� ���������� �� ��������������.

\page PageEvents ������� �������
� ���������� ����������� ������� �������, ����������� ������������ �� ���������� �� ������������ ���������� (��� ���������� � ����������).
������� ���������� ���������� � ������������ ������� � ��������� �����. ��������� ��� �������, ������������ ����� ��������� � ��� ���� �����������.
��� ������������� ������� ����� ������������ ����� �������������� ��� ��������������� �� ����������, ��� � ��������� �� ��������� �����.
\n������ ������� �������� � ������������ Rblib::EventCode.
\n���������� ����������� ������� �������������� � ������� ���������� �������, �������������� � ������ ::Rblib::EventDispetcher.


\page PageSkRb ����������� ������ ���������� � ������� ��������� "�����������"
- \subpage PageStdDev

\page PageStdDev ����������� �������

������ ����������, ���������� � ������� �� "�����������", ������ ������������� ��� ����������� �������.

��������� �� ��� ����������� �� ������ ����������:
- \subpage PageStdDevCoordinatesStorage

\page PageStdDevCoordinatesStorage �������� � ������ �������������� ���������
���������� ������ ��������� ������� ��������� ��������� � ��������� ���������� � ����������������� ������.\n
�������������� ������ �� ���� ���������, ��� ������� ���� ������ ���������� � ������ ��������� � ��� ����������� �������.

���������� ������ ��������� ������� ������� ��������� � ���������� � ����� ����������, ����� ����������� � ������.\n
������ �������������� ������, ���� � ���� ������ ������� ������ ����� ����������, ����������� � ��� �����������.\n
����� ������������ ������ ���� � ������ ���� ����������.

������ ���������� ����������� ������� Rblib::StdDevice::CoordinatesStorage.

\page PageFirTwig �������� FirTwig2

*/

#endif
