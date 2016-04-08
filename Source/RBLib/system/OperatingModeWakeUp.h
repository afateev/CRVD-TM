/// \file
/// \brief Методов и макросы, необходимые для реализации автоматического пробуждения процессора при выходе из прерывания.

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
	/// \brief Класс для хранения флага "необходимо проснуться при выходе из прерывания".
	/// \details Флаг устанавливается пользователем в обработчике прерывания.
	/// Также устанавливается режим, в который нужно проснуться.
	/// Состояние флага анализируется в библиотечном обработчике прерывания, 
	/// и процессор переводится в заданный режим сна при выходе из обработчика.
	/// \nШаблонный параметр данного класса не имеет смысла, 
	/// в клиентском коде следует использовать специализацию OperatingModeWakeUp.
	template<class UnusedTemplateArg = NullType>
	class _OperatingModeWakeUp
	{
	protected:

		///	Флаг поступления запроса на выход из режима сна по окончании обработки прерывания.
		static bool _needWakeUp;
		/// Режим, в который надо проснуться.
		static unsigned short _wakeUpMode;

	public:

		/// Установить флаг, обозначающий необходимость проснуться при выходе из прерывания.
		/// \param mode Режим, в который нужно будет проснуться.
		static inline void WakeUp(unsigned short mode)
		{
			_needWakeUp = true;
			_wakeUpMode = mode;
		}

		/// Сбросить флаг просыпания (отменить просыпание).
		static inline void WakeUpCancel()
		{
			_needWakeUp = false;
		}

		/// Нужно ли просыпаться?
		/// \return @c true, если флаг установлен, т.е. нужно проснуться, или @c false, если флаг не установлен.
		static inline bool& NeedWakeUp()
		{
			return _needWakeUp;
		}

		/// Возвращается установленный режим, в который надо проснуться.
		/// \return Режим, в который нужно проснуться.
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
	/// \brief Класс для хранения флага "необходимо проснуться при выходе из прерывания".
	/// \details Класс является единственной специализацией класса _OperatingModeWakeUp, в котором определена вся функциональность.
	/// \n Определён как <tt>typedef _OperatingModeWakeUp<NullType> %OperatingModeWakeUp;</tt>
	typedef _OperatingModeWakeUp<NullType> OperatingModeWakeUp;
}
}
/// Сбросить флаг просыпания.
/// \details вызывается при входе в прерывание.
#define RESET_WAKE_UP (Rblib::System::OperatingModeWakeUp::NeedWakeUp() = false)

#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F1611 \
	|| RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F2618

/// Если необходимо (установлен флаг просыпания) переводит процессор в заданный режим при выходе из прерывания.
/// \details вызывается при выходе из прерывание.
# define CHECK_WAKE_UP \
	if (Rblib::System::OperatingModeWakeUp::NeedWakeUp()) \
	{ \
		RBLIB_OPERATING_MODE_MSP430_WAKE_UP(Rblib::System::OperatingModeWakeUp::WakeUpMode()); \
	}

#elif RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5340 \
        || RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5435A \
	|| RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5438A \
	  || RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5659

/// Если необходимо (установлен флаг просыпания) переводит процессор в заданный режим при выходе из прерывания.
/// \details вызывается при выходе из прерывание.
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
