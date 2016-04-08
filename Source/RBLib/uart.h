///////////////////////////////////////////////////////////////////////////////
//	����� UART ����������
//	
///////////////////////////////////////////////////////////////////////////////

/*---------�������� ������������ ���������� UART (�����������)------------------

��� ������ �����������, �������� � ��������� ��������� � ��� ���������:
UART::_A<RxBufSize,TxBufSize>,
UART::_�<RxBufSize,TxBufSize>,
UART::_�<RxBufSize,TxBufSize>,
UART::_D<RxBufSize,TxBufSize> � �.�.

RxBufSize - ������ �������� ������ (������������ 255)
TxBufSize - ������ ����������� ������ (������������ 255)

���������� ������� ������� �� ��������� ���������.

����� ������ ��������� � ���������� ������� ����������� UART ����������.

-------------��������� ������-------------------------
enum DataSize {Data_8bit, Data_7bit};                   //������ ������
enum ParityControl {NoneParity,	OddParity, EvenParity}; //��� ��������
enum StopSize {OneStopBit, TwoStopBit};                 //�������� ����
enum ClockSource {ACLK = 0x40, SMCLK = 0x80};           //�������� ������������

//������� ���������� ������������� UART
//data - ������ ������
//parity - ��� ��������
//stop - �������� ����
//baudRate - ������� UART (���)
//source - ������� �������� ������������
//frequency - ������� �������� ��������� ������������
static void Initialization(DataSize data, ParityControl parity, StopSize stop, long baudRate, ClockSource source, long frequency)

//������� �������� ������ (�����������)
//data - ��������� �� �����, ��� ����� ������ ��� ��������
//length - ���������� ����, ������� ����� ���������
static void Write(unsigned char* data, int length)

//������� ����� ������ (�� �����������)
//data - ��������� �� �����, ���� ����� ������� ������
//length - ���������� ����, ������� ����� ��������
//����������� ���������� ����������� ������
static int Read(unsigned char* data, int length)

---------------- ����� ���������� ������--------------------------

----------������ ������������� ��� msp430f5438a---------------
typedef UART::_A<8,8> Com1;//������ ������� 8 ����
typedef UART::_B<8,8> Com2;
typedef UART::_C<8,8> Com3;
typedef UART::_D<8,8> Com4;

main()
{	
	Com1::Initialization(Com1::Data_8bit, Com1::NoneParity, Com1::OneStopBit, 115200, Com1::SMCLK, 1048576);
	Com2::Initialization(Com2::Data_8bit, Com2::NoneParity, Com2::OneStopBit, 115200, Com2::SMCLK, 1048576);
	
	GlobalInterruptEnable();
	
	unsigned char buf1[10]= {0,0,0,0,0,0,0,0,0,0};
	unsigned char buf2[10]= {0,0,0,0,0,0,0,0,0,0};
	unsigned char data[5] = {1,2,3,4,5};

	Com2::Write(data, 4);

	while(1)
	{
		int len = Com2::Read(buf1,10);
		Com2::Write(buf1, len);
		len = Com3::Read(buf2,10);
		Com3::Write(buf2, len);
	}
}
----------����� �������---------------

----------����� ��������---------------------------------*/

#ifndef UART_H
#define UART_H

#include "Platform.h"

#ifdef MSP430
#include "msp430f1611\uart.h"
#include "msp430f2618\uart.h"
#include "msp430f5340\uart.h"
//#include "msp430f5342\uart.h"
#include "msp430f5435a\uart.h"
#include "msp430f5438a\uart.h"
#include "msp430f5659\uart.h"
#endif

#ifdef EFM32
#include "efm32gg232f1024\uart.h"
#include "efm32gg990f1024\uart.h"
#endif
 
namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f1611, Msp430f1611::Uart,
			PlatformMsp430f2618, Msp430f2618::Uart,
			PlatformMsp430f5340, Msp430f5340::Uart,
			//PlatformMsp430f5342, Msp430f5340::Uart,
			PlatformMsp430f5435a, Msp430f5435a::Uart,
			PlatformMsp430f5438a, Msp430f5438a::Uart,
			PlatformMsp430f5659, Msp430f5659::Uart
#endif
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg232f1024::Uart,
			PlatformEfm32gg990f1024, Efm32gg990f1024::Uart
#endif
		>::Result UartTypes;
	
	// �������� ������ ���������� UART �����������
	template <Platform p>
	class UartSelector: public PlatformSelector<UartTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		UartSelector()
		{
		
		}
	};

	// ����� ���������� UART �����������
	typedef UartSelector<RBLIB_PALTFORM_AUTODETECT> Uart;
}

#endif //UART_H
