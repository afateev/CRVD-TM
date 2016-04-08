
#ifndef _RBLIB_BD_BPLUS_TYPE_SELECTOR_H_
#define _RBLIB_BD_BPLUS_TYPE_SELECTOR_H_

namespace Rblib
{
namespace Db
{
namespace BPlusTree
{

//
// выбор одного из двух типов, в зависимости от условия
// 
template<class T1, class T2, bool first>
class TypeSelector
{
public:
	typedef T1 T;	
};

template<class T1, class T2>
class TypeSelector<T1, T2, false>
{
public:
	typedef T2 T;
};

}
}
}

#endif // _RBLIB_BD_BPLUS_TYPE_SELECTOR_H_

