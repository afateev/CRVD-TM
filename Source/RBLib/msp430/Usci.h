///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������� USCI
//	MSP430
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef USCI_MSP430_H
#define USCI_MSP430_H

#include "../Events.h"
#include "../EventDispetcher.h"
#include "../Utils/Singletone.h"

namespace Rblib
{
namespace Msp430
{
	////////////////////////////////////////////////////
	// ����� �������� ����������
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// ��� (������) �������� ��������
			class PointerType,					// ��� (������) �������� ������� ���������
			PointerType Control0RegisterAddr,	// ����� �������� 0 ����������
			PointerType Control1RegisterAddr,	// ����� �������� 1 ����������
			class Control0RegisterStructure,	// ��������� �������� 0 ����������
			class Control1RegisterStructure		// ��������� �������� 1 ����������
		>
	class UsciControl
	{
	protected:
		// ������ ��������� ����������
		static volatile Control0RegisterStructure * const _control0RegisterAddr;
		static volatile Control1RegisterStructure * const _control1RegisterAddr;
  protected:
    static inline void Reset(bool reset)
    {
      _control1RegisterAddr->Reset = reset;
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
	volatile Control0RegisterStructure * const UsciControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
		::_control0RegisterAddr = reinterpret_cast<volatile  Control0RegisterStructure*>(Control0RegisterAddr);
	
	template <class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure>
	volatile Control1RegisterStructure * const UsciControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
		::_control1RegisterAddr = reinterpret_cast<volatile  Control1RegisterStructure*>(Control1RegisterAddr);

	////////////////////////////////////////////////////
	// �������� ���������� ���������
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// ��� (������) �������� ��������
			class PointerType,					// ��� (������) �������� ������� ���������
			PointerType BaudRateControl0RegisterAddr,	// ����� �������� 0 ����������
			PointerType BaudRateControl1RegisterAddr	// ����� �������� 1 ����������
		>
	class UsciBaudRateControl
	{
	protected:
		// ������ ��������� ����������
		static volatile RegisterDataType * const _baudRateControl0RegisterAddr;
		static volatile RegisterDataType * const _baudRateControl1RegisterAddr;

  protected:

    static inline void Set(RegisterDataType low, RegisterDataType high)
    {
      *_baudRateControl0RegisterAddr = low;
      *_baudRateControl1RegisterAddr = high;
    }

	};
	
	// ��������������� ����������� ����������
	template <class RegisterDataType, class PointerType, PointerType BaudRateControl0RegisterAddr, PointerType BaudRateControl1RegisterAddr>
	volatile RegisterDataType * const UsciBaudRateControl<RegisterDataType, PointerType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>
		::_baudRateControl0RegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(BaudRateControl0RegisterAddr);

	template <class RegisterDataType, class PointerType, PointerType BaudRateControl0RegisterAddr, PointerType BaudRateControl1RegisterAddr>
	volatile RegisterDataType * const UsciBaudRateControl<RegisterDataType, PointerType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>
		::_baudRateControl1RegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(BaudRateControl1RegisterAddr);

	////////////////////////////////////////////////////
	// ������� ���������� ����������
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// ��� (������) �������� ��������
			class PointerType,					// ��� (������) �������� ������� ���������
			PointerType ModulationControlRegisterAddr,	// ����� ��������
			class ModulationControlRegisterStructure	// ��������� ��������
		>
	class UsciModulationControl
	{
	protected:
		// ������ ��������� ����������
		static volatile ModulationControlRegisterStructure * const _modulationControlRegisterAddr;

    static inline void Set(RegisterDataType modulation)
    {
      _modulationControlRegisterAddr->OversamplingEnable = 0;
      _modulationControlRegisterAddr->FirstModulationStage = 0;
      _modulationControlRegisterAddr->SecondModulationStage = modulation;
    }

	};

	// ��������������� ����������� ����������
	template <class RegisterDataType, class PointerType, PointerType ModulationControlRegisterAddr, class ModulationControlRegisterStructure>
	volatile ModulationControlRegisterStructure * const UsciModulationControl<RegisterDataType, PointerType, ModulationControlRegisterAddr, ModulationControlRegisterStructure>
		::_modulationControlRegisterAddr = reinterpret_cast<volatile  ModulationControlRegisterStructure*>(ModulationControlRegisterAddr);

	////////////////////////////////////////////////////
	// ������� �������
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// ��� (������) �������� ��������
			class PointerType,					// ��� (������) �������� ������� ���������
			PointerType StatusRegisterAddr,	// ����� ��������
			class StatusRegisterStructure	// ��������� ��������
		>
	class UsciStatus
	{
	protected:
		// ������ ��������� ����������
		static volatile StatusRegisterStructure * const _statusRegisterAddr;
	};

	// ��������������� ����������� ����������
	template <class RegisterDataType, class PointerType, PointerType StatusRegisterAddr, class StatusRegisterStructure>
	volatile StatusRegisterStructure * const UsciStatus<RegisterDataType, PointerType, StatusRegisterAddr, StatusRegisterStructure>
		::_statusRegisterAddr = reinterpret_cast<volatile  StatusRegisterStructure*>(StatusRegisterAddr);

	////////////////////////////////////////////////////
	// �������� ���������� �������
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// ��� (������) �������� ��������
			class PointerType,					// ��� (������) �������� ������� ���������
			PointerType RxBufferRegisterAddr,	// ����� �������� ������ ������
			PointerType TxBufferRegisterAddr	// ����� �������� ������ ��������
		>
	class UsciBuffers
	{
	public:
		typedef RegisterDataType BufferType;	// ��� ������ ������
	protected:
		// ������ ��������� ����������
		static volatile RegisterDataType * const _rxBufferRegisterAddr;
		static volatile RegisterDataType * const _txBufferRegisterAddr;
	};
	
	// ��������������� ����������� ����������
	template <class RegisterDataType, class PointerType, PointerType RxBufferRegisterAddr, PointerType TxBufferRegisterAddr>
	volatile RegisterDataType * const UsciBuffers<RegisterDataType, PointerType, RxBufferRegisterAddr, TxBufferRegisterAddr>
		::_rxBufferRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(RxBufferRegisterAddr);

	template <class RegisterDataType, class PointerType, PointerType RxBufferRegisterAddr, PointerType TxBufferRegisterAddr>
	volatile RegisterDataType * const UsciBuffers<RegisterDataType, PointerType, RxBufferRegisterAddr, TxBufferRegisterAddr>
		::_txBufferRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(TxBufferRegisterAddr);

	////////////////////////////////////////////////////
	// �������� ������� I2C
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// ��� (������) �������� ��������
			class PointerType,					// ��� (������) �������� ������� ���������
			PointerType OwnAddressRegisterAddr,		// ����� �������� ������ ������
			PointerType SlaveAddressRegisterAddr	// ����� �������� ������ ��������
		>
	class UsciI2CAddress
	{
	protected:
		// ������ ��������� ����������
		static volatile RegisterDataType * const _ownAddressRegisterAddr;
		static volatile RegisterDataType * const _slaveAddressRegisterAddr;
	};
	
	// ��������������� ����������� ����������
	template <class RegisterDataType, class PointerType, PointerType OwnAddressRegisterAddr, PointerType SlaveAddressRegisterAddr>
	volatile RegisterDataType * const UsciI2CAddress<RegisterDataType, PointerType, OwnAddressRegisterAddr, SlaveAddressRegisterAddr>
		::_ownAddressRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(OwnAddressRegisterAddr);

	template <class RegisterDataType, class PointerType, PointerType OwnAddressRegisterAddr, PointerType SlaveAddressRegisterAddr>
	volatile RegisterDataType * const UsciI2CAddress<RegisterDataType, PointerType, OwnAddressRegisterAddr, SlaveAddressRegisterAddr>
		::_slaveAddressRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(SlaveAddressRegisterAddr);
	
	////////////////////////////////////////////////////
	// ����� ��� ���� �������� ����� ���������� ����� Uart
	////////////////////////////////////////////////////
  class UartConstants
  {
  public:

    // ������ ������
    enum DataSize
    {
      DataSize8Bit = 0,
      DataSize7Bit = 1
    };
    // ��������
    enum Parity
    {
      ParityNone	= 0,
      ParityOdd	= 2,
      ParityEven	= 3
    };
    // ����� �������� �����
    enum StopBits
    {
      StopBitsOne	= 0,
      StopBitsTwo	= 1
    };
    // ��������� ������������
    enum ClockSource
    {
      ClockSourceUsciClockInput		= 0,
      ClockSourceAuxClock				= 1,
      ClockSourceSubsystemMasterClock	= 2
    };

  };

	template <class Control,
			  class BaudRateControl,
			  class ModulationControl,
			  class Status,
			  class Buffers,
			  class InterruptControl,
			  class GpioPeripheralControl,
              class Constants
		>
	class UsciHardwareUart :
		public Control,
		public BaudRateControl,
		public ModulationControl,
		public Buffers,
		public Status,
		public InterruptControl,
		public GpioPeripheralControl,
        public Constants
	{
	public:
		typedef typename GpioPeripheralControl::Port1 RXPort;
		typedef typename GpioPeripheralControl::Port2 TXPort;
		static const unsigned char RXPin = GpioPeripheralControl::Pin1;
		static const unsigned char TXPin = GpioPeripheralControl::Pin2;

		typedef void (OnRxCallbackType)(typename Buffers::BufferType data);
		typedef bool (OnTxCallbackType)(typename Buffers::BufferType &data);
		typedef unsigned int (OnReadCallbackType)(typename Buffers::BufferType *data, unsigned int len);
		typedef unsigned int (OnWriteCallbackType)(typename Buffers::BufferType *data, unsigned int len);
		typedef void (OnOverrunCallbackType)();
        typedef bool (OnOutputBufferEmptyCallbackType)();

		static OnRxCallbackType *OnRxCallback;
		static OnTxCallbackType *OnTxCallback;
		static OnReadCallbackType *OnReadCallback;
		static OnWriteCallbackType *OnWriteCallback;
		static OnOverrunCallbackType *OnOverrunCallback;
        static OnOutputBufferEmptyCallbackType *OnOutputBufferEmptyCallback;
        
	public:
		// ������� ���������
		static const unsigned char UsciModulationTable[8][8];
        
        typedef typename Constants::DataSize DataSize;
        typedef typename Constants::Parity Parity;
        typedef typename Constants::StopBits StopBits;
        typedef typename Constants::ClockSource ClockSource;
            
	protected:
		// ��������� ������������ ������ ������� �������� ����
		static inline float GetError(long clock, long baudrate, unsigned int br, unsigned char brs)
		{
			// ������ � ������� ���������
			unsigned char brsTablePos = brs % 8;
			float errorNeg = 0;	// ������������ ������ �� ������ -
			float errorPos = 0; // ������������ ������ �� ������ +
			float tRealPrev = 0; // ��������� ����� ���� ���������� �����
			for (unsigned char i = 0; i <= 7; i++)
			{
				// ��������� ����� ����
				float tIdeal = (float)((1.0 / baudrate) * (i + 1));
				// �������� ����� ����
				float tReal = (float)((1.0 / clock) * (br + UsciModulationTable[brsTablePos][i])) + tRealPrev;
				tRealPrev = tReal;
				// ������ ����
				float error = (float)((tReal - tIdeal) * baudrate * 100.0);
				// ���������� ������������ �������� ������ (������������� � ������������� ��������)
				if (error < 0)
				{
					if (error < errorNeg)
						errorNeg = error;
				}
				else
				{
					if (error > errorPos)
						errorPos = error;
				}

			}

			// ���������� ������������ �� ������ ������
			return errorPos > -errorNeg ? errorPos : -errorNeg;
		}

		// ��������� ���������
		static inline float GetModulation(long clock, long baudrate, unsigned int br, unsigned char &brsOptimal)
		{
			// ��������� �������� �������� ��������� (�����������)
			float n = (float)clock / baudrate;
			float brsFloat = (n - br) * 8;

			// ����������
			unsigned char brs = (unsigned char)brsFloat;
			if((brsFloat - brs) >= 0.5)
				brs++;
			// ��������� ������ ��� ��������� �������� � ��� �������� ������ ��������� � ������ ��������� �� �������
			float errorCenter = GetError(clock, baudrate, br, brs);
			float errorLeft = GetError(clock, baudrate, br, brs - 1);
			float errorRight = GetError(clock, baudrate, br, brs + 1);

			//����� �� ��������, ��� �������� ������ ���������� ������
			float minError = errorLeft < errorRight ? errorLeft : errorRight;
			minError = minError < errorCenter ? minError : errorCenter;

			brsOptimal = brs;
			
			if (minError == errorLeft)
				brsOptimal = brs - 1;

			if (minError == errorRight)
				brsOptimal = brs + 1;

			return minError;
		}

    // ��������� ���������� �� �����
    static inline void RxHandlerBody()
    {
        // ������ ������������
        bool dataError = false;
        // ��������� �� ������
        if (Status::_statusRegisterAddr->RxError)
        {
          if (Status::_statusRegisterAddr->ParityError)
          {
            dataError = true;
          }
          if (Status::_statusRegisterAddr->OverrunError)
          {
            if (OnOverrunCallback)
              OnOverrunCallback();
          }
          if (Status::_statusRegisterAddr->FramingError)
          {
            dataError = true;
          }
        }
        // ������ ���������� �����
        typename Buffers::BufferType data = *Buffers::_rxBufferRegisterAddr;

        // ������������ ������ ����������
        if (!dataError)
        {
          // ���������� ������ ������������ ������
          if (OnRxCallback)
            OnRxCallback(data);
        }
    }

    // ��������� ���������� �� ��������
    static inline void TxHandlerBody()
    {
        if (OnTxCallback)
        {
          typename Buffers::BufferType data;
          // ���� � ������ ���� ������
          if (OnTxCallback(data))
          {
            if (OnOutputBufferEmptyCallback())
              InterruptControl::SetTxInterruptEnable(false);
            // �������� �� � ���������� �����
            *Buffers::_txBufferRegisterAddr = data;
          }
          else
          {
            // ������ ���, ������� ������ ����������
            InterruptControl::SetTxInterruptEnable(false);
          }
        }
        else
        {
          // �� ����� ��� ����������, ������� ������ ����������
          InterruptControl::SetTxInterruptEnable(false);
        }
    }

	public:
		// ������ � ������ �� UART ������ USCI ������
		static inline bool IsUartMode()
		{
			// ���������� ����� ��������, ������ � ������ UART
			return 0 == Control::_control0RegisterAddr->SynchronousModeEnable;
		}

    static void Enable(bool enable)
    {
      // ���������� �����
      Control::Reset(!enable);
      // ��������� ���������� �� �����
      InterruptControl::SetRxInterruptEnable(enable);
    }

    static inline void BaseInitialisation(DataSize dataSize, Parity parity, StopBits stopBits, ClockSource source)
    {
      // ����������� GPIO �����
      GpioEnable();

      // �������� ���������
      Control::SetGeneralSettings(0, dataSize, parity, stopBits);
      // �������� ������������
      Control::SetClockSource(source);
    }

    // �������������
    static void Init(DataSize dataSize, Parity parity, StopBits stopBits, long baudRate, ClockSource source, long frequency)
    {
      Enable(false);

      BaseInitialisation(dataSize, parity, stopBits, source);

			// ����������� ������� �� ������ ���������� �������� � ������� ��������� ������������
			float n = (float)frequency / baudRate;
			unsigned int br = (unsigned int)n;

      // �.�. ��-�� ���������� ������� ����� ����� �� ������� � ������ �������,
			// ��� ��������� ������ ��� �������� ���� ������ � ���� ������

			// �������� ��������� �������������� ������ ���������
			unsigned char brsCenter = 0;
			unsigned char brsLeft = 0;
			unsigned char brsRight = 0;

			float errorCenter = GetModulation(frequency, baudRate, br, brsCenter);
			float errorLeft = GetModulation(frequency, baudRate, br - 1, brsLeft);
			float errorRight = GetModulation(frequency, baudRate, br + 1, brsRight);

			float minError = errorLeft < errorRight ? errorLeft : errorRight;
			minError = minError < errorCenter ? minError : errorCenter;

			// �������� �������� � ���������� �������
			unsigned char brs = brsCenter;
			
			if (minError == errorLeft)
			{
				br = br - 1;
				brs = brsLeft;
			}

			if (minError == errorRight)
			{
				br = br + 1;
				brs = brsRight;
			}
			
			// ���������� �������
			BaudRateControl::Set((unsigned char)br, (unsigned char)(br >> 8));
			
			// ���������� ��������� ���������
			ModulationControl::Set(brs);
			
      Enable(true);
		}

		// ��������� ���������� �� �����
		static inline void RxHandler()
		{
			// ���������� ����
			if (InterruptControl::GetRxInterruptFlag())
			{
        		RxHandlerBody();
      		}
		}
		// ��������� ���������� �� ��������
		static inline void TxHandler()
		{
			// ���������� ����
			if (InterruptControl::GetTxInterruptFlag())
			{
        		TxHandlerBody();
			}
		}
		// ������ ������ �� �����
		static int Read(unsigned char* data, int length)
		{
			if (0 == OnReadCallback)
				return 0;

			// ��� ������������ ��������� ���������� �� �������
			InterruptControl::SetRxInterruptEnable(false);
			unsigned int readed = OnReadCallback(data, length);
			// ��������� ����������
			InterruptControl::SetRxInterruptEnable(true);
			// ���������� ������� ���������
			return readed;
		}
		// ������ ������ � ����
		static void Write(unsigned char* data, int length)
		{
			if (0 == OnWriteCallback)
				return;

			unsigned int remain = length;	// ������� ����� ������ ���������� � ����� �� ��������
			unsigned int copied = 0;		// ������� ����������� �� �������� �� ��������� ��������
			
			// ��� ���� ��� ������ �� ����� �������� � ����� �� ��������
			while (remain > 0)
			{
				// ��� ������������ ��������� ���������� �� �������
				InterruptControl::SetTxInterruptEnable(false);
				// �������� ������� �������� ������ ��������� � ����� �� ��������
				copied = OnWriteCallback(data, remain);
				// ������� ��������� �� �������� ������
				data += copied;
				// ��������� ������� �������� �����������
				remain -= copied;
				// �������� ���������� �� ��������
				InterruptControl::SetTxInterruptEnable(true);
			}
		}
		// �������� Gpio ����
		static inline void GpioEnable()
		{
			GpioPeripheralControl::EnableUartFunction();
		}

		// ��������� Gpio ����
		static inline void GpioDisable()
		{
			GpioPeripheralControl::DisableUartFunction();
		}
	};
	
	// ��������������� ����������� ����������
	template <class Control, class BaudRateControl, class ModulationControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl, class Constants>
	typename UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnOutputBufferEmptyCallbackType *
	UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnOutputBufferEmptyCallback = 0;
    
	template <class Control, class BaudRateControl, class ModulationControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl, class Constants>
	typename UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnRxCallbackType *
	UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnRxCallback = 0;

	template <class Control, class BaudRateControl, class ModulationControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl, class Constants>
	typename UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnTxCallbackType *
	UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnTxCallback = 0;

	template <class Control, class BaudRateControl, class ModulationControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl, class Constants>
	typename UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnReadCallbackType *
	UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnReadCallback = 0;

	template <class Control, class BaudRateControl, class ModulationControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl, class Constants>
	typename UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnWriteCallbackType *
	UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnWriteCallback = 0;
	
	template <class Control, class BaudRateControl, class ModulationControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl, class Constants>
	typename UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnOverrunCallbackType *
	UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::OnOverrunCallback = 0;

	template <class Control, class BaudRateControl, class ModulationControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl, class Constants>
	const unsigned char	UsciHardwareUart<Control, BaudRateControl, ModulationControl, Status, Buffers, InterruptControl, GpioPeripheralControl, Constants>::UsciModulationTable[8][8] = 
		{
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 1, 0, 0, 0, 0, 0, 0},
			{0, 1, 0, 0, 0, 1, 0, 0},
			{0, 1, 0, 1, 0, 1, 0, 0},
			{0, 1, 0, 1, 0, 1, 0, 1},
			{0, 1, 1, 1, 0, 1, 0, 1},
			{0, 1, 1, 1, 0, 1, 1, 1},
			{0, 1, 1, 1, 1, 1, 1, 1}
		};

