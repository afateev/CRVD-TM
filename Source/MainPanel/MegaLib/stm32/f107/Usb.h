////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		04/03/2013
////////////////////////////////////////////////////////////////////////////////

#ifndef USB_STM32_F107_H
#define USB_STM32_F107_H

#include "UsbParts.h"

template<unsigned int BaseAddr>
class UsbHardwareBase:
	public UsbAhbConfigurationRegister					<BaseAddr + 0x08>,
	public UsbConfigurationRegister						<BaseAddr + 0x0C>,
	public UsbCoreInterruptRegister						<BaseAddr + 0x14>,
	public UsbInterruptMaskRegister						<BaseAddr + 0x18>,
	public UsbReadStatusRegister						<BaseAddr + 0x1C>,
	public UsbPopReadStatusRegister						<BaseAddr + 0x20>,
	public UsbReceiveFifoSizeRegister					<BaseAddr + 0x24>,
	public UsbNonPeriodicTransmitFifoSizeRegister		<BaseAddr + 0x28>,
	public UsbGeneralCoreConfigurationRegister			<BaseAddr + 0x38>,
	public UsbPeriodicTransmitFifoSizeRegister			<BaseAddr + 0x100>,
	public UsbHostConfigurationRegister					<BaseAddr + 0x400>,
	public UsbHostChannelsInterruptRegister				<BaseAddr + 0x414>,
	public UsbHostChannelsInterruptMaskRegister			<BaseAddr + 0x418>,
	public UsbHostPortControlAndStatusRegister			<BaseAddr + 0x440>,
	public UsbHostChannelCharacteristicsRegister		<BaseAddr + 0x500>,
	public UsbHostChannelInterruptRegister				<BaseAddr + 0x508>,
	public UsbHostChannelInterruptMaskRegister			<BaseAddr + 0x50C>,
	public UsbHostChannelTransferSizeRegister			<BaseAddr + 0x510>,
	public UsbHostChannelFifoRegister					<BaseAddr + 0x1000>
{
};

//#pragma data_alignment = 1
struct SetupPacketStruct
{
	struct
	{
		unsigned char Recepient							: 5;
		unsigned char Type								: 2;
		unsigned char DataPhaseTransferDirection		: 1;	// 1 - ожидаются данные от устройства, 0 - данные отправляются на устройтсво
	} RequestType;
	unsigned char Request;
	unsigned short Value;
	unsigned short Index;
	unsigned short Length;
	
	SetupPacketStruct()
	{
		memset(this, 0, sizeof(SetupPacketStruct));
	}
};

template <class Usb>
class UsbChannel
{
public:
	enum EndPointType
	{
		// Значения совпадают с теми, которые должны прописываться в регистр
		EndPointTypeControl			= 0,
		EndPointTypeIsochronous		= 1,
		EndPointTypeBulk			= 2,
		EndPointTypeInterrupt		= 3
	};
	
	enum ChannelState
	{
		StateNotInited,
		StateIdle,
		StateTransfering,
		StateTransferComplete,
		StateTransferEagain,
		StateTransferError,
	};
	
	enum TransferResult
	{
		TransferNA,
		TransferOk,
		TransferNak,
		TransferError
	};
	
