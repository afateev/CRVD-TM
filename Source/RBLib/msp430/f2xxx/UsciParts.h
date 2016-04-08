///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������� ������� ������� USCI
//	MSP430F2XXX
//
//	������ ������ ������ ��������� �� ������ ������������� ��� �� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef USCI_PARTS_MSP430F2XXX_H
#define USCI_PARTS_MSP430F2XXX_H

namespace Rblib
{
// ����� USCI ����������� ��� ��������� MSP430 ��������� f2xxx
namespace Msp430f2xxx
{
	//////////////////////////////////////////////////////////////
	// ������� ���������� ���������� I2C
	//////////////////////////////////////////////////////////////

	// ��������� �������� ���������� ���������� I2C
	template<class RegisterDataType>
	struct UsciI2CInterruptEnableRegister
	{
		RegisterDataType ArbitrationLost	: 1;	// 
		RegisterDataType Start				: 1;	// 
		RegisterDataType Stop				: 1;	// 
		RegisterDataType Nack				: 1;	// 
		RegisterDataType Reserved			: 4;	// 
	};

	// ��������� �������� �������
	// � ������ I2C
	template<class RegisterDataType>
	struct UsciStatusRegisterI2C
	{
		RegisterDataType ArbitrationLost	: 1;	// 
		RegisterDataType Start				: 1;	// 
		RegisterDataType Stop				: 1;	// 
		RegisterDataType Nack				: 1;	// 
		RegisterDataType BusBusy			: 1;	// 
		RegisterDataType GeneralCall		: 1;	// 
		RegisterDataType SCLLow				: 1;	// 
		RegisterDataType Reserved			: 1;	// 
	};
	
	//////////////////////////////////////////////////////////////
	// ���������� ������������
	//////////////////////////////////////////////////////////////

	// ����� ���������� ������������ ���������� USCI_A0
	template<class Sfr>
	class InterruptControlA0
	{
	public:
		static inline void SetRxInterruptEnable(bool value)
		{
			Sfr::SetInterruptEnableA0Rx(value);
		}
		static inline void SetTxInterruptEnable(bool value)
		{
			Sfr::SetInterruptEnableA0Tx(value);
		}
		static inline bool GetRxInterruptFlag()
		{
			return Sfr::GetInterruptA0RxFlag();
		}
		static inline bool GetTxInterruptFlag()
		{
			return Sfr::GetInterruptA0TxFlag();
		}
		static inline void ClearRxInterruptFlag()
		{
			Sfr::ClearInterruptA0RxFlag();
		}
		static inline void ClearTxInterruptFlag()
		{
			Sfr::ClearInterruptA0TxFlag();
		}
	};

	// ����� ���������� ������������ ���������� USCI_A1
	template<class Sfr>
	class InterruptControlA1
	{
	public:
		static inline void SetRxInterruptEnable(bool value)
		{
			Sfr::SetInterruptEnableA1Rx(value);
		}
		static inline void SetTxInterruptEnable(bool value)
		{
			Sfr::SetInterruptEnableA1Tx(value);
		}
		static inline bool GetRxInterruptFlag()
		{
			return Sfr::GetInterruptA1RxFlag();
		}
		static inline bool GetTxInterruptFlag()
		{
			return Sfr::GetInterruptA1TxFlag();
		}
		static inline void ClearRxInterruptFlag()
		{
			Sfr::ClearInterruptA1RxFlag();
		}
		static inline void ClearTxInterruptFlag()
		{
			Sfr::ClearInterruptA1TxFlag();
		}
	};

	// ����� ���������� ������������ ���������� USCI_B0
	template<class Sfr>
	class InterruptControlB0
	{
	public:
		static inline void SetRxInterruptEnable(bool value)
		{
			Sfr::SetInterruptEnableB0Rx(value);
		}
		static inline void SetTxInterruptEnable(bool value)
		{
			Sfr::SetInterruptEnableB0Tx(value);
		}
		static inline bool GetRxInterruptFlag()
		{
			return Sfr::GetInterruptB0RxFlag();
		}
		static inline bool GetTxInterruptFlag()
		{
			return Sfr::GetInterruptB0TxFlag();
		}
		static inline void ClearRxInterruptFlag()
		{
			Sfr::ClearInterruptB0RxFlag();
		}
		static inline void ClearTxInterruptFlag()
		{
			Sfr::ClearInterruptB0TxFlag();
		}
	};

