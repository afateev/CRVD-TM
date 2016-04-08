///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������� ������������ �������
//	MSP430F5XXX
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSCONFIGURATIONREGISTERS_MSP430F5XXX_H
#define SYSCONFIGURATIONREGISTERS_MSP430F5XXX_H

namespace Rblib
{
namespace Msp430f5xxx
{
	// ������� ������� ���������� �� ������
	template <class RegisterDataType,		// ��� (������) �������� ��������
			class PointerType,				// ��� (������) �������� ������� ���������
			PointerType RegisterAddr		// ����� ��������
		>
	class SysResetInterruptVector
	{
	public:
		// ������ ������ ������������
		enum ResetReason
		{
			ResetReasonUnknown,
			ResetReasonBrownout,
			ResetReasonRstNmi,
			ResetReasonPMMSWBOR,
			ResetReasonWakeupFromLpmX5,
			ResetReasonSecurityViolation,
			ResetReasonSVSL,
			ResetReasonSVSH,
			ResetReasonSVML_OVP,
			ResetReasonSVMH_OVP,
			ResetReasonPMMSWPOR,
			ResetReasonWdtTimeOut,
			ResetReasonWdtPasswordViolation,
			ResetReasonFlashPasswordViolation,
			ResetReasonPLLUnlock,
			ResetReasonPERFPeripheralConfigurationAreaFetch,
			ResetReasonPMMPasswordViolation,
			ResetReasonReserved
		};
	protected:	
		// ����� ��������
		static volatile RegisterDataType * const _registerAddr;

		// ��������� ������� ������
		static ResetReason _lastResetReason;
	public:
		// �������� ��������� ������� ������
		static inline ResetReason GetLastResetReason()
		{
			// ���� �������� ������ ���������� ��������,
			// �.�. ��������� ��� ����� ����� ���� ���, ����� �� ����� ������
			RegisterDataType lastRegisterValue = 0;
			// �.�. ������ ������������ ����� ���������� �����,
			// �� ������ ���� ������� ���� � ��������� ���������
			do
			{
				// ������
				lastRegisterValue = *_registerAddr;
				// ���� ���� �������
				if (lastRegisterValue)
				{
					// ����������� �������� � �������� �������
					// ���������� ���
					switch(lastRegisterValue)
					{
					case 0x02:
						_lastResetReason = ResetReasonBrownout;
					break;
					case 0x04:
						_lastResetReason = ResetReasonRstNmi;
					break;
					case 0x06:
						_lastResetReason = ResetReasonPMMSWBOR;
					break;
					case 0x08:
						_lastResetReason = ResetReasonWakeupFromLpmX5;
					break;
					case 0x0A:
						_lastResetReason = ResetReasonSecurityViolation;
					break;
					case 0x0C:
						_lastResetReason = ResetReasonSVSL;
					break;
					case 0x0E:
						_lastResetReason = ResetReasonSVSH;
					break;
					case 0x10:
						_lastResetReason = ResetReasonSVML_OVP;
					break;
					case 0x12:
						_lastResetReason = ResetReasonSVMH_OVP;
					break;
					case 0x14:
						_lastResetReason = ResetReasonPMMSWPOR;
					break;
					case 0x16:
						_lastResetReason = ResetReasonWdtTimeOut;
					break;
					case 0x18:
						_lastResetReason = ResetReasonWdtPasswordViolation;
					break;
					case 0x1A:
						_lastResetReason = ResetReasonFlashPasswordViolation;
					break;
					case 0x1C:
						_lastResetReason = ResetReasonPLLUnlock;
					break;
					case 0x1E:
						_lastResetReason = ResetReasonPERFPeripheralConfigurationAreaFetch;
					break;
					case 0x20:
						_lastResetReason = ResetReasonPMMPasswordViolation;
					break;
					default:
						_lastResetReason = ResetReasonReserved;
					break;
					}
				}
			}
			while(lastRegisterValue);
			
			// ���������� ��������� ���������� �������
			return _lastResetReason;
		}
	};

	// ��������������� ����������� ����������
	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  RegisterDataType* const SysResetInterruptVector<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile  RegisterDataType*>(RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	typename SysResetInterruptVector<RegisterDataType, PointerType, RegisterAddr>::ResetReason
	SysResetInterruptVector<RegisterDataType, PointerType, RegisterAddr>
		::_lastResetReason = SysResetInterruptVector<RegisterDataType, PointerType, RegisterAddr>::ResetReasonUnknown;
}
}

#endif