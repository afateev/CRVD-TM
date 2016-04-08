///////////////////////////////////////////////////////////////////////////////
//	Класс UART интерфейса
//	Msp430f1611
///////////////////////////////////////////////////////////////////////////////

#ifndef UART_MSP430F1611_H
#define UART_MSP430F1611_H

#include "../msp430/x1xxx/Uart.h"
#include "../Gpio.h"
#include "../SpecialFunctionRegisters.h"

namespace Rblib
{
namespace Msp430f1611
{
  class UartHardware
  {
  public:
    typedef Msp430x1xxx::UartHardware<0x0070, 0x0071, 0x0072, 0x0074, 0x0075, 0x0073, 0x0076, 0x0077,
      Msp430x1xxx::EnableControl0<SpecialFunctionRegisters>,
      Msp430x1xxx::InterruptControl0<SpecialFunctionRegisters>, Gpio::Port3, 5, Gpio::Port3, 4>	_A;
    typedef Msp430x1xxx::UartHardware<0x0078, 0x0079, 0x007A, 0x007C, 0x007D, 0x007B, 0x007E, 0x007F,
      Msp430x1xxx::EnableControl1<SpecialFunctionRegisters>,
      Msp430x1xxx::InterruptControl1<SpecialFunctionRegisters>, Gpio::Port3, 7, Gpio::Port3, 6>	_B;
  };

	class Uart
	{
  public:

    template<class BufferType = Msp430::UartFastFifo<255> >
    class _A : public Msp430::Uart<1, UartHardware::_A, Msp430::UartFifo<1, BufferType, BufferType> > {};

    template<class BufferType = Msp430::UartFastFifo<255> >
    class _B : public Msp430::Uart<2, UartHardware::_B, Msp430::UartFifo<2, BufferType, BufferType> > {};
									
	};
}
}

#endif //UART_MSP430F1611_H
