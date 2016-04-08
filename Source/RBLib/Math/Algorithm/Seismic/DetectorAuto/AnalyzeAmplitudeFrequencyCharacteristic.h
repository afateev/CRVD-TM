///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - анализ АЧХ
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORAUTO_ANALYZEAMPLITUDEFREQUENCYCHARACTERISTIC_H
#define MATH_ALGORITHM_DETECTORAUTO_ANALYZEAMPLITUDEFREQUENCYCHARACTERISTIC_H

#include "../../AlgorithmBlockBase.h"
//#include <io430f5659.h>

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
namespace DetectorAutoParts
{
	template <
				typename valueType	// тип входных значений
			 >
	struct AnalyzeAmplFreqCharacteristicStruct
		{
			valueType MeanLevel;
			valueType MaxContrast;
		};
	
	template <
				typename valueTypeIn,	// тип входных значений
				unsigned int quantityFilter	// количество фильтров
			 >
	class AnalyzeAmplFreqCharacteristic : public AlgorithmBlockBase<valueTypeIn*, AnalyzeAmplFreqCharacteristicStruct<valueTypeIn> >
	{
	public:
		typedef valueTypeIn *ValueTypeIn;
		typedef valueTypeIn ValueType;
		typedef AnalyzeAmplFreqCharacteristicStruct<ValueType> ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		typedef CallbackWrapper <float *, int> TestPointCallbackType;
	public:
		static const unsigned int QuantityFilter = quantityFilter;
		static const unsigned int TimeConstant = 16;
		static const unsigned int NormalyzeConstant = 4096; // нормировочная константа по максимуму сигнала
		static const int OutputDelay = 10;	// задержка выдачи результата (кратна периоду выдачи результата)
	public:
		TestPointCallbackType TestPoint;
	protected:
		/*ValueType*/ long int _buffer[QuantityFilter];	// буфер накопления с затуханием
		/*ValueType*/ long int _normalizedBuffer[QuantityFilter];
		int _outputDelayCounter;
	public:
		AnalyzeAmplFreqCharacteristic()
		{
			Reset();
		}
		
		virtual ~AnalyzeAmplFreqCharacteristic()
		{
		}

		virtual void In(ValueTypeIn buff)
		{
//			P4OUT |= BIT4;
			ValueType Max = 0;
			// накопление с затуханием в 16 периодов
			for(int i = 0; i < QuantityFilter; i++)
			{
				_buffer[i] -= _buffer[i] / TimeConstant;
				_buffer[i] += buff[i];
				if(_buffer[i] > Max)
				{
					Max = _buffer[i];
				}
			}
			
			/*ValueType*/ long int MeanLevel= 0;
			/*ValueType*/ long int MaxContrast = 0;
			
			if (Max > 0)
			{
				for(int i = 0; i < QuantityFilter; i++)
				{
					// нормализуем
					_normalizedBuffer[i] = (_buffer[i] * NormalyzeConstant) / Max;
					// сичтаем среднее
//					MeanLevel += _normalizedBuffer[i] / QuantityFilter;
				}
				
				for(int i = 2; i < (QuantityFilter - 2); i++)
				{
					/*ValueType*/ long int contrast = _normalizedBuffer[i] - ((_normalizedBuffer[i-2] + _normalizedBuffer[i+2]) / 2);
					MeanLevel += _normalizedBuffer[i];
					
					if(contrast > MaxContrast)
					{
						MaxContrast = contrast;
					}
				}
				MeanLevel = MeanLevel / (QuantityFilter - 2);
			}
			
//			_normalizedBuffer[QuantityFilter + 1] = MeanLevel;
//			_normalizedBuffer[QuantityFilter + 2] = MaxContrast > 1 ? 1 : MaxContrast;
			
			if (_outputDelayCounter >= OutputDelay)
			{
				ValueTypeOut OutValue;
				OutValue.MeanLevel = MeanLevel;
				OutValue.MaxContrast = MaxContrast;
				Base::Output(OutValue);
			}
			else
			{
				_outputDelayCounter++;
			}
//			P4OUT &= ~BIT4;
			
//			TestPoint.Call(_normalizedBuffer, QuantityFilter);
		}
		
		virtual void Reset()
		{
			Base::Reset();
			for(int i = 0; i < QuantityFilter; i++)
			{
				_buffer[i] = 0;
				_normalizedBuffer[i] = 0;
			}
			_outputDelayCounter = 0;
		}
	protected:

	};
	
}
}
}
}
#endif