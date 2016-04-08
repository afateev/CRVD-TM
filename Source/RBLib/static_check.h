
#ifndef _RBLIB_STATIC_CHECK_H_
#define _RBLIB_STATIC_CHECK_H_

namespace Rblib { template<bool> class COMPILE_TIME_ERROR {}; }

//
// максрос проверяет выражение expr
// если expr равно true, будет выдана ошибка компиляции
// msg - сообщение об ошибке (должно соответствовать правилам именования идентификаторов в С++)
//
#define RBLIB_STATIC_CHECK(expr, msg) \
{ \
	struct ERROR_##msg { ERROR_##msg(Rblib::COMPILE_TIME_ERROR<false>){} }; \
	(void) sizeof((ERROR_##msg(Rblib::COMPILE_TIME_ERROR<(expr)>()))); \
}
#endif // _RBLIB_STATIC_CHECK_H_