	unsigned char N;
protected:
	EndPointType _epType;
	bool _isInput;
	unsigned short _maxPacketSize;
	unsigned char _endpointNumber;
	volatile ChannelState _state;
	unsigned char *_inputBuffer;
	unsigned char _inputBufferLen;
	TransferResult _lastTransferResult;
public:
	UsbChannel()
	{
		N = 255;
		_epType = EndPointTypeBulk;
		_isInput = false;
		_maxPacketSize = 8;
		_endpointNumber = 0;
		_state = StateNotInited;
		_inputBuffer = 0;
		_inputBufferLen = 0;
		_lastTransferResult = TransferNA;
	}
	void Init(EndPointType epType, bool isInput, unsigned char endpointNumber, unsigned short maxPacketSize)
	{
		_epType = epType;
		_isInput = isInput;
		_maxPacketSize = maxPacketSize;
		_endpointNumber = endpointNumber;
		
		Usb::HostChannelsInterruptEnable(true);
		Usb::ChannelInterruptEnable(N);
		switch(_epType)
		{
		case EndPointTypeControl:
		case EndPointTypeBulk:
			{
				Usb::ChannelTransferCompletedInterruptEnable(N, true);
				Usb::ChannelStallInterruptEnable(N, true);
				Usb::ChannelNakInterruptEnable(N, true);
				Usb::ChannelTransactionErrorInterruptEnable(N, true);
				Usb::ChannelDataToggleErrorInterruptEnable(N, true);
				
				if (_isInput)
				{
					Usb::ChannelBabbleErrorInterruptEnable(N, true);
				}
				else
				{
					Usb::ChannelResponseReceivedInterruptEnable(N, true);
				}
			}
			break;
		case EndPointTypeInterrupt:
			{
				Usb::ChannelTransferCompletedInterruptEnable(N, true);
				Usb::ChannelStallInterruptEnable(N, true);
				Usb::ChannelNakInterruptEnable(N, true);
				Usb::ChannelTransactionErrorInterruptEnable(N, true);
				Usb::ChannelFrameOverrunInterruptEnable(N, true);
				Usb::ChannelDataToggleErrorInterruptEnable(N, true);
				
				if (_isInput)
				{
					Usb::ChannelBabbleErrorInterruptEnable(N, true);
				}
			}
			break;
		case EndPointTypeIsochronous:
			{
				Usb::ChannelTransferCompletedInterruptEnable(N, true);
				Usb::ChannelAckInterruptEnable(N, true);
				Usb::ChannelFrameOverrunInterruptEnable(N, true);
				
				if (_isInput)
				{
					Usb::ChannelTransactionErrorInterruptEnable(N, true);
					Usb::ChannelBabbleErrorInterruptEnable(N, true);
				}
			}
			break;
		}
		
		Usb::ChannelSetMaxPacketSize(N, _maxPacketSize);
		Usb::ChannelSetEndpointNumber(N, _endpointNumber);
		Usb::ChannelSetEndpointAsInput(N, _isInput);
		Usb::ChannelSetEndpointType(N, _epType);
		
		_state = StateIdle;
	}
	
	bool Write(unsigned char *buf, unsigned char len)
	{
		Usb::ChannelSetTransferSize(N, len);
		Usb::ChannelSetTransferPacketCount(N, 1);
		Usb::ChannelSetTransferDataPid(N, 3);
		
		_lastTransferResult = TransferNA;
		_state = StateTransfering;
		Usb::ChannelSetEnabled(N, true);
		
		int i = 0;
		
		while(!Usb::IsNonPeriodicTxFifoEmptyInterrupt())
		{
			i++;
		}
		
		// длина в 32-х битных словах
		unsigned char buflen32 = (len + 3) >> 2;
		unsigned int *src = (unsigned int *)buf;
  		for (; buflen32 > 0; buflen32--)
    	{
      		unsigned int data = *src++;
			Usb::ChannelWrite(N, data);
    	}
		
		//Usb::NonPeriodicTxFifoEmptyInterruptEnable(true);
		
		while(_state == StateTransfering)
		{
			i++;
		}
		
		bool res = _state == StateTransferComplete;
		_state == StateIdle;
		
		return res;
	}
	
	unsigned char Read(unsigned char *buf, unsigned char len)
	{
		_inputBuffer = buf;
		_inputBufferLen = len;
		Usb::ChannelSetTransferSize(N, len);
		Usb::ChannelSetTransferPacketCount(N, 1);
		Usb::ChannelSetTransferDataPid(N, 2);
		
		int i = 0;
		bool repeate = true;
		while (repeate)
		{
			_lastTransferResult = TransferNA;
			_state = StateTransfering;
			Usb::ChannelSetEnabled(N, true);
			while(_state == StateTransfering)
			{
				i++;
			}
			
			repeate = _state == StateTransferEagain;
		}
		
		_inputBuffer = 0;
		_inputBufferLen = 0;
		
		if (_state == StateTransferComplete)
		{
			return len;
		}
		
		return 0;
	}
	
