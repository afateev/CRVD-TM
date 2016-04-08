#ifndef MAP_DYNAMIC_H
#define MAP_DYNAMIC_H

#include "map_base.h"

namespace Rblib
{	
	
template<class Key, class T>
class map_dynamic : public map_base<Key, T>
{
protected:
	typedef map_base<Key, T> Base;

protected:	
	unsigned int _capacity;
		
protected:
	// ��������� ������� �� ����������� �������
	pair_base<typename Base::iterator, bool> insert(unsigned int pos, const typename Base::value_type& value)
	{
		if(this->_size == _capacity)
		{
			unsigned int newSize;
			if(this->_size == 0)
				newSize = 1;
			else
				newSize = this->_size * 2;
			char* b = new(nothrow) char[newSize * sizeof(typename Base::value_type)];
			typename Base::iterator newBuf = (typename Base::iterator)b;
			if(newBuf == 0)
			{
				newSize = this->_size + 1;
				b = new(nothrow) char[newSize * sizeof(typename Base::value_type)];
				newBuf = (typename Base::iterator)b;
				if(newBuf == 0) // ���� �� ������� �������� ������
				{
					pair_base<typename Base::iterator, bool> pr((typename Base::iterator)&(this->_buf[pos]), false);
					return pr;
				}
			}			
			if(_capacity != 0)
			{
				::memcpy(newBuf, this->_buf, pos * sizeof(typename Base::value_type));
				unsigned int rem = this->_size - pos; // ������� ��� �����������
				::memcpy(newBuf + pos + 1, this->_buf + pos, rem * sizeof(typename Base::value_type));
				b = (char*)this->_buf;
				delete[] b;
			}
			this->_buf = newBuf;
			_capacity = newSize;
		}
		else
		{
			unsigned int rem = this->_size - pos; // ������� ��� �����������
			::memmove(this->_buf + pos + 1, this->_buf + pos, rem * sizeof(typename Base::value_type));
		}

		new(&(this->_buf[pos].first)) typename Base::key_type(value.first);
		new(&(this->_buf[pos].second)) typename Base::mapped_type(value.second);

		this->_size++;
		pair_base<typename Base::iterator, bool> pr(&(this->_buf[pos]), true);
		return pr;
	}
		
public:
	// �����������
	map_dynamic()
	{
		this->_size = 0;
		_capacity = 0;
		this->_buf = 0;
	}
	
	// ����������
	~map_dynamic()
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
			
	pair_base<typename Base::iterator, bool> insert(const typename Base::value_type& value)
	{
		unsigned int first = 0;
		unsigned int last = this->_size;
		unsigned int mid;
		
		typename Base::key_type k = value.first;
		
		if(this->_size == 0)
			return insert(0, value);
		else
			if(this->_buf[this->_size - 1].first < k)
				return insert(this->_size, value);
		
		while(first - last)
		{
			mid = first + ((last - first) >> 1);
			if(k <= this->_buf[mid].first)
				last = mid;
			else
				first = mid + 1;
		}
		
		if(this->_buf[last].first == value.first)
		{
			pair_base<typename Base::iterator, bool> pr(&(this->_buf[last]), false);
			return pr;
		}

		return insert(last, value);
	}
	
	// ������ �� �����
	typename Base::mapped_type& operator[](const typename Base::key_type& k)
	{
		typename Base::iterator iter = this->find(k);
		if(iter != this->end())
			return iter->second;

		typename Base::value_type val(k, typename Base::mapped_type());
		return insert(val).first->second;
	}

};

}
#endif // MAP_DYNAMIC_H













/* ������ ������� ����������
#ifndef MAP_DYNAMIC_H
#define MAP_DYNAMIC_H

#include <string.h>
#include <new>
#include "custom_destroyer.h"

namespace Rblib
{	
template<class T1, class T2>
class pair_dynamic
{
public:
	T1 first;
	T2 second;
	pair_dynamic(const T1 f, const T2 s) : first(f), second(s)
	{
	}
};
	
template<class Key, class T>
class map_dynamic
{
public:
	typedef Key key_type;
	typedef T mapped_type;
	typedef pair_dynamic<Key, T> value_type;
	typedef value_type* iterator; // ��������

protected:	
	unsigned int _size;
	unsigned int _capacity;
	iterator _buf;
		
protected:
	// ��������� ������� �� ����������� �������
	pair_dynamic<iterator, bool> insert(unsigned int pos, const value_type& value)
	{
		if(_size == _capacity)
		{
			unsigned int newSize;
			if(_size == 0)
				newSize = 1;
			else
				newSize = _size * 2;
			char* b = new(nothrow) char[newSize * sizeof(value_type)];
			iterator newBuf = (iterator)b;
			if(newBuf == 0)
			{
				newSize = _size + 1;
				b = new(nothrow) char[newSize * sizeof(value_type)];
				newBuf = (iterator)b;
				if(newBuf == 0) // ���� �� ������� �������� ������
				{
					pair_dynamic<iterator, bool> pr((iterator)&(_buf[pos]), false);
					return pr;
				}
			}			
			if(_capacity != 0)
			{
				::memcpy(newBuf, _buf, pos * sizeof(value_type));
				unsigned int rem = _size - pos; // ������� ��� �����������
				::memcpy(newBuf + pos + 1, _buf + pos, rem * sizeof(value_type));
				b = (char*)_buf;
				delete[] b;
			}
			_buf = newBuf;
			_capacity = newSize;
		}
		else
		{
			unsigned int rem = _size - pos; // ������� ��� �����������
			::memmove(_buf + pos + 1, _buf + pos, rem * sizeof(value_type));
		}

		new(&(_buf[pos].first)) key_type(value.first);
		new(&(_buf[pos].second)) mapped_type(value.second);

		_size++;
		pair_dynamic<iterator, bool> pr(&(_buf[pos]), true);
		return pr;
	}
		
public:
	// �����������
	map_dynamic()
	{
		_size = 0;
		_capacity = 0;
		_buf = 0;
	}
	
	// ����������
	~map_dynamic()
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
			
	pair_dynamic<iterator, bool> insert(const value_type& value)
	{
		unsigned int first = 0;
		unsigned int last = _size;
		unsigned int mid;
		
		key_type k = value.first;
		
		if(_size == 0)
			return insert(0, value);
		else
			if(_buf[_size - 1].first < k)
				return insert(_size, value);
		
		while(first - last)
		{
			mid = first + ((last - first) >> 1);
			if(k <= _buf[mid].first)
				last = mid;
			else
				first = mid + 1;
		}
		
		if(_buf[last].first == value.first)
		{
			pair_dynamic<iterator, bool> pr(&(_buf[last]), false);
			return pr;
		}

		return insert(last, value);
	}
	
	
	// ������ �� �����
	mapped_type& operator[](const key_type& k)
	{
		iterator iter = find(k);
		if(iter != end())
			return iter->second;

		value_type val(k, mapped_type());
		return insert(val).first->second;
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
	
	// ������� ��� �������� ����������
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
#endif // MAP_DYNAMIC_H*/
