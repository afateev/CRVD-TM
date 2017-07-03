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
#define RBLIB_PLATFORM_ID_ATMEGA164PA		2

namespace Lib
{
	// Cписок платформ
	enum Platform
	{
		PlatformNone = RBLIB_PLATFORM_ID_NONE,
		PlatformWin32 = RBLIB_PLATFORM_ID_WIN32,
		PlatformATmega164PA = RBLIB_PLATFORM_ID_ATMEGA164PA
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
		Platform p5 = PlatformNone, typename t5 = NullType
	> 
	struct MakePlatformTypelist
	{
	private:
		typedef typename MakePlatformTypelist
		<
			p2, t2,
			p3, t3,
			p4, t4,
			p5, t5
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
	#define RBLIB_PALTFORM_AUTODETECT		Lib::PlatformWin32
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_WIN32
#elif defined __ATmega164PA__
	#define RBLIB_PALTFORM_AUTODETECT		Lib::PlatformATmega164PA
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_ATMEGA164PA
#else
	#define RBLIB_PALTFORM_AUTODETECT		Lib::PlatformNone
	#define RBLIB_PALTFORM_AUTODETECT_ID	RBLIB_PLATFORM_ID_NONE
	#error Selected platform not supported by Library.
#endif

#endif