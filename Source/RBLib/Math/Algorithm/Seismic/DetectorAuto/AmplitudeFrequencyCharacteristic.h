///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - полосовой фильтр
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORAUTO_BANDPASSFILTER_H
#define MATH_ALGORITHM_DETECTORAUTO_BANDPASSFILTER_H

#include "../../AlgorithmBlockBase.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
namespace DetectorAutoParts
{
	template <
				typename valueTypeIn,	// тип входных значений
				unsigned int quantityFilter	// количество фильтров
			 >
	class AmplitudeFrequencyCharacteristic : public AlgorithmBlockBase<valueTypeIn, float*>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef float* ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		
	public:
		static const unsigned int QuantityFilter = quantityFilter;
		static const int dimensionFrequency = 38;
		static const float filterAlphaConstant[dimensionFrequency];
		static const float filterBetaConstant ;
		static const int FilterTimeConstant = 128;	// постоянная затухания
		static const int TimeConstant = 8;	// постоянная затухания
		static const int OutputMultiplicity = 128;	// Выдавать результат каждые OutputMultiplicity входных сэмплов
		static const int ValueHistoryCount = 3;
	protected:
		int _outputCounter;

		ValueTypeIn _inValue[ValueHistoryCount];

		long int _outValue[ValueHistoryCount][QuantityFilter];
		float _maxValue[QuantityFilter];
		long int int_filterAlphaConstant[dimensionFrequency];
		long int int_filterBetaConstant ;

	public:
		AmplitudeFrequencyCharacteristic()
		{
			Reset();
		}
		
		virtual ~AmplitudeFrequencyCharacteristic()
		{
		}

		virtual void In(ValueTypeIn sample)
		{
			float signal = 0;
			
			if(_outputCounter == 0)
			{
				for(int i = 0; i < QuantityFilter; i++)
				{
					_maxValue[i] = 0;
				}

			}

			for(int i = (ValueHistoryCount - 1); i > 0; i--)
			{
				_inValue[i] = _inValue[i - 1];
			}
			_inValue[0] = sample;

			for(int i = 0; i < QuantityFilter; i++)
			{
				_outValue[2][i] = _outValue[1][i];
				_outValue[1][i] = _outValue[0][i];

				_outValue[0][i]  = _inValue[0];
				_outValue[0][i] -= _inValue[2];
				_outValue[0][i] -= (int_filterAlphaConstant[i] * _outValue[1][i] + int_filterBetaConstant * _outValue[2][i]) / 256;

				signal = _outValue[0][i] / TimeConstant;
				signal = signal < 0 ? -signal : signal;
				
				if(signal > _maxValue[i])
				{
					_maxValue[i] = signal;
				}
			}
			
			_outputCounter++;
			if (_outputCounter >= OutputMultiplicity)
			{
				_outputCounter = 0;
				
				Base::Output(_maxValue);
			}
		}
		
		virtual void Reset()
		{
			Base::Reset();
			
			for(int j = 0 ; j < ValueHistoryCount; j ++)
			{
				_inValue[j] = 0;
			}
			for(int i = 0 ; i < dimensionFrequency; i ++)
			{
				int_filterAlphaConstant[i] = filterAlphaConstant[i] * 256;
			}
			int_filterBetaConstant = filterBetaConstant * 256;
			for(int i = 0; i < QuantityFilter; i++)
			{
				for(int j = 0 ; j < ValueHistoryCount; j ++)
				{
					_outValue[j][i] = 0;
				}
				_maxValue[i] = 0;
			}
			_outputCounter = 0;
		}
	protected:

	};
	
	template <typename valueTypeIn, unsigned int quantityFilter>
		const float AmplitudeFrequencyCharacteristic<valueTypeIn, quantityFilter>::filterAlphaConstant[] ={	-1.8438, -1.8047, -1.7656, -1.7190, -1.6641, -1.6094, -1.5469, -1.4844,
																			-1.4141, -1.3438, -1.2656, -1.1875, -1.1094, -1.0313, -0.9453, -0.8516,
																			-0.7656, -0.6719, -0.5781, -0.4844, -0.3906, -0.2969, -0.1953, -0.1016,
																			-0.0039,  0.1016,  0.1953,  0.2969,  0.3906,  0.4844,  0.5781,  0.6719,
																			 0.7656,  0.8516,  0.9453,  1.0313,  1.1094,  1.1875};
	
	template <typename valueTypeIn, unsigned int quantityFilter>
		const float AmplitudeFrequencyCharacteristic<valueTypeIn, quantityFilter>::filterBetaConstant = 0.9922;
}
}
}
}
#endif