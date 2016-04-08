///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - фильтн низких частот
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_LOWFREQUENCYFILTER_H
#define MATH_ALGORITHM_LOWFREQUENCYFILTER_H

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
	class LowFrequencyFilter : public AlgorithmBlockBase<valueTypeIn, valueTypeOut>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeOut ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		
		static const int FilterTimeConstant = 4;
		ValueTypeOut LastOutValue;
		ValueTypeOut CalculationOfIntermediate;
	protected:
	public:
		LowFrequencyFilter()
		{
			Reset();
		}
		
		virtual ~LowFrequencyFilter()
		{
		}
		virtual void Reset()
		{
			Base::Reset();
			LastOutValue = 0;
			CalculationOfIntermediate = 0;
		}
		
		virtual void In(ValueTypeIn val)
		{
			CalculationOfIntermediate *= (FilterTimeConstant - 1);
			CalculationOfIntermediate += val;
			CalculationOfIntermediate /= FilterTimeConstant;

			ValueTypeOut outValue = LastOutValue * (FilterTimeConstant - 1);
			outValue += CalculationOfIntermediate;
			outValue /= FilterTimeConstant;
			LastOutValue = outValue;
			
			Base::Output((ValueTypeOut)outValue);
		}
		
	protected:	

	};
}
}
}
#endif