	// ����� ���������� ������������ ���������� USCI_B1
	template<class Sfr>
	class InterruptControlB1
	{
	public:
		static inline void SetRxInterruptEnable(bool value)
		{
			Sfr::SetInterruptEnableB1Rx(value);
		}
		static inline void SetTxInterruptEnable(bool value)
		{
			Sfr::SetInterruptEnableB1Tx(value);
		}
		static inline bool GetRxInterruptFlag()
		{
			return Sfr::GetInterruptB1RxFlag();
		}
		static inline bool GetTxInterruptFlag()
		{
			return Sfr::GetInterruptB1TxFlag();
		}
		static inline void ClearRxInterruptFlag()
		{
			Sfr::ClearInterruptB1RxFlag();
		}
		static inline void ClearTxInterruptFlag()
		{
			Sfr::ClearInterruptB1TxFlag();
		}
	};

	////////////////////////////////////////////////////
	// ������� ���������� ���������� I2C
	////////////////////////////////////////////////////
	template <class RegisterDataType,			// ��� (������) �������� ��������
			class PointerType,					// ��� (������) �������� ������� ���������
			PointerType I2CInterruptEnableRegisterAddr,	// ����� ��������
			class I2CInterruptEnableRegisterStructure	// ��������� ��������
		>
	class UsciI2CInterruptEnable
	{
	protected:
		// ������ ��������� ����������
		static volatile I2CInterruptEnableRegisterStructure * const _I2CInterruptEnableRegisterAddr;
	public:
		static inline void SetStartInterruptEnable(bool value)
		{
			_I2CInterruptEnableRegisterAddr->Start = value;
		}
		static inline void SetStopInterruptEnable(bool value)
		{
			_I2CInterruptEnableRegisterAddr->Stop = value;
		}
		static inline void SetArbitrationLostInterruptEnable(bool value)
		{
			_I2CInterruptEnableRegisterAddr->ArbitrationLost = value;
		}
		static inline void SetNackInterruptEnable(bool value)
		{
			_I2CInterruptEnableRegisterAddr->Nack = value;
		}
	};

	// ��������������� ����������� ����������
	template <class RegisterDataType, class PointerType, PointerType I2CInterruptEnableRegisterAddr, class I2CInterruptEnableRegisterStructure>
	volatile I2CInterruptEnableRegisterStructure * const UsciI2CInterruptEnable<RegisterDataType, PointerType, I2CInterruptEnableRegisterAddr, I2CInterruptEnableRegisterStructure>
		::_I2CInterruptEnableRegisterAddr = reinterpret_cast<volatile  I2CInterruptEnableRegisterStructure*>(I2CInterruptEnableRegisterAddr);
	
	// ����� ���������� ������������ I2C
	// ����������� ����� ���������� �� ��������� SFR, �������� ���������� ���������� I2C � �������� �������
	template <class UsciInterruptControl,
			class I2CInterruptEnable,
			class Status
		>
	class InterruptControlI2C :
		public UsciInterruptControl,
		public I2CInterruptEnable,
		public Status
	{
	public:
		static inline bool GetStartInterruptFlag()
		{
			return Status::_statusRegisterAddr->Start;
		}
		static inline bool GetStopInterruptFlag()
		{
			return Status::_statusRegisterAddr->Stop;
		}
		static inline bool GetArbitrationLostInterruptFlag()
		{
			return Status::_statusRegisterAddr->ArbitrationLost;
		}
		static inline bool GetNackInterruptFlag()
		{
			return Status::_statusRegisterAddr->Nack;
		}
		static inline void ClearStartInterruptFlag()
		{
			Status::_statusRegisterAddr->Start = 0;
		}
		static inline void ClearStopInterruptFlag()
		{
			Status::_statusRegisterAddr->Stop = 0;
		}
		static inline void ClearArbitrationLostInterruptFlag()
		{
			Status::_statusRegisterAddr->ArbitrationLost = 0;
		}
		static inline void ClearNackInterruptFlag()
		{
			Status::_statusRegisterAddr->Nack = 0;
		}
	};
}
}

#endif