///////////////////////////////////////////////////////////////////////////////
//	–еализаци€ скал€рного сейсмического алгоритма
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_SEISMIC_PELENGALGORITHM_H
#define MATH_ALGORITHM_SEISMIC_PELENGALGORITHM_H

#include "../AlgorithmBlockBase.h"
#include "../MovingAverage.h"
#include "../Thinning.h"
#include "DetectorPeleng.h"
#include "DetectorPeleng/HighFrecuencyFilter.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
namespace Seismic
{
	struct PelengAlgorithmOutputStruct
	{
		unsigned short Peleng;
		unsigned short PelengError;
		
		PelengAlgorithmOutputStruct()
		{
			Reset();
		}
		
		void Reset()
		{
			Peleng = 0;
			PelengError = 0;
		}
	};
	
	class PelengAlgorithm : public AlgorithmBlockBase<unsigned short*, PelengAlgorithmOutputStruct>
	{
	public:
		typedef unsigned short SampleValueType;
		typedef SampleValueType* InSampleValueType;
		typedef PelengAlgorithmOutputStruct ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		
		typedef MovingAverage<SampleValueType, SampleValueType, 4> MovingAverage4Type;
		typedef MovingAverage<SampleValueType, SampleValueType, 7> MovingAverage7Type;
		typedef MovingAverage<SampleValueType, SampleValueType, 10> MovingAverage10Type;
		typedef Thinning<SampleValueType, SampleValueType, 4> Thinning4Type;
		typedef DetectorPelengParts::HighFrecuencyFilter<SampleValueType, signed short> HighFrecuencyFilterType;
		typedef DetectorPeleng< signed short> DetectorPelengType;
		
	public:
		MovingAverage4Type MovingAverage4[3];
		MovingAverage7Type MovingAverage7[3];
		MovingAverage10Type MovingAverage10[3];
		Thinning4Type Thinning4[3];
		DetectorPelengType DetectorPeleng;
		HighFrecuencyFilterType HighFrecuencyFilter;
	public:
	protected:
		ValueTypeOut _currentOutputValue;
		SampleValueType _samples[3];
	public:
		PelengAlgorithm()
		{
			Reset();
			
			MovingAverage4[0].Out.Set(MovingAverage7[0].InCallback, &MovingAverage7[0]);
			MovingAverage4[1].Out.Set(MovingAverage7[1].InCallback, &MovingAverage7[1]);
			MovingAverage4[2].Out.Set(MovingAverage7[2].InCallback, &MovingAverage7[2]);
			MovingAverage7[0].Out.Set(MovingAverage10[0].InCallback, &MovingAverage10[0]);
			MovingAverage7[1].Out.Set(MovingAverage10[1].InCallback, &MovingAverage10[1]);
			MovingAverage7[2].Out.Set(MovingAverage10[2].InCallback, &MovingAverage10[2]);
			MovingAverage10[0].Out.Set(Thinning4[0].InCallback, &Thinning4[0]);
			MovingAverage10[1].Out.Set(Thinning4[1].InCallback, &Thinning4[1]);
			MovingAverage10[2].Out.Set(Thinning4[2].InCallback, &Thinning4[2]);
			Thinning4[0].Out.Set(Thinning4Callback0, this);
			Thinning4[1].Out.Set(Thinning4Callback1, this);
			Thinning4[2].Out.Set(Thinning4Callback2, this);
			HighFrecuencyFilter.Out.Set(DetectorPeleng.InCallback, &DetectorPeleng);
			DetectorPeleng.Out.Set(DetectorPelengCallback, this);
		}
		
		virtual void In(unsigned short * sample)
		{
			MovingAverage4[0].In((SampleValueType)sample[0] * 10);
			MovingAverage4[1].In((SampleValueType)sample[1] * 10);
			MovingAverage4[2].In((SampleValueType)sample[2] * 10);
		}
		
		virtual void Reset()
		{
			Base::Reset();
			for(int i = 0; i < 3; i++)
			{
				MovingAverage4[i].Reset();
				MovingAverage7[i].Reset();
				MovingAverage10[i].Reset();
				Thinning4[i].Reset();
			}
			HighFrecuencyFilter.Reset();
			DetectorPeleng.Reset();

			_currentOutputValue.Reset();
		}
	protected:
		static void Thinning4Callback0(void *callbackParam, SampleValueType val)
		{
			PelengAlgorithm *im = (PelengAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im-> _samples[0] = val;
		}

		static void Thinning4Callback1(void *callbackParam, SampleValueType val)
		{
			PelengAlgorithm *im = (PelengAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im-> _samples[1] = val;
		}
		
		static void Thinning4Callback2(void *callbackParam, SampleValueType val)
		{
			PelengAlgorithm *im = (PelengAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im-> _samples[2] = val;
			im-> HighFrecuencyFilter.In(im->_samples);
			//im-> DetectorPeleng.In(im-> _samples);
		}
		
		static void DetectorPelengCallback(void *callbackParam, DetectorPelengType::ValueTypeOut val)
		{
			PelengAlgorithm *im = (PelengAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			im-> _currentOutputValue.Peleng = val.Peleng;
			im-> _currentOutputValue.PelengError = val.PelengError;
			im-> Output(im-> _currentOutputValue);
		}
	};
}
}
}
}

#endif