	void Halt(TransferResult res)
	{
		_lastTransferResult = res;
		Usb::ChannelHaltedInterruptEnable(N, true);
		Usb::ChannelHalt(N);
	}
	
	void OnDataReceived(unsigned char len)
	{
		// длина в 32-х битных словах
		unsigned char buflen32 = (len + 3) >> 2;
		//unsigned int *src = (unsigned int *)buf;
  		int i = 0;
		Usb::ChannelRead(0);
		for (; buflen32 > 0; buflen32--)
    	{
      		unsigned int data = Usb::ChannelRead(0);
			if (_inputBuffer)
			{
				for (unsigned char b = 0; b < 4; b++)
				{
					unsigned char nextByte = data >> (8 * b);
					if (_inputBufferLen > i)
					{
						_inputBuffer[i] = nextByte;
					}
					
					i++;
				}
			}
    	}
	}
	
	void OnInterrupt()
	{
		if (Usb::IsChannelInterrupt(N) && Usb::ChannelInterruptEnabled(N))
		{
			if(_isInput)
			{
				OnInterruptIn();
			}
			else
			{
				OnInterruptOut();
			}
		}
	}
	
	void OnInterruptIn()
	{
		if (Usb::ChannelIsAckInterrupt(N))
		{
			Usb::ChannelResetAckInterrupt(N);
		}
		else if (Usb::ChannelIsStallInterrupt(N))
		{
			Usb::ChannelResetStallInterrupt(N);
			Usb::ChannelResetNakInterrupt(N);
			Halt(TransferError);
		}
		else if (Usb::ChannelIsDataToggleErrorInterrupt(N))
		{
			Usb::ChannelResetDataToggleErrorInterrupt(N);
			Usb::ChannelResetNakInterrupt(N);
			Halt(TransferError);
		}
		else if (Usb::ChannelIsFrameOverrunInterrupt(N))
		{
			Usb::ChannelResetFrameOverrunInterrupt(N);
			Halt(TransferError);
		}
		else if (Usb::ChannelIsTransferCompletedInterrupt(N))
		{
			Usb::ChannelResetTransferCompletedInterrupt(N);
			if (_state == StateTransfering)
			{
				_state = StateTransferComplete;
			}
			Halt(TransferOk);
		}
		else if (Usb::ChannelIsHaltedInterrupt(N))
		{
			Usb::ChannelHaltedInterruptEnable(N, false);
			Usb::ChannelResetHaltedInterrupt(N);
			
			if (_lastTransferResult == TransferOk)
			{
				if (_state == StateTransfering)
				{
					_state = StateTransferComplete;
				}
			}
			else if (_lastTransferResult == TransferNak)
			{
				_state = StateTransferEagain;
			}
			else
			{
				if (_state == StateTransfering)
				{
					_state = StateTransferError;
				}
			}
		}
		else if (Usb::ChannelIsTransactionErrorInterrupt(N))
		{
			Usb::ChannelResetTransactionErrorInterrupt(N);
			Halt(TransferError);
		}
		else if (Usb::ChannelIsNakInterrupt(N))
		{
			Usb::ChannelResetNakInterrupt(N);
			Halt(TransferNak);
		}
	}
	
