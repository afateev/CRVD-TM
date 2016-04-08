///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - определение пеленга
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORPELENG_H
#define MATH_ALGORITHM_DETECTORPELENG_H

#include "../AlgorithmBlockBase.h"
#include "DetectorPeleng/PelengStepSector.h"
#include "DetectorPeleng/AnalyzePelengSector.h"
#include "DetectorPeleng/ThreeStepFilter.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
	struct DetectorPelengOutStruct
	{
		unsigned short Peleng;
		unsigned short PelengError;
		
		DetectorPelengOutStruct()
		{
			Reset();
		}
		
		void Reset()
		{
			Peleng = 0;
			PelengError = 0;
		}

	};

	template <
				typename valueTypeIn	// тип входных значений
			 >
	class DetectorPeleng : public AlgorithmBlockBase<valueTypeIn*, DetectorPelengOutStruct>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeIn* InputValueType;
		typedef DetectorPelengOutStruct ValueTypeOut;
		typedef AlgorithmBlockBase<InputValueType, ValueTypeOut> Base;

		typedef DetectorPelengParts::ThreeStepFilter<ValueTypeIn> ThreeStepFilterType;
		typedef DetectorPelengParts::PelengStepSector<ValueTypeIn> PelengStepSectorType;
		typedef DetectorPelengParts::AnalyzePelengSector<typename PelengStepSectorType::ValueTypeOut> AnalyzePelengSectorType;
		
	public:
		ThreeStepFilterType ThreeStepFilter;
		PelengStepSectorType PelengStepSector;
		AnalyzePelengSectorType AnalyzePelengSector;
	protected:
	public:
		DetectorPeleng()
		{
			Reset();
			ThreeStepFilter.Out.Set(PelengStepSector.InCallback, &PelengStepSector);
			PelengStepSector.Out.Set(AnalyzePelengSector.InCallback, &AnalyzePelengSector);
			AnalyzePelengSector.Out.Set(AnalyzePelengSectorCallback, this);
		}
		
		virtual ~DetectorPeleng()
		{
		}

		virtual void In(InputValueType sample)
		{
			//ThreeStepFilter.In(sample);
			PelengStepSector.In(sample);
		}
		
		virtual void Reset()
		{
			Base::Reset();
		}
	protected:
		static void AnalyzePelengSectorCallback(void *callbackParam, typename AnalyzePelengSectorType::ValueTypeOut val)
		{
			DetectorPeleng *im = (DetectorPeleng *)callbackParam;
			if (!im)
			{
				return;
			}
			ValueTypeOut OutValue;
			OutValue.Peleng = val.Peleng;
			OutValue.PelengError = val.PelengError;
			im-> Output(OutValue);
		}
	};
}
}
}
#endif