///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - трех ступенчатый фильтр
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORPELENG_THREESTEPFILTER_H
#define MATH_ALGORITHM_DETECTORPELENG_THREESTEPFILTER_H

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
				typename valueTypeIn	// тип входных значений
			 >
	class ThreeStepFilter : public AlgorithmBlockBase<valueTypeIn*, signed short*>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeIn * ValueTypeInSample;
		typedef signed short* ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeInSample, ValueTypeOut> Base;
		
	public:
		static const float Gain;
		static const signed short Scale[3];
		static const signed short CoeffA0[3];
		static const signed short CoeffA1[3];
		static const signed short CoeffB[3];
	protected:
		signed long int _inputSignalBuff1s[3][3];		// буфер для хранения значений сигнала до фильтрации
		signed long int _outputSignalBuff1s[3][3];	// буфер для хранения значений сигнала после фильтрации

		signed long int _inputSignalBuff2s[3][3];		// буфер для хранения значений сигнала до фильтрации
		signed long int _outputSignalBuff2s[3][3];	// буфер для хранения значений сигнала после фильтрации

		signed long int _inputSignalBuff3s[3][3];		// буфер для хранения значений сигнала до фильтрации
		signed long int _outputSignalBuff3s[3][3];	// буфер для хранения значений сигнала после фильтрации
		int _1StadiCompleteCount;
		int _2StadiCompleteCount;
		int _3StadiCompleteCount;
	public:
		ThreeStepFilter()
		{
			Reset();
		}
		
		virtual ~ThreeStepFilter()
		{
		}

		virtual void In(ValueTypeInSample samples)
		{
			///////////////////////////
			///  первая ступень  //////
			///////////////////////////
			for(int i = 2; i > 0; i--)
			{
				for(int j = 0; j < 3; j++)
				{
					_inputSignalBuff1s[j][i] = _inputSignalBuff1s[j][i-1];
					_outputSignalBuff1s[j][i] = _outputSignalBuff1s[j][i-1];
				}
			}
			for(int i = 0; i < 3; i++)
			{
				_inputSignalBuff1s[i][0] = samples[i];
			}
			_1StadiCompleteCount ++;
			for(int i = 0; i < 3; i++)
			{
				_outputSignalBuff1s[i][0]  = CoeffB[0] * _inputSignalBuff1s[i][0];
				_outputSignalBuff1s[i][0] += CoeffB[1] * _inputSignalBuff1s[i][1];
				_outputSignalBuff1s[i][0] += CoeffB[2] * _inputSignalBuff1s[i][2];
				_outputSignalBuff1s[i][0] -= CoeffA0[0] * _outputSignalBuff1s[i][1];
				_outputSignalBuff1s[i][0] -= CoeffA1[0] * _outputSignalBuff1s[i][2];
				_outputSignalBuff1s[i][0] /= Scale[i];
			}	
			if(_1StadiCompleteCount >= 3)
			{
				_1StadiCompleteCount = 3;
			}
			else
			{
				return;
			}
			///////////////////////////
			///  вторая ступень  //////
			///////////////////////////
			for(int i = 2; i > 0; i--)
			{
				for(int j = 0; j < 3; j++)
				{
					_inputSignalBuff2s[j][i] = _inputSignalBuff2s[j][i-1];
					_outputSignalBuff2s[j][i] = _outputSignalBuff2s[j][i-1];
				}
			}
			for(int i = 0; i < 3; i++)
			{
				_inputSignalBuff2s[i][0] = _outputSignalBuff1s[i][0];
			}
			_2StadiCompleteCount ++;
			for(int i = 0; i < 3; i++)
			{
				_outputSignalBuff2s[i][0]  = CoeffB[0] * _inputSignalBuff2s[i][0];
				_outputSignalBuff2s[i][0] += CoeffB[1] * _inputSignalBuff2s[i][1];
				_outputSignalBuff2s[i][0] += CoeffB[2] * _inputSignalBuff2s[i][2];
				_outputSignalBuff2s[i][0] -= CoeffA0[1] * _outputSignalBuff2s[i][1];
				_outputSignalBuff2s[i][0] -= CoeffA1[1] * _outputSignalBuff2s[i][2];
				_outputSignalBuff2s[i][0] /= Scale[i];
			}	
			if(_2StadiCompleteCount >= 3)
			{
				_2StadiCompleteCount = 3;
			}
			else
			{
				return;
			}

			///////////////////////////
			///  третья ступень  //////
			///////////////////////////
			for(int i = 2; i > 0; i--)
			{
				for(int j = 0; j < 3; j++)
				{
					_inputSignalBuff3s[j][i] = _inputSignalBuff3s[j][i-1];
					_outputSignalBuff3s[j][i] = _outputSignalBuff3s[j][i-1];
				}
			}
			for(int i = 0; i < 3; i++)
			{
				_inputSignalBuff3s[i][0] = _outputSignalBuff2s[i][0];
			}
			_3StadiCompleteCount ++;
			for(int i = 0; i < 3; i++)
			{
				_outputSignalBuff3s[i][0]  = CoeffB[0] * _inputSignalBuff3s[i][0];
				_outputSignalBuff3s[i][0] += CoeffB[1] * _inputSignalBuff3s[i][1];
				_outputSignalBuff3s[i][0] += CoeffB[2] * _inputSignalBuff3s[i][2];
				_outputSignalBuff3s[i][0] -= CoeffA0[2] * _outputSignalBuff3s[i][1];
				_outputSignalBuff3s[i][0] -= CoeffA1[2] * _outputSignalBuff3s[i][2];
				_outputSignalBuff3s[i][0] /= Scale[i];
			}	
			if(_3StadiCompleteCount >= 3)
			{
				_3StadiCompleteCount = 3;
			}
			else
			{
				return;
			}


			signed short outValue[3];
			outValue[0] = (signed short)(_outputSignalBuff3s[0][0] * Gain);
			outValue[1] = (signed short)(_outputSignalBuff3s[1][0] * Gain);
			outValue[2] = (signed short)(_outputSignalBuff3s[2][0] * Gain);

			Base::Output(outValue);
   		}
		
		virtual void Reset()
		{
			Base::Reset();
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					_inputSignalBuff1s[i][j] = 0;
					_outputSignalBuff1s[i][j] = 0;
					_inputSignalBuff2s[i][j] = 0;
					_outputSignalBuff2s[i][j] = 0;
					_inputSignalBuff3s[i][j] = 0;
					_outputSignalBuff3s[i][j] = 0;
				}
			}
			_1StadiCompleteCount = 0;
			_2StadiCompleteCount = 0;
			_3StadiCompleteCount = 0;

		}
	protected:
	};
	
	template <typename valueTypeIn>
	const signed short ThreeStepFilter<valueTypeIn>::Scale[] = {1.0 * 256, 1.0 * 256, 1.0 * 256};

	template <typename valueTypeIn>
	const float ThreeStepFilter<valueTypeIn>::Gain = 0.0755;

	template <typename valueTypeIn>
	const signed short ThreeStepFilter<valueTypeIn>::CoeffA0[] ={-0.541758 * 256, -0.404484 * 256, -0.352863 * 256};

	template <typename valueTypeIn>
	const signed short ThreeStepFilter<valueTypeIn>::CoeffA1[] ={0.608118 * 256, 0.200644 * 256, 0.047415 * 256};

	template <typename valueTypeIn>
	const signed short ThreeStepFilter<valueTypeIn>::CoeffB[] ={1.0 * 256, -2.0 * 256, 1.0 * 256};

}
}
}
}
#endif