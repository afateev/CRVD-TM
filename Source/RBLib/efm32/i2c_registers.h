
#ifndef EMF32_I2C_REGISTERS_H
#define EMF32_I2C_REGISTERS_H

#include "Register.h"

namespace Rblib
{
namespace Efm32
{
namespace I2C
{

	// Тип значения регистра
	typedef unsigned int ValueDataType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;

    // I2Cn_CTRL - Control Register
    template<RegisterAddressType Address>
    class ControlRegister
    {
    public:

        // значения CLHR
        enum ClockRatio;

        // значения BITO
        enum BusIdleTimeout;

        // значения CLTO
        enum ClockLowTimeout;

    public:

        static inline ValueDataType& Value()
        {
            return (ValueDataType&) *Register::Ptr();
        }

        // enable
        static inline void Enable()
        {
            Register::Ptr()->Enable = true;
        }

        static inline void Disable()
        {
            Register::Ptr()->Enable = false;
        }

        static inline bool Enabled()
        {
            return Register::Ptr()->Enable;
        }

        // slave/master
        static inline void SetSlave()
        {
            Register::Ptr()->Slave = true;
        }

        static inline void SetMaster()
        {
            Register::Ptr()->Slave = false;
        }

        static inline bool Slave()
        {
            return Register::Ptr()->Slave;
        }

        static inline bool Master()
        {
            return !Register::Ptr()->Slave;
        }

        // auto ack
        static inline void AutoAckEnable()
        {
            Register::Ptr()->AutoAck = true;
        }

        static inline void AutoAckDisable()
        {
            Register::Ptr()->AutoAck = false;
        }

        static inline bool AutoAckEnabled()
        {
            return Register::Ptr()->AutoAck;
        }

        // auto stop when empty
        static inline void AutoStopWhenEmptyEnable()
        {
            Register::Ptr()->AutoStopEmpty = true;
        }

        static inline void AutoStopWhenEmptyDisable()
        {
            Register::Ptr()->AutoStopEmpty = false;
        }

        static inline bool AutoStopWhenEmptyEnabled()
        {
            return Register::Ptr()->AutoStopEmpty;
        }

        // auto stop on nack
        static inline void AutoStopOnNackEnable()
        {
            Register::Ptr()->AutoStopNack = true;
        }

        static inline void AutoStopOnNackDisable()
        {
            Register::Ptr()->AutoStopNack = false;
        }

        static inline bool AutoStopOnNackEnabled()
        {
            return Register::Ptr()->AutoStopNack;
        }

        // arbitration
        static inline void ArbitrationEnable()
        {
            Register::Ptr()->ArbitrationDisable = false;
        }

        static inline void ArbitrationDisable()
        {
            Register::Ptr()->ArbitrationDisable = true;
        }

        static inline bool ArbitrationEnabled()
        {
            return Register::Ptr()->ArbitrationDisable;
        }

        // general call enable
        static inline void GeneralCallEnable()
        {
            Register::Ptr()->GeneralCallEnable = true;
        }

        static inline void GeneralCallDisable()
        {
            Register::Ptr()->GeneralCallEnable = false;
        }

        static inline bool GeneralCallEnabled()
        {
            return Register::Ptr()->GeneralCallEnable;
        }

        // clock low high ratio
        static inline void SetClockRatio(ClockRatio ratio)
        {
            Register::Ptr()->ClockRatio = ratio;
        }

        static inline ClockRatio GetClockRatio()
        {
            return (ClockRatio) Register::Ptr()->ClockRatio;
        }

        static inline void BusIdleTimeoutEnable()
        {
            Register::Ptr()->BusIdleTimeoutEnable = true;
        }

        static inline bool BusIdleTimeoutEnabled()
        {
            return Register::Ptr()->BusIdleTimeoutEnable;
        }

        static inline void BusIdleTimeoutDisable()
        {
            Register::Ptr()->BusIdleTimeoutEnable = false;
        }

        // bus idle timeout (BITO + GIBITO)
        static inline void SetBusIdleTimeout(BusIdleTimeout timeout)
        {
            if (timeout == BusIdleTimeoutOff)
                Register::Ptr()->BusIdleTimeoutEnable = false;
            else
                Register::Ptr()->BusIdleTimeoutEnable = true;

            Register::Ptr()->BusIdleTimeout = timeout; 
        }