	void OnInterruptOut()
	{
		if (Usb::ChannelIsAckInterrupt(N))
		{
			Usb::ChannelResetAckInterrupt(N);
		}
		else if (Usb::ChannelIsFrameOverrunInterrupt(N))
		{
			Usb::ChannelResetFrameOverrunInterrupt(N);
			Halt(TransferError);
		}
		else if (Usb::ChannelIsTransferCompletedInterrupt(N))
		{
			Usb::ChannelResetTransferCompletedInterrupt(N);
			//Halt(TransferOk);
			if (_state == StateTransfering)
			{
				_state = StateTransferComplete;
			}
		}
		else if (Usb::ChannelIsStallInterrupt(N))
		{
			Usb::ChannelResetStallInterrupt(N);
			Halt(TransferError);
		}
		else if (Usb::ChannelIsNakInterrupt(N))
		{
			Usb::ChannelResetNakInterrupt(N);
			Halt(TransferNak);
		}
		else if (Usb::ChannelIsTransactionErrorInterrupt(N))
		{
			Usb::ChannelResetTransactionErrorInterrupt(N);
			Halt(TransferError);
		}
		else if (Usb::ChannelIsDataToggleErrorInterrupt(N))
		{
			Usb::ChannelResetDataToggleErrorInterrupt(N);
			Usb::ChannelResetNakInterrupt(N);
			Halt(TransferError);
		}
		else if (Usb::ChannelIsHaltedInterrupt(N))
		{
			Usb::ChannelHaltedInterruptEnable(N, false);
			Usb::ChannelResetHaltedInterrupt(N);
			
			if (_lastTransferResult == TransferOk)
			{
				if (_state == StateTransfering)
				{
					_state = StateTransferComplete;
				}
			}
			else if (_lastTransferResult == TransferNak)
			{
				_state = StateTransferEagain;
			}
			else
			{
				if (_state == StateTransfering)
				{
					_state = StateTransferError;
				}
			}
		}
	}
};


