
#ifndef USCI_PARTS_MSP430X1XXX_H
#define USCI_PARTS_MSP430X1XXX_H

namespace Rblib
{
namespace Msp430x1xxx
{

  // ��������� �������� ���������� UxCTL
  // � ������ Uart
  template<class RegisterDataType>
  struct UsartControlRegisterUart
  {
    RegisterDataType Reset : 1; // �����
    RegisterDataType MultiprocessorMode : 1;	// ����� ������
    RegisterDataType SynchronousModeEnable	: 1;	// ���������� �����
    RegisterDataType ListenEnable : 1;	//
    RegisterDataType CharacterLength		: 1;	// ����� �������� (7 ��� 8 ���)
    RegisterDataType StopBits				: 1;	// ���������� �������� �����
    RegisterDataType Parity					: 2;	// ��������
  };

  // ��������� �������� ���������� UxTCTL
  // � ������ Uart
  template<class RegisterDataType>
  struct UsartTransmitControlRegisterUart
  {
    RegisterDataType TransmitterEmpty : 1;
    RegisterDataType NotUsed0 : 1;
    RegisterDataType TransmitterWake : 1;
    RegisterDataType StartEdjeReceiveEnable : 1;
    RegisterDataType ClockSource : 2;
    RegisterDataType ClockPolarity : 1;
    RegisterDataType NotUsed : 1;
  };

  // ��������� �������� ���������� UxRCTL
  // � ������ Uart
  template<class RegisterDataType>
  struct UsartReceiveControlRegisterUart
  {
    RegisterDataType FramingError : 1;
    RegisterDataType ParityError : 1;
    RegisterDataType OverrunError : 1;
    RegisterDataType Break : 1;
    RegisterDataType ReceiveErroneousCharInterruptEnable : 1;
    RegisterDataType ReceiveWakeUpInterruptEnable : 1;
    RegisterDataType ReceiveWakeUpFlag : 1;
    RegisterDataType RxError : 1;
  };

  // ��������� �������� ���������� UxMCTL
  // � ������ Uart
  template<class RegisterDataType>
  struct UsartModulationControlRegisterUart
  {
    RegisterDataType Modulation;
  };

////////////////////////////////////

  // ����� ��� ���������� ���������� ������ ������ SUART0
  template<class Sfr>
  class EnableControl0
  {
  public:
    
    static inline void EnableRx(bool value)
    {
      Sfr::EnableURx0(value);
    }

    static inline void EnableTx(bool value)
    {
      Sfr::EnableUTx0(value);
    }
  };

  // ����� ��� ���������� ���������� ������ ������ SUART1
  template<class Sfr>
  class EnableControl1
  {
  public:

    static inline void EnableRx(bool value)
    {
      Sfr::EnableURx1(value);
    }

    static inline void EnableTx(bool value)
    {
      Sfr::EnableUTx1(value);
    }
  };

  // ����� ���������� ������������ ���������� USART0
  template<class Sfr>
  class InterruptControl0
  {
  public:
    static inline void SetRxInterruptEnable(bool value)
    {
      Sfr::SetInterruptEnableURx0(value);
    }
    static inline void SetTxInterruptEnable(bool value)
    {
      Sfr::SetInterruptEnableUTx0(value);
    }
    static inline bool GetRxInterruptFlag()
    {
      return Sfr::GetInterruptFlagURx0();
    }
    static inline bool GetTxInterruptFlag()
    {
      return Sfr::GetInterruptFlagUTx0();
    }
    static inline void ClearRxInterruptFlag()
    {
      Sfr::ClearInterruptFlagURx0();
    }
    static inline void ClearTxInterruptFlag()
    {
      Sfr::ClearInterruptFlagUTx0();
    }
  };

  // ����� ���������� ������������ ���������� USART1
  template<class Sfr>
  class InterruptControl1
  {
  public:
    static inline void SetRxInterruptEnable(bool value)
    {
      Sfr::SetInterruptEnableURx1(value);
    }
    static inline void SetTxInterruptEnable(bool value)
    {
      Sfr::SetInterruptEnableUTx1(value);
    }
    static inline bool GetRxInterruptFlag()
    {
      return Sfr::GetInterruptFlagURx1();
    }
    static inline bool GetTxInterruptFlag()
    {
      return Sfr::GetInterruptFlagUTx1();
    }
    static inline void ClearRxInterruptFlag()
    {
      Sfr::ClearInterruptFlagURx1();
    }
    static inline void ClearTxInterruptFlag()
    {
      Sfr::ClearInterruptFlagUTx1();
    }
  };

  ////////////////////////////////////////////////////
  // ����� �������� ����������
  ////////////////////////////////////////////////////
  template <class RegisterDataType,			// ��� (������) �������� ��������
  class PointerType,					// ��� (������) �������� ������� ���������
    PointerType Control0RegisterAddr,	// ����� �������� UxCTL
    PointerType Control1RegisterAddr,	// ����� �������� UxTCTL
  class Control0RegisterStructure,
  class Control1RegisterStructure
  >
  class UsartControl
  {
  protected:
    // ������ ��������� ����������
    static volatile Control0RegisterStructure * const _control0RegisterAddr;
    static volatile Control1RegisterStructure * const _control1RegisterAddr;
  protected:
    static inline void Reset(bool reset)
    {
      _control0RegisterAddr->Reset = reset;
    }

    static inline void SetGeneralSettings(bool synchModeEnable, RegisterDataType charLen, RegisterDataType parity, RegisterDataType stopBits)
    {
      _control0RegisterAddr->SynchronousModeEnable = synchModeEnable;
      _control0RegisterAddr->CharacterLength = charLen;
      _control0RegisterAddr->Parity = parity;
      _control0RegisterAddr->StopBits = stopBits;
    }

    static inline void SetClockSource(RegisterDataType source)
    {
      _control1RegisterAddr->ClockSource = source;
    }
  };

  // ��������������� ����������� ����������
  template <class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure>
  volatile Control0RegisterStructure * const UsartControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
    ::_control0RegisterAddr = reinterpret_cast<volatile  Control0RegisterStructure*>(Control0RegisterAddr);

  template <class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure>
  volatile Control1RegisterStructure * const UsartControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
    ::_control1RegisterAddr = reinterpret_cast<volatile  Control1RegisterStructure*>(Control1RegisterAddr);

  ////////////////////////////////////////////////////
  // ������� ���������� ����������
  ////////////////////////////////////////////////////
  template <class RegisterDataType,			// ��� (������) �������� ��������
  class PointerType,					// ��� (������) �������� ������� ���������
    PointerType ModulationControlRegisterAddr,	// ����� ��������
  class ModulationControlRegisterStructure	// ��������� ��������
  >
  class UsartModulationControl
  {
  protected:
    // ������ ��������� ����������
    static volatile ModulationControlRegisterStructure * const _modulationControlRegisterAddr;
  protected:

    static inline void Set(RegisterDataType modulation)
    {
      _modulationControlRegisterAddr->Modulation = modulation;
    }

  };

  // ��������������� ����������� ����������
  template <class RegisterDataType, class PointerType, PointerType ModulationControlRegisterAddr, class ModulationControlRegisterStructure>
  volatile ModulationControlRegisterStructure * const UsartModulationControl<RegisterDataType, PointerType, ModulationControlRegisterAddr, ModulationControlRegisterStructure>
    ::_modulationControlRegisterAddr = reinterpret_cast<volatile  ModulationControlRegisterStructure*>(ModulationControlRegisterAddr);
}
}

#endif // USCI_PARTS_MSP430X1XXX_H
