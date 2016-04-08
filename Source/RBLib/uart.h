///////////////////////////////////////////////////////////////////////////////
//	Класс UART интерфейса
//	
///////////////////////////////////////////////////////////////////////////////

/*---------описание программного интерфейса UART (асинхронный)------------------

Все классы статические, названия и буквенная нумерация у них попорядку:
UART::_A<RxBufSize,TxBufSize>,
UART::_В<RxBufSize,TxBufSize>,
UART::_С<RxBufSize,TxBufSize>,
UART::_D<RxBufSize,TxBufSize> и т.д.

RxBufSize - размер приёмного буфера (максимальный 255)
TxBufSize - размер передающего буфера (максимальный 255)

Количество классов зависит от выбранной платформы.

Номер класса совпадает с порядковым номером аппаратного UART интерфейса.

-------------Интерфейс класса-------------------------
enum DataSize {Data_8bit, Data_7bit};                   //размер данных
enum ParityControl {NoneParity,	OddParity, EvenParity}; //бит чётности
enum StopSize {OneStopBit, TwoStopBit};                 //стоповые биты
enum ClockSource {ACLK = 0x40, SMCLK = 0x80};           //источник тактирования

//функция аппаратной инициализации UART
//data - размер данных
//parity - бит чётности
//stop - стоповые биты
//baudRate - частота UART (бод)
//source - входной источник тактирования
//frequency - частота входного источника тактирования
static void Initialization(DataSize data, ParityControl parity, StopSize stop, long baudRate, ClockSource source, long frequency)

//функция отправки данных (блокирующая)
//data - указатель на буфер, где лежат данные для отправки
//length - количество байт, которые нужно отправить
static void Write(unsigned char* data, int length)

//функция приёма данных (не блокирующая)
//data - указатель на буфер, куда хотим принять данные
//length - количество байт, которые хотим получить
//возращается количество прочитанных байтов
static int Read(unsigned char* data, int length)

---------------- конец интерфейса класса--------------------------

----------пример использования для msp430f5438a---------------
typedef UART::_A<8,8> Com1;//размер буферов 8 байт
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
----------конец примера---------------

----------конец описания---------------------------------*/

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
	// Селектор платформа / пространство имён
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
	
	// Селектор класса управления UART интерфейсом
	template <Platform p>
	class UartSelector: public PlatformSelector<UartTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		UartSelector()
		{
		
		}
	};

	// Класс управления UART интерфейсом
	typedef UartSelector<RBLIB_PALTFORM_AUTODETECT> Uart;
}

#endif //UART_H
