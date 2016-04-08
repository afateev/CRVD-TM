
#ifndef OPERATING_MODE_WISE_H
#define OPERATING_MODE_WISE_H

#ifdef __ICC430__

#include "./OperatingMode.h"
#include "./system/OperatingModeWakeUp.h"

namespace Rblib
{
/// Расширенный класс управления режимами сна.
/// \details Дополнен методами работы с флагом запроса на пробуждение, 
/// управлящим автоматическим пробуждением процессора при выходе из прерывания.
class OperatingModeWise : public OperatingMode
{
public:

	/// Сообщает библиотеке о необходимости пробуждения при выходе из прерывания, устаонавливая флаг.
	/// \param mode Режим сна, в который нужно проснуться.
	static inline void WakeUp(unsigned short mode = OperatingMode::Active)
	{
		System::OperatingModeWakeUp::WakeUp(mode);
	}

	/// Отменяет запрос на пробуждение.
	static inline void WakeUpCancel()
	{
		System::OperatingModeWakeUp::WakeUpCancel();
	}
};

}

#endif // __ICC430__

#endif // OPERATING_MODE_WISE_H
