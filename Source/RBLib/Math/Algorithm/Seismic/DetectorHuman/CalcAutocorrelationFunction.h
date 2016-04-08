///////////////////////////////////////////////////////////////////////////////
//	���� ��������� ��������� ������� - ������ ������� ��������������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORHUMAN_CALCAUTOCORRELATIONFUNKTION_H
#define MATH_ALGORITHM_DETECTORHUMAN_CALCAUTOCORRELATIONFUNKTION_H

#include "../../AlgorithmBlockBase.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
namespace DetectorHumanParts
{
	template <
				typename valueType	// ��� ��������
			 >
	struct CalcAutocorrelationFunctionOutStruct
		{
			bool *ClipBuffer;
			unsigned int Index;
			valueType Sample;
		};
	
	template <
				typename valueTypeIn,	// ��� ������� ��������
		  		typename valueTypeOut,	// ��� �������� ��������
				int bufferSize			// ������ ������
			 >
	class CalcAutocorrelationFunction : public AlgorithmBlockBase<CalcAutocorrelationFunctionOutStruct<valueTypeIn>, valueTypeOut *>
	{
	public:
		typedef CalcAutocorrelationFunctionOutStruct<valueTypeIn> ValueTypeIn;
		typedef valueTypeOut *ValueTypeOut;
		typedef valueTypeOut ValueOutType;

		typedef AlgorithmBlockBase<CalcAutocorrelationFunctionOutStruct<valueTypeIn>,ValueTypeOut> Base;

		static const int BufferSize = bufferSize;	// ������ ������ ����������
		static const int FilterTimeConstant = 128;	// ���������� ��������� ������������������ �������
		static const int OutputMultiplicity = 16;	// �������� ��������� ������ OutputMultiplicity ������� �������
	protected:
		valueTypeOut _buffer[BufferSize];	// ����� �������� ��������� ����������
		int _outputCounter;
	public:
		CalcAutocorrelationFunction()
		{
			Reset();
		}
		
		virtual ~CalcAutocorrelationFunction()
		{
		}

		virtual void In(CalcAutocorrelationFunctionOutStruct<valueTypeIn> inStruct)
		{
			int clipIndex = inStruct.Index;
			
			for (int i = 0; i < bufferSize; i++, clipIndex --)
			{
				_buffer[i] -= _buffer[i] / FilterTimeConstant;
				
				if(inStruct.ClipBuffer[clipIndex])
				{
					_buffer[i] += inStruct.Sample * FilterTimeConstant;
				}

				if(clipIndex < 0)
				{
					clipIndex = BufferSize - 1;
				}
			}
			
			_outputCounter++;
			if (_outputCounter >= OutputMultiplicity)
			{
				_outputCounter = 0;
				
				Base::Output(_buffer);
			}
		}
		
		virtual void Reset()
		{
			Base::Reset();
			
			for(int i = 0; i < BufferSize; i++)
			{
				_buffer[i] = 0;
			}
			
			_outputCounter = 0;
		}
	protected:
	};
}
}
}
}
#endif