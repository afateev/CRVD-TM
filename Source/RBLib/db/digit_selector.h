
#ifndef _RBLIB_BD_DIGIT_SELECTOR_H_
#define _RBLIB_BD_DIGIT_SELECTOR_H_

namespace Rblib
{
namespace Db
{
//
// ����� 32/64-������� ���� ������
// 
// 32 ���
template<bool _64bit>
class DigitSelector
{
public:
	typedef unsigned long T;
};

// 64 ���
template<>
class DigitSelector<true>
{
public:
	typedef unsigned long long T;
};

}
}

#endif // _RBLIB_BD_DIGIT_SELECTOR_H_