        // bus idle timeout (BITO), только таймаут, не трогает флаг GIBITO
        static inline void SetBusBusyTimeout(BusIdleTimeout timeout)
        {
            Register::Ptr()->BusIdleTimeout = timeout; 
        }

        static inline BusIdleTimeout GetBusIdleTimeout()
        {
            return Register::Ptr()->BusIdleTimeout;
        }

        // clock low timeout
        static inline void SetCLockLowTimeout(ClockLowTimeout timeout)
        {
            Register::Ptr()->ClockLowTimeout = timeout;
        }

        static inline ClockLowTimeout GetCLockLowTimeout()
        {
            return Register::Ptr()->ClockLowTimeout;
        }

    public:

        // значения CLHR
        enum ClockRatio
        {
            ClockRatioStandard = 0,
            ClockRatioAsymmetric = 1,
            ClodkRatioFast = 2
        };

        // значения BITO
        enum BusIdleTimeout
        {
            BusIdleTimeoutOff = 0,
            BusIdleTimeout40Cycles = 1,
            BusIdleTimeout80Cycles = 2,
            BusIdleTimeout160Cycles = 3
        };

        // значения CLTO
        enum ClockLowTimeout
        {
            ClockLowTimeoutOff = 0,
            ClockLowTimeout40Cycles = 1,
            ClockLowTimeout80Cycles = 2,
            ClockLowTimeout160Cycles = 3,
            ClockLowTimeout320Cycles = 4,
            ClockLowTimeout1024Cycles = 5
        };

    private:

        struct RegStruct
        {
            ValueDataType Enable : 1; // EN
            ValueDataType Slave : 1; // SLAVE
            ValueDataType AutoAck : 1; // AUTOACK
            ValueDataType AutoStopEmpty : 1; // AUTOSE
            ValueDataType AutoStopNack : 1; // AUTOSN
            ValueDataType ArbitrationDisable : 1; // ARBDIS
            ValueDataType GeneralCallEnable : 1; // GCAMEN
            ValueDataType : 1;
            ValueDataType ClockRatio : 2; // CLHR
            ValueDataType : 2;
            ValueDataType BusIdleTimeout : 2; // BITO
            ValueDataType : 1;
            ValueDataType BusIdleTimeoutEnable : 1; // GIBITO
            ValueDataType ClockLowTimeout : 3;
            ValueDataType : 13;
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };

    // I2Cn_CMD - Command Register
    template<RegisterAddressType Address>
    class CommandRegister
    {
    public:

        static inline ValueDataType& Value()
        {
            return (ValueDataType&) *Register::Ptr();
        }

        static inline void Start()
        {
            Register::Ptr()->Start = true;
        }

        static inline void Restart()
        {
            static const RegStruct commands = {1, 1, 0, 0, 0, 0, 0, 0}; // Stop + Start
            (*(RegStruct*)Register::Ptr()) = commands;
        }

        static inline void Stop()
        {
            Register::Ptr()->Stop = true;
        }

        static inline void Ack()
        {
            Register::Ptr()->Ack = true;
        }

        static inline void Nack()
        {
            Register::Ptr()->Nack = true;
        }

        static inline void Continue()
        {
            Register::Ptr()->Continue = true;
        }
		
        static inline void Abort()
        {
            Register::Ptr()->Abort = true;
        }

        static inline void StopAndAbort()
        {
            static const RegStruct commands = {0, 1, 0, 0, 0, 1, 0, 0}; // Stop + Abort
            (*(RegStruct*)Register::Ptr()) = commands;
        }

        static inline void ClearTransmitter()
        {
            Register::Ptr()->ClearTransmitter = true;
        }

        static inline void ClearPendingCommangs()
        {
            Register::Ptr()->ClearPendingCommangs = true;
        }

    private:

        struct RegStruct
        {
            ValueDataType Start : 1;
            ValueDataType Stop : 1;
            ValueDataType Ack : 1;
            ValueDataType Nack : 1;
            ValueDataType Continue : 1;
            ValueDataType Abort : 1;
            ValueDataType ClearTransmitter : 1;
            ValueDataType ClearPendingCommangs : 1;
            ValueDataType : 24;
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };

