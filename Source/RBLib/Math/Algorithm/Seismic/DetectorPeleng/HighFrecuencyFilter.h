///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - фильтр высоких частот, усредняет сигнал (что бы не отнимать 2048)
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORPELENG_HIGHFRECUENCYFILTER_H
#define MATH_ALGORITHM_DETECTORPELENG_HIGHFRECUENCYFILTER_H

#include "../../AlgorithmBlockBase.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
namespace DetectorPelengParts
{
	template <
				typename valueTypeIn,	// тип входных значений
				typename valueTypeOut
			 >
	class HighFrecuencyFilter : public AlgorithmBlockBase<valueTypeIn*,valueTypeOut*>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeIn * ValueTypeInSample;
		typedef valueTypeOut * ValueTypeOut;
		typedef valueTypeOut ValueTypeOutSample;
		typedef AlgorithmBlockBase<ValueTypeInSample, ValueTypeOut> Base;
		
	public:
		static const signed short Scale = (signed short)(1.0 * 256.0);
		static const signed short CoeffA0 = (signed short)(-1.861361 * 256.0);
		static const signed short CoeffA1 = (signed short)(0.870367 * 256.0);
		static const signed short CoeffB0 = 256;
		static const signed short CoeffB1 = -512;
		static const signed short CoeffB2 = 256;
	protected:
		signed long int _inputSignalBuff[3][3];		// буфер для хранения значений сигнала до фильтрации
		signed long int _outputSignalBuff[3][3];	// буфер для хранения значений сигнала после фильтрации

	public:
		HighFrecuencyFilter()
		{
			Reset();
		}
		
		virtual ~HighFrecuencyFilter()
		{
		}

		virtual void In(ValueTypeInSample samples)
		{
			for(int i = 2; i > 0; i--)
			{
				for(int j = 0; j < 3; j++)
				{
					_inputSignalBuff[j][i] = _inputSignalBuff[j][i-1];
					_outputSignalBuff[j][i] = _outputSignalBuff[j][i-1];
				}
			}
			for(int i = 0; i < 3; i++)
			{
				_inputSignalBuff[i][0] = samples[i];
			}
			for(int i = 0; i < 3; i++)
			{
				_outputSignalBuff[i][0]  = CoeffB0 * _inputSignalBuff[i][0];
				_outputSignalBuff[i][0] += CoeffB1 * _inputSignalBuff[i][1];
				_outputSignalBuff[i][0] += CoeffB2 * _inputSignalBuff[i][2];
				_outputSignalBuff[i][0] -= CoeffA0 * _outputSignalBuff[i][1];
				_outputSignalBuff[i][0] -= CoeffA1 * _outputSignalBuff[i][2];
				_outputSignalBuff[i][0] /= Scale;
			}	
			ValueTypeOutSample outValue[3];
			outValue[0] = (ValueTypeOutSample)(_outputSignalBuff[0][0]);
			outValue[1] = (ValueTypeOutSample)(_outputSignalBuff[1][0]);
			outValue[2] = (ValueTypeOutSample)(_outputSignalBuff[2][0]);

			Base::Output(outValue);
   		}
		
		virtual void Reset()
		{
			Base::Reset();
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					_inputSignalBuff[i][j] = 0.0;
					_outputSignalBuff[i][j] = 0.0;
				}
			}
		}
	protected:
	};
}
}
}
}
#endif