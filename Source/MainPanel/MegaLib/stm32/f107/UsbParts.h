////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		04/03/2013
////////////////////////////////////////////////////////////////////////////////

#ifndef USBPARTS_STM32_F107_H
#define USBPARTS_STM32_F107_H

#include "..\..\RegisterReference.h"
#include <string.h>

template<RegisterPointerType RegisterAddr>
class UsbAhbConfigurationRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType GlobalInterruptEnable			: 1;
		RegisterDataType Reserved0						: 6;
		RegisterDataType TxFifoEmptyLevel				: 1;
		RegisterDataType PeriodicTxFifoEmptyLevel		: 1;
		RegisterDataType Reserved1						: 23;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void GlobalInterruptEnable(bool enable = true)
	{
		RegRef::_register->GlobalInterruptEnable = enable;
	}
	
	inline static void SetTxFifoEmptyLevel(unsigned char value)
	{
		RegRef::_register->TxFifoEmptyLevel = value;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbConfigurationRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType FsTimeoutCalibration					: 3;
		RegisterDataType Reserved0								: 4;
		RegisterDataType FullSpeedSerialTransceiverSelect		: 1;
		RegisterDataType SrpCapabilityEnable					: 1;
		RegisterDataType HnpCapabilityEnable					: 1;
		RegisterDataType UsbTurnaroundTime						: 4;
		RegisterDataType Reserved1								: 15;
		RegisterDataType ForceHostMode							: 1;
		RegisterDataType ForceDeviceMode						: 1;
		RegisterDataType CorruptTxPacket						: 1;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void FullSpeedSerialTransceiverSelect(bool enable = true)
	{
		RegRef::_register->FullSpeedSerialTransceiverSelect = enable;
	}
	
	inline static void SrpCapabilityEnable(bool enable = true)
	{
		RegRef::_register->SrpCapabilityEnable = enable;
	}
	
	inline static void ForceHostModeEnable(bool enable = true)
	{
		RegRef::_register->ForceHostMode = enable;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbCoreInterruptRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType CurrentMode										: 1;
		RegisterDataType ModeMismatch										: 1;
		RegisterDataType Otg												: 1;
		RegisterDataType StartOfFrame										: 1;
		RegisterDataType ReceiveFifoNonEmpty								: 1;
		RegisterDataType NonPeriodicTxFifoEmpty								: 1;
		RegisterDataType GlobalNonPeriodicInNakEffective					: 1;
		RegisterDataType GlobalOutNakEffective								: 1;
		RegisterDataType Reserved1											: 2;
		RegisterDataType EarlySuspend										: 1;
		RegisterDataType UsbSuspend											: 1;
		RegisterDataType UsbReset											: 1;
		RegisterDataType EnumerationDone									: 1;
		RegisterDataType IsochronousOutPacketDropped						: 1;
		RegisterDataType EndOfPeriodicFrame									: 1;
		RegisterDataType Reserved2											: 2;
		RegisterDataType InEndpoint											: 1;
		RegisterDataType OutEndpoint										: 1;
		RegisterDataType IncompleteIsochronousInTransfer					: 1;
		RegisterDataType IncompletePeriodicOrIsochronousOutTransfer			: 1;
		RegisterDataType Reserved3											: 2;
		RegisterDataType HostPort											: 1;
		RegisterDataType HostChannels										: 1;
		RegisterDataType PeriodicTxFifoEmpty								: 1;
		RegisterDataType Reserved4											: 1;
		RegisterDataType ConnectorIdStatusChange							: 1;
		RegisterDataType DisconnectDetected									: 1;
		RegisterDataType SessionRequest										: 1;
		RegisterDataType Resume												: 1;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
	typedef RegisterReference<unsigned int, RegisterAddr> RegWriteRef;
public:
	inline static bool IsReceiveFifoNonEmptyInterrupt()
	{
		return RegRef::_register->ReceiveFifoNonEmpty;
	}
	
	inline static bool IsHostPortInterrupt()
	{
		return RegRef::_register->HostPort;
	}
	
	inline static bool IsNonPeriodicTxFifoEmptyInterrupt()
	{
		return RegRef::_register->NonPeriodicTxFifoEmpty;
	}
	
	inline static bool IsHostChannelsInterrupt()
	{
		return RegRef::_register->HostChannels;
	}
	
	/* —брасываетс€, только если сбросить породившие это прерывание флаги в регистре OTG_FS_HPRT (UsbHostPortControlAndStatusRegister)
	inline static void ResetHostPortInterrupt()
	{
		RegisterStruct<unsigned int> tmp = SafeRead();
		tmp.HostPort = 1;
		SafeWrite(tmp);
	}*/
protected:
	inline static RegisterStruct<unsigned int> SafeRead()
	{
		RegisterStruct<unsigned int> res;
		memset(&res, 0, sizeof(res));
		return res;
	}
	
	inline static void SafeWrite(RegisterStruct<unsigned int> val)
	{
		unsigned int *tmp = (unsigned int *)&val;
		*RegWriteRef::_register = *tmp;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbInterruptMaskRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Reserved0											: 1;
		RegisterDataType ModeMismatch										: 1;
		RegisterDataType Otg												: 1;
		RegisterDataType StartOfFrame										: 1;
		RegisterDataType ReceiveFifoNonEmpty								: 1;
		RegisterDataType NonPeriodicTxFifoEmpty								: 1;
		RegisterDataType GlobalNonPeriodicInNakEffective					: 1;
		RegisterDataType GlobalOutNakEffective								: 1;
		RegisterDataType Reserved1											: 2;
		RegisterDataType EarlySuspend										: 1;
		RegisterDataType UsbSuspend											: 1;
		RegisterDataType UsbReset											: 1;
		RegisterDataType EnumerationDone									: 1;
		RegisterDataType IsochronousOutPacketDropped						: 1;
		RegisterDataType EndOfPeriodicFrame									: 1;
		RegisterDataType Reserved2											: 1;
		RegisterDataType EndpointMismatch									: 1;
		RegisterDataType InEndpoints										: 1;
		RegisterDataType OutEndpoints										: 1;
		RegisterDataType IncompleteIsochronousInTransfer					: 1;
		RegisterDataType IncompletePeriodicOrIsochronousOutTransfer			: 1;
		RegisterDataType Reserved3											: 2;
		RegisterDataType HostPort											: 1;
		RegisterDataType HostChannels										: 1;
		RegisterDataType PeriodicTxFifoEmpty								: 1;
		RegisterDataType Reserved4											: 1;
		RegisterDataType ConnectorIdStatusChange							: 1;
		RegisterDataType DisconnectDetected									: 1;
		RegisterDataType SessionRequest										: 1;
		RegisterDataType Resume												: 1;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void ReceiveFifoNonEmptyInterruptEnable(bool enable = true)
	{
		RegRef::_register->ReceiveFifoNonEmpty = enable;
	}
	
	inline static bool ReceiveFifoNonEmptyInterruptEnabled()
	{
		return RegRef::_register->ReceiveFifoNonEmpty;
	}
	
	inline static void OtgInterruptEnable(bool enable = true)
	{
		RegRef::_register->Otg = enable;
	}
	
	inline static void NonPeriodicTxFifoEmptyInterruptEnable(bool enable = true)
	{
		RegRef::_register->NonPeriodicTxFifoEmpty = enable;
	}
	
	inline static bool NonPeriodicTxFifoEmptyInterruptEnabled()
	{
		return RegRef::_register->NonPeriodicTxFifoEmpty;
	}
	
	inline static void ModeMismatchInterruptEnable(bool enable = true)
	{
		RegRef::_register->ModeMismatch = enable;
	}
	
	inline static void HostPortInterruptEnable(bool enable = true)
	{
		RegRef::_register->HostPort = enable;
	}
	
	inline static bool HostPortInterruptEnabled()
	{
		return RegRef::_register->HostPort;
	}
	
	inline static void HostChannelsInterruptEnable(bool enable = true)
	{
		RegRef::_register->HostChannels = enable;
	}
	
	inline static bool HostChannelsInterruptEnabled()
	{
		return RegRef::_register->HostChannels;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbReadStatusRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType ChannelNumber		: 4;
		RegisterDataType ByteCount			: 11;
		RegisterDataType DataPid			: 2;
		RegisterDataType PacketStatus		: 4;
		RegisterDataType Reserved0			: 11;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static unsigned char GetReceivedChannelNumber()
	{
		return RegRef::_register->ChannelNumber;
	}
	inline static unsigned short GetReceivedByteCount()
	{
		return RegRef::_register->ByteCount;
	}
	inline static unsigned char GetReceivedDataPid()
	{
		return RegRef::_register->DataPid;
	}
	inline static unsigned char GetReceivedPacketStatus()
	{
		return RegRef::_register->PacketStatus;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbPopReadStatusRegister
{
protected:
	
	typedef RegisterReference<unsigned int, RegisterAddr> RegRef;
public:
	inline static unsigned int PopReceiveFifo()
	{
		return *RegRef::_register;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbReceiveFifoSizeRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType FifoDepth		: 16;
		RegisterDataType Reserved0		: 16;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void SetReceiveFifoSize(unsigned short dwordsCount)
	{
		RegRef::_register->FifoDepth = dwordsCount;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbNonPeriodicTransmitFifoSizeRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType StartAddress	: 16;
		RegisterDataType FifoDepth		: 16;
		
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void SetNonPeriodicTransmitFifoSize(unsigned short startAddress, unsigned short dwordsCount)
	{
		RegRef::_register->StartAddress = startAddress;
		RegRef::_register->FifoDepth = dwordsCount;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbGeneralCoreConfigurationRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Reserved0						: 16;
		RegisterDataType PowerDown						: 1;
		RegisterDataType Reserved1						: 1;
		RegisterDataType EnableVbusSensingDeviceA		: 1;
		RegisterDataType EnableVbusSensingDeviceB		: 1;
		RegisterDataType SartOfFrameOutputEnable		: 1;
		RegisterDataType Reserved2						: 11;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void SetPowerDown(bool enable = true)
	{
		RegRef::_register->PowerDown = !enable;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbPeriodicTransmitFifoSizeRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType StartAddress	: 16;
		RegisterDataType FifoDepth		: 16;
		
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void SetPeriodicTransmitFifoSize(unsigned short startAddress, unsigned short dwordsCount)
	{
		RegRef::_register->StartAddress = startAddress;
		RegRef::_register->FifoDepth = dwordsCount;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbHostConfigurationRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType ClockSelect		: 2;
		RegisterDataType FsSupport			: 1;
		RegisterDataType Reserved0			: 29;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void SetClockSpeed(unsigned char value)
	{
		RegRef::_register->ClockSelect = value;
	}
	
	inline static bool IsFullSpeedSupport()
	{
		return RegRef::_register->FsSupport;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbHostChannelsInterruptRegister
{
protected:
	typedef RegisterReference<unsigned int, RegisterAddr> RegRef;
public:
	inline static bool IsChannelInterrupt(unsigned char channel)
	{
		if (channel > 15)
		{
			return false;
		}
		
		unsigned int mask = (1 << channel);
				
		return (*RegRef::_register & mask) == mask;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbHostChannelsInterruptMaskRegister
{
protected:
	typedef RegisterReference<unsigned int, RegisterAddr> RegRef;
public:
	inline static void ChannelInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 15)
		{
			return;
		}
		
		unsigned int mask = (1 << channel);
		
		if (enable)
		{
			*RegRef::_register |= mask;
		}
		else
		{
			*RegRef::_register &= ~mask;
		}
	}
	
	inline static bool ChannelInterruptEnabled(unsigned char channel)
	{
		if (channel > 15)
		{
			return false;
		}
		
		unsigned int mask = (1 << channel);
				
		return (*RegRef::_register & mask) == mask;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbHostPortControlAndStatusRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType ConnectedStatus			: 1;	// PCSTS
		RegisterDataType ConnectDetected			: 1;
		RegisterDataType PortEnabled				: 1;
		RegisterDataType PortEnableChanged			: 1;
		RegisterDataType PortOvercurrentActive		: 1;
		RegisterDataType PortOvercurrentChange		: 1;
		RegisterDataType PortResume					: 1;
		RegisterDataType PortSuspend				: 1;
		RegisterDataType PortReset					: 1;
		RegisterDataType Reserved0					: 1;
		RegisterDataType PortLineStatus				: 2;
		RegisterDataType PortPower					: 1;
		RegisterDataType PortTestControl			: 4;
		RegisterDataType PortSpeed					: 2;
		RegisterDataType Reserved1					: 13;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
	typedef RegisterReference<unsigned int, RegisterAddr> RegWriteRef;
public:
	inline static bool IsConnected()
	{
		return RegRef::_register->ConnectedStatus;
	}
	
	inline static bool IsConnectDetected()
	{
		return RegRef::_register->ConnectDetected;
	}
	
	inline static void ResetConnectDetected()
	{
		RegisterStruct<unsigned int> tmp = SafeRead();
		tmp.ConnectDetected = 1;
		SafeWrite(tmp);
	}
	
	inline static bool IsPortEnableChanged()
	{
		return RegRef::_register->PortEnableChanged;
	}
	
	inline static void ResetPortEnableChanged()
	{
		RegisterStruct<unsigned int> tmp = SafeRead();
		tmp.PortEnableChanged = 1;
		SafeWrite(tmp);
	}
	
	inline static void SetPortReset(bool enable = true)
	{
		RegisterStruct<unsigned int> tmp = SafeRead();
		tmp.PortReset = enable;
		SafeWrite(tmp);
	}
	
	inline static void SetPortPower(bool enable = true)
	{
		RegisterStruct<unsigned int> tmp = SafeRead();
		tmp.PortPower = enable;
		SafeWrite(tmp);
	}
	
	inline static bool IsFullSpeed()
	{
		return RegRef::_register->PortSpeed == 1;
	}
protected:
	inline static RegisterStruct<unsigned int> SafeRead()
	{
		unsigned int val;
		val = *RegWriteRef::_register;
		
		RegisterStruct<unsigned int> *res = (RegisterStruct<unsigned int> *)&val;
		
		res->ConnectDetected = 0;
		res->PortEnabled = 0;
		res->PortEnableChanged = 0;
		res->PortOvercurrentChange = 0;
		
		return *res;
	}
	
	inline static void SafeWrite(RegisterStruct<unsigned int> val)
	{
		unsigned int *tmp = (unsigned int *)&val;
		*RegWriteRef::_register = *tmp;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbHostChannelCharacteristicsRegister
{
protected:
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType MaximumPacketSize		: 11;
		RegisterDataType EndpointNumber			: 4;
		RegisterDataType EndpointDirection		: 1;
		RegisterDataType Reserved0				: 1;
		RegisterDataType LowSpeedDevice			: 1;
		RegisterDataType EndpointType			: 2;
		RegisterDataType Multicount				: 2;
		RegisterDataType DeviceAddress			: 7;
		RegisterDataType OddFrame				: 1;
		RegisterDataType ChannelDisable			: 1;
		RegisterDataType ChannelEnable			: 1;
			
		RegisterDataType Pad[0x20 / sizeof(RegisterDataType) - 1];
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void ChannelSetMaxPacketSize(unsigned char channel, unsigned short value)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].MaximumPacketSize = value;
	}
	
	inline static void ChannelSetEndpointNumber(unsigned char channel, unsigned char value)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].EndpointNumber = value;
	}
	
	inline static void ChannelSetEndpointAsInput(unsigned char channel, bool value)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].EndpointDirection = value;
	}
	
	inline static void ChannelSetEndpointType(unsigned char channel, unsigned char value)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].EndpointType = value;
	}
	
	inline static void ChannelSetEnabled(unsigned char channel, bool value)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].ChannelEnable = value;
	}
	
	inline static void ChannelHalt(unsigned char channel)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].ChannelDisable = 1;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbHostChannelInterruptRegister
{
protected:
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType TransferCompleted		: 1;
		RegisterDataType ChannelHalted			: 1;
		RegisterDataType Reserved0				: 1;
		RegisterDataType Stall					: 1;
		RegisterDataType Nak					: 1;
		RegisterDataType Ack					: 1;
		RegisterDataType ResponseReceived		: 1;
		RegisterDataType TransactionError		: 1;
		RegisterDataType BabbleError			: 1;
		RegisterDataType FrameOverrun			: 1;
		RegisterDataType DataToggleError		: 1;
		RegisterDataType Reserved1				: 21;
			
		RegisterDataType Pad[0x20 / sizeof(RegisterDataType) - 1];
	};
	
	template<class RegisterDataType>
	struct RegisterStructReadWrite
	{
		RegisterDataType Value;
		RegisterDataType Pad[0x20 / sizeof(RegisterDataType) - 1];
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
	typedef RegisterReference<RegisterStructReadWrite<unsigned int>, RegisterAddr> RegWriteRef;
public:
	inline static bool ChannelIsTransferCompletedInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].TransferCompleted;
	}
	
	inline static void ChannelResetTransferCompletedInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return;
		}
		
		RegisterStruct<unsigned int> tmp = SafeRead(channel);
		tmp.TransferCompleted = 1;
		SafeWrite(channel, tmp);
	}
	
	inline static bool ChannelIsHaltedInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].ChannelHalted;
	}
	
	inline static void ChannelResetHaltedInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return;
		}
		
		RegisterStruct<unsigned int> tmp = SafeRead(channel);
		tmp.ChannelHalted = 1;
		SafeWrite(channel, tmp);
	}
	
	inline static bool ChannelIsStallInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].Stall;
	}
	
	inline static void ChannelResetStallInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return;
		}
		
		RegisterStruct<unsigned int> tmp = SafeRead(channel);
		tmp.Stall = 1;
		SafeWrite(channel, tmp);
	}
	
	inline static bool ChannelIsNakInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].Nak;
	}
	
	inline static void ChannelResetNakInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return;
		}
		
		RegisterStruct<unsigned int> tmp = SafeRead(channel);
		tmp.Nak = 1;
		SafeWrite(channel, tmp);
	}
	
	inline static bool ChannelIsAckInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].Ack;
	}
	
	inline static void ChannelResetAckInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return;
		}
		
		RegisterStruct<unsigned int> tmp = SafeRead(channel);
		tmp.Ack = 1;
		SafeWrite(channel, tmp);
	}
	
	inline static bool ChannelIsResponseReceivedInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].ResponseReceived;
	}
	
	inline static bool ChannelIsTransactionErrorInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].TransactionError;
	}
	
	inline static void ChannelResetTransactionErrorInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return;
		}
		
		RegisterStruct<unsigned int> tmp = SafeRead(channel);
		tmp.TransactionError = 1;
		SafeWrite(channel, tmp);
	}
	
	inline static bool ChannelIsBabbleErrorInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].BabbleError;
	}
	
	inline static bool ChannelIsFrameOverrunInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].FrameOverrun;
	}
	
	inline static void ChannelResetFrameOverrunInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return;
		}
		
		RegisterStruct<unsigned int> tmp = SafeRead(channel);
		tmp.FrameOverrun = 1;
		SafeWrite(channel, tmp);
	}
	
	inline static bool ChannelIsDataToggleErrorInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].DataToggleError;
	}
	
	inline static void ChannelResetDataToggleErrorInterrupt(unsigned char channel)
	{
		if (channel > 7)
		{
			return;
		}
		
		RegisterStruct<unsigned int> tmp = SafeRead(channel);
		tmp.DataToggleError = 1;
		SafeWrite(channel, tmp);
	}