    // I2Cn_STATE - State Register
    template<RegisterAddressType Address>
    class StateRegister
    {
    public:

        enum TransmissionState;
	  
    public:

        static inline ValueDataType& Value()
        {
            return (ValueDataType&) *Register::Ptr();
        }

        static inline bool Busy()
        {
            return Register::Ptr()->Busy;
        }

        static inline bool Master()
        {
            return Register::Ptr()->Master;
        }

        static inline bool Transmitter()
        {
            return Register::Ptr()->Transmitter;
        }

        static inline bool Nacked()
        {
            return Register::Ptr()->Nacked;
        }

        static inline bool BusHold()
        {
            return Register::Ptr()->BusHold;
        }

        static inline TransmissionState State()
        {                    
            return (TransmissionState) Register::Ptr()->State; 
        }                    

    public:

        enum TransmissionState
        {
            StateIdle = 0,
            StateWait = 1,
            StateStart = 2,
            StateAddr = 3,
            StateAddrAck = 4,
            StateData = 5,
            StateDataAck = 6
        };

    private:

        struct RegStruct
        {
            ValueDataType Busy : 1;
            ValueDataType Master : 1;
            ValueDataType Transmitter : 1;
            ValueDataType Nacked : 1;
            ValueDataType BusHold : 1;
            ValueDataType State : 3;
            ValueDataType : 24;
        };

    private:

       typedef Register<Address, RegStruct> Register;

    };

    // I2Cn_STATUS - Status Register
    template<RegisterAddressType Address>
    class StatusRegister
    {
    public:

        static inline ValueDataType& Value()
        {
            return (ValueDataType&) *Register::Ptr();
        }

        static inline bool Start()
        {
            return Register::Ptr()->Start;
        }

        static inline bool Stop()
        {
            return Register::Ptr()->Stop;
        }

        static inline bool Ack()
        {
            return Register::Ptr()-Ack;
        }

        static inline bool Nack()
        {
            return Register::Ptr()-Nack;
        }

        static inline bool Continue()
        {
            return Register::Ptr()->Continue;
        }

        static inline bool Abort()
        {
            return Register::Ptr()->Abort;
        }

        static inline bool TxComplete()
        {
            return Register::Ptr()->TxComplete;
        }

        static inline bool TxBufferLevel()
        {
            return Register::Ptr()->TxBufferLevel;
        }

        static inline bool RxDataValid()
        {
            return Register::Ptr()->RxDataValid;
        }

    private:

        struct RegStruct
        {
            ValueDataType Start : 1;
            ValueDataType Stop : 1;
            ValueDataType Ack : 1;
            ValueDataType Nack : 1;
            ValueDataType Continue : 1;
            ValueDataType Abort : 1;
            ValueDataType TxComplete: 1;
            ValueDataType TxBufferLevel: 1;
            ValueDataType RxDataValid: 1;
            ValueDataType : 23;
        };

    private:

       typedef Register<Address, RegStruct> Register;
        
    };

    // I2Cn_CLKDIV - Clock Division Register
    template<RegisterAddressType Address>
    class ClockDivisionRegister
    {
    public:

        static inline void SetDivider(ValueDataType divider)
        {
            Register::Ptr()->Divider = divider;
        }

        static inline ValueDataType GetDivider()
        {
            return Register::Ptr()->Divider;
        }

    private:

        struct RegStruct
        {
            ValueDataType Divider : 9;
            ValueDataType : 23;
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };

    // I2Cn_SADDR - Slave Address Register
    template<RegisterAddressType Address>
    class SlaveAddressRegister
    {
    public:

        static inline void SetAddress(ValueDataType address)
        {
            Register::Ptr()->SlaveAddress = address;
        }

        static inline ValueDataType GetAddress()
        {
            return Register::Ptr()->SlaveAddress;
        }

    private:

        struct RegStruct
        {
            ValueDataType : 1;
            ValueDataType SlaveAddress : 7;
            ValueDataType : 24;
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };

