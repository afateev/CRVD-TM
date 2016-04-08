
#ifndef SPECIALFUNCTIONREGISTERS_MSP430X1XXX_H
#define SPECIALFUNCTIONREGISTERS_MSP430X1XXX_H

namespace Rblib
{
namespace Msp430x1xxx
{
  // ��������� �������� ���������� ���������� 1
  template<class RegisterDataType>
  struct SfrInterruptEnable1Register
  {
    RegisterDataType Watchdog : 1;	// ��������� ���������� ���������� �������
    RegisterDataType OscillatorFault : 1;	// ������ ������������
    RegisterDataType Reserved0 : 2;	// 
    RegisterDataType NmiPin : 1;	// 
    RegisterDataType FlashControllerAccessViolation	: 1;	// 
    RegisterDataType URx0 : 1;	// 
    RegisterDataType UTx0 : 1;	// 
  };

  // ��������� �������� ���������� ���������� 2
  template<class RegisterDataType>
  struct SfrInterruptEnable2Register
  {
    RegisterDataType Reserved0 : 4;
    RegisterDataType URx1 : 1;
    RegisterDataType UTx1 : 1;
    RegisterDataType Reserved1 : 2;
  };

  // ��������� �������� ������ ���������� 1
  template<class RegisterDataType>
  struct SfrInterruptFlag1Register
  {
    RegisterDataType Watchdog : 1;	// ��������� ���������� ���������� �������
    RegisterDataType OscillatorFault : 1;	// ������ ������������
    RegisterDataType Reserved0 : 2;	// 
    RegisterDataType NmiPin : 1;	// 
    RegisterDataType Reserved1 : 1;	// 
    RegisterDataType URx0 : 1;	// 
    RegisterDataType UTx0 : 1;	// 
  };

  // ��������� �������� ������ ���������� 2
  template<class RegisterDataType>
  struct SfrInterruptFlag2Register
  {
    RegisterDataType Reserved0 : 4;
    RegisterDataType URx1 : 1;
    RegisterDataType UTx1 : 1;
    RegisterDataType Reserved1 : 2;
  };

  // ��������� �������� ME1
  template<class RegisterDataType>
  struct SfrModuleEnable1Register
  {
    RegisterDataType Reserved0 : 6;	// 
    RegisterDataType URx0 : 1;	// 
    RegisterDataType UTx0 : 1;	// 
  };

  // ��������� �������� ME2
  template<class RegisterDataType>
  struct SfrModuleEnable2Register
  {
    RegisterDataType Reserved0 : 4;	// 
    RegisterDataType URx1 : 1;	// 
    RegisterDataType UTx1 : 1;	// 
    RegisterDataType Reserved1 : 2;	//
  };

  // ������� ME1
  template <class RegisterDataType,				// ��� (������) �������� ��������
  class PointerType,						// ��� (������) �������� ������� ���������
    PointerType RegisterAddr				// ����� ��������
  >
  class ModuleEnable1Register
  {
  protected:

    static volatile SfrModuleEnable1Register<RegisterDataType> * const _registerAddr;

  public:

    static inline void EnableURx0(bool value)
    {
      _registerAddr->URx0 = value;
    }

    static inline void EnableUTx0(bool value)
    {
      _registerAddr->UTx0 = value;
    }

  };

  // ������� ME2
  template <class RegisterDataType,				// ��� (������) �������� ��������
  class PointerType,						// ��� (������) �������� ������� ���������
    PointerType RegisterAddr				// ����� ��������
  >
  class ModuleEnable2Register
  {
  protected:

    static volatile SfrModuleEnable2Register<RegisterDataType> * const _registerAddr;

  public:

    static inline void EnableURx1(bool value)
    {
      _registerAddr->URx1 = value;
    }

    static inline void EnableUTx1(bool value)
    {
      _registerAddr->UTx1 = value;
    }

  };

  // ������� ���������� ���������� 1
  template <class RegisterDataType,				// ��� (������) �������� ��������
  class PointerType,						// ��� (������) �������� ������� ���������
    PointerType RegisterAddr				// ����� ��������
  >
  class InterruptEnable1Register
  {
  protected:

    static volatile SfrInterruptEnable1Register<RegisterDataType> * const _registerAddr;

  public:

    static inline void EnableOscillatorFault()
    {
      _registerAddr->OscillatorFault = 1;
    }

    static inline void DisableOscillatorFault()
    {
      _registerAddr->OscillatorFault = 0;
    }

    static inline void SetInterruptEnableURx0(bool value)
    {
      _registerAddr->URx0 = value;
    }
    
    static inline void SetInterruptEnableUTx0(bool value)
    {
      _registerAddr->UTx0 = value;
    }

  };