protected:
	inline static RegisterStruct<unsigned int> SafeRead(unsigned char channel)
	{
		RegisterStruct<unsigned int> res;
		memset(&res, 0, sizeof(unsigned int));
		
		return res;
	}
	
	inline static void SafeWrite(unsigned char channel, RegisterStruct<unsigned int> val)
	{
		unsigned int *tmp = (unsigned int *)&val;
		RegWriteRef::_register[channel].Value = *tmp;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbHostChannelInterruptMaskRegister
{
protected:
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType TransferCompleted		: 1;
		RegisterDataType ChannelHalted			: 1;
		RegisterDataType Reserved0				: 1;
		RegisterDataType Stall					: 1;
		RegisterDataType Nak					: 1;
		RegisterDataType Ack					: 1;
		RegisterDataType ResponseReceived		: 1;
		RegisterDataType TransactionError		: 1;
		RegisterDataType BabbleError			: 1;
		RegisterDataType FrameOverrun			: 1;
		RegisterDataType DataToggleError		: 1;
		RegisterDataType Reserved1				: 21;
			
		RegisterDataType Pad[0x20 / sizeof(RegisterDataType) - 1];
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void ChannelTransferCompletedInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].TransferCompleted = enable;
	}
	
	inline static bool ChannelTransferCompletedInterruptEnabled(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].TransferCompleted;
	}
	
	inline static void ChannelHaltedInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].ChannelHalted = enable;
	}
	
	inline static void ChannelStallInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].Stall = enable;
	}
	
	inline static bool ChannelStallInterruptEnabled(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].Stall;
	}
	
	inline static void ChannelNakInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].Nak = enable;
	}
	
	inline static bool ChannelNakInterruptEnabled(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].Nak;
	}
	
	inline static void ChannelAckInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].Ack = enable;
	}
	
	inline static bool ChannelAckInterruptEnabled(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].Ack;
	}
	
	inline static void ChannelResponseReceivedInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].ResponseReceived = enable;
	}
	
	inline static bool ChannelResponseReceivedInterruptEnabled(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].ResponseReceived;
	}
	
	inline static void ChannelTransactionErrorInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].TransactionError = enable;
	}
	
	inline static bool ChannelTransactionErrorInterruptEnabled(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].TransactionError;
	}
	
	inline static void ChannelBabbleErrorInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].BabbleError = enable;
	}
	
	inline static bool ChannelBabbleErrorInterruptEnabled(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].BabbleError;
	}
	
	inline static void ChannelFrameOverrunInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].FrameOverrun = enable;
	}
	
	inline static bool ChannelFrameOverrunInterruptEnabled(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].FrameOverrun;
	}
	
	inline static void ChannelDataToggleErrorInterruptEnable(unsigned char channel, bool enable = true)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].DataToggleError = enable;
	}
	
	inline static bool ChannelDataToggleErrorInterruptEnabled(unsigned char channel)
	{
		if (channel > 7)
		{
			return false;
		}
		return RegRef::_register[channel].DataToggleError;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbHostChannelTransferSizeRegister
{
protected:
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType TransferSize			: 19;
		RegisterDataType PacketCount			: 10;
		RegisterDataType DataPid				: 2;
		RegisterDataType Reserved0				: 1;
			
		RegisterDataType Pad[0x20 / sizeof(RegisterDataType) - 1];
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void ChannelSetTransferSize(unsigned char channel, unsigned short value)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].TransferSize = value;
	}
	
	inline static void ChannelSetTransferPacketCount(unsigned char channel, unsigned short value)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].PacketCount = value;
	}
	
	inline static void ChannelSetTransferDataPid(unsigned char channel, unsigned char value)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].DataPid = value;
	}
};

template<RegisterPointerType RegisterAddr>
class UsbHostChannelFifoRegister
{
protected:
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType Data;
			
		RegisterDataType Pad[0x1000 / sizeof(RegisterDataType) - 1];
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void ChannelWrite(unsigned char channel, unsigned int value)
	{
		if (channel > 7)
		{
			return;
		}
		RegRef::_register[channel].Data = value;
	}
	
	inline static unsigned int ChannelRead(unsigned char channel)
	{
		if (channel > 7)
		{
			return 0;
		}
		return RegRef::_register[channel].Data;
	}
};

#endif