    // I2Cn_SADDRMASK - Slave Address Mask Register
    template<RegisterAddressType Address>
    class SlaveAddressMaskRegister
    {
    public:

        static inline void SetAddressMask(ValueDataType addressMask)
        {
            Register::Ptr()->SlaveAddressMask = addressMask;
        }

        static inline ValueDataType GetAddressMask()
        {
            return Register::Ptr()->SlaveAddressMask;
        }

    private:

        struct RegStruct
        {
            ValueDataType : 1;
            ValueDataType SlaveAddressMask : 7;
            ValueDataType : 24;
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };
	
    // I2Cn_RXDATA - Receive Buffer Data Register
    template<RegisterAddressType Address>
    class ReceiveBufferRegister
    {
    public:

        static inline ValueDataType Read()
        {
            return Register::Ptr()->Data;
        }

    private:

        struct RegStruct
        {
            ValueDataType Data : 8;
            ValueDataType : 24;
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };

    // I2Cn_RXDATAP - Receive Buffer Data Peek Register
    template<RegisterAddressType Address>
    class ReceiveBufferPeekRegister
    {
    public:

        static inline ValueDataType Read()
        {
            return Register::Ptr()->Data;
        }

    private:

        struct RegStruct
        {
            ValueDataType Data : 8;
            ValueDataType : 24;
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };

    // I2Cn_TXDATA - Transmit Buffer Data Register
    template<RegisterAddressType Address>
    class TransmitBufferRegister
    {
    public:

        static inline void Write(ValueDataType data)
        {
            Register::Ptr()->Data = data;
        }

    private:

        struct RegStruct
        {
            ValueDataType Data : 8;
            ValueDataType : 24;
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };

    // I2Cn_IF - Interrupt Flag Register
    template<RegisterAddressType Address>
    class InterruptFlagRegister
    {
    public:

        static inline bool Start()
        {
            return Register::Ptr()->Start;
        }

        static inline bool RepeatedStart()
        {
            return Register::Ptr()->RepeatedStart;
        }

        static inline bool MasterStop()
        {
            return Register::Ptr()->MasterStop;
        }

        static inline bool SlaveStop()
        {
            return Register::Ptr()->SlaveStop;
        }

        static inline bool AddressAccepted()
        {
            return Register::Ptr()->AddressAccepted;
        }

        static inline bool TxComplete()
        {
            return Register::Ptr()->TxComplete;
        }

        static inline bool TxBufferLevel()
        {
            return Register::Ptr()->TxBufferLevel;
        }

        static inline bool DataReceived()
        {
            return Register::Ptr()->DataReceived;
        }

        static inline bool Ack()
        {
            return Register::Ptr()->Ack;
        }

        static inline bool Nack()
        {
            return Register::Ptr()->Nack;
        }

        static inline bool ArbitrationLost()
        {
            return Register::Ptr()->ArbitrationLost;
        }

        static inline bool BusError()
        {
            return Register::Ptr()->BusError;
        }

        static inline bool BusHold()
        {
            return Register::Ptr()->BusHold;
        }

        static inline bool TransmitBufferOverflow()
        {
            return Register::Ptr()->TransmitBufferOverflow;
        }

        static inline bool ReceiveBufferUnderflow()
        {
            return Register::Ptr()->ReceiveBufferUnderflow;
        }

        static inline bool BusIdleTimeout()
        {
            return Register::Ptr()->BusIdleTimeout;
        }

        static inline bool ClockLowTimeout()
        {
            return Register::Ptr()->ClockLowTimeout;
        }

    private:

        struct RegStruct
        {
            ValueDataType Start : 1; // START
            ValueDataType RepeatedStart : 1; // PSTART
            ValueDataType AddressAccepted : 1; // ADDR
            ValueDataType TxComplete : 1; // TXC
            ValueDataType TxBufferLevel: 1; // TXBL
            ValueDataType DataReceived : 1; // RXDATAV
            ValueDataType Ack : 1; // ACK
            ValueDataType Nack : 1; // NACK
            ValueDataType MasterStop : 1; // MSTOP
            ValueDataType ArbitrationLost : 1; // ARBLOST
            ValueDataType BusError : 1; // BUSERR
            ValueDataType BusHold : 1; // BUSHOLD
            ValueDataType TransmitBufferOverflow : 1; // TXOF
            ValueDataType ReceiveBufferUnderflow : 1; // RXUF
            ValueDataType BusIdleTimeout : 1; // BITO
            ValueDataType ClockLowTimeout : 1; // CLTO
            ValueDataType SlaveStop : 1; // SSTOP
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };
	
