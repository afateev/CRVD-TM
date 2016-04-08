/// \file
/// \brief ���������� �������� ��� ����������� �������� MSP430x5xx.

#ifndef OPERATING_MODE_MSP430F5XXX_H
#define OPERATING_MODE_MSP430F5XXX_H

#include "../OperatingMode.h"
#include "../../NullType.h"

namespace Rblib
{
namespace Msp430f5xxx
{
	/// ������ ��� ���������� �������� ��� ����������� ��������� MSP430x5xx.
	/// \details ������� �� Msp430::OperatingMode ������� ������ � ������� ������� ���������� FLL.
	/// \n��������� �������� ������� ������ �� ����� ������, 
	/// � ���������� ���� ������� ������������ ������������� OperatingMode.
	template<class UnusedTemplateArg>
	class _OperatingMode : public Msp430::OperatingMode
	{
	protected:

		static bool _fllEnabled;

	public:

		enum FlagsEx
		{
			FlagAllNoFll = 0x00B0
		};

		enum ModesEx
		{
			ActiveNoFll = FlagScg0
		};

	public:

		/// �������� FLL.
		static inline void EnableFll()
		{
			RBLIB_BIC_SR(Msp430::StatusRegisterScg0);
			_fllEnabled = true;
		}

		/// ��������� FLL.
		static inline void DisableFll()
		{
			RBLIB_BIS_SR(Msp430::StatusRegisterScg0);
			_fllEnabled = false;
		}

		/// ��������� ������� �� FLL.
		static inline bool IsFllEnabled()
		{
			return _fllEnabled;
		}

		/// ���������� ����� ���.
		/// \details ���� FLL ��� �������� ������� DisableFll, �� ���������� ���������� ���������� �� ��������� ������.
		/// \param mode ����� ���.
		static inline void Sleep(unsigned short mode = SleepLevel3)
		{
			if (_fllEnabled || (mode & (FlagScg0 | FlagScg1 | FlagOscOff)))
			{
			}
			else
			{
				mode |= FlagScg0;
			}
			Msp430::OperatingMode::Sleep(mode);
		}

		// 
		// ��� ������ ����� ������������, ���� ������������� 
		// ��������� ����������� ������� (function inlining).
		// � ��������� ������� ���� ������������ ������� (��. ����).
		// {{
		//
		static inline void WakeUp(unsigned short mode = Active)
		{
			if (_fllEnabled || (mode & (FlagScg0 | FlagScg1 | FlagOscOff)))
			{
			}
			else
			{
				mode |= FlagScg0;
			}
			Msp430::OperatingMode::WakeUp(mode);
		}
		//
		// }}
		//
	};

	template<class UnusedTemplateArg>
	bool _OperatingMode<UnusedTemplateArg>::_fllEnabled = true;

	/// \class Rblib::Msp430f5xxx::OperatingMode
	/// \brief ������ ���������� �������� ���.
	/// \details ����� �������� ������������ �������������� ������ _OperatingMode, � ������� ���������� ��� ����������������.
	/// \n �������� ��� <tt>typedef _OperatingMode<NullType> %OperatingMode;</tt>
	typedef _OperatingMode<NullType> OperatingMode;

}
}

/// ���������� (�������������) � �������� ����� ����� ������ �� ����������.
/// \details ���� FLL ��� �������� ������� DisableFll, �� ���������� ���������� ���������� �� ��������� ������.
#define RBLIB_OPERATING_MODE_MSP430F5XXX_WAKE_UP(mode) \
{ \
	unsigned short __mode__ = mode; \
	if (Rblib::Msp430f5xxx::OperatingMode::IsFllEnabled() || (__mode__ & (Rblib::Msp430f5xxx::OperatingMode::FlagScg0 | Rblib::Msp430f5xxx::OperatingMode::FlagScg1 | Rblib::Msp430f5xxx::OperatingMode::FlagOscOff))) \
	{ \
	} \
	else \
	{ \
		__mode__ |= Rblib::Msp430f5xxx::OperatingMode::FlagScg0; \
	} \
	RBLIB_OPERATING_MODE_MSP430_WAKE_UP(__mode__); \
}

#endif // OPERATING_MODE_MSP430F5XXX_H
