///////////////////////////////////////////////////////////////////////////////
//	Ѕлок алгоритма обработки сигнала - расчет функции автокоррел€ции
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
				typename valueTypeIn,	// тип входных значений
		  		typename valueType,		// тип значений
				int bufferSize			// размер буфера
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


		static const int BufferSize = bufferSize;	// размер буфера сортировки
		static const int CorrelatorMultiplierConst = 100; 
		static const int CorrelatorGateLevel = 40;
		static const int MinStepDuration = 8;	// мин. длит. 2 шагов, в 1/16 сек = 0.25 сек на шаг = 4 √ц
		static const int MaxStepDuration = 40;	// макс. длит. 2 шагов, в 1/16 сек = 2.5 сек 0.8 √ц
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
			// поиск первого минимума функции коррел€ции
			found &= FindMin(1, _buff, BufferSize, indexMin1, min1);
			// поиск первого максимума функции коррел€ции
			found &= FindMax(indexMin1 + 1, _buff, BufferSize, indexMax1, max1);
			if(min1 > max1)
			{
				found = false;
			}
			// поиск второго минимума функции коррел€ции
			found &= FindMin(indexMax1 + 1, _buff, BufferSize, indexMin2, min2);
			// поиск второго максисума функции коррел€ции
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
				// уровень хот€ бы больше порога
				if (amplitude >= CorrelatorGateLevel)
				{
					// крайний максимум в пределах расчЄтного периода пары шагов человека (между бегом и шагом)
					if ((indexMax2 >= MinStepDuration) && (indexMax2 <= MaxStepDuration))
					{
						// периоды отличаютс€ не более чем на половину
						if ((( indexMax1 + indexMax1 / 2) > (indexMax2 - indexMax1)) && (((indexMax2 - indexMax1) + (indexMax2 - indexMax1) / 2) > indexMax1))
						{
							// человек DETECTED
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
				// €мка
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
				// бугорок
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