    // I2Cn_IFS - Interrupt Flag Set Register
    template<RegisterAddressType Address>
    class InterruptFlagSetRegister
    {
    public:

        static inline void SetStart()
        {
            Register::Ptr()->Start = true;
        }

        static inline void SetRepeatedStart()
        {
            Register::Ptr()->RepeatedStart = true;
        }

        static inline void SetMasterStop()
        {
            Register::Ptr()->MasterStop = true;
        }

        static inline void SetSlaveStop()
        {
            Register::Ptr()->SlaveStop = true;
        }

        static inline void SetAddressAccepted()
        {
            Register::Ptr()->AddressAccepted = true;
        }

        static inline void SetTxComplete()
        {
            Register::Ptr()->TxComplete = true;
        }

        static inline void SetTxBufferLevel()
        {
            Register::Ptr()->TxBufferLevel = true;
        }

        static inline void SetRxDataValid()
        {
            Register::Ptr()->DataReceived = true;
        }

        static inline void SetAck()
        {
            Register::Ptr()->Ack = true;
        }

        static inline void SetNack()
        {
            Register::Ptr()->Nack = true;
        }

        static inline void SetArbitrationLost()
        {
            Register::Ptr()->ArbitrationLost = true;
        }

        static inline void SetBusError()
        {
            Register::Ptr()->BusError = true;
        }

        static inline void SetBusHold()
        {
            Register::Ptr()->BusHold = true;
        }

        static inline void SetTransmitBufferOverflow()
        {
            Register::Ptr()->TransmitBufferOverflow = true;
        }

        static inline void SetReceiveBufferUnderflow()
        {
            Register::Ptr()->ReceiveBufferUnderflow = true;
        }

        static inline void SetBusIdleTimeout()
        {
            Register::Ptr()->BusIdleTimeout = true;
        }

        static inline void SetClockLowTimeout()
        {
            Register::Ptr()->ClockLowTimeout = true;
        }

    private:

        struct RegStruct
        {
            ValueDataType Start : 1; // START
            ValueDataType RepeatedStart : 1; // PSTART
            ValueDataType AddressAccepted : 1; // ADDR
            ValueDataType TxComplete : 1; // TXC
            ValueDataType TxBufferLevel: 1; // TXBL
            ValueDataType DataReceived : 1; // RXDATAV
            ValueDataType Ack : 1; // ACK
            ValueDataType Nack : 1; // NACK
            ValueDataType MasterStop : 1; // MSTOP
            ValueDataType ArbitrationLost : 1; // ARBLOST
            ValueDataType BusError : 1; // BUSERR
            ValueDataType BusHold : 1; // BUSHOLD
            ValueDataType TransmitBufferOverflow : 1; // TXOF
            ValueDataType ReceiveBufferUnderflow : 1; // RXUF
            ValueDataType BusIdleTimeout : 1; // BITO
            ValueDataType ClockLowTimeout : 1; // CLTO
            ValueDataType SlaveStop : 1; // SSTOP
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };

    // I2Cn_IFC - Interrupt Flag Clear Register
    template<RegisterAddressType Address>
    class InterruptFlagClearRegister
    {
    public:

        static inline void ClearAll()
        {
            *((unsigned int*)Register::Ptr()) = 0x1FFFF;
        }

        static inline void ClearStart()
        {
            Register::Ptr()->Start = true;
        }

        static inline void ClearRepeatedStart()
        {
            Register::Ptr()->RepeatedStart = true;
        }

        static inline void ClearMasterStop()
        {
            Register::Ptr()->MasterStop = true;
        }

        static inline void ClearSlaveStop()
        {
            Register::Ptr()->SlaveStop = true;
        }

        static inline void ClearAddressAccepted()
        {
            Register::Ptr()->AddressAccepted = true;
        }

