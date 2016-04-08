#ifndef QUEUE_H
#define QUEUE_H

#include <string.h>
#include <new>

template<class T, int Capacity = 100>
class queue_static
{
public:
	typedef T value_type; // тип элемента	
	typedef T* iterator; // итератор

protected:
	volatile unsigned int _size;
	volatile unsigned int _head; 
	volatile unsigned int _tail;
	unsigned char _charBuf[sizeof(T) * Capacity];
	const iterator _buf;

	void destroy()
	{
		while(!empty())
			pop();		
	}

public:
	// конструктор
	queue_static() : _buf((iterator)_charBuf)
	{
		_size = 0;
		_head = 0; 
		_tail = 0; 
	}
	
	// деструктор
	~queue_static()
	{
		destroy();
	}

	// добавляет элемент в конец очереди
	// если вернётся false, то самый старый элемент был потерян
	bool push(const T& value)
	{
		_head++;
		if(_head == Capacity)
			_head = 0;
		unsigned int temp = _head;
		bool flag = true;
		if(temp == _tail)//если переполнение, то теряем первый элемент очереди
		{
			_tail++;
			if(_tail == Capacity)
				_tail = 0;		
			custom_destroyer<T>::destroy(&(_buf[_head]));
			flag = false;			
		}
		else
			_size++;

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
		if(_tail == Capacity)
			_tail = 0;
		custom_destroyer<T>::destroy(&(_buf[_tail]));
	}
	
	// возращает ссылку на первый элемент очереди
	T& front()
	{
		if(_size == 0)//очередь пуста
			return *((T*)&_buf[0]);
		
		unsigned int temp = _tail + 1;
		if(temp == Capacity)
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

#endif // QUEUE_H