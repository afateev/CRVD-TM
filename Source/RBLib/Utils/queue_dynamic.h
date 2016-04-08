#ifndef QUEUEDYNAMIC_H
#define QUEUEDYNAMIC_H

#include <string.h>
#include <new>

namespace Rblib
{
	
template<class T>
class queue_dynamic
{
public:
	typedef T value_type; // тип элемента
	typedef T* iterator; // итератор

protected:
//	unsigned char* _buf;
	iterator _buf;
	unsigned int _capacity;
	volatile unsigned int _size;
	volatile unsigned int _head; 
	volatile unsigned int _tail;

protected:
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
			::memcpy(newBuf, _buf, _tail * sizeof(value_type));
			unsigned int rem = _capacity - (_tail + 1); // остаток для копирования
			::memcpy(newBuf + (newCapacity - rem), _buf + (_tail + 1), rem * sizeof(value_type));
			char* b = (char*)_buf;  
			delete b;
			_tail += newCapacity - _capacity;
		}
		_buf = newBuf;
		_capacity = newCapacity;
		return true;	
	}
	
	void destroy()
	{
		while(!empty())
			pop();
		
		if(_capacity != 0)
		{
			char* b = (char*)_buf;  
			delete b;
			_buf = 0;
			_capacity = 0;
		}
	}

public:
	// конструктор
	queue_dynamic()
	{
		_buf = 0;
		_capacity = 0;
		_size = 0;
		_head = 0; 
		_tail = 0; 
	}

	// деструктор
	~queue_dynamic()
	{
		destroy();
	}

	// добавляет элемент в конец очереди
	// если вернётся false, то самый старый элемент был потерян
	bool push(const T& value)
	{
		_head++;
		if(_head >= _capacity)
			_head = 0;
		unsigned int temp = _head;
		
		bool flag = true;
		
		if(temp == _tail)//если переполнение, то делаем перелокацию
		{			
			unsigned int temp;
			if(_capacity == 0)
				temp = 2;
			else
				temp = _capacity * 2;
			
			if(!reserve(temp))
			{
				if(!reserve(_capacity + 1))
				{
					if(_capacity == 0)
						return false;
					_tail++;
					if(_tail >= _capacity)
					_tail = 0;
					flag = false;
				}
			}								
			if(_capacity == 2)
				_head++;							
		}
		
		if(flag)
			_size++;
		else
		{
			if(_size != 0)
				custom_destroyer<T>::destroy(&(_buf[_head]));
		}
		
		new(&(_buf[_head])) value_type(value);
		return flag;
	}

	// удаляем первый элемент очереди
	void pop()
	{
		if(_size == 0)//очередь пуста
			return;

		_size--;
		_tail++;
		if(_tail == _capacity)
			_tail = 0;
		custom_destroyer<T>::destroy(&(_buf[_tail]));
	}
	
	// возращает ссылку на первый элемент очереди
	T& front()
	{
		if(_size == 0)//очередь пуста
			return _buf[0];
		
		unsigned int temp = _tail + 1;
		if(temp == _capacity)
			temp = 0;
		
		return _buf[temp];
	}
	
	// возвращает истину, если вызывающий вектор пуст
	bool empty() const
	{
		if(_size == 0)
			return true;
		return false;
	}
	
	// выдает количество элементов очереди
	unsigned int size() const
	{
		return _size;	
	}	
		
};

}

#endif // QUEUEDYNAMIC_H