        static inline void ClearTxComplete()
        {
            Register::Ptr()->TxComplete = true;
        }

        static inline void ClearTxBufferLevel()
        {
            Register::Ptr()->TxBufferLevel = true;
        }

        static inline void ClearRxDataValid()
        {
            Register::Ptr()->DataReceived = true;
        }

        static inline void ClearAck()
        {
            Register::Ptr()->Ack = true;
        }

        static inline void ClearNack()
        {
            Register::Ptr()->Nack = true;
        }

        static inline void ClearArbitrationLost()
        {
            Register::Ptr()->ArbitrationLost = true;
        }

        static inline void ClearBusError()
        {
            Register::Ptr()->BusError = true;
        }

        static inline void ClearBusHold()
        {
            Register::Ptr()->BusHold = true;
        }

        static inline void ClearTransmitBufferOverflow()
        {
            Register::Ptr()->TransmitBufferOverflow = true;
        }

        static inline void ClearReceiveBufferUnderflow()
        {
            Register::Ptr()->ReceiveBufferUnderflow = true;
        }

        static inline void ClearBusIdleTimeout()
        {
            Register::Ptr()->BusIdleTimeout = true;
        }

        static inline void ClearClockLowTimeout()
        {
            Register::Ptr()->ClockLowTimeout = true;
        }

    private:

        struct RegStruct
        {
            ValueDataType Start : 1; // START
            ValueDataType RepeatedStart : 1; // PSTART
            ValueDataType AddressAccepted : 1; // ADDR
            ValueDataType TxComplete : 1; // TXC
            ValueDataType TxBufferLevel: 1; // TXBL
            ValueDataType DataReceived : 1; // RXDATAV
            ValueDataType Ack : 1; // ACK
            ValueDataType Nack : 1; // NACK
            ValueDataType MasterStop : 1; // MSTOP
            ValueDataType ArbitrationLost : 1; // ARBLOST
            ValueDataType BusError : 1; // BUSERR
            ValueDataType BusHold : 1; // BUSHOLD
            ValueDataType TransmitBufferOverflow : 1; // TXOF
            ValueDataType ReceiveBufferUnderflow : 1; // RXUF
            ValueDataType BusIdleTimeout : 1; // BITO
            ValueDataType ClockLowTimeout : 1; // CLTO
            ValueDataType SlaveStop : 1; // SSTOP
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };
	
    // I2Cn_IEN - Interrupt Enable Register
    template<RegisterAddressType Address>
    class InterruptEnableRegister
    {
    public:

        static inline ValueDataType& Value()
        {
            return (ValueDataType&) *Register::Ptr();
        }

        static inline void SetEnableFlags(ValueDataType flags)
        {
            *((ValueDataType*)Register::Ptr()) = flags;
        }

	  	// enable
	  
        static inline void StartEnable()
        {
            Register::Ptr()->Start = true;
        }

        static inline void RepeatedStartEnable()
        {
            Register::Ptr()->RepeatedStart = true;
        }

        static inline void MasterStopEnable()
        {
            Register::Ptr()->MasterStop = true;
        }

        static inline void SlaveStopEnable()
        {
            Register::Ptr()->SlaveStop = true;
        }

        static inline void AddressAcceptedEnable()
        {
            Register::Ptr()->AddressAccepted = true;
        }

        static inline void TxCompleteEnable()
        {
            Register::Ptr()->TxComplete = true;
        }

        static inline void TxBufferLevelEnable()
        {
            Register::Ptr()->TxBufferLevel = true;
        }

        static inline void RxDataValidEnable()
        {
            Register::Ptr()->DataReceived = true;
        }

        static inline void AckEnable()
        {
            Register::Ptr()->Ack = true;
        }

        static inline void NackEnable()
        {
            Register::Ptr()->Nack = true;
        }

        static inline void ArbitrationLostEnable()
        {
            Register::Ptr()->ArbitrationLost = true;
        }

        static inline void BusErrorEnable()
        {
            Register::Ptr()->BusError = true;
        }

        static inline void BusHoldEnable()
        {
            Register::Ptr()->BusHold = true;
        }

        static inline void TransmitBufferOverflowEnable()
        {
            Register::Ptr()->TransmitBufferOverflow = true;
        }

