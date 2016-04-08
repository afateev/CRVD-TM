
#ifndef EMF32_I2C_H
#define EMF32_I2C_H

#include "i2c_registers.h"
#include "../IdObjectBase.h"

namespace Rblib
{
namespace Efm32
{
namespace I2C
{

    template< unsigned int IdObj
            , RegisterAddressType BaseAddr
            , RegisterAddressType ControlRegisterAddr
            , RegisterAddressType CommandRegisterAddr
            , RegisterAddressType StateRegisterAddr
            , RegisterAddressType StatusRegisterAddr
            , RegisterAddressType ClockDivisionRegisterAddr
            , RegisterAddressType SlaveAddressRegisterAddr
            , RegisterAddressType SlaveAddressMaskRegisterAddr
            , RegisterAddressType ReceiveBufferRegisterAddr
            , RegisterAddressType ReceiveBufferPeekRegisterAddr
            , RegisterAddressType TransmitBufferRegisterAddr
            , RegisterAddressType InterruptFlagRegisterAddr
            , RegisterAddressType InterruptFlagSetRegisterAddr
            , RegisterAddressType InterruptFlagClearRegisterAddr
            , RegisterAddressType InterruptEnableRegisterAddr
            , RegisterAddressType IORoutingRegisterAddr>
    class I2CLogic : public IdObjectBase<IdObj>, public IORoutingRegister<BaseAddr + IORoutingRegisterAddr>
    {

	public:
	  
	  enum ErrorCode;
		
    public:

        I2CLogic()
        {
        }

        static inline bool Busy()
        {
            return _busy;
        }

        static void Init(unsigned long busFreq, unsigned long sourceFreq)
        {
            ClockDivision::SetDivider(CalcDivider(busFreq, sourceFreq));
            Control::SetClockRatio(Control::ClockRatioStandard);
            Control::SetMaster();
            Control::AutoAckDisable();
            Control::ArbitrationEnable();
            Control::AutoStopOnNackDisable();
            Control::AutoStopWhenEmptyDisable();
            Control::SetBusIdleTimeout(Control::BusIdleTimeout160Cycles);
            Control::Enable();
            Command::Abort();
			RepeatedStartOff();
        }
		
        // некоторым устройстван нужно послать NACK дл€ завершени€ передачи
        // функции управл€ют этим режимом
        static inline void NackLastByteEnable()
        {
            _nackLastByte = true;
        }

        static inline bool NackLastByteEnabled()
        {
            return _nackLastByte;
        }

        static inline void NackLastByteDisable()
        {
            _nackLastByte = false;
        }

        // отправить асинхронно
        static bool WriteAsynch(unsigned short address, unsigned char* data, unsigned int length)
        {
            if (Busy())
            {
                return false;
            }

            if (length > _outputBufferLength) 
                return false;

            Lock();

            // init variables
            _reading = false;
            _outputLength = length;
            _address = address;
            _outputIndex = 0;
            memcpy(_outputBuffer, data, length);

            InitForTransfer();

            // начало передачи
			Command::Start();

            return true;
        }

        // считать асинхронно
        static bool ReadAsynch(unsigned short address, unsigned char* data, unsigned int length)
        {
            if (Busy())
                return false;

            Lock();

            // init variables
            _reading = true;
            _inputLength = length;
            _address = address;
            _inputIndex = 0;
            _inputBuffer = data;

            InitForTransfer();

            // начало передачи
            Command::Start();
			
			return true;
		}

        // записать синхронно
        static ErrorCode Write(unsigned short address, unsigned char* data, unsigned int length)
        {
            if (!WriteAsynch(address, data, length))
                return ErrorCodeBusy;

            _eventsEnabled = false;
            while (Busy());
            _eventsEnabled = true; 

            return _lastOperationStatus;
        }

        // считать синхронно
        static ErrorCode Read(unsigned short address, unsigned char* data, unsigned int length)
        {
            if (!ReadAsynch(address, data, length))
                return ErrorCodeBusy;

            _eventsEnabled = false;
            while (Busy());
            _eventsEnabled = true;

            return _lastOperationStatus;
        }

		// записать синхронно, не посыла€ stop, чтобы можно было использовать repeated start
		static ErrorCode WriteNoStop(unsigned short address, unsigned char* data, unsigned int length)
		{
			RepeatedStartOn(); // выключитс€ при завершении
			return Write(address, data, length);
		}

		// считать синхронно, не посыла€ stop, чтобы можно было использовать repeated start
		static ErrorCode ReadNoStop(unsigned short address, unsigned char* data, unsigned int length)
		{
			RepeatedStartOn(); // выключитс€ при завершении
			return Read(address, data, length);
		}

		// статус последней операции
        static inline ErrorCode LastOperationStatus()
        {
            return _lastOperationStatus;
        }

        // обработчики прерываний

        static void InterruptHandler()
        {
            if (InterruptFlag::Nack())
            {
                NackHandler();
            }
            else if (InterruptFlag::Start())
            {
                StartHandler();
            }
            else if (InterruptFlag::Ack())
            {
                AckHandler();
            }
            else if (InterruptFlag::DataReceived())
            {
                DataReceivedHandler();
            }
            else if (InterruptFlag::ArbitrationLost())
            {
                ArbitrationLostHandler();
            }
            InterruptFlagClear::ClearAll();
        }

