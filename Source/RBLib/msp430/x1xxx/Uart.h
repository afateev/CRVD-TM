
#ifndef UART_MSP430X1XXX_H
#define UART_MSP430X1XXX_H

#include "../Uart.h"
#include "./UsciParts.h"

namespace Rblib
{
namespace Msp430x1xxx
{

  // Тип значения регистра
  typedef Msp430::UartRegisterDataType UartRegisterDataType;
  // Тип адреса регистра
  typedef Msp430::UartRegisterAddressType UartRegisterAddressType;

  class UartConstants : public Msp430::UartConstants
  {
  public:

    // Размер данных
    enum DataSize
    {
      DataSize7Bit = 0,
      DataSize8Bit = 1
    };

  };

  ///////////////////////////////////////////////////////////
  // Шаблон аппаратной части UART
  //////////////////////////////////////////////////////////
  template<
    UartRegisterAddressType ControlRegister0Addr,
    UartRegisterAddressType TransmittControlRegisterAddr,
    UartRegisterAddressType ReceiveControlRegisterAddr,
    UartRegisterAddressType BaudRateControl0RegisterAddr,
    UartRegisterAddressType BaudRateControl1RegisterAddr,
    UartRegisterAddressType ModulationControlRegisterAddr,
    UartRegisterAddressType RxBufferRegisterAddr,
    UartRegisterAddressType TxBufferRegisterAddr,
    class EnableControl,
    class InterruptControl,
    class RxPort, unsigned char RxPin,
    class TxPort, unsigned char TxPin
  >
  class UartHardware :
    public Msp430::UsciHardwareUart<
    UsartControl< UartRegisterDataType, UartRegisterAddressType,
    // адреса
    ControlRegister0Addr, TransmittControlRegisterAddr,
    // структуры
    UsartControlRegisterUart<UartRegisterDataType>, UsartTransmitControlRegisterUart<UartRegisterDataType>
    >,
    Msp430::UsciBaudRateControl< UartRegisterDataType, UartRegisterAddressType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>,
    UsartModulationControl< UartRegisterDataType, UartRegisterAddressType,
    // адрес
    ModulationControlRegisterAddr,
    // структура
    UsartModulationControlRegisterUart<UartRegisterDataType>
    >,
    Msp430::UsciStatus< UartRegisterDataType, UartRegisterAddressType,
    // адрес
    ReceiveControlRegisterAddr,
    // структура
    UsartReceiveControlRegisterUart<UartRegisterDataType>
    >,
    Msp430::UsciBuffers< UartRegisterDataType, UartRegisterAddressType, RxBufferRegisterAddr, TxBufferRegisterAddr>,
    InterruptControl,
    Msp430::GpioPeripheralFunctionControl<RxPort, RxPin, TxPort, TxPin>,
    UartConstants
    >
  {
  public:

    typedef Msp430::UsciHardwareUart<
      UsartControl< UartRegisterDataType, UartRegisterAddressType,
      // адреса
      ControlRegister0Addr, TransmittControlRegisterAddr,
      // структуры
      UsartControlRegisterUart<UartRegisterDataType>, UsartTransmitControlRegisterUart<UartRegisterDataType>
      >,
      Msp430::UsciBaudRateControl< UartRegisterDataType, UartRegisterAddressType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>,
      UsartModulationControl< UartRegisterDataType, UartRegisterAddressType,
      // адрес
      ModulationControlRegisterAddr,
      // структура
      UsartModulationControlRegisterUart<UartRegisterDataType>
      >,
      Msp430::UsciStatus< UartRegisterDataType, UartRegisterAddressType,
      // адрес
      ReceiveControlRegisterAddr,
      // структура
      UsartReceiveControlRegisterUart<UartRegisterDataType>
      >,
      Msp430::UsciBuffers< UartRegisterDataType, UartRegisterAddressType, RxBufferRegisterAddr, TxBufferRegisterAddr>,
      InterruptControl,
      Msp430::GpioPeripheralFunctionControl<RxPort, RxPin, TxPort, TxPin>,
      UartConstants
    > _Base;

    typedef Msp430::UsciBaudRateControl< UartRegisterDataType, UartRegisterAddressType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr> _BaudRateControl;
    typedef UsartModulationControl< UartRegisterDataType, UartRegisterAddressType, ModulationControlRegisterAddr, UsartModulationControlRegisterUart<UartRegisterDataType> > _ModulationControl;

  public:

    static void Init(typename _Base::DataSize dataSize, typename  _Base::Parity parity, typename _Base::StopBits stopBits, long baudRate, typename _Base::ClockSource source, long frequency)
    {
      _Base::Enable(false);
      _Base::BaseInitialisation(dataSize, parity, stopBits, source);

      float n = (float)frequency / baudRate;
      unsigned int br = (unsigned int)n;

      // Выставляем битрейт
      _BaudRateControl::Set((unsigned char)br, (unsigned char)(br >> 8));

      long modulation = ((frequency - br * baudRate) * 100) / baudRate;
      unsigned char modVal = 0;

      if(modulation < 5)
      {
        modVal = 0x00;
      }
      else if(modulation >= 5 && modulation < 20)
      {
        modVal = 0x10;
      }
      else if(modulation >= 20 && modulation < 31)
      {
        modVal = 0x22;
      }
      else if(modulation >= 31 && modulation < 45)
      {
        modVal = 0x29;
      }
      else if(modulation >= 45 && modulation < 55)
      {
        modVal = 0x55;
      }
      else if(modulation >= 55 && modulation < 67)
      {
        modVal = 0xD6;
      }
      else if(modulation >= 67 && modulation < 81)
      {
        modVal = 0xDD;
      }
      else if(modulation >= 81 && modulation < 95)
      {
        modVal = 0xEF;
      }
      else if(modulation >= 95)
      {
        modVal = 0xFF;
      }

      // Выставляем параметры модуляции
      _ModulationControl::Set(modVal);

      _Base::Enable(true);

      EnableControl::EnableRx(true);
      EnableControl::EnableTx(true);
    }

    // Обработка прерывания на прием
    static inline void RxHandler()
    {
        _Base::RxHandlerBody();
    }

    // Обработка прерывания на отправку
    static inline void TxHandler()
    {
        _Base::TxHandlerBody();
    }

  };

}
}

#endif // UART_MSP430X1XXX_H
