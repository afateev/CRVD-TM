///////////////////////////////////////////////////////////////////////////////
//	���� ��������� ��������� ������� - ������ ������� ��������������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORHUMAN_ANALYZEAUTOCORRELATIONFUNKTION_H
#define MATH_ALGORITHM_DETECTORHUMAN_ANALYZEAUTOCORRELATIONFUNKTION_H

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
				typename valueTypeIn,	// ��� ������� ��������
		  		typename valueType,		// ��� ��������
				int bufferSize			// ������ ������
			 >
	class AnalyzeAutocorrelationFunction : public AlgorithmBlockBase<valueTypeIn, bool>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueType ValueType;
		typedef bool ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		typedef CallbackWrapper<float> TestPointCallbackType;
		typedef CallbackWrapper<ValueTypeIn> TestPointCallbackCorrType;


		static const int BufferSize = bufferSize;	// ������ ������ ����������
		static const int CorrelatorMultiplierConst = 100; 
		static const int CorrelatorGateLevel = 40;
		static const int MinStepDuration = 8;	// ���. ����. 2 �����, � 1/16 ��� = 0.25 ��� �� ��� = 4 ��
		static const int MaxStepDuration = 40;	// ����. ����. 2 �����, � 1/16 ��� = 2.5 ��� 0.8 ��
		ValueTypeIn _buff;
		TestPointCallbackType TestPoint;
		TestPointCallbackCorrType TestPointCorrBuff;

	protected:
	public:
		AnalyzeAutocorrelationFunction()
		{
			Reset();
		}
		
		virtual ~AnalyzeAutocorrelationFunction()
		{
		}

		virtual void In(ValueTypeIn buff)
		{
			_buff = buff;
			TestPointCorrBuff.Call(_buff);

			bool humanDetected = Analyze();
			
			Base::Output(humanDetected);
		}
		
		virtual void Reset()
		{
			Base::Reset();
		}
	protected:
		bool Analyze()
		{
			ValueType min1 = 0;
			ValueType min2 = 0;
			ValueType max1 = 0;
			ValueType max2 = 0;
			unsigned int indexMin1 = 0;
			unsigned int indexMin2 = 0;
			unsigned int indexMax1 = 0;
			unsigned int indexMax2 = 0;
			bool result = false;
			
//			TestPointCorrBuff.Call(_buff);
			bool found = true;
			// ����� ������� �������� ������� ����������
			found &= FindMin(1, _buff, BufferSize, indexMin1, min1);
			// ����� ������� ��������� ������� ����������
			found &= FindMax(indexMin1 + 1, _buff, BufferSize, indexMax1, max1);
			if(min1 > max1)
			{
				found = false;
			}
			// ����� ������� �������� ������� ����������
			found &= FindMin(indexMax1 + 1, _buff, BufferSize, indexMin2, min2);
			// ����� ������� ��������� ������� ����������
			found &= FindMax(indexMin2 + 1, _buff, BufferSize, indexMax2, max2);
			if(min2 > max2)
			{
				found = false;
			}
			float amplitude = 0;

			if(found)
			{
				if(_buff[0] > 0)
				{
					amplitude = (max1 + max2 - min1 - min2) * CorrelatorMultiplierConst / _buff[0];
				}
				// ������� ���� �� ������ ������
				if (amplitude >= CorrelatorGateLevel)
				{
					// ������� �������� � �������� ���������� ������� ���� ����� �������� (����� ����� � �����)
					if ((indexMax2 >= MinStepDuration) && (indexMax2 <= MaxStepDuration))
					{
						// ������� ���������� �� ����� ��� �� ��������
						if ((( indexMax1 + indexMax1 / 2) > (indexMax2 - indexMax1)) && (((indexMax2 - indexMax1) + (indexMax2 - indexMax1) / 2) > indexMax1))
						{
							// ������� DETECTED
							result = true;
						}
					}
				}

				
			}
			TestPoint.Call(amplitude);
			return result;
		}
		
		bool FindMin(unsigned int startIndex, ValueTypeIn buffer, unsigned int bufSize, unsigned int &index, ValueType &val)
		{
			for (int i = startIndex + 1; i < bufSize - 1; i++)
			{
				// ����
				if (buffer[i - 1] > buffer[i] && buffer[i] < buffer[i + 1])
				{
					index = i;
					val = buffer[i];
					return true;
				}
			}
			
			return false;
		}
		
		bool FindMax(unsigned int startIndex, ValueTypeIn buffer, unsigned int bufSize, unsigned int &index, ValueType &val)
		{
			for (int i = startIndex + 1; i < bufSize - 1; i++)
			{
				// �������
				if (buffer[i - 1] < buffer[i] && buffer[i] > buffer[i + 1])
				{
					index = i;
					val = buffer[i];
					return true;
				}
			}
			
			return false;
		}
	};
}
}
}
}
#endif