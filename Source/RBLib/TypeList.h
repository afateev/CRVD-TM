///////////////////////////////////////////////////////////////////////////////
//	Список типов
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TYPE_LIST_H
#define TYPE_LIST_H

#include "NullType.h"

namespace Rblib
{
	//////////
	//////////
	
	template <class T, class U>
	struct TypeList
	{
		typedef T Head;
		typedef U Tail;
	};
	
	//////////
	//////////
	
	template
	<
		typename T1 = NullType,
		typename T2 = NullType,
		typename T3 = NullType,
		typename T4 = NullType,
		typename T5 = NullType
	> 
	struct MakeTypeList
	{
	private:
		typedef typename MakeTypeList
		<
			T2,
			T3,
			T4,
			T5
		>
		::Result TailResult;
	public:
		typedef TypeList<T1, TailResult> Result;
	};
	
	template<>
	struct MakeTypeList<>
	{
		typedef NullType Result;
	};
	
	////////
	////////
	
	template <class TList, unsigned int index> struct TypeAt;
	
	template <class Head, class Tail>
	struct TypeAt<TypeList<Head, Tail>, 0>
	{
		typedef Head Result;
	};
	
	template <class Head, class Tail, unsigned int i>
	struct TypeAt<TypeList<Head, Tail>, i>
	{
		typedef typename TypeAt<Tail, i - 1>::Result Result;
	};
	
	////////
	////////
	
	template <class TList, unsigned int index,
	    typename DefaultType = NullType>
	struct TypeAtNonStrict
	{
		typedef DefaultType Result;
	};
	
	template <class Head, class Tail, typename DefaultType>
	struct TypeAtNonStrict<TypeList<Head, Tail>, 0, DefaultType>
	{
		typedef Head Result;
	};
	
	template <class Head, class Tail, unsigned int i, typename DefaultType>
	struct TypeAtNonStrict<TypeList<Head, Tail>, i, DefaultType>
	{
		typedef typename
			TypeAtNonStrict<Tail, i - 1, DefaultType>::Result Result;
	};
}

#endif // TYPE_LIST_H