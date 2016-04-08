/// \file
/// \brief ������� � �������, ����������� ��� ���������� ��������������� ����������� ���������� ��� ������ �� ����������.

#ifndef OPERATING_MODE_WAKE_UP_SYSTEM_H
#define OPERATING_MODE_WAKE_UP_SYSTEM_H

#ifdef __ICC430__

#include "../Platform.h"
#include "../OperatingMode.h"
#include "../NullType.h"

namespace Rblib
{
namespace System
{
	/// \brief ����� ��� �������� ����� "���������� ���������� ��� ������ �� ����������".
	/// \details ���� ��������������� ������������� � ����������� ����������.
	/// ����� ��������������� �����, � ������� ����� ����������.
	/// ��������� ����� ������������� � ������������ ����������� ����������, 
	/// � ��������� ����������� � �������� ����� ��� ��� ������ �� �����������.
	/// \n��������� �������� ������� ������ �� ����� ������, 
	/// � ���������� ���� ������� ������������ ������������� OperatingModeWakeUp.
	template<class UnusedTemplateArg = NullType>
	class _OperatingModeWakeUp
	{
	protected:

		///	���� ����������� ������� �� ����� �� ������ ��� �� ��������� ��������� ����������.
		static bool _needWakeUp;
		/// �����, � ������� ���� ����������.
		static unsigned short _wakeUpMode;

	public:

		/// ���������� ����, ������������ ������������� ���������� ��� ������ �� ����������.
		/// \param mode �����, � ������� ����� ����� ����������.
		static inline void WakeUp(unsigned short mode)
		{
			_needWakeUp = true;
			_wakeUpMode = mode;
		}

		/// �������� ���� ���������� (�������� ����������).
		static inline void WakeUpCancel()
		{
			_needWakeUp = false;
		}

		/// ����� �� �����������?
		/// \return @c true, ���� ���� ����������, �.�. ����� ����������, ��� @c false, ���� ���� �� ����������.
		static inline bool& NeedWakeUp()
		{
			return _needWakeUp;
		}

		/// ������������ ������������� �����, � ������� ���� ����������.
		/// \return �����, � ������� ����� ����������.
		static inline unsigned short& WakeUpMode()
		{
			return _wakeUpMode;
		}
	};

	template<class UnusedTemplateArg>
	bool _OperatingModeWakeUp<UnusedTemplateArg>::_needWakeUp = false;

	template<class UnusedTemplateArg>
	unsigned short _OperatingModeWakeUp<UnusedTemplateArg>::_wakeUpMode;

	/// \class Rblib::System::OperatingModeWakeUp
	/// \brief ����� ��� �������� ����� "���������� ���������� ��� ������ �� ����������".
	/// \details ����� �������� ������������ �������������� ������ _OperatingModeWakeUp, � ������� ���������� ��� ����������������.
	/// \n �������� ��� <tt>typedef _OperatingModeWakeUp<NullType> %OperatingModeWakeUp;</tt>
	typedef _OperatingModeWakeUp<NullType> OperatingModeWakeUp;
}
}
/// �������� ���� ����������.
/// \details ���������� ��� ����� � ����������.
#define RESET_WAKE_UP (Rblib::System::OperatingModeWakeUp::NeedWakeUp() = false)

#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F1611 \
	|| RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F2618

/// ���� ���������� (���������� ���� ����������) ��������� ��������� � �������� ����� ��� ������ �� ����������.
/// \details ���������� ��� ������ �� ����������.
# define CHECK_WAKE_UP \
	if (Rblib::System::OperatingModeWakeUp::NeedWakeUp()) \
	{ \
		RBLIB_OPERATING_MODE_MSP430_WAKE_UP(Rblib::System::OperatingModeWakeUp::WakeUpMode()); \
	}

#elif RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5340 \
        || RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5435A \
	|| RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5438A \
	  || RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5659

/// ���� ���������� (���������� ���� ����������) ��������� ��������� � �������� ����� ��� ������ �� ����������.
/// \details ���������� ��� ������ �� ����������.
# define CHECK_WAKE_UP \
	if (Rblib::System::OperatingModeWakeUp::NeedWakeUp()) \
	{ \
		RBLIB_OPERATING_MODE_MSP430F5XXX_WAKE_UP(Rblib::System::OperatingModeWakeUp::WakeUpMode()); \
	}

#else

# define CHECK_WAKE_UP

#endif

#endif // __ICC430__

#endif // OPERATING_MODE_WAKE_UP_SYSTEM_H