    public:

        enum ErrorCode
        {
            ErrorCodeNone = 0,
            ErrorCodeBadAddress = 1,
            ErrorCodeBadData = 2,
            ErrorCodeBusy = 3,
            ErrorCodeArbitrationLost = 4
        };

    private:
		
        // обработчики прерываний

        static inline void StartHandler()
        {
            SendSlaveAddress();
        }

        static inline void AckHandler()
        {
            if (!_reading)
                SendData();
        }

        static inline void NackHandler()
        {
            DisableInterrupts();
            Command::Stop();
            if (_reading)
                ReadCompleteEvent(State::State() == State::StateDataAck ? ErrorCodeBadData : ErrorCodeBadAddress); 
            else
                WriteCompleteEvent(State::State() == State::StateDataAck ? ErrorCodeBadData : ErrorCodeBadAddress); 
            Unlock();
        }

        static inline void DataReceivedHandler()
        {
            if (_reading)
                ReadData();
        }
	  	
        static inline void ArbitrationLostHandler()
        {
            DisableInterrupts();
            Command::Abort();
            Command::ClearTransmitter();
            Command::ClearPendingCommangs();
            InterruptFlagClear::ClearAll();

            if (_reading)
                ReadCompleteEvent(ErrorCodeArbitrationLost); 
            else
                WriteCompleteEvent(ErrorCodeArbitrationLost); 

            Unlock();
        }

        // сервисные функции

        static inline void SendSlaveAddress()
        {
            TransmitBuffer::Write(((_address << 1) & 0xFF) | _reading); // 7 битный адрес и флаг чтение/запись
        }

        static void SendData()
        {
            if (_outputIndex < _outputLength)
            {
                TransmitBuffer::Write(_outputBuffer[_outputIndex++]);
                return;
            }

            DisableInterrupts();
			StopWise(); 
			WriteCompleteEvent(ErrorCodeNone);
            Unlock();
        }

        static void ReadData()
        {
            if (_inputIndex < _inputLength)
            {
                _inputBuffer[_inputIndex++] = ReceiveBuffer::Read();
            }

            if (_inputIndex < _inputLength)
            {
                Command::Ack();
                return;
            }

            DisableInterrupts();

            if (NackLastByteEnabled())
            {
                Command::Nack();
            }
            else
            {
                Command::Nack();
            }

            StopWise();
            ReadCompleteEvent(ErrorCodeNone);
            Unlock();
        }

        static inline void WriteCompleteEvent(ErrorCode code)
        {
            _lastOperationStatus = code;

            if (_eventsEnabled)
            {
                Event e(EventI2CAsynchWriteComplete, IdObj, code); 
                Singletone<EventDispetcher>::Instance().PushEvent(e);
            }
        }

        static inline void ReadCompleteEvent(ErrorCode code)
        {
            _lastOperationStatus = code;

            if (_eventsEnabled)
            {
                Event e(EventI2CAsynchReadComplete, IdObj, code);
                Singletone<EventDispetcher>::Instance().PushEvent(e);
            }
        }

		// выставл€ет stop, если только мы не хотим повторный start
		static inline void StopWise()
		{
			if (!_repeatedStart)
				Command::Stop();
		}

        static inline void Lock()
        {
            _busy = true;
        }

        static inline void Unlock()
        {
			RepeatedStartOff();
            _busy = false;
        }

        static unsigned long CalcDivider(unsigned long busFreq, unsigned long sourceFreq)
        {
            return (sourceFreq - 4 * busFreq) / (busFreq * (4 + 4)) - 1; // fSCL = fHFPERCLK/(((Nlow + Nhigh) x (DIV + 1)) + 4)
        } 

        static void EnableInterrupts()
        {
            InterruptEnable::SetEnableFlags(InterruptEnable::Start
                                            | InterruptEnable::RepeatedStart
                                            | InterruptEnable::Ack
                                            | InterruptEnable::Nack
                                            | InterruptEnable::ArbitrationLost
                                            | InterruptEnable::BusError
                                            | InterruptEnable::BusHold);
        }

        static inline void DisableInterrupts()
        {
            InterruptEnable::SetEnableFlags(0);
        }
		
        static void InitForTransfer()
        {
            Command::Abort();
            Command::ClearTransmitter();
            Command::ClearPendingCommangs();
            InterruptFlagClear::ClearAll();
            EnableInterrupts();
        }

		// ‘ункци€ активирует режим, при котором после удачного завершени€ передачи не будет отправл€тьс€ stop
		// и можно будет отправл€ть повторный старт (repeated start)
		static inline void RepeatedStartOn()
		{
			_repeatedStart = true;
		}

		// всегда будет отправл€тьс€ обычный stop
		static inline void RepeatedStartOff()
		{
			_repeatedStart = false;
		}

		static inline bool RepeatedStart()
		{
			return _repeatedStart;
		}

    private:

        static const unsigned int _outputBufferLength = 256;
        static unsigned char _outputBuffer[_outputBufferLength];

