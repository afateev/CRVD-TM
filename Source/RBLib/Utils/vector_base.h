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
	typedef T value_type; // ��� ��������	
	typedef T* iterator; // ��������

protected:
	unsigned int _size;
	iterator _buf;

	
public:
	// ��������� �� ������ �������
	inline iterator begin()
	{
		return &(_buf[0]);
	}
	
	// ��������� �� �������, ��������� �� ���������
	inline iterator end()
	{
		return &(_buf[_size]);
	}
		
	// ������� ��������� ������� (�� ���������� ��������!)
	void pop_back()
	{
		if(_size != 0)
		{
			_size--;
			custom_destroyer<T>::destroy(&(_buf[_size]));
		}
	}
	
	// ������ ���������� ��������� �������
	unsigned int size() const
	{
		return _size;	
	}
	
	// ���������� ������, ���� ���������� ������ ����
	bool empty() const
	{
		if(_size == 0)
			return true;
		return false;
	}
	
	// ������� ��� �������� �������
	void clear()
	{
		for(unsigned int i = 0; i < _size; i++)
			custom_destroyer<T>::destroy(&(_buf[i]));
		_size = 0;
	}
	
	// ������ ��� �������� ���������
	T& operator[](unsigned int pos)
	{
		return _buf[pos];
	}
	
	// ������ �������
	T& front()
	{
		return *begin();
	}

	// ��������� �������
	T& back()
	{
		if(_size == 0)
			return _buf[0];
		return _buf[_size - 1];
	}

	// ������� �������
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
