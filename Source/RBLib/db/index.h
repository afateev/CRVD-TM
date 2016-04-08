
#ifndef _RBLIB_BD_INDEX_H_
#define _RBLIB_BD_INDEX_H_

#include "b_plus_tree.h"
#include "digit_selector.h"

namespace Rblib
{
// Index - класс, реализующий файловый индекс
// FS - доступ к файловой система
// Key - тип ключа, по которому будем сортировать и искать записи
// _64bit -  выбор разрядности хранимых данных и смещения узлов дерева.
// _pageSize размер физической страницы, которую удобно считывать за 1 раз
template<class FS, class Key, bool _64bit = false, unsigned long _pageSize = 512>
class Index : public BPlusTree<FS, Key, typename Db::DigitSelector<_64bit>::T, _64bit, _pageSize>
{
public:

	typedef typename BPlusTree<FS, Key, typename Db::DigitSelector<_64bit>::T, _64bit, _pageSize>::PairType PairType;

public:

	Index(FS& fs) : BPlusTree<FS, Key, typename Db::DigitSelector<_64bit>::T, _64bit, _pageSize>(fs)
	{
	}

};

}

#endif // _RBLIB_BD_INDEX_H_