  // ������� ���������� ���������� 2
  template <class RegisterDataType,				// ��� (������) �������� ��������
  class PointerType,						// ��� (������) �������� ������� ���������
    PointerType RegisterAddr				// ����� ��������
  >
  class InterruptEnable2Register
  {
  protected:

    static volatile SfrInterruptEnable2Register<RegisterDataType> * const _registerAddr;

  public:

    static inline void SetInterruptEnableURx1(bool value)
    {
      _registerAddr->URx1 = value;
    }

    static inline void SetInterruptEnableUTx1(bool value)
    {
      _registerAddr->UTx1 = value;
    }

  };

  // ������� ������ ���������� 1
  template <class RegisterDataType,				// ��� (������) �������� ��������
  class PointerType,						// ��� (������) �������� ������� ���������
    PointerType RegisterAddr				// ����� ��������
  >
  class InterruptFlag1Register
  {
  protected:
    static volatile SfrInterruptFlag1Register<RegisterDataType> * const _registerAddr;
  public:
    // ��������� ���� ������������ Watchdog
    static inline bool GetWatchdogFlag()
    {
      return _registerAddr->Watchdog;
    }
    // �������� ���� ������������ Watchdog
    static inline void ClearWatchdogFlag()
    {
      _registerAddr->Watchdog = 0;
    }
    // �������� ���� ������ ������������
    static inline void ClearOscillatorFault()
    {
      _registerAddr->OscillatorFault = 0;
    }

    static inline bool GetInterruptFlagURx0()
    {
      return _registerAddr->URx0;
    }

    static inline bool GetInterruptFlagUTx0()
    {
      return _registerAddr->UTx0;
    }

    static inline void ClearInterruptFlagURx0()
    {
      _registerAddr->URx0 = 0;
    }

    static inline void ClearInterruptFlagUTx0()
    {
      _registerAddr->UTx0 = 0;
    }

  };

  // ������� ������ ���������� 2
  template <class RegisterDataType,				// ��� (������) �������� ��������
  class PointerType,						// ��� (������) �������� ������� ���������
    PointerType RegisterAddr				// ����� ��������
  >
  class InterruptFlag2Register
  {
  protected:
    static volatile SfrInterruptFlag2Register<RegisterDataType> * const _registerAddr;
  public:

    static inline bool GetInterruptFlagURx1()
    {
      return _registerAddr->URx1;
    }

    static inline bool GetInterruptFlagUTx1()
    {
      return _registerAddr->UTx1;
    }

    static inline void ClearInterruptFlagURx1()
    {
      _registerAddr->URx1 = 0;
    }

    static inline void ClearInterruptFlagUTx1()
    {
      _registerAddr->UTx1 = 0;
    }

  };

  // ��������������� ����������� ����������
  template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
  volatile  SfrModuleEnable1Register<RegisterDataType>* const ModuleEnable1Register<RegisterDataType, PointerType, RegisterAddr>
    ::_registerAddr = reinterpret_cast<volatile SfrModuleEnable1Register<RegisterDataType>*>(RegisterAddr);

  // ��������������� ����������� ����������
  template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
  volatile  SfrModuleEnable2Register<RegisterDataType>* const ModuleEnable2Register<RegisterDataType, PointerType, RegisterAddr>
    ::_registerAddr = reinterpret_cast<volatile SfrModuleEnable2Register<RegisterDataType>*>(RegisterAddr);

  // ��������������� ����������� ����������
  template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
  volatile  SfrInterruptEnable1Register<RegisterDataType>* const InterruptEnable1Register<RegisterDataType, PointerType, RegisterAddr>
    ::_registerAddr = reinterpret_cast<volatile SfrInterruptEnable1Register<RegisterDataType>*>(RegisterAddr);

  template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
  volatile  SfrInterruptEnable2Register<RegisterDataType>* const InterruptEnable2Register<RegisterDataType, PointerType, RegisterAddr>
    ::_registerAddr = reinterpret_cast<volatile SfrInterruptEnable2Register<RegisterDataType>*>(RegisterAddr);

  template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
  volatile  SfrInterruptFlag1Register<RegisterDataType>* const InterruptFlag1Register<RegisterDataType, PointerType, RegisterAddr>
    ::_registerAddr = reinterpret_cast<volatile SfrInterruptFlag1Register<RegisterDataType>*>(RegisterAddr);

  template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
  volatile  SfrInterruptFlag2Register<RegisterDataType>* const InterruptFlag2Register<RegisterDataType, PointerType, RegisterAddr>
    ::_registerAddr = reinterpret_cast<volatile SfrInterruptFlag2Register<RegisterDataType>*>(RegisterAddr);

}
}

#endif // SPECIALFUNCTIONREGISTERS_MSP430X1XXX_H
