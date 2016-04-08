
#ifndef _RBLIB_BD_INDEX_SIZE_ADDITION_H_
#define _RBLIB_BD_INDEX_SIZE_ADDITION_H_

namespace Rblib
{
namespace Db
{

#pragma pack(push, 1)
template<unsigned long size>
class SizeAddidion
{
private:
	char _unused[size];
};

template<>
class SizeAddidion<0>
{
};
#pragma pack (pop)

}
}

#endif // _RBLIB_BD_INDEX_SIZE_ADDITION_H_
