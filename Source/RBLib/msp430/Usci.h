///////////////////////////////////////////////////////////////////////////////
//	Классы управления модулем USCI
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
	// Общие регистры управления
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// Тип (размер) значения регистра
			class PointerType,					// Тип (размер) значения адресов регистров
			PointerType Control0RegisterAddr,	// Адрес регистра 0 управления
			PointerType Control1RegisterAddr,	// Адрес регистра 1 управления
			class Control0RegisterStructure,	// Структура регистра 0 управления
			class Control1RegisterStructure		// Структура регистра 1 управления
		>
	class UsciControl
	{
	protected:
		// Адреса регистров управления
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

	// Инстанцирование статических переменных
	template <class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure>
	volatile Control0RegisterStructure * const UsciControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
		::_control0RegisterAddr = reinterpret_cast<volatile  Control0RegisterStructure*>(Control0RegisterAddr);
	
	template <class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure>
	volatile Control1RegisterStructure * const UsciControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
		::_control1RegisterAddr = reinterpret_cast<volatile  Control1RegisterStructure*>(Control1RegisterAddr);

	////////////////////////////////////////////////////
	// Регистры управления битрейтом
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// Тип (размер) значения регистра
			class PointerType,					// Тип (размер) значения адресов регистров
			PointerType BaudRateControl0RegisterAddr,	// Адрес регистра 0 управления
			PointerType BaudRateControl1RegisterAddr	// Адрес регистра 1 управления
		>
	class UsciBaudRateControl
	{
	protected:
		// Адреса регистров управления
		static volatile RegisterDataType * const _baudRateControl0RegisterAddr;
		static volatile RegisterDataType * const _baudRateControl1RegisterAddr;

  protected:

    static inline void Set(RegisterDataType low, RegisterDataType high)
    {
      *_baudRateControl0RegisterAddr = low;
      *_baudRateControl1RegisterAddr = high;
    }

	};
	
	// Инстанцирование статических переменных
	template <class RegisterDataType, class PointerType, PointerType BaudRateControl0RegisterAddr, PointerType BaudRateControl1RegisterAddr>
	volatile RegisterDataType * const UsciBaudRateControl<RegisterDataType, PointerType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>
		::_baudRateControl0RegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(BaudRateControl0RegisterAddr);

	template <class RegisterDataType, class PointerType, PointerType BaudRateControl0RegisterAddr, PointerType BaudRateControl1RegisterAddr>
	volatile RegisterDataType * const UsciBaudRateControl<RegisterDataType, PointerType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>
		::_baudRateControl1RegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(BaudRateControl1RegisterAddr);

	////////////////////////////////////////////////////
	// Регистр управления модуляцией
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// Тип (размер) значения регистра
			class PointerType,					// Тип (размер) значения адресов регистров
			PointerType ModulationControlRegisterAddr,	// Адрес регистра
			class ModulationControlRegisterStructure	// Структура регистра
		>
	class UsciModulationControl
	{
	protected:
		// Адреса регистров управления
		static volatile ModulationControlRegisterStructure * const _modulationControlRegisterAddr;

    static inline void Set(RegisterDataType modulation)
    {
      _modulationControlRegisterAddr->OversamplingEnable = 0;
      _modulationControlRegisterAddr->FirstModulationStage = 0;
      _modulationControlRegisterAddr->SecondModulationStage = modulation;
    }

	};

	// Инстанцирование статических переменных
	template <class RegisterDataType, class PointerType, PointerType ModulationControlRegisterAddr, class ModulationControlRegisterStructure>
	volatile ModulationControlRegisterStructure * const UsciModulationControl<RegisterDataType, PointerType, ModulationControlRegisterAddr, ModulationControlRegisterStructure>
		::_modulationControlRegisterAddr = reinterpret_cast<volatile  ModulationControlRegisterStructure*>(ModulationControlRegisterAddr);

	////////////////////////////////////////////////////
	// Регистр статуса
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// Тип (размер) значения регистра
			class PointerType,					// Тип (размер) значения адресов регистров
			PointerType StatusRegisterAddr,	// Адрес регистра
			class StatusRegisterStructure	// Структура регистра
		>
	class UsciStatus
	{
	protected:
		// Адреса регистров управления
		static volatile StatusRegisterStructure * const _statusRegisterAddr;
	};

	// Инстанцирование статических переменных
	template <class RegisterDataType, class PointerType, PointerType StatusRegisterAddr, class StatusRegisterStructure>
	volatile StatusRegisterStructure * const UsciStatus<RegisterDataType, PointerType, StatusRegisterAddr, StatusRegisterStructure>
		::_statusRegisterAddr = reinterpret_cast<volatile  StatusRegisterStructure*>(StatusRegisterAddr);

	////////////////////////////////////////////////////
	// Регистры аппаратных буферов
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// Тип (размер) значения регистра
			class PointerType,					// Тип (размер) значения адресов регистров
			PointerType RxBufferRegisterAddr,	// Адрес регистра бефера приема
			PointerType TxBufferRegisterAddr	// Адрес регистра буфера отправки
		>
	class UsciBuffers
	{
	public:
		typedef RegisterDataType BufferType;	// тип размер буфера
	protected:
		// Адреса регистров управления
		static volatile RegisterDataType * const _rxBufferRegisterAddr;
		static volatile RegisterDataType * const _txBufferRegisterAddr;
	};
	
	// Инстанцирование статических переменных
	template <class RegisterDataType, class PointerType, PointerType RxBufferRegisterAddr, PointerType TxBufferRegisterAddr>
	volatile RegisterDataType * const UsciBuffers<RegisterDataType, PointerType, RxBufferRegisterAddr, TxBufferRegisterAddr>
		::_rxBufferRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(RxBufferRegisterAddr);

	template <class RegisterDataType, class PointerType, PointerType RxBufferRegisterAddr, PointerType TxBufferRegisterAddr>
	volatile RegisterDataType * const UsciBuffers<RegisterDataType, PointerType, RxBufferRegisterAddr, TxBufferRegisterAddr>
		::_txBufferRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(TxBufferRegisterAddr);

	////////////////////////////////////////////////////
	// Регистры адресов I2C
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// Тип (размер) значения регистра
			class PointerType,					// Тип (размер) значения адресов регистров
			PointerType OwnAddressRegisterAddr,		// Адрес регистра своего адреса
			PointerType SlaveAddressRegisterAddr	// Адрес регистра адреса ведомого
		>
	class UsciI2CAddress
	{
	protected:
		// Адреса регистров управления
		static volatile RegisterDataType * const _ownAddressRegisterAddr;
		static volatile RegisterDataType * const _slaveAddressRegisterAddr;
	};
	
	// Инстанцирование статических переменных
	template <class RegisterDataType, class PointerType, PointerType OwnAddressRegisterAddr, PointerType SlaveAddressRegisterAddr>
	volatile RegisterDataType * const UsciI2CAddress<RegisterDataType, PointerType, OwnAddressRegisterAddr, SlaveAddressRegisterAddr>
		::_ownAddressRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(OwnAddressRegisterAddr);

	template <class RegisterDataType, class PointerType, PointerType OwnAddressRegisterAddr, PointerType SlaveAddressRegisterAddr>
	volatile RegisterDataType * const UsciI2CAddress<RegisterDataType, PointerType, OwnAddressRegisterAddr, SlaveAddressRegisterAddr>
		::_slaveAddressRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(SlaveAddressRegisterAddr);
	
	////////////////////////////////////////////////////
	// Общий для всех семейств класс аппаратной части Uart
	////////////////////////////////////////////////////
  class UartConstants
  {
  public:

    // Размер данных
    enum DataSize
    {
      DataSize8Bit = 0,
      DataSize7Bit = 1
    };
    // Четность
    enum Parity
    {
      ParityNone	= 0,
      ParityOdd	= 2,
      ParityEven	= 3
    };
    // Число стоповых битов
    enum StopBits
    {
      StopBitsOne	= 0,
      StopBitsTwo	= 1
    };
    // Источники тактирования
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
		// Таблица модуляции
		static const unsigned char UsciModulationTable[8][8];
        
        typedef typename Constants::DataSize DataSize;
        typedef typename Constants::Parity Parity;
        typedef typename Constants::StopBits StopBits;
        typedef typename Constants::ClockSource ClockSource;
            
	protected:
		// Посчитать максимальную ошибку времени передачи бита
		static inline float GetError(long clock, long baudrate, unsigned int br, unsigned char brs)
		{
			// Строка в таблице модуляции
			unsigned char brsTablePos = brs % 8;
			float errorNeg = 0;	// максимальная ошибка со знаком -
			float errorPos = 0; // максимальная ошибка со знаком +
			float tRealPrev = 0; // суммарное время всех предыдущих битов
			for (unsigned char i = 0; i <= 7; i++)
			{
				// идеальное время бита
				float tIdeal = (float)((1.0 / baudrate) * (i + 1));
				// реальное время бита
				float tReal = (float)((1.0 / clock) * (br + UsciModulationTable[brsTablePos][i])) + tRealPrev;
				tRealPrev = tReal;
				// ошибка бита
				float error = (float)((tReal - tIdeal) * baudrate * 100.0);
				// запоминаем максимальное значение ошибки (положительные и отрицательные отдельно)
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

			// возвращаем максимальную по модулю ошибку
			return errorPos > -errorNeg ? errorPos : -errorNeg;
		}

		// Вычислить модуляцию
		static inline float GetModulation(long clock, long baudrate, unsigned int br, unsigned char &brsOptimal)
		{
			// Вычисляем основное значение модуляции (центральное)
			float n = (float)clock / baudrate;
			float brsFloat = (n - br) * 8;

			// округление
			unsigned char brs = (unsigned char)brsFloat;
			if((brsFloat - brs) >= 0.5)
				brs++;
			// Вычисляем ошибку для основного значение и для значений меньше основного и больше основного на единицу
			float errorCenter = GetError(clock, baudrate, br, brs);
			float errorLeft = GetError(clock, baudrate, br, brs - 1);
			float errorRight = GetError(clock, baudrate, br, brs + 1);

			//вернём то значение, для которого ошибка получилась меньше
			float minError = errorLeft < errorRight ? errorLeft : errorRight;
			minError = minError < errorCenter ? minError : errorCenter;

			brsOptimal = brs;
			
			if (minError == errorLeft)
				brsOptimal = brs - 1;

			if (minError == errorRight)
				brsOptimal = brs + 1;

			return minError;
		}

    // Обработка прерывания на прием
    static inline void RxHandlerBody()
    {
        // данные закосячились
        bool dataError = false;
        // Проверяем на ошибки
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
        // читаем аппаратный буфер
        typename Buffers::BufferType data = *Buffers::_rxBufferRegisterAddr;

        // Закосяченные данные игнорируем
        if (!dataError)
        {
          // Правильные данные обрабатываем дальше
          if (OnRxCallback)
            OnRxCallback(data);
        }
    }

    // Обработка прерывания на отправку
    static inline void TxHandlerBody()
    {
        if (OnTxCallback)
        {
          typename Buffers::BufferType data;
          // Если в буфере есть данные
          if (OnTxCallback(data))
          {
            if (OnOutputBufferEmptyCallback())
              InterruptControl::SetTxInterruptEnable(false);
            // помещаем их в аппаратный буфер
            *Buffers::_txBufferRegisterAddr = data;
          }
          else
          {
            // данных нет, незачем дёргать прерывание
            InterruptControl::SetTxInterruptEnable(false);
          }
        }
        else
        {
          // не знаем что отправлять, незачем дёргать прерывание
          InterruptControl::SetTxInterruptEnable(false);
        }
    }

	public:
		// Понять в режиме ли UART данный USCI модуль
		static inline bool IsUartMode()
		{
			// Синхронный режим выключен, значит в режиме UART
			return 0 == Control::_control0RegisterAddr->SynchronousModeEnable;
		}

    static void Enable(bool enable)
    {
      // Выставляем сброс
      Control::Reset(!enable);
      // Разрешаем прерывания на прием
      InterruptControl::SetRxInterruptEnable(enable);
    }

    static inline void BaseInitialisation(DataSize dataSize, Parity parity, StopBits stopBits, ClockSource source)
    {
      // Настраиваем GPIO порты
      GpioEnable();

      // Основные настройки
      Control::SetGeneralSettings(0, dataSize, parity, stopBits);
      // Источник тактирования
      Control::SetClockSource(source);
    }

    // Инициализация
    static void Init(DataSize dataSize, Parity parity, StopBits stopBits, long baudRate, ClockSource source, long frequency)
    {
      Enable(false);

      BaseInitialisation(dataSize, parity, stopBits, source);

			// Расчитываем битрейт на основе требуемого битрейта и частоты источника тактирования
			float n = (float)frequency / baudRate;
			unsigned int br = (unsigned int)n;

      // т.к. из-за оттсечения дробной части могли не попасть в нужный битрейт,
			// тоо вычисляем ошибку для битрейта чуть больше и чуть меньше

			// значения модуляции соответсвующие разным битрейтам
			unsigned char brsCenter = 0;
			unsigned char brsLeft = 0;
			unsigned char brsRight = 0;

			float errorCenter = GetModulation(frequency, baudRate, br, brsCenter);
			float errorLeft = GetModulation(frequency, baudRate, br - 1, brsLeft);
			float errorRight = GetModulation(frequency, baudRate, br + 1, brsRight);

			float minError = errorLeft < errorRight ? errorLeft : errorRight;
			minError = minError < errorCenter ? minError : errorCenter;

			// выбираем значения с наименьшей ошибкой
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
			
			// Выставляем битрейт
			BaudRateControl::Set((unsigned char)br, (unsigned char)(br >> 8));
			
			// Выставляем параметры модуляции
			ModulationControl::Set(brs);
			
      Enable(true);
		}

		// Обработка прерывания на прием
		static inline void RxHandler()
		{
			// Прерывание есть
			if (InterruptControl::GetRxInterruptFlag())
			{
        		RxHandlerBody();
      		}
		}
		// Обработка прерывания на отправку
		static inline void TxHandler()
		{
			// Прерывание есть
			if (InterruptControl::GetTxInterruptFlag())
			{
        		TxHandlerBody();
			}
		}
		// Чтение данных из порта
		static int Read(unsigned char* data, int length)
		{
			if (0 == OnReadCallback)
				return 0;

			// для безопасности выключаем прерывания на отпраку
			InterruptControl::SetRxInterruptEnable(false);
			unsigned int readed = OnReadCallback(data, length);
			// разрешаем прерывания
			InterruptControl::SetRxInterruptEnable(true);
			// возвращаем сколько прочитано
			return readed;
		}
		// Запись данных в порт
		static void Write(unsigned char* data, int length)
		{
			if (0 == OnWriteCallback)
				return;

			unsigned int remain = length;	// сколько нужно данных записхнуть в буфер на отправку
			unsigned int copied = 0;		// сколько скопировано на отправку за последнюю итерацию
			
			// ждём пока все данные не будут помещены в буфер на отправку
			while (remain > 0)
			{
				// для безопасности выключаем прерывания на отпраку
				InterruptControl::SetTxInterruptEnable(false);
				// пытаемся сколько осталось данных запихнуть в буфер на отправку
				copied = OnWriteCallback(data, remain);
				// двигаем указатель на исходные данные
				data += copied;
				// вычисляем сколько осталось скопировать
				remain -= copied;
				// включаем прерывания на отправку
				InterruptControl::SetTxInterruptEnable(true);
			}
		}
		// Включить Gpio порт
		static inline void GpioEnable()
		{
			GpioPeripheralControl::EnableUartFunction();
		}

		// Отключить Gpio порт
		static inline void GpioDisable()
		{
			GpioPeripheralControl::DisableUartFunction();
		}
	};
	
	// Инстанцирование статических переменных
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
	// Общий для всех семейств класс аппаратной части I2C
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
		// Источники тактирования
		enum ClockSource
		{
			ClockSourceUsciClockInput		= 0,
			ClockSourceAuxClock				= 1,
			ClockSourceSubsystemMasterClock	= 2
		};
		// Режимы адресации
		enum AddressSize
		{
			AddressSize7Bit = 0,
			AddressSize10Bit = 1
		};
		// Набор состояний
		enum State
		{
			StateIdle = 0,		// ничего не делаем
			StateBusBusy,		// шина занята
			StateWorkAsMaster,	// идёт работа в режиме мастера
			StateNack,			// неподтверждение
			StateArbitrationLost,	// потеря арбитража
			StateSlaveWaitStart,	// ожидание старта
			StateSlaveTransfer,		// передача данных
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
		static typename Buffers::BufferType *_buffer;	// куда складывать данные
		static volatile unsigned int _bufferPos;		// позиция в буфере
		static unsigned int _requestedBytesCount;		// сколько требуется отправить или принять
		static volatile State _state;

		static OnHardwareResetCallbackType *OnHardwareResetCallback;
		static AsynchCallbackType *AsynchCallback;
		
		static OnSlaveStartCallbackType *_onSlaveStartCallback;
		static OnSlaveStopCallbackType *_onSlaveStopCallback;
		static OnSlaveRxCallbackType *_onSlaveRxCallback;
		static OnSlaveTxCallbackType *_onSlaveTxCallback;
	protected:
		
		// Ждать пока шина занята
		static inline bool WaitBusy()
		{
			if (Status::_statusRegisterAddr->BusBusy)
			{
				// нужен аппаратный сброс
				if (OnHardwareResetCallback)
					OnHardwareResetCallback();
				// если после аппаратного сброса всё ещё заняты
				if (Status::_statusRegisterAddr->BusBusy)
				{
					// сбрасываем USCI
					Reset();
				}
				// если и после этого шина занята
				if (Status::_statusRegisterAddr->BusBusy)
				{
					// то нельзя продолжать транзакцию
					_state = StateBusBusy;
					return false;
				}
			}

			return true;
		}

		// Ждать пока выполняется старт
		static inline bool WaitStart()
		{
			while (Control::_control1RegisterAddr->TransmitStart)
			{
			}
			return true;
		}

		// Включить все прервания
		static inline void EnableInterrupts()
		{
			// Разрешаем прерывания на прием
			InterruptControl::SetRxInterruptEnable(true);
			// Разрешаем прерывания на отправку
			InterruptControl::SetTxInterruptEnable(true);
			// будем ждать NACK
			InterruptControl::SetNackInterruptEnable(true);
			// если работаем в режиме ведомого
			if (!Control::_control0RegisterAddr->Master)
			{
				// будем ждать START
				InterruptControl::SetStartInterruptEnable(true);
				// будем ждать STOP
				InterruptControl::SetStopInterruptEnable(true);
			}
		}

		// Сбросить состояние
		static inline void ResetState()
		{
			_buffer = 0;
			_bufferPos = 0;
			_requestedBytesCount = 0;
			AsynchCallback = 0;
			_state = StateIdle;
		}

		// Сбросить модуль
		static inline void Reset()
		{
			Control::_control1RegisterAddr->Reset = 1;
			Control::_control1RegisterAddr->TransmitStart = 0;
			Control::_control1RegisterAddr->TransmitStop = 0;
			Control::_control1RegisterAddr->Reset = 0;
			EnableInterrupts();
		}

		// Вызвать функцию обатного вызова
		static inline void ExecuteAsynchCallback(bool result)
		{
			// Если операция была асинхронная
			if (0 != AsynchCallback)
			{
				AsynchCallbackType *execute = AsynchCallback;
				// сбрасываем состояние
				ResetState();
				// вызываем функцию обратного вызова с соответсвующим флагом
				execute(result);
			}
			// если неуспешно завершилась асинхронная операция, сообщаем событием
			if (!result)
			{
				SendI2CErrorEvent();
			}
		}

		// Начать транзакцию в роли мастера
		static inline bool StartMasterTransaction(bool write, unsigned char* data, int length, AsynchCallbackType *asynchCallback)
		{
			if (0 == data)
				return false;
		  	if (length < 1)
				return false;

			// будем мастером
			Control::_control0RegisterAddr->Master = 1;
			// будем читать
			Control::_control1RegisterAddr->TransmitterEnable = write;
			
			_buffer = data;
			_bufferPos = 0;
			_requestedBytesCount = length;
			
			AsynchCallback = asynchCallback;

			EnableInterrupts();

			// проверить занятость шины
			if (!WaitBusy())
			{
				ResetState();
				return false;
			}
			_state = StateWorkAsMaster;
			// генерируем СТАРТ
			Control::_control1RegisterAddr->TransmitStart = 1;

			// если требуется приём всего одного байта
			if (length == 1 && !write)
			{
				// то ждём пока не начнётся передача превого байта
				if (!WaitStart())
				{
					ResetState();
					return false;
				}
				// и сразу генерируем СТОП
				Control::_control1RegisterAddr->TransmitStop = 1;
			}

			// если опреция асинхронная, то ждать не будем
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
		
		// Отправить событие "ошибка I2C"
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

		// Понять в режиме ли I2C данный USCI модуль
		static inline bool IsI2CMode()
		{
			// Синхронный режим выключен, значит в режиме UART
			return 1 == Control::_control0RegisterAddr->SynchronousModeEnable && 3 == Control::_control0RegisterAddr->Mode;
		}
		
		// Инициализация
		static void Init(ClockSource clockSource, unsigned int prescaller)
		{
			_state = StateIdle;
			
			// Делитель меньше 4-х нельзя выставлять (см. DataSheet)
			if (prescaller < 4)
				prescaller = 4;
			
			// Выставляем сброс перед конфигурированием
			Control::_control1RegisterAddr->Reset = 1;

			// Настраиваем GPIO порты
			GpioEnable();
			
			// Основные настройки
			Control::_control0RegisterAddr->SynchronousModeEnable = 1;	// синхронный режим
			Control::_control0RegisterAddr->Mode = 3;					// режим I2C
			Control::_control0RegisterAddr->Master = 1;
			Control::_control0RegisterAddr->MultiMasterEnable = 0;		// режим с одним мастером
			// Источник тактирования
			Control::_control1RegisterAddr->ClockSource = clockSource;
			
			// Выставляем битрейт
			*BaudRateControl::_baudRateControl0RegisterAddr = (unsigned char)prescaller;
			*BaudRateControl::_baudRateControl1RegisterAddr = (unsigned char)(prescaller >> 8);
			
			// Снимаем сброс после окончания настройки
			Control::_control1RegisterAddr->Reset = 0;
			
			EnableInterrupts();
		}
		
		// Инициализация как Slave
		static void InitSlave(unsigned char address)
		{
			// Выставляем сброс перед конфигурированием
			Control::_control1RegisterAddr->Reset = 1;
			// Наш Slave адрес
			SetOwnAddress(address);
			
			// Настраиваем GPIO порты
			GpioEnable();
			
			// Основные настройки
			Control::_control0RegisterAddr->SynchronousModeEnable = 1;	// синхронный режим
			Control::_control0RegisterAddr->Mode = 3;					// режим I2C
			Control::_control0RegisterAddr->Master = 0;					// Slave
			Control::_control0RegisterAddr->MultiMasterEnable = 0;		// режим с одним мастером
			
			// Снимаем сброс после окончания настройки
			Control::_control1RegisterAddr->Reset = 0;
			
			_state = StateSlaveWaitStart;
			
			EnableInterrupts();
		}
		
		static void Pause()
		{
			// Разрешаем прерывания на прием
			InterruptControl::SetRxInterruptEnable(false);
			// Разрешаем прерывания на отправку
			InterruptControl::SetTxInterruptEnable(false);
		}
		
		static void Resume()
		{
			// Разрешаем прерывания на прием
			InterruptControl::SetRxInterruptEnable(true);
			// Разрешаем прерывания на отправку
			InterruptControl::SetTxInterruptEnable(true);
		}
		
		// Задать функцию аппаратного сброса I2C
		static inline void SetBusHardwareResetCallback(OnHardwareResetCallbackType *callback)
		{
			OnHardwareResetCallback = callback;
		}
		
		// Обработчик Start
		static inline void SetOnSlaveStartCallback(OnSlaveStartCallbackType *onSlaveStartCallback)
		{
			_onSlaveStartCallback = onSlaveStartCallback;
		}
		
		// Обработчик Stop
		static inline void SetOnSlaveStopCallback(OnSlaveStopCallbackType *onSlaveStopCallback)
		{
			_onSlaveStopCallback = onSlaveStopCallback;
		}
		
		// Обработчик Rx
		static inline void SetOnSlaveRxCallback(OnSlaveRxCallbackType *onSlaveRxCallback)
		{
			_onSlaveRxCallback = onSlaveRxCallback;
		}
		
		// Обработчик Tx
		static inline void SetOnSlaveTxCallback(OnSlaveTxCallbackType *onSlaveTxCallback)
		{
			_onSlaveTxCallback = onSlaveTxCallback;
		}
		
		// Установка "своего" адреса
		static inline void SetOwnAddress(unsigned short address, AddressSize addressSize = AddressSize7Bit)
		{
			Control::_control0RegisterAddr->OwnAddressSize = addressSize;
			*Address::_ownAddressRegisterAddr = address;
		}

		// Установка адреса ведомого
		static inline void SetSlaveAddress(unsigned short address, AddressSize addressSize = AddressSize7Bit)
		{
			Control::_control0RegisterAddr->SlaveAddressSize = addressSize;
			*Address::_slaveAddressRegisterAddr = address;
		}
		
		// Обработка прерывания
		static inline void InterruptHandler()
		{
			// если работаем в режиме ведущего
			if (Control::_control0RegisterAddr->Master)
			{
				// Пришел отказ подтвержения
				if (InterruptControl::GetNackInterruptFlag())
				{
					// генерируем СТОП
				  	if (_requestedBytesCount != 1)
						Control::_control1RegisterAddr->TransmitStop = 1;
					InterruptControl::ClearNackInterruptFlag();
					// Читать или писать нет смысла
					InterruptControl::ClearRxInterruptFlag();
					InterruptControl::ClearTxInterruptFlag();
					_state = StateNack;
					// проверяем, если опреация была асинхронная
					ExecuteAsynchCallback(false);
				}

				// принят следующий байт
				if (InterruptControl::GetRxInterruptFlag())
				{
					// если почти всё принято/отправлено
					if (_bufferPos == _requestedBytesCount - 2)
					{
						// то пока принимается последний байт, надо остановить транзакцию
						// генерируем СТОП
						Control::_control1RegisterAddr->TransmitStop = 1;
					}

					// вычитать надо обязательно, хотя бы во временную переменную
					typename Buffers::BufferType data = *Buffers::_rxBufferRegisterAddr;

					// если буфер задан
					if (0 != _buffer)
					{
						_buffer[_bufferPos] = data;
						// двигаем указатель в буфере
						_bufferPos++;
						// если всё принято/отправлено
						if (_bufferPos >= _requestedBytesCount)
						{
							// забудем указатель на буфер, чтобы случайно не попортить память в будущем
							_buffer = 0;
							// проверяем, если операция была асинхронная
							ExecuteAsynchCallback(true);
						}
					}
				}
				
				// пора отпралять следующий байт
				if (InterruptControl::GetTxInterruptFlag())
				{
					// если ещё не всё отправлено
					if (_bufferPos < _requestedBytesCount)
					{
						// отправляемый байт
						typename Buffers::BufferType data = 0xFF;
						// что будем отправлять
						if (0 != _buffer)
							data = _buffer[_bufferPos];
						// отправляем байт
						*Buffers::_txBufferRegisterAddr = data;
						// двигаем указатель в буфере
						_bufferPos++;
					}
					else
					{
						// всё отправлено
						// генерируем СТОП
						Control::_control1RegisterAddr->TransmitStop = 1;
						// больше прерываний не надо
						InterruptControl::ClearTxInterruptFlag();
						// проверяем, если операция была асинхронная
						ExecuteAsynchCallback(true);
					}
				}
			}
			else
			{
				// старт
				if(InterruptControl::GetStartInterruptFlag())
				{
					InterruptControl::ClearStartInterruptFlag();
					
					if (_onSlaveStartCallback)
					{
						_onSlaveStartCallback(Control::_control1RegisterAddr->TransmitterEnable);
					}
					
					_state = StateSlaveTransfer;
				}
				
				// стоп (пока принимаем, не выдаем стоп, т.к. во время приема последнего байта стоп выставляется сразу)
				if(InterruptControl::GetStopInterruptFlag() && !InterruptControl::GetRxInterruptFlag())
				{
					InterruptControl::ClearStopInterruptFlag();
					
					if (_onSlaveStopCallback)
					{
						_onSlaveStopCallback();
					}
					
					_state = StateSlaveWaitStart;
				}

			  	// потеря арбитража
				if(InterruptControl::GetArbitrationLostInterruptFlag())
				{
					InterruptControl::ClearArbitrationLostInterruptFlag();
					_state = StateArbitrationLost;
					// Сбросим USCI
					Reset();
					// проверяем, если операция была асинхронная
					ExecuteAsynchCallback(false);
				}
				
				// мастер передал следующий байт
				if (InterruptControl::GetRxInterruptFlag())
				{
					// вычитать надо обязательно, хотя бы во временную переменную
					typename Buffers::BufferType data = *Buffers::_rxBufferRegisterAddr;
					
					// при передаче одного байта пролетает сразу стоп, без старта,
					// поэтому для одинаковой логики при передачи одного или нескольких байт эмулируем старт
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
				
				// мастер запросил следующий байт
				// при этом он реально уже нужен, т.е. мы держим шину,
				// а не просто возникло прерывание, которое всего лишь означает что байт помещен в сдвиговый регистр, но реально может быть ещё не востребован мастером
				// если просто отправлять, то _onSlaveTxCallback вызывается на один раз больше чем нужно
				if (InterruptControl::GetTxInterruptFlag() && Status::_statusRegisterAddr->SCLLow)
				{
					// отправляемый байт
					typename Buffers::BufferType data = 0xFF;
					
					if (_onSlaveTxCallback)
					{
						data = _onSlaveTxCallback();
					}
					
					// отправляем байт
					*Buffers::_txBufferRegisterAddr = data;
				}
			}
		}

		// Запрос данных с ведомого
		static bool Read(unsigned char* data, int length, AsynchCallbackType *asynchCallback = 0)
		{
			bool result = StartMasterTransaction(false, data, length, asynchCallback);
			if (!result)
			{
				SendI2CErrorEvent();
			}
			return result;
		}

		// Отправка данных ведомому
		static bool Write(unsigned char* data, int length, AsynchCallbackType *asynchCallback = 0)
		{
			bool result = StartMasterTransaction(true, data, length, asynchCallback);
			if (!result)
			{
				SendI2CErrorEvent();
			}
			return result;
		}

		// Прервать транзакцию
		static inline void Break()
		{
			ResetState();
			Reset();
		}

		// Включить Gpio порт
		static inline void GpioEnable()
		{
			GpioPeripheralControl::EnableI2CFunction();
		}

		// Отключить Gpio порт
		static inline void GpioDisable()
		{
			GpioPeripheralControl::DisabelI2CFunction();
		}
	};

	// Инстанцирование статических переменных
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
	// Общий для всех семейств класс аппаратной части SPI
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
		static volatile typename Buffers::BufferType *_src;	// откуда брать данные
		static volatile typename Buffers::BufferType *_dst;	// куда складывать данные
		static volatile unsigned int _txPos;		// позиция в буфере на отправку
		static volatile unsigned int _rxPos;		// позиция в буфере на прием
		static unsigned int _bytesCount;			// сколько требуется отправить / принять
	public:
		// Источники тактирования
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
		// Понять в режиме ли SPI данный USCI модуль
		static inline bool IsSpiMode()
		{
			// Синхронный режим выключен, значит в режиме UART
			return 1 == Control::_control0RegisterAddr->SynchronousModeEnable && 0 == Control::_control0RegisterAddr->Mode;
		}
		
		// Инициализация
		static void Init(ClockSource clockSource, unsigned int prescaller, bool msbFirst, bool clockPolarity, bool clockPhase)
		{
			ResetBuffers();
			
			// Делитель меньше 4-х нельзя выставлять (см. DataSheet)
			if (prescaller < 1)
				prescaller = 1;
			
			// Выставляем сброс перед конфигурированием
			Control::_control1RegisterAddr->Reset = 1;

			// Настраиваем GPIO порты
			GpioEnable();
			
			// Основные настройки
			Control::_control0RegisterAddr->SynchronousModeEnable = 1;	// синхронный режим
			Control::_control0RegisterAddr->Mode = 0;					// режим SPI, 3 провода
			Control::_control0RegisterAddr->CharacterLength = 0;		// 8 бит данных
			Control::_control0RegisterAddr->MsbFirstEnable = msbFirst;	// старший первый
			Control::_control0RegisterAddr->Master = 1;
			Control::_control0RegisterAddr->ClockPolarity = clockPolarity;
			Control::_control0RegisterAddr->ClockPhase = clockPhase;
			// Источник тактирования
			Control::_control1RegisterAddr->ClockSource = clockSource;
			
			// Выставляем битрейт
			*BaudRateControl::_baudRateControl0RegisterAddr = (unsigned char)prescaller;
			*BaudRateControl::_baudRateControl1RegisterAddr = (unsigned char)(prescaller >> 8);
			
			// Снимаем сброс после окончания настройки
			Control::_control1RegisterAddr->Reset = 0;
			
			// Передавать пока нечего
			InterruptControl::SetTxInterruptEnable(false);
			// Принимать будем одновременно с передачей, поэтому можно включить
			InterruptControl::SetRxInterruptEnable(true);
			//EnableInterrupts();
		}
		
		// Включить Gpio порт
		static inline void GpioEnable()
		{
			GpioPeripheralControl::EnableSpiFunction();
		}

		// Отключить Gpio порт
		static inline void GpioDisable()
		{
			GpioPeripheralControl::DisabelSpiFunction();
		}
		
		// Отправка данных с одновременным чтением в буфер dst
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
			
			// включаем прерывания, процесс пошел
			InterruptControl::SetTxInterruptEnable(true);
			
			// если не передали адрес на функцию обратного вызова, значит синхронная передача
			if (0 == _onCompleteCallback)
			{
				int j = 0;
				while(_rxPos < _bytesCount)
				{
					j++;
				}
			}
		}
		
		// Обработка прерывания на прием
		static inline void RxHandler()
		{
			// Прерывание есть
			if (InterruptControl::GetRxInterruptFlag())
			{
        		RxHandlerBody();
      		}
		}
		// Обработка прерывания на отправку
		static inline void TxHandler()
		{
			// Прерывание есть
			if (InterruptControl::GetTxInterruptFlag())
			{
        		TxHandlerBody();
			}
		}
		
		// Обработка прерывания на прием
		static inline void RxHandlerBody()
		{
			// Проверяем на ошибки
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
			
			// читаем аппаратный буфер
			typename Buffers::BufferType data = *Buffers::_rxBufferRegisterAddr;
			
			if (0 != _dst)
			{
				_dst[_rxPos] = data;
				_rxPos++;
				
				// приняли сколько надо
				if (_rxPos >= _bytesCount)
				{
					// сообщим об этом
					if (_onCompleteCallback)
					{
						_onCompleteCallback();
					}
					// сбросим состояние
					ResetBuffers();
				}
				else
				{
					// есть что читать, а чтобы прочитать надо что-то отправить
					InterruptControl::SetTxInterruptEnable(true);
				}
			}
		}

		// Обработка прерывания на отправку
		static inline void TxHandlerBody()
		{
			// пока тормозим отправку чтобы успеть прочитать
			InterruptControl::SetTxInterruptEnable(false);
			if (0 != _src)
			{
				if (_txPos < _bytesCount)
				{
					// отправляем байт
					*Buffers::_txBufferRegisterAddr = _src[_txPos];
					_txPos++;
					
				}
				else
				{
					// Нечего передавать
					InterruptControl::SetTxInterruptEnable(false);
				}
			}
			else
			{
				// Нечего передавать
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