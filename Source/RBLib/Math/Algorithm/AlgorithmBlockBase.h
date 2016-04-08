///////////////////////////////////////////////////////////////////////////////
//	Базовый блок алгоритма обработки сигнала
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_ALGORITHMBLOCKBASE_H
#define MATH_ALGORITHM_ALGORITHMBLOCKBASE_H

#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
	template <
				typename valueTypeIn,
		  		typename valueTypeOut
			 >
	class AlgorithmBlockBase
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeOut ValueTypeOut;
		typedef CallbackWrapper <ValueTypeOut> OutCallbackType;
	public:
		OutCallbackType Out;
		OutCallbackType DebugOut;
	public:
		AlgorithmBlockBase()
		{
			Reset();
		}
		
		virtual ~AlgorithmBlockBase()
		{
		}
		
		virtual void In(ValueTypeIn val)
		{
		}
		
		static void InCallback(void *callbackParam, ValueTypeIn val)
		{
			AlgorithmBlockBase *im = (AlgorithmBlockBase *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->In(val);
		}
		
		virtual void Reset()
		{
		}
	protected:	
		virtual void Output(ValueTypeOut val)
		{
			Out.Call(val);
			DebugOut.Call(val);
		}
		
		static void Output(void *callbackParam, ValueTypeOut val)
		{
			AlgorithmBlockBase *im = (AlgorithmBlockBase *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->Output(val);
		}
	};
}
}
}
#endif