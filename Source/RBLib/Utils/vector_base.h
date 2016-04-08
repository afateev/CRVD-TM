#ifndef VECTOR_BASE_H
#define VECTOR_BASE_H

#include <string.h>
#include <new>
#include "custom_destroyer.h"

namespace Rblib
{
	
template<class T>
class vector_base
{
public:
	typedef T value_type; // тип элемента	
	typedef T* iterator; // итератор

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
		
	// удаляет последний элемент (не возвращает значение!)
	void pop_back()
	{
		if(_size != 0)
		{
			_size--;
			custom_destroyer<T>::destroy(&(_buf[_size]));
		}
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
	
	// удаляет все элементы вектора
	void clear()
	{
		for(unsigned int i = 0; i < _size; i++)
			custom_destroyer<T>::destroy(&(_buf[i]));
		_size = 0;
	}
	
	// доступ без проверки диапазона
	T& operator[](unsigned int pos)
	{
		return _buf[pos];
	}
	
	// первый элемент
	T& front()
	{
		return *begin();
	}

	// последний элемент
	T& back()
	{
		if(_size == 0)
			return _buf[0];
		return _buf[_size - 1];
	}

	// удалить элемент
	iterator erase(iterator where)
	{
		if(_size == 0)
			return end();
		if( (where < begin()) || (where >= end()) )
			return end();

		custom_destroyer<T>::destroy(where);
		
		iterator next = where + 1;
		memcpy(where, next, (char*)end() - (char*)next);
		_size--;		
		return where;
	}
};

}
#endif // VECTOR_BASE_H
