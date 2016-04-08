#ifndef VECTOR_STATIC_H
#define VECTOR_STATIC_H

#include "vector_base.h"

namespace Rblib
{
	
template<class T, int Capacity = 10>
class vector_static : public vector_base<T>
{
protected:
	typedef vector_base<T> Base;

protected:
	unsigned char _charBuf[sizeof(T) * Capacity];
	
public:
	// конструктор
	vector_static()
	{
		this->_buf = (typename Base::iterator)_charBuf;
		this->_size = 0;
	}

	// деструктор
	~vector_static()
	{
		this->clear();
	}	
	
	// добавляет элемент в конец вектора
	bool push_back(const T& value)
	{
		if(this->_size == Capacity)
			return false;
		new(&(this->_buf[this->_size++])) typename Base::value_type(value);
		return true;			
	}
	
	// изменяет размер контейнера
	bool resize(unsigned int size)
	{
		if(size > Capacity)
			return false;
		if(size > this->_size)
		{
			for(unsigned int i = this->_size; i < size; i++)
				new(&(this->_buf[i])) typename Base::value_type();  			
		}
		if(size < this->_size)
		{
			for(unsigned int i = size; i < this->_size; i++)
				custom_destroyer<T>::destroy(&(this->_buf[i]));
		}	
		this->_size = size;
		return true;
	}
	
};

}
#endif // VECTOR_STATIC_H







/* старая рабочая реализация
#ifndef VECTOR_STATIC_H
#define VECTOR_STATIC_H

#include <string.h>
#include <new>
#include "custom_destroyer.h"

namespace Rblib
{
	
template<class T, int Capacity = 10>
class vector_static
{
public:
	typedef T value_type; // тип элемента	
	typedef T* iterator; // итератор

protected:
	unsigned int _size;
	unsigned char _charBuf[sizeof(T) * Capacity];
	const iterator _buf;

	
public:
	// конструктор
	vector_static() : _buf((iterator)_charBuf)
	{
		_size = 0;
	}

	// деструктор
	~vector_static()
	{
		clear();
	}
	
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
	
	// добавляет элемент в конец вектора
	bool push_back(const T& value)
	{
		if(_size == Capacity)
			return false;
		new(&(_buf[_size++])) value_type(value);
		return true;			
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
	
	// изменяет размер контейнера
	bool resize(unsigned int size)
	{
		if(size > Capacity)
			return false;
		if(size > _size)
		{
			for(unsigned int i = _size; i < size; i++)
				new(&(_buf[i])) value_type();  			
		}
		if(size < _size)
		{
			for(unsigned int i = size; i < _size; i++)
				custom_destroyer<T>::destroy(&(_buf[i]));
		}	
		_size = size;
		return true;
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
#endif // VECTOR_STATIC_H*/