	////////////////////////////////////////////////////
	// ����� ��� ���� �������� ����� ���������� ����� I2C
	////////////////////////////////////////////////////
	template <unsigned char HardwareId,
			  class Control,
			  class BaudRateControl,
			  class Status,
			  class Buffers,
			  class Address,
			  class InterruptControl,
			  class GpioPeripheralControl
		>
	class UsciHardwareI2C :
		public Control,
		public BaudRateControl,
		public Buffers,
		public Address,
		public Status,
		public InterruptControl,
		public GpioPeripheralControl
	{
	public:
		// ��������� ������������
		enum ClockSource
		{
			ClockSourceUsciClockInput		= 0,
			ClockSourceAuxClock				= 1,
			ClockSourceSubsystemMasterClock	= 2
		};
		// ������ ���������
		enum AddressSize
		{
			AddressSize7Bit = 0,
			AddressSize10Bit = 1
		};
		// ����� ���������
		enum State
		{
			StateIdle = 0,		// ������ �� ������
			StateBusBusy,		// ���� ������
			StateWorkAsMaster,	// ��� ������ � ������ �������
			StateNack,			// ���������������
			StateArbitrationLost,	// ������ ���������
			StateSlaveWaitStart,	// �������� ������
			StateSlaveTransfer,		// �������� ������
		};

		typedef typename GpioPeripheralControl::Port1 SDAPort;
		typedef typename GpioPeripheralControl::Port2 SCLPort;
		static const unsigned char SDAPin = GpioPeripheralControl::Pin1;
		static const unsigned char SCLPin = GpioPeripheralControl::Pin2;

		typedef void (OnHardwareResetCallbackType)();
		typedef void (AsynchCallbackType)(bool result);
		
		typedef void (OnSlaveStartCallbackType)(bool read);
		typedef void (OnSlaveStopCallbackType)();
		typedef void (OnSlaveRxCallbackType)(unsigned char data);
		typedef unsigned char (OnSlaveTxCallbackType)();
	protected:
		static typename Buffers::BufferType *_buffer;	// ���� ���������� ������
		static volatile unsigned int _bufferPos;		// ������� � ������
		static unsigned int _requestedBytesCount;		// ������� ��������� ��������� ��� �������
		static volatile State _state;

		static OnHardwareResetCallbackType *OnHardwareResetCallback;
		static AsynchCallbackType *AsynchCallback;
		
		static OnSlaveStartCallbackType *_onSlaveStartCallback;
		static OnSlaveStopCallbackType *_onSlaveStopCallback;
		static OnSlaveRxCallbackType *_onSlaveRxCallback;
		static OnSlaveTxCallbackType *_onSlaveTxCallback;
	protected:
		
		// ����� ���� ���� ������
		static inline bool WaitBusy()
		{
			if (Status::_statusRegisterAddr->BusBusy)
			{
				// ����� ���������� �����
				if (OnHardwareResetCallback)
					OnHardwareResetCallback();
				// ���� ����� ����������� ������ �� ��� ������
				if (Status::_statusRegisterAddr->BusBusy)
				{
					// ���������� USCI
					Reset();
				}
				// ���� � ����� ����� ���� ������
				if (Status::_statusRegisterAddr->BusBusy)
				{
					// �� ������ ���������� ����������
					_state = StateBusBusy;
					return false;
				}
			}

			return true;
		}

		// ����� ���� ����������� �����
		static inline bool WaitStart()
		{
			while (Control::_control1RegisterAddr->TransmitStart)
			{
			}
			return true;
		}

		// �������� ��� ���������
		static inline void EnableInterrupts()
		{
			// ��������� ���������� �� �����
			InterruptControl::SetRxInterruptEnable(true);
			// ��������� ���������� �� ��������
			InterruptControl::SetTxInterruptEnable(true);
			// ����� ����� NACK
			InterruptControl::SetNackInterruptEnable(true);
			// ���� �������� � ������ ��������
			if (!Control::_control0RegisterAddr->Master)
			{
				// ����� ����� START
				InterruptControl::SetStartInterruptEnable(true);
				// ����� ����� STOP
				InterruptControl::SetStopInterruptEnable(true);
			}
		}

		// �������� ���������
		static inline void ResetState()
		{
			_buffer = 0;
			_bufferPos = 0;
			_requestedBytesCount = 0;
			AsynchCallback = 0;
			_state = StateIdle;
		}

		// �������� ������
		static inline void Reset()
		{
			Control::_control1RegisterAddr->Reset = 1;
			Control::_control1RegisterAddr->TransmitStart = 0;
			Control::_control1RegisterAddr->TransmitStop = 0;
			Control::_control1RegisterAddr->Reset = 0;
			EnableInterrupts();
		}

		// ������� ������� �������� ������
		static inline void ExecuteAsynchCallback(bool result)
		{
			// ���� �������� ���� �����������
			if (0 != AsynchCallback)
			{
				AsynchCallbackType *execute = AsynchCallback;
				// ���������� ���������
				ResetState();
				// �������� ������� ��������� ������ � �������������� ������
				execute(result);
			}
			// ���� ��������� ����������� ����������� ��������, �������� ��������
			if (!result)
			{
				SendI2CErrorEvent();
			}
		}

		// ������ ���������� � ���� �������
		static inline bool StartMasterTransaction(bool write, unsigned char* data, int length, AsynchCallbackType *asynchCallback)
		{
			if (0 == data)
				return false;
		  	if (length < 1)
				return false;

			// ����� ��������
			Control::_control0RegisterAddr->Master = 1;
			// ����� ������
			Control::_control1RegisterAddr->TransmitterEnable = write;
			
			_buffer = data;
			_bufferPos = 0;
			_requestedBytesCount = length;
			
			AsynchCallback = asynchCallback;

			EnableInterrupts();

			// ��������� ��������� ����
			if (!WaitBusy())
			{
				ResetState();
				return false;
			}
			_state = StateWorkAsMaster;
			// ���������� �����
			Control::_control1RegisterAddr->TransmitStart = 1;

			// ���� ��������� ���� ����� ������ �����
			if (length == 1 && !write)
			{
				// �� ��� ���� �� ������� �������� ������� �����
				if (!WaitStart())
				{
					ResetState();
					return false;
				}
				// � ����� ���������� ����
				Control::_control1RegisterAddr->TransmitStop = 1;
			}

			// ���� ������� �����������, �� ����� �� �����
			if (0 != AsynchCallback)
				return true;

			while(_bufferPos < _requestedBytesCount)
			{
				if (StateWorkAsMaster != _state)
				{
					ResetState();
				  	return false;
				}
			}

			ResetState();
			return true;
		}
		
		// ��������� ������� "������ I2C"
		static void SendI2CErrorEvent()
		{
			Event e(EventI2CError, HardwareId);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
		}
	public:

		static inline bool Busy()
		{
			return Status::_statusRegisterAddr->BusBusy;
		}

		// ������ � ������ �� I2C ������ USCI ������
		static inline bool IsI2CMode()
		{
			// ���������� ����� ��������, ������ � ������ UART
			return 1 == Control::_control0RegisterAddr->SynchronousModeEnable && 3 == Control::_control0RegisterAddr->Mode;
		}
		
		// �������������
		static void Init(ClockSource clockSource, unsigned int prescaller)
		{
			_state = StateIdle;
			
			// �������� ������ 4-� ������ ���������� (��. DataSheet)
			if (prescaller < 4)
				prescaller = 4;
			
			// ���������� ����� ����� �����������������
			Control::_control1RegisterAddr->Reset = 1;

			// ����������� GPIO �����
			GpioEnable();
			
			// �������� ���������
			Control::_control0RegisterAddr->SynchronousModeEnable = 1;	// ���������� �����
			Control::_control0RegisterAddr->Mode = 3;					// ����� I2C
			Control::_control0RegisterAddr->Master = 1;
			Control::_control0RegisterAddr->MultiMasterEnable = 0;		// ����� � ����� ��������
			// �������� ������������
			Control::_control1RegisterAddr->ClockSource = clockSource;
			
			// ���������� �������
			*BaudRateControl::_baudRateControl0RegisterAddr = (unsigned char)prescaller;
			*BaudRateControl::_baudRateControl1RegisterAddr = (unsigned char)(prescaller >> 8);
			
			// ������� ����� ����� ��������� ���������
			Control::_control1RegisterAddr->Reset = 0;
			
			EnableInterrupts();
		}
		
		// ������������� ��� Slave
		static void InitSlave(unsigned char address)
		{
			// ���������� ����� ����� �����������������
			Control::_control1RegisterAddr->Reset = 1;
			// ��� Slave �����
			SetOwnAddress(address);
			
			// ����������� GPIO �����
			GpioEnable();
			
			// �������� ���������
			Control::_control0RegisterAddr->SynchronousModeEnable = 1;	// ���������� �����
			Control::_control0RegisterAddr->Mode = 3;					// ����� I2C
			Control::_control0RegisterAddr->Master = 0;					// Slave
			Control::_control0RegisterAddr->MultiMasterEnable = 0;		// ����� � ����� ��������
			
			// ������� ����� ����� ��������� ���������
			Control::_control1RegisterAddr->Reset = 0;
			
			_state = StateSlaveWaitStart;
			
			EnableInterrupts();
		}
		
		static void Pause()
		{
			// ��������� ���������� �� �����
			InterruptControl::SetRxInterruptEnable(false);
			// ��������� ���������� �� ��������
			InterruptControl::SetTxInterruptEnable(false);
		}
		
		static void Resume()
		{
			// ��������� ���������� �� �����
			InterruptControl::SetRxInterruptEnable(true);
			// ��������� ���������� �� ��������
			InterruptControl::SetTxInterruptEnable(true);
		}
		
		// ������ ������� ����������� ������ I2C
		static inline void SetBusHardwareResetCallback(OnHardwareResetCallbackType *callback)
		{
			OnHardwareResetCallback = callback;
		}
		
		// ���������� Start
		static inline void SetOnSlaveStartCallback(OnSlaveStartCallbackType *onSlaveStartCallback)
		{
			_onSlaveStartCallback = onSlaveStartCallback;
		}
		
		// ���������� Stop
		static inline void SetOnSlaveStopCallback(OnSlaveStopCallbackType *onSlaveStopCallback)
		{
			_onSlaveStopCallback = onSlaveStopCallback;
		}
		
		// ���������� Rx
		static inline void SetOnSlaveRxCallback(OnSlaveRxCallbackType *onSlaveRxCallback)
		{
			_onSlaveRxCallback = onSlaveRxCallback;
		}
		
		// ���������� Tx
		static inline void SetOnSlaveTxCallback(OnSlaveTxCallbackType *onSlaveTxCallback)
		{
			_onSlaveTxCallback = onSlaveTxCallback;
		}
		
		// ��������� "������" ������
		static inline void SetOwnAddress(unsigned short address, AddressSize addressSize = AddressSize7Bit)
		{
			Control::_control0RegisterAddr->OwnAddressSize = addressSize;
			*Address::_ownAddressRegisterAddr = address;
		}

		// ��������� ������ ��������
		static inline void SetSlaveAddress(unsigned short address, AddressSize addressSize = AddressSize7Bit)
		{
			Control::_control0RegisterAddr->SlaveAddressSize = addressSize;
			*Address::_slaveAddressRegisterAddr = address;
		}
		
		// ��������� ����������
		static inline void InterruptHandler()
		{
			// ���� �������� � ������ ��������
			if (Control::_control0RegisterAddr->Master)
			{
				// ������ ����� ������������
				if (InterruptControl::GetNackInterruptFlag())
				{
					// ���������� ����
				  	if (_requestedBytesCount != 1)
						Control::_control1RegisterAddr->TransmitStop = 1;
					InterruptControl::ClearNackInterruptFlag();
					// ������ ��� ������ ��� ������
					InterruptControl::ClearRxInterruptFlag();
					InterruptControl::ClearTxInterruptFlag();
					_state = StateNack;
					// ���������, ���� �������� ���� �����������
					ExecuteAsynchCallback(false);
				}

				// ������ ��������� ����
				if (InterruptControl::GetRxInterruptFlag())
				{
					// ���� ����� �� �������/����������
					if (_bufferPos == _requestedBytesCount - 2)
					{
						// �� ���� ����������� ��������� ����, ���� ���������� ����������
						// ���������� ����
						Control::_control1RegisterAddr->TransmitStop = 1;
					}

					// �������� ���� �����������, ���� �� �� ��������� ����������
					typename Buffers::BufferType data = *Buffers::_rxBufferRegisterAddr;

					// ���� ����� �����
					if (0 != _buffer)
					{
						_buffer[_bufferPos] = data;
						// ������� ��������� � ������
						_bufferPos++;
						// ���� �� �������/����������
						if (_bufferPos >= _requestedBytesCount)
						{
							// ������� ��������� �� �����, ����� �������� �� ��������� ������ � �������
							_buffer = 0;
							// ���������, ���� �������� ���� �����������
							ExecuteAsynchCallback(true);
						}
					}
				}
				
				// ���� ��������� ��������� ����
				if (InterruptControl::GetTxInterruptFlag())
				{
					// ���� ��� �� �� ����������
					if (_bufferPos < _requestedBytesCount)
					{
						// ������������ ����
						typename Buffers::BufferType data = 0xFF;
						// ��� ����� ����������
						if (0 != _buffer)
							data = _buffer[_bufferPos];
						// ���������� ����
						*Buffers::_txBufferRegisterAddr = data;
						// ������� ��������� � ������
						_bufferPos++;
					}
					else
					{
						// �� ����������
						// ���������� ����
						Control::_control1RegisterAddr->TransmitStop = 1;
						// ������ ���������� �� ����
						InterruptControl::ClearTxInterruptFlag();
						// ���������, ���� �������� ���� �����������
						ExecuteAsynchCallback(true);
					}
				}
			}
			else
			{
				// �����
				if(InterruptControl::GetStartInterruptFlag())
				{
					InterruptControl::ClearStartInterruptFlag();
					
					if (_onSlaveStartCallback)
					{
						_onSlaveStartCallback(Control::_control1RegisterAddr->TransmitterEnable);
					}
					
					_state = StateSlaveTransfer;
				}
				
				// ���� (���� ���������, �� ������ ����, �.�. �� ����� ������ ���������� ����� ���� ������������ �����)
				if(InterruptControl::GetStopInterruptFlag() && !InterruptControl::GetRxInterruptFlag())
				{
					InterruptControl::ClearStopInterruptFlag();
					
					if (_onSlaveStopCallback)
					{
						_onSlaveStopCallback();
					}
					
					_state = StateSlaveWaitStart;
				}

			  	// ������ ���������
				if(InterruptControl::GetArbitrationLostInterruptFlag())
				{
					InterruptControl::ClearArbitrationLostInterruptFlag();
					_state = StateArbitrationLost;
					// ������� USCI
					Reset();
					// ���������, ���� �������� ���� �����������
					ExecuteAsynchCallback(false);
				}
				
				// ������ ������� ��������� ����
				if (InterruptControl::GetRxInterruptFlag())
				{
					// �������� ���� �����������, ���� �� �� ��������� ����������
					typename Buffers::BufferType data = *Buffers::_rxBufferRegisterAddr;
					
					// ��� �������� ������ ����� ��������� ����� ����, ��� ������,
					// ������� ��� ���������� ������ ��� �������� ������ ��� ���������� ���� ��������� �����
					if (StateSlaveWaitStart == _state)
					{
						_state = StateSlaveTransfer;
						if (_onSlaveStartCallback)
						{
							_onSlaveStartCallback(Control::_control1RegisterAddr->TransmitterEnable);
						}
					}
					
					if (_onSlaveRxCallback)
					{
						_onSlaveRxCallback(data);
					}
				}
				
				// ������ �������� ��������� ����
				// ��� ���� �� ������� ��� �����, �.�. �� ������ ����,
				// � �� ������ �������� ����������, ������� ����� ���� �������� ��� ���� ������� � ��������� �������, �� ������� ����� ���� ��� �� ����������� ��������
				// ���� ������ ����������, �� _onSlaveTxCallback ���������� �� ���� ��� ������ ��� �����
				if (InterruptControl::GetTxInterruptFlag() && Status::_statusRegisterAddr->SCLLow)
				{
					// ������������ ����
					typename Buffers::BufferType data = 0xFF;
					
					if (_onSlaveTxCallback)
					{
						data = _onSlaveTxCallback();
					}
					
					// ���������� ����
					*Buffers::_txBufferRegisterAddr = data;
				}
			}
		}

		// ������ ������ � ��������
		static bool Read(unsigned char* data, int length, AsynchCallbackType *asynchCallback = 0)
		{
			bool result = StartMasterTransaction(false, data, length, asynchCallback);
			if (!result)
			{
				SendI2CErrorEvent();
			}
			return result;
		}

		// �������� ������ ��������
		static bool Write(unsigned char* data, int length, AsynchCallbackType *asynchCallback = 0)
		{
			bool result = StartMasterTransaction(true, data, length, asynchCallback);
			if (!result)
			{
				SendI2CErrorEvent();
			}
			return result;
		}

		// �������� ����������
		static inline void Break()
		{
			ResetState();
			Reset();
		}

		// �������� Gpio ����
		static inline void GpioEnable()
		{
			GpioPeripheralControl::EnableI2CFunction();
		}

		// ��������� Gpio ����
		static inline void GpioDisable()
		{
			GpioPeripheralControl::DisabelI2CFunction();
		}
	};

