///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - определение человека
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORHUMAN_H
#define MATH_ALGORITHM_DETECTORHUMAN_H

#include "../AlgorithmBlockBase.h"
#include "DetectorHuman/LowFrequencyFilter.h"
#include "DetectorHuman/CalcAutocorrelationFunction.h"
#include "DetectorHuman/AnalyzeAutocorrelationFunction.h"

namespace _PrivateBuffProvider
{
	template<class SampleType>
	class BuffProvider;
	
	template<>
	class BuffProvider<unsigned char>
	{
	public:
	  typedef unsigned int BuffType;
	};
	
	template<>
	class BuffProvider<char>
	{
	public:
	  typedef int BuffType;
	};
	
	template<>
	class BuffProvider<unsigned short>
	{
	public:
	  typedef unsigned long BuffType;
	};
	
	template<>
	class BuffProvider<short>
	{
	public:
	  typedef long BuffType;
	};
	  
	template<>
	class BuffProvider<unsigned int>
	{
	public:
	  typedef unsigned long BuffType;
	};
	
	template<>
	class BuffProvider<int>
	{
	public:
	  typedef long BuffType;
	};
	
	template<>
	class BuffProvider<long>
	{
	public:
	  typedef long long BuffType;
	};
	
	template<>
	class BuffProvider<float>
	{
	public:
	  typedef double BuffType;
	};
	
	template<>  
	class BuffProvider<double>
	{
	public:
	  typedef double BuffType;
	};
	
}
namespace Rblib
{
namespace Math
{
namespace Algorithm
{
	template <
				typename valueTypeIn	// тип входных значений
			 >
	class DetectorHuman : public AlgorithmBlockBase<valueTypeIn, bool>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef bool ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;

		static const unsigned int BufferSize = 64;
		typedef DetectorHumanParts::LowFrequencyFilter<ValueTypeIn, BufferSize> LowFrequencyFilterType;
		typedef DetectorHumanParts::CalcAutocorrelationFunction<ValueTypeIn, typename _PrivateBuffProvider::BuffProvider<ValueTypeIn>::BuffType/* ValueTypeIn unsigned long int*/, BufferSize> CalcAutocorrelationFunctionType;
		typedef DetectorHumanParts::AnalyzeAutocorrelationFunction<typename _PrivateBuffProvider::BuffProvider<ValueTypeIn>::BuffType*/* ValueTypeIn unsigned long int* */,typename _PrivateBuffProvider::BuffProvider<ValueTypeIn>::BuffType/*ValueTypeIn*/, BufferSize> AnalyzeAutocorrelationFunctionType;

	public:
		LowFrequencyFilterType LowFrequencyFilter;
		CalcAutocorrelationFunctionType CalcAutocorrelationFunction;
		AnalyzeAutocorrelationFunctionType AnalyzeAutocorrelationFunction;
		ValueTypeIn _lastSample;

	protected:
		bool HumanFlag;
	public:
		DetectorHuman()
		{
			Reset();
			
			LowFrequencyFilter.Out.Set(LowFrequencyFilterOutCallback, this);
			CalcAutocorrelationFunction.Out.Set(AnalyzeAutocorrelationFunction.InCallback, &AnalyzeAutocorrelationFunction);
			AnalyzeAutocorrelationFunction.Out.Set(DetectorHuman::Output, this);
		}
		
		virtual ~DetectorHuman()
		{
		}

		virtual void In(ValueTypeIn sample)
		{
			_lastSample = sample;
			LowFrequencyFilter.In(sample);
		}
		
		virtual void Reset()
		{
			Base::Reset();
			LowFrequencyFilter.Reset();
			CalcAutocorrelationFunction.Reset();
			AnalyzeAutocorrelationFunction.Reset();
		}
	protected:
		static void LowFrequencyFilterOutCallback(void * callbackParam, typename LowFrequencyFilterType::ValueTypeOut val)
		{
			DetectorHuman *im = (DetectorHuman *)callbackParam;
			if (!im)
			{
				return;
			}
			
			typename CalcAutocorrelationFunctionType::ValueTypeIn in;
			in.ClipBuffer = val.ClipBuffer;
			in.Index = val.Index;
			in.Sample = im->_lastSample;
			
			im->CalcAutocorrelationFunction.In(in);
		}


	};
}
}
}
#endif