        static inline void ReceiveBufferUnderflowEnable()
        {
            Register::Ptr()->ReceiveBufferUnderflow = true;
        }

        static inline void BusIdleTimeoutEnable()
        {
            Register::Ptr()->BusIdleTimeout = true;
        }

        static inline void ClockLowTimeoutEnable()
        {
            Register::Ptr()->ClockLowTimeout = true;
        }

        // enabled

        static inline void StartEnabled()
        {
            Register::Ptr()->Start = true;
        }

        static inline void RepeatedStartEnabled()
        {
            Register::Ptr()->RepeatedStart = true;
        }

        static inline void MasterStopEnabled()
        {
            Register::Ptr()->MasterStop = true;
        }

        static inline void SlaveStopEnabled()
        {
            Register::Ptr()->SlaveStop = true;
        }

        static inline void AddressAcceptedEnabled()
        {
            Register::Ptr()->AddressAccepted = true;
        }

        static inline void TxCompleteEnabled()
        {
            Register::Ptr()->TxComplete = true;
        }

        static inline void TxBufferLevelEnabled()
        {
            Register::Ptr()->TxBufferLevel = true;
        }

        static inline void RxDataValidEnabled()
        {
            Register::Ptr()->DataReceived = true;
        }

        static inline void AckEnabled()
        {
            Register::Ptr()->Ack = true;
        }

        static inline void NackEnabled()
        {
            Register::Ptr()->Nack = true;
        }

        static inline void ArbitrationLostEnabled()
        {
            Register::Ptr()->ArbitrationLost = true;
        }

        static inline void BusErrorEnabled()
        {
            Register::Ptr()->BusError = true;
        }

        static inline void BusHoldEnabled()
        {
            Register::Ptr()->BusHold = true;
        }

        static inline void TransmitBufferOverflowEnabled()
        {
            Register::Ptr()->TransmitBufferOverflow = true;
        }

        static inline void ReceiveBufferUnderflowEnabled()
        {
            Register::Ptr()->ReceiveBufferUnderflow = true;
        }

        static inline void BusIdleTimeoutEnabled()
        {
            Register::Ptr()->BusIdleTimeout = true;
        }

        static inline void ClockLowTimeoutEnabled()
        {
            Register::Ptr()->ClockLowTimeout = true;
        }

		// disable
		
        static inline void StartDisable()
        {
            Register::Ptr()->Start = false;
        }

        static inline void RepeatedStartDisable()
        {
            Register::Ptr()->RepeatedStart = false;
        }

        static inline void MasterStopDisable()
        {
            Register::Ptr()->MasterStop = false;
        }

        static inline void SlaveStopDisable()
        {
            Register::Ptr()->SlaveStop = false;
        }

        static inline void AddressAcceptedDisable()
        {
            Register::Ptr()->AddressAccepted = false;
        }

        static inline void TxCompleteDisable()
        {
            Register::Ptr()->TxComplete = false;
        }

        static inline void TxBufferLevelDisable()
        {
            Register::Ptr()->TxBufferLevel = false;
        }

        static inline void RxDataValidDisable()
        {
            Register::Ptr()->DataReceived = false;
        }

        static inline void AckDisable()
        {
            Register::Ptr()->Ack = false;
        }

        static inline void NackDisable()
        {
            Register::Ptr()->Nack = false;
        }

        static inline void ArbitrationLostDisable()
        {
            Register::Ptr()->ArbitrationLost = false;
        }

        static inline void BusErrorDisable()
        {
            Register::Ptr()->BusError = false;
        }

        static inline void BusHoldDisable()
        {
            Register::Ptr()->BusHold = false;
        }

        static inline void TransmitBufferOverflowDisable()
        {
            Register::Ptr()->TransmitBufferOverflow = false;
        }

        static inline void ReceiveBufferUnderflowDisable()
        {
            Register::Ptr()->ReceiveBufferUnderflow = false;
        }

        static inline void BusIdleTimeoutDisable()
        {
            Register::Ptr()->BusIdleTimeout = false;
        }

        static inline void ClockLowTimeoutDisable()
        {
            Register::Ptr()->ClockLowTimeout = false;
        }

