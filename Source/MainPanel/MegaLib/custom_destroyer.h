#ifndef CUSTOM_DESTROYER_H
#define CUSTOM_DESTROYER_H

template<class T>
class custom_destroyer
{
public:
	static void destroy(T* p)
	{
		p->~T();
	}
};

#endif
