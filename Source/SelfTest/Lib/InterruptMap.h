///////////////////////////////////////////////////////////////////////////////
//	Класс управления вектором прерываний
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTMAP_H
#define INTERRUPTMAP_H

#include "Platform.h"

#ifdef __ICCAVR__
#include "ATmega164PA\InterruptMap.h"
#endif

namespace Lib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef __ICCAVR__
			PlatformATmega164PA, ATmega164PA::InterruptMap
#endif
		>::Result InterruptMapTypes;
	
	// Селектор класса вектора прерываний
	template <Platform p>
    class InterruptMapSelector: public PlatformSelector<InterruptMapTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию
		InterruptMapSelector()
		{
		}
	};

	// Класс управления вектором прерываний
	typedef InterruptMapSelector<RBLIB_PALTFORM_AUTODETECT> InterruptMap;
}

#endif