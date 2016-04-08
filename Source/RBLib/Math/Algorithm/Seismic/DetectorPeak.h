///////////////////////////////////////////////////////////////////////////////
//	���� ��������� ��������� ������� - ������� ��������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORPEAK_H
#define MATH_ALGORITHM_DETECTORPEAK_H

#include "../AlgorithmBlockBase.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
	template <
				typename valueTypeIn
			 >
	class DetectorPeak : public AlgorithmBlockBase<valueTypeIn, float>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef float ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		
		static const int TimeConstant = 64;
		static const int OutputMultiplicity = 16;		// ��������� ������ ������
		static const int MaxSignalValue = 1000;

	protected:
		float CalculationOfIntermediate;		// ��������������� ���������� ��� ����������
		ValueTypeOut PeakLevel;							// ������� �������� ���������
		int _outputCounter;
	public:
		DetectorPeak()
		{
			Reset();
		}
		
		virtual ~DetectorPeak()
		{
		}

		virtual void In(ValueTypeIn sample)
		{
			if(sample > PeakLevel)
			{
				PeakLevel = sample;
				CalculationOfIntermediate = sample * TimeConstant;
			}
			else	// ���������
			{
				CalculationOfIntermediate -= (CalculationOfIntermediate / TimeConstant);
				PeakLevel = CalculationOfIntermediate / TimeConstant;
			}
			_outputCounter++;
			if (_outputCounter >= OutputMultiplicity)
			{
				_outputCounter = 0;
				
				Base::Output(PeakLevel);
			}
		}
		
		virtual void Reset()
		{
			Base::Reset();
			CalculationOfIntermediate = 0;
			PeakLevel = 0;
			_outputCounter = 0;
		}
	protected:
	};

}
}
}
#endif