	// ��������������� ����������� ����������
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class Address, class InterruptControl, class GpioPeripheralControl>
	typename UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::OnHardwareResetCallbackType *
	UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::OnHardwareResetCallback = 0;
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class Address, class InterruptControl, class GpioPeripheralControl>
	typename UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::AsynchCallbackType *
	UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::AsynchCallback = 0;
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class Address, class InterruptControl, class GpioPeripheralControl>
	typename UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::OnSlaveStartCallbackType *
	UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::_onSlaveStartCallback = 0;
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class Address, class InterruptControl, class GpioPeripheralControl>
	typename UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::OnSlaveStopCallbackType *
	UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::_onSlaveStopCallback = 0;
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class Address, class InterruptControl, class GpioPeripheralControl>
	typename UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::OnSlaveTxCallbackType *
	UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::_onSlaveTxCallback = 0;
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class Address, class InterruptControl, class GpioPeripheralControl>
	typename UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::OnSlaveRxCallbackType *
	UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::_onSlaveRxCallback = 0;

	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class Address, class InterruptControl, class GpioPeripheralControl>
	typename Buffers::BufferType *UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::_buffer = 0;

	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class Address, class InterruptControl, class GpioPeripheralControl>
	volatile unsigned int UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::_bufferPos = 0;

	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class Address, class InterruptControl, class GpioPeripheralControl>
	unsigned int UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::_requestedBytesCount = 0;

	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class Address, class InterruptControl, class GpioPeripheralControl>
	volatile typename UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::State
	UsciHardwareI2C<HardwareId, Control, BaudRateControl, Status, Buffers, Address, InterruptControl, GpioPeripheralControl>::_state = StateIdle;
	
