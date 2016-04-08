/// \file
/// \brief ���������� �������� ��� ����������� �������� MSP430.

#ifndef OPERATING_MODE_MSP430_H
#define OPERATING_MODE_MSP430_H

#include "./StatusRegister.h"

namespace Rblib
{
namespace Msp430
{

	/// ������ ��� ���������� �������� ��� ����������� ��������� MSP430.
	class OperatingMode
	{
	public:

		/// ����� SR �������� ���������� �� ���.
		enum Flags
		{
			/// SCG0.
			FlagScg0 = 0x0040,
			/// SCG1.
			FlagScg1 = 0x0080,
			/// OSCOFF.
			FlagOscOff = 0x0020, 
			/// CPUOFF.
			FlagCpuOff = 0x0010,
			/// SCG0 | SCG1 | OSCOFF | CPUOFF
			FlagAll = 0x00F0
		};

		/// ������ ���. ���������� ��������� ������.
		enum Modes
		{
			/// ��������� �������� �����.
			Active = 0,
			/// LPM0. CPU Off, MCLK Off, SMCLK On, DCO On, ACLK On.
			SleepLevel0 = FlagCpuOff,
			/// LPM1. CPU Off, MCLK Off, SMCLK On, DCO Off, ACLK On.
			SleepLevel1 = FlagCpuOff | FlagScg0,
			/// LPM2. CPU Off, MCLK Off, SMCLK Off, DCO Off, ACLK On.
			SleepLevel2 = FlagCpuOff | FlagScg1,
			/// LPM3. CPU Off, MCLK Off, SMCLK Off, DCO Off, ACLK On.
			SleepLevel3 = FlagCpuOff | FlagScg1 | FlagScg0,
			/// LPM4. ���� � ��� ��������� ��������� ������� �����������.
			SleepLevel4 = FlagCpuOff | FlagScg1 | FlagScg0 | FlagOscOff,
		};

	protected:

		enum Constants
		{
			_dependentFlagsMask = FlagScg0 | FlagScg1
		};

	public:

		/// ���������� ����� � �������� SR.
		/// \param flags ����� ������. ����, ��������������� ������, ������� ���� ����������, ������ ���� ����������� � 1.
		static inline void SetFlags(unsigned short flags)
		{
			RBLIB_BIS_SR(flags);
		}

		/// �������� ����� � �������� SR.
		/// \param flags ����� ������. ����, ��������������� ������, ������� ���� ��������, ������ ���� ����������� � 1.
		static inline void ClearFlags(unsigned short flags)
		{
			RBLIB_BIC_SR(flags);
		}

		/// �������� ���� ����������.
		/// \details ���������� ���� CPUOFF.
		static inline void EnableCpu()
		{
			RBLIB_BIC_SR(FlagCpuOff);
		}

		/// ��������� ���� ����������.
		/// \details ����������� ���� CPUOFF.
		static inline void DisableCpu()
		{
			RBLIB_BIS_SR(FlagCpuOff);
		}

		/// ���������� ����� ���.
		/// \details ���������� � �������� SR ����� ������, ��������������� ��������� ������ ���.
		/// \param mode ����� ���.
		static inline void Sleep(unsigned short mode = SleepLevel3)
		{
			if ((mode & _dependentFlagsMask) == _dependentFlagsMask)
				RBLIB_BIS_SR(mode);
			else
			{
				RBLIB_BIS_SR(_dependentFlagsMask);
				RBLIB_BIC_SR((~mode) & _dependentFlagsMask);
				RBLIB_BIS_SR(mode);
			}
		}

		// 
		// ��� ������ ����� ������������, ���� ������������� 
		// ��������� ����������� ������� (function inlining).
		// � ��������� ������� ���� ������������ ������� (��. ����).
		// {{
		//
		static inline void SetFlagsOnExit(unsigned short flags)
		{
			RBLIB_BIS_SR_ON_EXIT(flags);
		}

		static inline void ClearFlagsOnExit(unsigned short flags)
		{
			RBLIB_BIC_SR_ON_EXIT(flags);
		}

		static inline void WakeUp(unsigned short mode = Active)
		{
			RBLIB_BIC_SR_ON_EXIT((~mode) & FlagAll);
			RBLIB_BIS_SR_ON_EXIT(mode);
		}
		//
		// }}
		//

	protected:

		static inline void SetSleepMode(unsigned short mode)
		{
			if ((mode & _dependentFlagsMask) == _dependentFlagsMask)
				RBLIB_BIS_SR(mode);
			else
			{
				RBLIB_BIS_SR(_dependentFlagsMask);
				RBLIB_BIC_SR((~mode) & _dependentFlagsMask);
				RBLIB_BIS_SR(mode);
			}
		}

	};

}
}

//
// ������� ��������� ������ ����������������� ��� ������ �� ����������
//

/// ���������� ����� � SR �������� ��� ������ �� ����������.
#define RBLIB_OPERATING_MODE_MSP430_SET_FLAGS_ON_EXIT(flags) RBLIB_BIS_SR_ON_EXIT(flags)
/// �������� ����� � SR �������� ��� ������ �� ����������.
#define RBLIB_OPERATING_MODE_MSP430_CLR_FLAGS_ON_EXIT(flags) RBLIB_BIC_SR_ON_EXIT(flags)
/// ���������� (�������������) � ������ ����� ��� ��� ������ �� ����������.
#define RBLIB_OPERATING_MODE_MSP430_WAKE_UP(mode) RBLIB_BIC_SR_ON_EXIT((~mode) & Rblib::Msp430::OperatingMode::FlagAll); \
													RBLIB_BIS_SR_ON_EXIT(mode)

#endif // OPERATING_MODE_MSP430_H