		static unsigned int _outputLength;
        static unsigned int _outputIndex;
		
        static unsigned char* _inputBuffer;
        static unsigned int _inputLength;
        static unsigned int _inputIndex;

        static bool _reading;
        static unsigned short _address; // адрес получател€

        static bool _busy;
        static bool _eventsEnabled;
		static bool _nackLastByte;
		static bool _repeatedStart;
		
        static ErrorCode _lastOperationStatus;
		// регистры

	public:
	  
        typedef ControlRegister<BaseAddr + ControlRegisterAddr> Control;
        typedef CommandRegister<BaseAddr + CommandRegisterAddr> Command;
        typedef StateRegister<BaseAddr + StateRegisterAddr> State;
        typedef StatusRegister<BaseAddr + StatusRegisterAddr> Status;
        typedef ClockDivisionRegister<BaseAddr + ClockDivisionRegisterAddr> ClockDivision;
        typedef SlaveAddressRegister<BaseAddr + SlaveAddressRegisterAddr> SlaveAddress;
        typedef SlaveAddressMaskRegister<BaseAddr + SlaveAddressMaskRegisterAddr> SlaveAddressMask;
        typedef ReceiveBufferRegister<BaseAddr + ReceiveBufferRegisterAddr> ReceiveBuffer;
        typedef ReceiveBufferPeekRegister<BaseAddr + ReceiveBufferPeekRegisterAddr> ReceiveBufferPeek;
        typedef TransmitBufferRegister<BaseAddr + TransmitBufferRegisterAddr> TransmitBuffer;
        typedef InterruptFlagRegister<BaseAddr + InterruptFlagRegisterAddr> InterruptFlag;
        typedef InterruptFlagSetRegister<BaseAddr + InterruptFlagSetRegisterAddr> InterruptFlagSet;
        typedef InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegisterAddr> InterruptFlagClear;
        typedef InterruptEnableRegister<BaseAddr + InterruptEnableRegisterAddr> InterruptEnable;
    };

//
// макросы, чтобы укоротить определение статических членов
// 

// заголовок шаблона
#define TEMPLATE_HEAD \
    template< unsigned int IdObj \
            , RegisterAddressType BaseAddr \
            , RegisterAddressType ControlRegisterAddr \
            , RegisterAddressType CommandRegisterAddr \
            , RegisterAddressType StateRegisterAddr \
            , RegisterAddressType StatusRegisterAddr \
            , RegisterAddressType ClockDivisionRegisterAddr \
            , RegisterAddressType SlaveAddressRegisterAddr \
            , RegisterAddressType SlaveAddressMaskRegisterAddr \
            , RegisterAddressType ReceiveBufferRegisterAddr \
            , RegisterAddressType ReceiveBufferPeekRegisterAddr \
            , RegisterAddressType TransmitBufferRegisterAddr \
            , RegisterAddressType InterruptFlagRegisterAddr \
            , RegisterAddressType InterruptFlagSetRegisterAddr \
            , RegisterAddressType InterruptFlagClearRegisterAddr \
            , RegisterAddressType InterruptEnableRegisterAddr \
            , RegisterAddressType IORoutingRegisterAddr>
			
// класс  
#define CLASS\
    I2CLogic<IdObj  \
            , BaseAddr \
            , ControlRegisterAddr \
            , CommandRegisterAddr \
            , StateRegisterAddr \
            , StatusRegisterAddr \
            , ClockDivisionRegisterAddr \
            , SlaveAddressRegisterAddr \
            , SlaveAddressMaskRegisterAddr \
            , ReceiveBufferRegisterAddr \
            , ReceiveBufferPeekRegisterAddr \
            , TransmitBufferRegisterAddr \
            , InterruptFlagRegisterAddr \
            , InterruptFlagSetRegisterAddr \
            , InterruptFlagClearRegisterAddr \
            , InterruptEnableRegisterAddr \
            , IORoutingRegisterAddr>

// статические члены

    TEMPLATE_HEAD unsigned short CLASS::_address = 0;
    TEMPLATE_HEAD bool CLASS::_reading = false;
    TEMPLATE_HEAD bool CLASS::_busy = false;
    TEMPLATE_HEAD bool CLASS::_eventsEnabled = true;
	TEMPLATE_HEAD bool CLASS::_nackLastByte = false;
	TEMPLATE_HEAD bool CLASS::_repeatedStart = false;
    TEMPLATE_HEAD CLASS::ErrorCode CLASS::_lastOperationStatus = CLASS::ErrorCodeNone;

    TEMPLATE_HEAD unsigned char CLASS::_outputBuffer[CLASS::_outputBufferLength];
    TEMPLATE_HEAD unsigned int CLASS::_outputLength = 0;
    TEMPLATE_HEAD unsigned int CLASS::_outputIndex = 0;

    TEMPLATE_HEAD unsigned char* CLASS::_inputBuffer = 0;
    TEMPLATE_HEAD unsigned int CLASS::_inputLength = 0;
	TEMPLATE_HEAD unsigned int CLASS::_inputIndex = 0;
}
}
}

#endif // EMF32_I2C_H

