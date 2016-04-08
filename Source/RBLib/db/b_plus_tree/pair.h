
#ifndef _RBLIB_BD_BPLUS_PAIR_H_
#define _RBLIB_BD_BPLUS_PAIR_H_

namespace Rblib
{
namespace Db
{
namespace BPlusTree
{

#pragma pack(push, 1)
template<class KeyT, class ValueT>
class Pair
{
public:

	inline ValueT Value() const
	{
		return _value;
	}

	inline KeyT Key() const
	{
		return _key;
	}

	inline void Set(KeyT key, ValueT value)
	{
		_key = key;
		_value = value;
	}

	inline void Set(ValueT value)
	{
		_value = value;
	}

	inline void SetKey(KeyT key)
	{
		_key = key;
	}

	inline void SetValue(ValueT value)
	{
		_value = value;
	}

private:

	KeyT _key;
	ValueT _value;
};
#pragma pack (pop)

}
}
}

#endif // _RBLIB_BD_BPLUS_PAIR_H_

