///////////////////////////////////////////////////////////////////////////////
//	 ласс управлени€ сторожевым таймером
//	MSP430
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef WDT_MSP430_H
#define WDT_MSP430_H

#include <string.h>

namespace Rblib
{
namespace Msp430
{
	//  ласс управлени€ сторожевым таймером
	template <class WdtControlRegister			// —труктура управл€ющего регистра, и методы доступа к ней
		>
	class Wdt : public WdtControlRegister
	{
	public:
		typedef typename WdtControlRegister::WdtIntrvals	WdtIntrvals;
		typedef typename WdtControlRegister::WdtClockSource WdtClockSource;
		typedef typename WdtControlRegister::Struct			Struct;
		typedef WdtControlRegister							Control;
	public:
		// ќстановить таймер
		static inline void Stop()
		{
			// временное значение регистра
			Struct tmp;
			// читаем что там
			Control::GetControl(&tmp);
			// измен€ем что надо
			tmp.Hold = 1;
			// записываем обратно
			Control::SetControl(&tmp);
		}
		// «апустить таймер
		static inline void Start()
		{
			// временное значение регистра
			Struct tmp;
			// читаем что там
			Control::GetControl(&tmp);
			// измен€ем что надо
			tmp.Hold = 0;
			// записываем обратно
			Control::SetControl(&tmp);
		}
		// —брос таймера
		static inline void Clear()
		{
			// временное значение регистра
			Struct tmp;
			// читаем что там
			Control::GetControl(&tmp);
			// измен€ем что надо
			tmp.Clear = 1;
			// записываем обратно
			Control::SetControl(&tmp);
		}
		// ”становить интервал срабатывани€ таймера
		static inline void SetInterval(WdtIntrvals value)
		{
			// временное значение регистра
			Struct tmp;
			// читаем что там
			Control::GetControl(&tmp);
			// измен€ем что надо
			tmp.Interval = value;
			// надо почистить счЄтчик, иначе может сработать
			tmp.Clear = 1;
			// записываем обратно
			Control::SetControl(&tmp);
		}
		// ”становить источник тактировани€
		static inline void SetClockSource(WdtClockSource value)
		{
			// временное значение регистра
			Struct tmp;
			// читаем что там
			Control::GetControl(&tmp);
			// измен€ем что надо
			tmp.ClockSource = value;
			// записываем обратно
			Control::SetControl(&tmp);
		}
		static inline void GeneratePUC()
		{
			Control::SetControlRaw(0);
		}
	};
}
}

#endif