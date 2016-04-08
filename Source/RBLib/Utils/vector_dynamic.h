#ifndef VECTOR_DYNAMIC_H
#define VECTOR_DYNAMIC_H

#include "vector_base.h"

namespace Rblib
{
template<class T>
class vector_dynamic : public vector_base<T>
{
protected:
	typedef vector_base<T> Base;

protected:	
	unsigned int _capacity;

public:	
	// конструктор
	vector_dynamic()
	{
		this->_size = 0;
		_capacity = 0;
		this->_buf = 0;
	}
	
	// копирующий конструктор
	vector_dynamic(const vector_dynamic& v)
	{
		this->_buf = 0;
		this->_size = 0;
		_capacity = 0;

		if(v._capacity != 0)
		{
			char* b = new(nothrow) char[v._capacity * sizeof(typename Base::value_type)];
			this->_buf = (typename Base::iterator)b;
			if(this->_buf != 0)
			{
				this->_size = v._size;
				_capacity = v._capacity;
				::memcpy(this->_buf, v._buf, this->_size * sizeof(typename Base::value_type));
			}
		}
	}
	
	// -----------
	vector_dynamic& operator=(const vector_dynamic& v)
	{
		destroy();

		if(v._capacity != 0)
		{
			char* b = new(nothrow) char[v._capacity * sizeof(typename Base::value_type)];
			this->_buf = (typename Base::iterator)b;
			if(this->_buf != 0)
			{
				this->_size = v._size;
				_capacity = v._capacity;
				::memcpy(this->_buf, v._buf, this->_size * sizeof(typename Base::value_type));
			}
		}
		
		return *this;
	}

	
	void destroy()
	{
		this->clear();
		
		if(_capacity != 0)
		{
			char* b = (char*)this->_buf;  
			delete b;
			this->_buf = 0;
			_capacity = 0;
		}
	}
	
	// деструктор
	~vector_dynamic()
	{
		destroy();
	}
	
	// резервирует место под определённое количество элементов
	bool reserve(unsigned int newCapacity)
	{
		if(newCapacity <= _capacity)
			return true;

		unsigned char* b = new(nothrow) unsigned char[newCapacity * sizeof(typename Base::value_type)];
		typename Base::iterator newBuf = (typename Base::iterator)b;
		if(newBuf == 0)
			return false;
		
		if(_capacity != 0)
		{
			::memcpy(newBuf, this->_buf, this->_size * sizeof(typename Base::value_type));			
			char* b = (char*)this->_buf;  
			delete b;
		}
		this->_buf = newBuf;
		_capacity = newCapacity;
		return true;		
	}
	
	// добавляет элемент в конец вектора
	bool push_back(const T& value)
	{
		if(this->_size == _capacity)
		{
			unsigned int temp;
			if(this->_size == 0)
				temp = 1;
			else
				temp = this->_size * 2;
			
			if(!reserve(temp))
				if(!reserve(this->_size + 1))
					return false;
		}
		
		new(&(this->_buf[this->_size++])) typename Base::value_type(value); 
			
		return true;
	}
	
	// изменяет количество элементов в контейнере
	bool resize(unsigned int size)
	{
		if(size > _capacity)
		{
			if(!reserve(size))
				return false;
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
#endif // VECTOR_DYNAMIC_H








/* старая рабочая реализация
#ifndef VECTOR_DYNAMIC_H
#define VECTOR_DYNAMIC_H

#include <string.h>
#include <new>
#include "custom_destroyer.h"

namespace Rblib
{
template<class T>
class vector_dynamic
{
public:
	typedef T value_type; // тип элемента
	typedef T* iterator; // итератор

protected:	
	unsigned int _size;
	unsigned int _capacity;
	iterator _buf;

public:	
	// конструктор
	vector_dynamic()
	{
		_size = 0;
		_capacity = 0;
		_buf = 0;
	}
	
	// копирующий конструктор
	vector_dynamic(const vector_dynamic& v)
	{
		_buf = 0;
		_size = 0;
		_capacity = 0;

		if(v._capacity != 0)
		{
			char* b = new(nothrow) char[v._capacity * sizeof(value_type)];
			_buf = (iterator)b;
			if(_buf != 0)
			{
				_size = v._size;
				_capacity = v._capacity;
				::memcpy(_buf, v._buf, _size * sizeof(value_type));
			}
		}
	}
	
	// -----------
	vector_dynamic& operator=(const vector_dynamic& v)
	{
		destroy();

		if(v._capacity != 0)
		{
			char* b = new(nothrow) char[v._capacity * sizeof(value_type)];
			_buf = (iterator)b;
			if(_buf != 0)
			{
				_size = v._size;
				_capacity = v._capacity;
				::memcpy(_buf, v._buf, _size * sizeof(value_type));
			}
		}
		
		return *this;
	}

	
	void destroy()
	{
		clear();
		
		if(_capacity != 0)
		{
			char* b = (char*)_buf;  
			delete b;
			_buf = 0;
			_capacity = 0;
		}
	}
	
	// деструктор
	~vector_dynamic()
	{
		destroy();
	}
	
	// резервирует место под определённое количество элементов
	bool reserve(unsigned int newCapacity)
	{
		if(newCapacity <= _capacity)
			return true;

		unsigned char* b = new(nothrow) unsigned char[newCapacity * sizeof(value_type)];
		iterator newBuf = (iterator)b;
		if(newBuf == 0)
			return false;
		
		if(_capacity != 0)
		{
			::memcpy(newBuf, _buf, _size * sizeof(value_type));			
			char* b = (char*)_buf;  
			delete b;
		}
		_buf = newBuf;
		_capacity = newCapacity;
		return true;		
	}
	
	// добавляет элемент в конец вектора
	bool push_back(const T& value)
	{
		if(_size == _capacity)
		{
			unsigned int temp;
			if(_size == 0)
				temp = 1;
			else
				temp = _size * 2;
			
			if(!reserve(temp))
				if(!reserve(_size + 1))
					return false;
		}
		
		new(&(_buf[_size++])) value_type(value); 
			
		return true;
	}
	
	// изменяет количество элементов в контейнере
	bool resize(unsigned int size)
	{
		if(size > _capacity)
		{
			if(!reserve(size))
				return false;
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
#endif // VECTOR_DYNAMIC_H*/