    public:

        enum EnableFlags
        {
            Start = 0x00000001, // START
            RepeatedStart = 0x00000002, // PSTART
            AddressAccepted = 0x00000004, // ADDR
            TxComplete = 0x00000008, // TXC
            TxBufferLevel = 0x00000010, // TXBL
            DataReceived = 0x00000020, // RXDATAV
            Ack = 0x00000040, // ACK
            Nack = 0x00000080, // NACK
            MasterStop = 0x00000100, // MSTOP
            ArbitrationLost = 0x00000200, // ARBLOST
            BusError = 0x00000400, // BUSERR
            BusHold = 0x00000800, // BUSHOLD
            TransmitBufferOverflow = 0x00001000, // TXOF
            ReceiveBufferUnderflow = 0x00002000, // RXUF
            BusIdleTimeout = 0x00004000, // BITO
            ClockLowTimeout = 0x00008000, // CLTO
            SlaveStop = 0x00010000 // SSTOP
        };

    private:

        struct RegStruct
        {
            ValueDataType Start : 1; // START
            ValueDataType RepeatedStart : 1; // PSTART
            ValueDataType AddressAccepted : 1; // ADDR
            ValueDataType TxComplete : 1; // TXC
            ValueDataType TxBufferLevel: 1; // TXBL
            ValueDataType DataReceived : 1; // RXDATAV
            ValueDataType Ack : 1; // ACK
            ValueDataType Nack : 1; // NACK
            ValueDataType MasterStop : 1; // MSTOP
            ValueDataType ArbitrationLost : 1; // ARBLOST
            ValueDataType BusError : 1; // BUSERR
            ValueDataType BusHold : 1; // BUSHOLD
            ValueDataType TransmitBufferOverflow : 1; // TXOF
            ValueDataType ReceiveBufferUnderflow : 1; // RXUF
            ValueDataType BusIdleTimeout : 1; // BITO
            ValueDataType ClockLowTimeout : 1; // CLTO
            ValueDataType SlaveStop : 1; // SSTOP
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };

    // I2Cn_ROUTE - I/O Routing Register
    template<RegisterAddressType Address>
    class IORoutingRegister
    {
    public:

        enum Location;

    public:

        static inline ValueDataType& Value()
        {
            return (ValueDataType&) *Register::Ptr();
        }

        // location

        static inline void SetLocation(Location location)
        {
            Register::Ptr()->Location = location;
        }

        static inline Location GetLocation()
        {
            return Register::Ptr()->Location;
        }

        // scl 

        static inline void SclEnable()
        {
            Register::Ptr()->SclEnabled = true;
        }

        static inline void SclDisable()
        {
            Register::Ptr()->SclEnabled = false;
        }

        static inline bool SclEnabled()
        {
            return Register::Ptr()->SclEnabled;
        }

        // sda

        static inline void SdaEnable()
        {
            Register::Ptr()->SdaEnabled = true;
        }

        static inline void SdaDisable()
        {
            Register::Ptr()->SdaEnabled = false;
        }

        static inline bool SdaEnabled()
        {
            return Register::Ptr()->SdaEnabled;
        }

        // all

        static inline void SetIORouting(Location location, bool sclEnable, bool sdaEnable)
        {
            Register::Ptr()->Location = location;
            Register::Ptr()->SclEnabled = sclEnable;
            Register::Ptr()->SdaEnabled = sdaEnable;
        }

        static inline void SetIORouting(Location location)
        {
            Register::Ptr()->Location = location;
            Register::Ptr()->SclEnabled = true;
            Register::Ptr()->SdaEnabled = true;
        }

    public:

        enum Location
        {
            Location0 = 0,
            Location1 = 1,
            Location2 = 2,
            Location3 = 3,
            Location4 = 4,
            Location5 = 5,
            Location6 = 6
        };

    private:

        struct RegStruct
        {
            ValueDataType SdaEnabled : 1;
            ValueDataType SclEnabled : 1;
            ValueDataType : 6;
            ValueDataType Location : 3;
        };

    private:

        typedef Register<Address, RegStruct> Register;

    };
}
}
}

#endif // EMF32_I2C_REGISTERS_H

