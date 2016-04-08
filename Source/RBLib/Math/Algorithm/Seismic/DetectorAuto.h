///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - определение автомобиля
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORAUTO_H
#define MATH_ALGORITHM_DETECTORAUTO_H

#include "../AlgorithmBlockBase.h"
#include "DetectorAuto/AmplitudeFrequencyCharacteristic.h"
#include "DetectorAuto/AnalyzeAmplitudeFrequencyCharacteristic.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
	template <
				typename valueTypeIn	// тип входных значений
			 >
	class DetectorAuto : public AlgorithmBlockBase<valueTypeIn, bool>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef bool ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		
		static const unsigned int SpectralContrastThreshold = 80;
		static const unsigned int QuantityFilter = 30;	// количество полосовых фильтров
		typedef DetectorAutoParts::AmplitudeFrequencyCharacteristic<ValueTypeIn, QuantityFilter> AmplitudeFrequencyCharacteristicType;	//проверить тип ValueTypeIn для выходных значений
		typedef DetectorAutoParts::AnalyzeAmplFreqCharacteristic<float, QuantityFilter> AnalyzeAmplFreqCharacteristicType;
		typedef CallbackWrapper<float, float, float> TestPointCallbackType;
	public:
		AmplitudeFrequencyCharacteristicType AmplitudeFrequencyCharacteristic;
		AnalyzeAmplFreqCharacteristicType AnalyzeAmplFreqCharacteristic;
		TestPointCallbackType TestPoint;
	public:
		DetectorAuto()
		{
			Reset();
			
			AmplitudeFrequencyCharacteristic.Out.Set(AnalyzeAmplFreqCharacteristic.InCallback, &AnalyzeAmplFreqCharacteristic);
			AnalyzeAmplFreqCharacteristic.Out.Set(AnalyzeResultCallback, this);
		}
		
		virtual ~DetectorAuto()
		{
		}

		virtual void In(ValueTypeIn sample)
		{
			AmplitudeFrequencyCharacteristic.In(sample);
		}
		
		virtual void Reset()
		{
			Base::Reset();
		}
	protected:
		static void AnalyzeResultCallback(void * callbackParam, typename AnalyzeAmplFreqCharacteristicType::ValueTypeOut val)
		{
			DetectorAuto *im = (DetectorAuto *)callbackParam;
			if (!im)
			{
				return;
			}
			
			if (val.MeanLevel > 0)
			{
				
				float spectralContrast = val.MaxContrast / val.MeanLevel;
				if(spectralContrast < 0)
				{
					spectralContrast = 0;
				}
				
				im->TestPoint.Call(val.MeanLevel, val.MaxContrast, spectralContrast);
				
				// переводим отношение величин в проценты и сравниваем с порогом
				bool result = spectralContrast * 100.0 > SpectralContrastThreshold;
				
				im->Output(result);
			}
			else
			{
				im->TestPoint.Call(val.MeanLevel, val.MaxContrast, 0);
			}
		}

	};
}
}
}
#endif