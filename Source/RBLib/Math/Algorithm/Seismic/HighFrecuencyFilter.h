///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - фильтн низких частот
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_HIGHFREQUENCYFILTER_H
#define MATH_ALGORITHM_HIGHFREQUENCYFILTER_H

#include "../AlgorithmBlockBase.h"

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
	class HighFrequencyFilter : public AlgorithmBlockBase<valueTypeIn, valueTypeOut>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeOut ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		
		ValueTypeOut LastOutValue;
		ValueTypeIn LastInValue;
	protected:
	public:
		HighFrequencyFilter()
		{
			Reset();
		}
		
		virtual ~HighFrequencyFilter()
		{
		}
		virtual void Reset()
		{
			Base::Reset();
			LastOutValue = 0;
			LastInValue = 0;
		}
		
		virtual void In(ValueTypeIn val)
		{
			ValueTypeOut outValue = val - LastInValue + LastOutValue;
			outValue *= 4.0;
			outValue -= LastOutValue;
			outValue /= 4.0;

			LastInValue = val;
			LastOutValue = outValue;
			
			Base::Output(outValue);
		}
		
	protected:	

	};
}
}
}
#endif