#ifndef MAP_BASE_H
#define MAP_BASE_H

#include <string.h>
#include <new>
#include "custom_destroyer.h"

namespace Rblib
{	
template<class T1, class T2>
class pair_base
{
public:
	T1 first;
	T2 second;
	pair_base(const T1 f, const T2 s) : first(f), second(s)
	{
	}
};
	
template<class Key, class T>
class map_base
{
public:
	typedef Key key_type;
	typedef T mapped_type;
	typedef pair_base<Key, T> value_type;
	typedef value_type* iterator; // итератор

protected:	
	unsigned int _size;
	iterator _buf;
				
public:	
	// указывает на первый элемент
	inline iterator begin()
	{
		return &(_buf[0]);
	}
	
	// указывает на элемент, следующий за последним
	inline iterator end()
	{
		return &(_buf[_size]);
	}
	
	iterator find(const key_type& k) const
	{
		unsigned int first = 0;
		unsigned int last = _size;
		unsigned int mid;
		
		if(_size == 0)
			return &(_buf[_size]);
		else
			if(_buf[_size - 1].first < k)
				return &(_buf[_size]);
		
		while(first - last)
		{
			mid = first + ((last - first) >> 1);
			if(k <= _buf[mid].first)
				last = mid;
			else
				first = mid + 1;
		}
		
		if(_buf[last].first == k)
			return &(_buf[last]);
		return &(_buf[_size]);
	}
	
	// выдает количество элементов вектора
	unsigned int size() const
	{
		return _size;	
	}
	
	// возвращает истину, если вызывающий вектор пуст
	bool empty() const
	{
		if(_size == 0)
			return true;
		return false;
	}
	
	// удаляет все элементы контейнера
	void clear()
	{
		for(unsigned int i = 0; i < _size; i++)
		{
			key_type* k = &(_buf[0].first);
			custom_destroyer<Key>::destroy(k);

			mapped_type* m = &(_buf[0].second);
			custom_destroyer<T>::destroy(m);			
		}
		_size = 0;
	}
	
	iterator erase(iterator where)
	{
		if(_size == 0)
			return end();
		if( (where < begin()) || (where >= end()) )
			return end();
		
		custom_destroyer<key_type>::destroy(&(where->first));
		custom_destroyer<mapped_type>::destroy(&(where->second));

		iterator next = where + 1;
		memcpy(where, next, (char*)end() - (char*)next);
		_size--;		
		return where;
	}
		
};

}
#endif // MAP_BASE_H