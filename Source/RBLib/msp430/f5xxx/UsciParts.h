///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������� ������� ������� USCI
//	MSP430F5XXX
//
//	������ ������ ������ ��������� �� ������ ������������� ��� �� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef USCI_PARTS_MSP430F5XXX_H
#define USCI_PARTS_MSP430F5XXX_H

namespace Rblib
{
// ����� USCI ����������� ��� ��������� MSP430 ��������� f2xxx
namespace Msp430f5xxx
{
	//////////////////////////////////////////////////////////////
	// ������� �������
	//////////////////////////////////////////////////////////////
	
	// ��������� �������� �������
	// � ������ I2C
	template<class RegisterDataType>
	struct UsciStatusRegisterI2C
	{
		RegisterDataType Reserved0			: 4;	// 
		RegisterDataType BusBusy			: 1;	// 
		RegisterDataType GeneralCall		: 1;	// 
		RegisterDataType SCLLow				: 1;	// 
		RegisterDataType Reserved1			: 1;	// 
	};
	
	// ��������� �������� �������
	// � ������ SPI
	template<class RegisterDataType>
	struct UsciStatusRegisterSpi
	{
		RegisterDataType Busy			: 1;	// 
		RegisterDataType Reserved		: 4;	// 
		RegisterDataType OverrunError	: 1;	// 
		RegisterDataType FramingError	: 1;	// 
		RegisterDataType Listen			: 1;	// 
	};
	
	//////////////////////////////////////////////////////////////
	// ������� ���������� ������������
	//////////////////////////////////////////////////////////////
	
	// ��������� �������� ���������� ����������
	// � ������ Uart
	template<class RegisterDataType>
	struct UsciInterruptEnableRegisterUart
	{
		RegisterDataType Rx			: 1;	// 
		RegisterDataType Tx			: 1;	// 
		RegisterDataType Reserved	: 6;	// 
	};

	// ��������� �������� ���������� ����������
	// � ������ I2C
	template<class RegisterDataType>
	struct UsciInterruptEnableRegisterI2C
	{
		RegisterDataType Rx					: 1;	// 
		RegisterDataType Tx					: 1;	// 
		RegisterDataType Start				: 1;	// 
		RegisterDataType Stop				: 1;	// 
		RegisterDataType ArbitrationLost	: 1;	// 
		RegisterDataType Nack				: 1;	// 
		RegisterDataType Reserved			: 2;	// 
	};

	// ��������� �������� ������ ����������
	// � ������ Uart
	template<class RegisterDataType>
	struct UsciInterruptFlagRegisterUart
	{
		RegisterDataType Rx			: 1;	// 
		RegisterDataType Tx			: 1;	// 
		RegisterDataType Reserved	: 6;	// 
	};

	// ��������� �������� ������ ����������
	// � ������ I2C
	template<class RegisterDataType>
	struct UsciInterruptFlagRegisterI2C
	{
		RegisterDataType Rx					: 1;	// 
		RegisterDataType Tx					: 1;	// 
		RegisterDataType Start				: 1;	// 
		RegisterDataType Stop				: 1;	// 
		RegisterDataType ArbitrationLost	: 1;	// 
		RegisterDataType Nack				: 1;	// 
		RegisterDataType Reserved			: 2;	// 
	};
	
	// ��������� �������� ������ ����������
	// � ������ SPI
	template<class RegisterDataType>
	struct UsciInterruptFlagRegisterSpi
	{
		RegisterDataType Rx			: 1;	// 
		RegisterDataType Tx			: 1;	// 
		RegisterDataType Reserved	: 6;	//
	};