template<unsigned int BaseAddr>
class UsbHardwareImpl:
	public UsbHardwareBase<BaseAddr>
{
protected:
	static const unsigned short ReceiveFifoSize = 512;
	static const unsigned short NonPeriodicTransmitFifoSize = 384;
	static const unsigned short PeriodicTransmitFifoSize = 384;
	static const unsigned char ChannelCount = 8;
	
	enum StateCode
	{
		StateNotInited,
		StateWaitConnect,
		StateConnectDetected,
		StateConnected,
		StateDisconnected
	};
	
	typedef UsbChannel< UsbHardwareImpl<BaseAddr> > ChannelType;
	
	struct SelfFields
	{
		StateCode State;
		ChannelType Channel[ChannelCount];
		
		SelfFields()
		{
			State = StateNotInited;
			for (int i = 0; i < ChannelCount; i++)
			{
				Channel[i].N = i;
			}
		}
	};
	
	static SelfFields _self;
	
public:
	typedef UsbHardwareBase<BaseAddr> Base;
public:
	static void Init()
	{
		Base::FullSpeedSerialTransceiverSelect();
		Base::SetPowerDown(false);
		
		// Common core initialization
		Base::GlobalInterruptEnable(true);
		//Base::ResetReceiveFifoNonEmptyInterrupt();
		Base::SetTxFifoEmptyLevel(1);
		//Base::SrpCapabilityEnable(true);
		Base::OtgInterruptEnable(true);
		//Base::ModeMismatchInterruptEnable(true);
		
		// Only Host Mode
		Base::ForceHostModeEnable(true);
		
		for (unsigned long i = 0; i < 25000;)
		{
			i++;
		}
		
		// Размеры задаются в 32-битных словах
		unsigned short offset = 0;
		Base::SetReceiveFifoSize(ReceiveFifoSize / 4);
		offset += ReceiveFifoSize / 4;
		Base::SetNonPeriodicTransmitFifoSize(offset, NonPeriodicTransmitFifoSize / 4);
		offset += NonPeriodicTransmitFifoSize / 4;
		Base::SetPeriodicTransmitFifoSize(offset, PeriodicTransmitFifoSize / 4);
		
		// Host initialization
		Base::HostPortInterruptEnable(true);
		Base::SetClockSpeed(1);
		Base::SetPortPower(true);
		
		_self.State = StateWaitConnect;
	}
	
	static void PortReset()
	{
		Base::SetPortReset(true);
		
		for (unsigned long i = 0, j = 1; i < 100000; i++)
		{
			j *= 5;
		}
		
		Base::SetPortReset(false);
	}
	
	static void Run()
	{
		switch(_self.State)
		{
		case StateNotInited:
			{
			}
			break;
		case StateWaitConnect:
			{
			}
			break; 
		case StateConnectDetected:
			{
			}
			break;
		case StateConnected:
			{
				_self.Channel[0].Init(ChannelType::EndPointTypeControl, false, 0, 32);
				_self.Channel[1].Init(ChannelType::EndPointTypeControl, true, 0, 32);
				/*	
				SetupPacketStruct p;
				p.RequestType.Recepient = 0;
				p.RequestType.Type = 0;
				p.RequestType.DataPhaseTransferDirection = 1;
				p.Request = 0x00;
				p.Value = 0;
				p.Index = 0;
				p.Length = 2;
					
				unsigned char pLen = sizeof(p);
					
				unsigned short status = 0;
					
				_self.Channel[0].Write((unsigned char *)&p, pLen);
				if (_self.Channel[1].Read((unsigned char *)&status, sizeof(status)))
				{
					
				}
				*/
				SetupPacketStruct p;
				p.RequestType.Recepient = 0;
				p.RequestType.Type = 0;
				p.RequestType.DataPhaseTransferDirection = 1;
				p.Request = 0x06;
				p.Value = 0x0100;
				p.Index = 0;
				p.Length = 18;
					
				unsigned char pLen = sizeof(p);
					
				unsigned char status[18];
					
				if (_self.Channel[0].Write((unsigned char *)&p, pLen))
				{
					if (_self.Channel[1].Read((unsigned char *)&status, 18))
					{
						//if (_self.Channel[0].Write(NULL, 0))
						{
							p.Value = 0x0100;
							p.Index = 0;
							p.Length = 18;
							if (_self.Channel[0].Write((unsigned char *)&p, pLen))
							{
								if (_self.Channel[1].Read((unsigned char *)&status, 18))
								{
									if (status[0] == 21)
									{
										_self.State = StateNotInited;
									}
								}
							}
						}
					}
				}
				_self.State = StateNotInited;
			}
			break;
		}
	}
	
	inline static void InterruptHandler()
	{
		if (Base::IsHostPortInterrupt() && Base::HostPortInterruptEnabled())
		{
			if (Base::IsConnectDetected())
			{
				Base::ResetConnectDetected();
				
				_self.State = StateConnectDetected;
				
				PortReset();
				return;
			}
			
			if (Base::IsPortEnableChanged())
			{
				Base::ResetPortEnableChanged();
				
				if (Base::IsFullSpeed())
				{
					
				}
				
				if (Base::IsConnected())
				{
					_self.State = StateConnected;
					Base::ReceiveFifoNonEmptyInterruptEnable(true);
				}
				else
				{
					_self.State = StateDisconnected;
					Base::ReceiveFifoNonEmptyInterruptEnable(false);
				}
				
				return;
			}
		}
		
		if (Base::IsNonPeriodicTxFifoEmptyInterrupt() && Base::NonPeriodicTxFifoEmptyInterruptEnabled())
		{
			Base::NonPeriodicTxFifoEmptyInterruptEnable(false);
		}
		
		if (Base::IsHostChannelsInterrupt() && Base::HostChannelsInterruptEnabled())
		{
			for (unsigned char i = 0; i < ChannelCount; i++)
			{
				_self.Channel[i].OnInterrupt();
			}
		}
		
		if (Base::IsReceiveFifoNonEmptyInterrupt() && Base::ReceiveFifoNonEmptyInterruptEnabled())
		{
			unsigned char status = Base::GetReceivedPacketStatus();
			unsigned char channel = Base::GetReceivedChannelNumber();
			unsigned char dataPid = Base::GetReceivedDataPid();
			unsigned char len = Base::GetReceivedByteCount();

			if (status == 2)
			{
				if (channel < ChannelCount)
				{
					_self.Channel[channel].OnDataReceived(len);
				}
			}
			
			Base::PopReceiveFifo();
		}
	}
};

template<unsigned int BaseAddr>
typename UsbHardwareImpl<BaseAddr>::SelfFields
UsbHardwareImpl<BaseAddr>::_self;


class UsbHardware
{
public:
	typedef UsbHardwareImpl<0x50000000> _A;
};

#endif