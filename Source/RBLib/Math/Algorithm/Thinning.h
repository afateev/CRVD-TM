///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - прореживание
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_THINNING_H
#define MATH_ALGORITHM_THINNING_H

#include "AlgorithmBlockBase.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
	template <
				typename valueTypeIn,	// тип входных значений
		  		typename valueTypeOut,	// тип выходных значений
				int countThinning			// значение величины прореживания
			 >
	class Thinning : public AlgorithmBlockBase<valueTypeIn, valueTypeOut>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeOut ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		static const int CountThinning = countThinning;
	protected:
		unsigned char _index;	// индекс элемента в буфере
	public:
		Thinning() : _index(0)
		{
			Reset();
		}
		
		virtual ~Thinning()
		{
		}
		
		virtual void In(ValueTypeIn val)
		{
			_index ++;
			if(_index >= countThinning)
			{
				_index = 0;
				ValueTypeOut _valOut = (ValueTypeOut) val;
				Base::Output(_valOut);
			}
		}
		
		virtual void Reset()
		{
			Base::Reset();
			_index = 0;
		}
	protected:	

	};
}
}
}
#endif