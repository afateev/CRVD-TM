
#ifndef _RBLIB_BD_INDEX_H_
#define _RBLIB_BD_INDEX_H_

#include "b_plus_tree.h"
#include "digit_selector.h"

namespace Rblib
{
// Index - �����, ����������� �������� ������
// FS - ������ � �������� �������
// Key - ��� �����, �� �������� ����� ����������� � ������ ������
// _64bit -  ����� ����������� �������� ������ � �������� ����� ������.
// _pageSize ������ ���������� ��������, ������� ������ ��������� �� 1 ���
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
