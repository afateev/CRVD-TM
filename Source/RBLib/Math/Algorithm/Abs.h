///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - выпрямление
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_ABS_H
#define MATH_ALGORITHM_ABS_H

#include "AlgorithmBlockBase.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
	template <
				typename valueTypeIn,	// тип входных значений
		  		typename valueTypeOut	// тип выходных значений
			 >
	class Abs : public AlgorithmBlockBase<valueTypeIn, valueTypeOut>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeOut ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
	protected:
	public:
		Abs()
		{
			Reset();
		}
		
		virtual ~Abs()
		{
		}
		
		virtual void In(ValueTypeIn val)
		{
			ValueTypeOut _valOut = (ValueTypeOut) (val < 0 ? -val : val);
			Base::Output(_valOut);
		}
		
		virtual void Reset()
		{
			Base::Reset();
		}
	protected:	

	};
}
}
}
#endif