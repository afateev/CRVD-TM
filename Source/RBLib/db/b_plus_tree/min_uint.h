
#ifndef _RBLIB_BD_BPLUS_MIN_UINT_H_
#define _RBLIB_BD_BPLUS_MIN_UINT_H_

namespace Rblib
{
namespace Db
{
namespace BPlusTree
{

namespace _private
{
	enum
	{
		CharMax = 0xFF,
		ShortMax = 0xFFFF,
		LongMax = 0xFFFFFFFF,
		LongLongMax = 0xFFFFFFFFFFFFFFFFLL
	};

	template<unsigned char passedConditionsNumber> struct MinUIntHelper{};
	template<> struct MinUIntHelper<3>{ typedef unsigned char T; static const T MaxVal = CharMax; };
	template<> struct MinUIntHelper<2>{ typedef unsigned short T; static const T MaxVal = ShortMax; };
	template<> struct MinUIntHelper<1>{ typedef unsigned long T; static const T MaxVal = LongMax; };
	template<> struct MinUIntHelper<0>{ typedef unsigned long long T; static const T MaxVal = LongLongMax; };
}

//
// класс для подбора минимального типа данных, вмещающего maxValue
//
template<unsigned long long maxValue>
struct MinUInt
{
private:
	typedef typename _private::MinUIntHelper<(maxValue <= _private::CharMax) +
											(maxValue <= _private::ShortMax) +
											(maxValue <= _private::LongMax)> Helper;
public:
	typedef typename Helper::T T;
	static const T MaxVal = Helper::MaxVal;
};

}
}
}

#endif // _RBLIB_BD_BPLUS_MIN_UINT_H_

