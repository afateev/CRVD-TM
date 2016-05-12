///////////////////////////////////////////////////////////////////////////////
//	Список поддерживаемых библиотекой платформ
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PLATFORM_H
#define PLATFORM_H

#include "NullType.h"

// Идентификаторы поддерживаемых платформ
#define RBLIB_PLATFORM_ID_NONE				0
#define RBLIB_PLATFORM_ID_WIN32				1
#define RBLIB_PLATFORM_ID_MSP430F1611		2
#define RBLIB_PLATFORM_ID_MSP430F2618		3
#define RBLIB_PLATFORM_ID_MSP430F5340		4
#define RBLIB_PLATFORM_ID_MSP430F5342		5
#define RBLIB_PLATFORM_ID_MSP430F5435A		6
#define RBLIB_PLATFORM_ID_MSP430F5438A		7
#define RBLIB_PLATFORM_ID_MSP430F5659		8
#define RBLIB_PLATFORM_ID_EFM32GG232F1024	9
#define RBLIB_PLATFORM_ID_EFM32GG990F1024	10
#define RBLIB_PLATFORM_ID_STM32F1			11
#define RBLIB_PLATFORM_ID_STM32F4			12

namespace Rblib
{
	// Cписок платформ
	enum Platform
	{
		PlatformNone = RBLIB_PLATFORM_ID_NONE,
		PlatformWin32 = RBLIB_PLATFORM_ID_WIN32,
		PlatformMsp430f1611 = RBLIB_PLATFORM_ID_MSP430F1611,
		PlatformMsp430f2618 = RBLIB_PLATFORM_ID_MSP430F2618,
		PlatformMsp430f5340 = RBLIB_PLATFORM_ID_MSP430F5340,
		PlatformMsp430f5342 = RBLIB_PLATFORM_ID_MSP430F5342,
		PlatformMsp430f5435a = RBLIB_PLATFORM_ID_MSP430F5435A,
		PlatformMsp430f5438a = RBLIB_PLATFORM_ID_MSP430F5438A,
		PlatformMsp430f5659 = RBLIB_PLATFORM_ID_MSP430F5659,
		PlatformEfm32gg232f1024 = RBLIB_PLATFORM_ID_EFM32GG232F1024,
		PlatformEfm32gg990f1024 = RBLIB_PLATFORM_ID_EFM32GG990F1024,
    	PlatformStm32F1 = RBLIB_PLATFORM_ID_STM32F1,
		PlatformStm32F4 = RBLIB_PLATFORM_ID_STM32F4
	};

	template <Platform p,
		class t,
		class u>
	struct PlatformTypelist
	{
		static const Platform platform = p;
		typedef t Head;
		typedef u Tail;
	};

	template
	<
		Platform p1 = PlatformNone, typename t1 = NullType,
		Platform p2 = PlatformNone, typename t2 = NullType,
		Platform p3 = PlatformNone, typename t3 = NullType,
		Platform p4 = PlatformNone, typename t4 = NullType,
		Platform p5 = PlatformNone, typename t5 = NullType,
		Platform p6 = PlatformNone, typename t6 = NullType,
		Platform p7 = PlatformNone, typename t7 = NullType
	> 
	struct MakePlatformTypelist
	{
	private:
		typedef typename MakePlatformTypelist
		<
			p2, t2,
			p3, t3,
			p4, t4,
			p5, t5,
			p6, t6,
			p7, t7
		>
		::Result TailResult;
	public:
		typedef PlatformTypelist<p1, t1, TailResult> Result;
	};

	template<>
	struct MakePlatformTypelist<>
	{
		typedef NullType Result;
	};

	template <class TList, Platform ppp> struct PlatformSelector;

	template <Platform p, class Head, class Tail>
	struct PlatformSelector<PlatformTypelist<p, Head, Tail>, p>
	{
		typedef Head Result;
	};

	template <Platform p, class Head, class Tail, Platform requiredPlt>
	struct PlatformSelector<PlatformTypelist<p, Head, Tail>, requiredPlt>
	{
		typedef typename PlatformSelector<Tail, requiredPlt>::Result Result;
	};
}

// Автоматический выбор платформы
// и одновременно проверка на поддержку выбранной платформы библиотекой

// При задании платформы в качестве параметра шаблона рекомендуется
// использовать символ RBLIB_PALTFORM_AUTODETECT
// При проверке идентификатора платформы в дерективах препроцессора рекомендуется
// использовать символ RBLIB_PALTFORM_AUTODETECT_ID

#if defined WIN32
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformWin32
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_WIN32
#elif defined __MSP430F1611__
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformMsp430f1611
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_MSP430F1611
#elif defined __MSP430F2618__
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformMsp430f2618
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_MSP430F2618
#elif defined __MSP430F5340__
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformMsp430f5340
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_MSP430F5340
#elif defined __MSP430F5342__
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformMsp430f5340
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_MSP430F5340
#elif defined __MSP430F5435A__
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformMsp430f5435a
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_MSP430F5435A
#elif defined __MSP430F5438A__
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformMsp430f5438a
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_MSP430F5438A
#elif defined __MSP430F5659__
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformMsp430f5659
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_MSP430F5659
#elif defined EFM32GG232F1024
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformEfm32gg232f1024
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_EFM32GG232F1024
#elif defined EFM32GG990F1024
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformEfm32gg990f1024
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_EFM32GG990F1024
#elif defined STM32F1
	#define STM32
	#define RBLIB_PALTFORM_AUTODETECT Rblib::PlatformStm32F1
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_STM32F1
#elif defined STM32F4
	#define STM32
	#define RBLIB_PALTFORM_AUTODETECT Rblib::PlatformStm32F4
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_STM32F4
#else
	#define RBLIB_PALTFORM_AUTODETECT		Rblib::PlatformNone
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_NONE
	#error Selected platform not supported by RBLib.
#endif

#endif