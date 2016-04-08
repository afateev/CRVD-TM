///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - фильтн низких частот при детектирование человека
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORHUMAN_LOWFREQUENCYFILTER_H
#define MATH_ALGORITHM_DETECTORHUMAN_LOWFREQUENCYFILTER_H

#include "../../AlgorithmBlockBase.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
namespace DetectorHumanParts
{
	struct LowFrequencyOutStruct
		{
			bool *ClipBuffer;
			unsigned int Index;
		};
	
	template <
				typename valueType,		// тип входных значений
				unsigned int bufferSize	// размер буфера
			 >
	class LowFrequencyFilter : public AlgorithmBlockBase<valueType, LowFrequencyOutStruct>
	{
	public:
		typedef valueType ValueTypeIn;
		typedef LowFrequencyOutStruct ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		
		typedef CallbackWrapper<ValueTypeIn> TestPointCallbackType;

		static const int FilterTimeConstant = 16;	// постоянная времени фильтра
		static const unsigned int BufferSize = bufferSize;	// размер буфера

		TestPointCallbackType TestPoint;
	protected:
		unsigned int _indexBufferElement;			// индекс элемента в буфере
		bool _buffer[BufferSize];	// буфер хранения элементов клиппирования
		ValueTypeIn _currentClipThreshold;		// текущий порог клипирования
	public:
		LowFrequencyFilter()
		{
			Reset();
		}
		
		virtual ~LowFrequencyFilter()
		{
		}
		
		virtual void Reset()
		{
			Base::Reset();
			_currentClipThreshold = 0;
			_indexBufferElement = 0;
			for(int i = 0; i < BufferSize; i++)
			{
				_buffer[i] = 0;
			}
		}

		virtual void In(ValueTypeIn sample)
		{
			_currentClipThreshold *= (FilterTimeConstant - 1);
			_currentClipThreshold /= FilterTimeConstant;
			_currentClipThreshold += sample;

			TestPoint.Call(_currentClipThreshold);

			_buffer[_indexBufferElement] = FilterTimeConstant * sample > _currentClipThreshold;
			
			ValueTypeOut outFilter;
			outFilter.ClipBuffer = _buffer;
			outFilter.Index = _indexBufferElement;
			
			_indexBufferElement++;
			if (_indexBufferElement >= BufferSize)
			{
				_indexBufferElement = 0;
			}

			Base::Output(outFilter);
		}
	};
}
}
}
}
#endif