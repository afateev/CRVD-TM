///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ����������� �������
//	MSP430F5XXX
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPECIALFUNCTIONREGISTERS_MSP430F5XXX_H
#define SPECIALFUNCTIONREGISTERS_MSP430F5XXX_H

namespace Rblib
{
namespace Msp430f5xxx
{
	// ��������� �������� ���������� ����������
	template<class RegisterDataType>
	struct SfrInterruptEnableRegister
	{
		RegisterDataType Watchdog						: 1;	// ��������� ���������� ���������� �������
		RegisterDataType OscillatorFault				: 1;	// 
		RegisterDataType Reserved0						: 1;	// 
		RegisterDataType VacantMemoryAccess				: 1;	// 
		RegisterDataType NmiPin							: 1;	// 
		RegisterDataType FlashControllerAccessViolation	: 1;	// 
		RegisterDataType JtagMailboxInput				: 1;	// 
		RegisterDataType JtagMailboxOutput				: 1;	// 
		RegisterDataType Reserved1						: 8;	// 
	};

	// ��������� �������� ������ ���������� (SFRIFG1)
	template<class RegisterDataType>
	struct SfrInterruptFlagRegister
	{
		RegisterDataType Watchdog			: 1;	// ���� ������������ Wdt (WDTIFG)
		RegisterDataType OscillatorFault	: 1;	// OFIFG
		RegisterDataType Reserved0			: 1;	// 
		RegisterDataType VacantMemoryAccess	: 1;	// VMAIFG
		RegisterDataType NmiPin				: 1;	// NMIIFG
		RegisterDataType Reserved1			: 1;	// JMBINIFG
		RegisterDataType JtagMailboxInput	: 1;	// JMBOUTIFG
		RegisterDataType JtagMailboxOutput	: 1;	// JMBOUTIFG
		RegisterDataType Reserved2			: 8;	// 
	};

	// ��������� �������� ���������� ������� ������
	template<class RegisterDataType>
	struct SfrResetPinControlRegister
	{
		RegisterDataType NmiSelect				: 1;	// 
		RegisterDataType NmiEdgeSelect			: 1;	// 
		RegisterDataType ResetResistorPinPullup	: 1;	// 
		RegisterDataType ResetPinResistorEnable	: 1;	// 
		RegisterDataType Reserved				: 12;	// 
	};

	// ������� ���������� ����������
	template <class RegisterDataType,				// ��� (������) �������� ��������
			class PointerType,						// ��� (������) �������� ������� ���������
			PointerType RegisterAddr				// ����� ��������
		>
	class InterruptEnableRegister
	{
	protected:
		static volatile SfrInterruptEnableRegister<RegisterDataType> * const _registerAddr;
	public:

		static inline void EnableOscillatorFault()
		{
			_registerAddr->OscillatorFault = 1;
		}

		static inline void DisableOscillatorFault()
		{
			_registerAddr->OscillatorFault = 0;
		}

	};

	// ������� ������ ����������
	template <class RegisterDataType,				// ��� (������) �������� ��������
			class PointerType,						// ��� (������) �������� ������� ���������
			PointerType RegisterAddr				// ����� ��������
		>
	class InterruptFlagRegister
	{
	protected:
		static volatile SfrInterruptFlagRegister<RegisterDataType> * const _registerAddr;
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
		static inline void ClearOscillatorFault()
		{
			_registerAddr->OscillatorFault = 0;
		}

	};

	// ������� ���������� ������� ������
	template <class RegisterDataType,				// ��� (������) �������� ��������
			class PointerType,						// ��� (������) �������� ������� ���������
			PointerType RegisterAddr				// ����� ��������
		>
	class ResetPinControlRegister
	{
	protected:
		static volatile SfrResetPinControlRegister<RegisterDataType> * const _registerAddr;
	};

	// ��������������� ����������� ����������
	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  SfrInterruptEnableRegister<RegisterDataType>* const InterruptEnableRegister<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile SfrInterruptEnableRegister<RegisterDataType>*>(RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  SfrInterruptFlagRegister<RegisterDataType>* const InterruptFlagRegister<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile SfrInterruptFlagRegister<RegisterDataType>*>(RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  SfrResetPinControlRegister<RegisterDataType>* const ResetPinControlRegister<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile SfrResetPinControlRegister<RegisterDataType>*>(RegisterAddr);
}
}

#endif