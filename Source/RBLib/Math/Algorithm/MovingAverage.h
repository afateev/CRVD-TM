///////////////////////////////////////////////////////////////////////////////
//	���� ��������� ��������� ������� - ��������� �������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_MOVINGAVERANGE_H
#define MATH_ALGORITHM_MOVINGAVERANGE_H

#include "AlgorithmBlockBase.h"
namespace _PrivateSumProvider
{
	template<class SampleType>
	class SumProvider;
	
	template<>
	class SumProvider<unsigned char>
	{
	public:
	  typedef unsigned int SumType;
	};
	
	template<>
	class SumProvider<char>
	{
	public:
	  typedef int SumType;
	};
	
	template<>
	class SumProvider<unsigned short>
	{
	public:
	  typedef unsigned long SumType;
	};
	
	template<>
	class SumProvider<short>
	{
	public:
	  typedef long SumType;
	};
	  
	template<>
	class SumProvider<unsigned int>
	{
	public:
	  typedef unsigned long SumType;
	};
	
	template<>
	class SumProvider<int>
	{
	public:
	  typedef long SumType;
	};
	
	template<>
	class SumProvider<long>
	{
	public:
	  typedef long long SumType;
	};
	
	template<>
	class SumProvider<float>
	{
	public:
	  typedef double SumType;
	};
	
	template<>  
	class SumProvider<double>
	{
	public:
	  typedef double SumType;
	};
	
}

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
	template <
				typename valueTypeIn,	// ��� ������� ��������
		  		typename valueTypeOut,	// ��� �������� ��������
				int windowSize			// ������ ����
			 >
	class MovingAverage : public AlgorithmBlockBase<valueTypeIn, valueTypeOut>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeOut ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		static const int WindowSize = windowSize;
	protected:
		ValueTypeIn _buffer[WindowSize];	// ����� �������� ��������� ������ ����
		typename _PrivateSumProvider::SumProvider<ValueTypeIn>::SumType _sum;	// ���������� ����� ��������� ������ ����
		unsigned char _index;	// ������ �������� � ������
		bool _notInitBuffer;	// ���� ���������� � �������, ���� �� ���������� �����
	public:
		MovingAverage() : _index(0), _notInitBuffer(true), _sum(0)
		{
			Reset();
		}
		
		virtual ~MovingAverage()
		{
		}
		
		virtual void In(ValueTypeIn val)
		{
			AddSample(val);
			if(!_notInitBuffer)
			{
				ValueTypeOut _valOut = GetValue();
				Base::Output(_valOut);
			}
		}
		
		virtual void Reset()
		{
			Base::Reset();
			_index = 0;
			_sum = 0;
			_notInitBuffer = true;
		}
	protected:	
		void AddSample(ValueTypeIn sample)
		{
    		if (_notInitBuffer)
    		{
				_sum += sample;
				_buffer[_index] = sample;
				_index++;
				if (_index >= WindowSize)
				{
					_notInitBuffer = false;
				}
    		}
    		else
    		{
				if (_index >= WindowSize)
					_index = 0;
				_sum = _sum - _buffer[_index] + sample;
				_buffer[_index] = sample;
				_index++;
    		}
		}
		
		inline ValueTypeOut GetValue()
		{
		  return (ValueTypeOut)(_sum / (ValueTypeOut) WindowSize);
		}

	};
}
}
}
#endif