	template <class RegisterDataType,					// ��� (������) �������� ��������
			class PointerType,							// ��� (������) �������� ������� ���������
			PointerType InterruptEnableRegisterAddr,	// ����� �������� ���������� ����������
			PointerType InterruptFlagRegisterAddr,		// ����� �������� ������ ����������
			class InterruptEnableRegisterStructure,		// ��������� �������� ���������� ����������
			class InterruptFlagRegisterStructure		// ��������� �������� ������ ����������
		>
	class InterruptControl
	{
	protected:
		// ������ ��������� ����������
		static volatile InterruptEnableRegisterStructure * const _interruptEnableRegisterAddr;
		static volatile InterruptFlagRegisterStructure * const _interruptFlagRegisterAddr;
	public:
		// ����� ��� ���� ������� USCI
		static inline void SetRxInterruptEnable(bool value)
		{
			_interruptEnableRegisterAddr->Rx = value;
		}
		static inline void SetTxInterruptEnable(bool value)
		{
			_interruptEnableRegisterAddr->Tx = value;
		}
		static inline bool GetRxInterruptFlag()
		{
			return _interruptFlagRegisterAddr->Rx;
		}
		static inline bool GetTxInterruptFlag()
		{
			return _interruptFlagRegisterAddr->Tx;
		}
		static inline void ClearRxInterruptFlag()
		{
			_interruptFlagRegisterAddr->Rx = 0;
		}
		static inline void ClearTxInterruptFlag()
		{
			_interruptFlagRegisterAddr->Tx = 0;
		}

		// ����������� ��� I2C
		static inline void SetStartInterruptEnable(bool value)
		{
			_interruptEnableRegisterAddr->Start = value;
		}
		static inline void SetStopInterruptEnable(bool value)
		{
			_interruptEnableRegisterAddr->Stop = value;
		}
		static inline void SetArbitrationLostInterruptEnable(bool value)
		{
			_interruptEnableRegisterAddr->ArbitrationLost = value;
		}
		static inline void SetNackInterruptEnable(bool value)
		{
			_interruptEnableRegisterAddr->Nack = value;
		}
		static inline bool GetStartInterruptFlag()
		{
			return _interruptFlagRegisterAddr->Start;
		}
		static inline bool GetStopInterruptFlag()
		{
			return _interruptFlagRegisterAddr->Stop;
		}
		static inline bool GetArbitrationLostInterruptFlag()
		{
			return _interruptFlagRegisterAddr->ArbitrationLost;
		}
		static inline bool GetNackInterruptFlag()
		{
			return _interruptFlagRegisterAddr->Nack;
		}
		static inline void ClearStartInterruptFlag()
		{
			_interruptFlagRegisterAddr->Start = 0;
		}
		static inline void ClearStopInterruptFlag()
		{
			_interruptFlagRegisterAddr->Stop = 0;
		}
		static inline void ClearArbitrationLostInterruptFlag()
		{
			_interruptFlagRegisterAddr->ArbitrationLost = 0;
		}
		static inline void ClearNackInterruptFlag()
		{
			_interruptFlagRegisterAddr->Nack = 0;
		}
	};

	// ��������������� ����������� ����������
	template <class RegisterDataType, class PointerType, PointerType InterruptEnableRegisterAddr, PointerType InterruptFlagRegisterAddr, class InterruptEnableRegisterStructure, class InterruptFlagRegisterStructure>
	volatile InterruptEnableRegisterStructure * const InterruptControl<RegisterDataType, PointerType, InterruptEnableRegisterAddr, InterruptFlagRegisterAddr, InterruptEnableRegisterStructure, InterruptFlagRegisterStructure>
		::_interruptEnableRegisterAddr = reinterpret_cast<volatile  InterruptEnableRegisterStructure*>(InterruptEnableRegisterAddr);

	template <class RegisterDataType, class PointerType, PointerType InterruptEnableRegisterAddr, PointerType InterruptFlagRegisterAddr, class InterruptEnableRegisterStructure, class InterruptFlagRegisterStructure>
	volatile InterruptFlagRegisterStructure * const InterruptControl<RegisterDataType, PointerType, InterruptEnableRegisterAddr, InterruptFlagRegisterAddr, InterruptEnableRegisterStructure, InterruptFlagRegisterStructure>
		::_interruptFlagRegisterAddr = reinterpret_cast<volatile  InterruptFlagRegisterStructure*>(InterruptFlagRegisterAddr);
}
}

#endif