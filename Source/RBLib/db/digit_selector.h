
#ifndef _RBLIB_BD_DIGIT_SELECTOR_H_
#define _RBLIB_BD_DIGIT_SELECTOR_H_

namespace Rblib
{
namespace Db
{
//
// выбор 32/64-битного типа данных
// 
// 32 бит
template<bool _64bit>
class DigitSelector
{
public:
	typedef unsigned long T;
};

// 64 бит
template<>
class DigitSelector<true>
{
public:
	typedef unsigned long long T;
};

}
}

#endif // _RBLIB_BD_DIGIT_SELECTOR_H_