	////////////////////////////////////////////////////
	// ����� ��� ���� �������� ����� ���������� ����� SPI
	////////////////////////////////////////////////////
	template <unsigned char HardwareId,
			  class Control,
			  class BaudRateControl,
			  class Status,
			  class Buffers,
			  class InterruptControl,
			  class GpioPeripheralControl
		>
	class UsciHardwareSpi :
		public Control,
		public BaudRateControl,
		public Buffers,
		public Status,
		public InterruptControl,
		public GpioPeripheralControl
	{
	protected:
		static volatile typename Buffers::BufferType *_src;	// ������ ����� ������
		static volatile typename Buffers::BufferType *_dst;	// ���� ���������� ������
		static volatile unsigned int _txPos;		// ������� � ������ �� ��������
		static volatile unsigned int _rxPos;		// ������� � ������ �� �����
		static unsigned int _bytesCount;			// ������� ��������� ��������� / �������
	public:
		// ��������� ������������
		enum ClockSource
		{
			ClockSourceUsciClockInput		= 0,
			ClockSourceAuxClock				= 1,
			ClockSourceSubsystemMasterClock	= 2
		};
		
		typedef void (OnCompleteCallbackType)();
	protected:
		static OnCompleteCallbackType *_onCompleteCallback;
	protected:
		static void ResetBuffers()
		{
			_src = 0;
			_dst = 0;
			_txPos = 0;
			_rxPos = 0;
			_bytesCount = 0;
			_onCompleteCallback = 0;
		}
	public:
		// ������ � ������ �� SPI ������ USCI ������
		static inline bool IsSpiMode()
		{
			// ���������� ����� ��������, ������ � ������ UART
			return 1 == Control::_control0RegisterAddr->SynchronousModeEnable && 0 == Control::_control0RegisterAddr->Mode;
		}
		
		// �������������
		static void Init(ClockSource clockSource, unsigned int prescaller, bool msbFirst, bool clockPolarity, bool clockPhase)
		{
			ResetBuffers();
			
			// �������� ������ 4-� ������ ���������� (��. DataSheet)
			if (prescaller < 1)
				prescaller = 1;
			
			// ���������� ����� ����� �����������������
			Control::_control1RegisterAddr->Reset = 1;

			// ����������� GPIO �����
			GpioEnable();
			
			// �������� ���������
			Control::_control0RegisterAddr->SynchronousModeEnable = 1;	// ���������� �����
			Control::_control0RegisterAddr->Mode = 0;					// ����� SPI, 3 �������
			Control::_control0RegisterAddr->CharacterLength = 0;		// 8 ��� ������
			Control::_control0RegisterAddr->MsbFirstEnable = msbFirst;	// ������� ������
			Control::_control0RegisterAddr->Master = 1;
			Control::_control0RegisterAddr->ClockPolarity = clockPolarity;
			Control::_control0RegisterAddr->ClockPhase = clockPhase;
			// �������� ������������
			Control::_control1RegisterAddr->ClockSource = clockSource;
			
			// ���������� �������
			*BaudRateControl::_baudRateControl0RegisterAddr = (unsigned char)prescaller;
			*BaudRateControl::_baudRateControl1RegisterAddr = (unsigned char)(prescaller >> 8);
			
			// ������� ����� ����� ��������� ���������
			Control::_control1RegisterAddr->Reset = 0;
			
			// ���������� ���� ������
			InterruptControl::SetTxInterruptEnable(false);
			// ��������� ����� ������������ � ���������, ������� ����� ��������
			InterruptControl::SetRxInterruptEnable(true);
			//EnableInterrupts();
		}
		
		// �������� Gpio ����
		static inline void GpioEnable()
		{
			GpioPeripheralControl::EnableSpiFunction();
		}

		// ��������� Gpio ����
		static inline void GpioDisable()
		{
			GpioPeripheralControl::DisabelSpiFunction();
		}
		
		// �������� ������ � ������������� ������� � ����� dst
		static void Write(unsigned char* src, unsigned char* dst, int length, OnCompleteCallbackType *onCompleteCallback)
		{
			ResetBuffers();
			
			if (0 == src || 0 == dst || 0 == length)
			{
				return;
			}
			
			_src = src;
			_dst = dst;
			_bytesCount = length;
			_onCompleteCallback = onCompleteCallback;
			
			// �������� ����������, ������� �����
			InterruptControl::SetTxInterruptEnable(true);
			
			// ���� �� �������� ����� �� ������� ��������� ������, ������ ���������� ��������
			if (0 == _onCompleteCallback)
			{
				int j = 0;
				while(_rxPos < _bytesCount)
				{
					j++;
				}
			}
		}
		
		// ��������� ���������� �� �����
		static inline void RxHandler()
		{
			// ���������� ����
			if (InterruptControl::GetRxInterruptFlag())
			{
        		RxHandlerBody();
      		}
		}
		// ��������� ���������� �� ��������
		static inline void TxHandler()
		{
			// ���������� ����
			if (InterruptControl::GetTxInterruptFlag())
			{
        		TxHandlerBody();
			}
		}
		
		// ��������� ���������� �� �����
		static inline void RxHandlerBody()
		{
			// ��������� �� ������
			/*
			if (Status::_statusRegisterAddr->OverrunError)
			{
				if (OnOverrunCallback)
					OnOverrunCallback();
			}
			
			if (Status::_statusRegisterAddr->FramingError)
			{
				
			}
			*/
			
			// ������ ���������� �����
			typename Buffers::BufferType data = *Buffers::_rxBufferRegisterAddr;
			
			if (0 != _dst)
			{
				_dst[_rxPos] = data;
				_rxPos++;
				
				// ������� ������� ����
				if (_rxPos >= _bytesCount)
				{
					// ������� �� ����
					if (_onCompleteCallback)
					{
						_onCompleteCallback();
					}
					// ������� ���������
					ResetBuffers();
				}
				else
				{
					// ���� ��� ������, � ����� ��������� ���� ���-�� ���������
					InterruptControl::SetTxInterruptEnable(true);
				}
			}
		}

		// ��������� ���������� �� ��������
		static inline void TxHandlerBody()
		{
			// ���� �������� �������� ����� ������ ���������
			InterruptControl::SetTxInterruptEnable(false);
			if (0 != _src)
			{
				if (_txPos < _bytesCount)
				{
					// ���������� ����
					*Buffers::_txBufferRegisterAddr = _src[_txPos];
					_txPos++;
					
				}
				else
				{
					// ������ ����������
					InterruptControl::SetTxInterruptEnable(false);
				}
			}
			else
			{
				// ������ ����������
				InterruptControl::SetTxInterruptEnable(false);
			}
		}
	};
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl>
	volatile typename Buffers::BufferType *UsciHardwareSpi<HardwareId, Control, BaudRateControl, Status, Buffers, InterruptControl, GpioPeripheralControl>::_src = 0;
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl>
	volatile typename Buffers::BufferType *UsciHardwareSpi<HardwareId, Control, BaudRateControl, Status, Buffers, InterruptControl, GpioPeripheralControl>::_dst = 0;
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl>
	volatile unsigned int UsciHardwareSpi<HardwareId, Control, BaudRateControl, Status, Buffers, InterruptControl, GpioPeripheralControl>::_txPos = 0;
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl>
	volatile unsigned int UsciHardwareSpi<HardwareId, Control, BaudRateControl, Status, Buffers, InterruptControl, GpioPeripheralControl>::_rxPos = 0;
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl>
	unsigned int UsciHardwareSpi<HardwareId, Control, BaudRateControl, Status, Buffers, InterruptControl, GpioPeripheralControl>::_bytesCount = 0;
	
	template <unsigned char HardwareId, class Control, class BaudRateControl, class Status, class Buffers, class InterruptControl, class GpioPeripheralControl>
		typename UsciHardwareSpi<HardwareId, Control, BaudRateControl, Status, Buffers, InterruptControl, GpioPeripheralControl>::OnCompleteCallbackType *
		UsciHardwareSpi<HardwareId, Control, BaudRateControl, Status, Buffers, InterruptControl, GpioPeripheralControl>::_onCompleteCallback = 0;
}
}

#endif