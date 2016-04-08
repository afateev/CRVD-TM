///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - определение пеленга
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORPELENG_ANALYZEPELENGSECTOR_H
#define MATH_ALGORITHM_DETECTORPELENG_ANALYZEPELENGSECTOR_H

#include "../../AlgorithmBlockBase.h"
#include <math.h>

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
namespace DetectorPelengParts
{
	struct AnalyzePelengSectorOutStruct
	{
		unsigned short Peleng;
		unsigned short PelengError;

		AnalyzePelengSectorOutStruct()
		{
			Reset();
		}
		
		void Reset()
		{
			Peleng = 0;
			PelengError = 0;
		}

	};
	
	template <
				typename valueTypeIn	// тип входных значений
			 >
	class AnalyzePelengSector : public AlgorithmBlockBase<valueTypeIn, AnalyzePelengSectorOutStruct>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeIn * ValueTypeInSample;
		typedef AnalyzePelengSectorOutStruct ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		float Angle;
	public:
		static const unsigned int IntegrZYSumBuffSize = 5;
		//////////////////////////////////////////////////////////////
		static const unsigned int SinCosBuffSize = 5;

		float SinBuff[SinCosBuffSize];
		float CosBuff[SinCosBuffSize];
		unsigned int indexSinCos;
		float Peleng_Error;

		////////////////////////////////////////////////////////////////
	protected:
		unsigned int _indexPeleng;
		signed long int _integrZYSumBuff[IntegrZYSumBuffSize];
		signed long int _integrZXSumBuff[IntegrZYSumBuffSize];
		bool _flagIntegrBuffReady;
	private : bool _flagSinCosBuffReady;
	public:
		AnalyzePelengSector()
		{
			Reset();
		}
		
		virtual ~AnalyzePelengSector()
		{
		}

		virtual void In(ValueTypeIn inValues)
		{
			_integrZYSumBuff[_indexPeleng] = inValues.SumIntegrZY;
			_integrZXSumBuff[_indexPeleng] = inValues.SumIntegrZX;

			// блокировка выброса мусора, т.е. пока не наполнятся массивы _integrZYSumBuff
			// и _integrZXSumBuff выходим из алгоритма.
			if(!_flagIntegrBuffReady)
			{
				if(_indexPeleng < IntegrZYSumBuffSize - 1)
				{
					_indexPeleng ++;
//					SeismoResults.alarm_flags &= ~AFL_PELENGOK;
					return;
				}
				else
				{
					_flagIntegrBuffReady = true;
				}
			}

			_indexPeleng ++;
			if(_indexPeleng >= IntegrZYSumBuffSize)
			{
				_indexPeleng = 0;
			}

			// ститаем среднюю сумму итнегральных значений.
			signed long int AverageSumIntegrZY = 0;
			signed long int AverageSumIntegrZX = 0;
			for(int i = 0; i < IntegrZYSumBuffSize; i++)
			{
				AverageSumIntegrZY += _integrZYSumBuff[i];
				AverageSumIntegrZX += _integrZXSumBuff[i];
			}
	
			// выститываем угол
			Angle = 180.0 + atan2((float)AverageSumIntegrZY, (float)AverageSumIntegrZX) * (180.0 / 3.1415926);
//			SeismoResults.alarm_flags |= AFL_PELENGOK;

////////////////////////////////////////////////////////////////////////////////
			// блокировка выброса мусора, т.е. пока не наполнятся массивы SinBuff
			// и CosBuff выходим из алгоритма.
			if(!_flagSinCosBuffReady)
			{
				if(indexSinCos < SinCosBuffSize)
				{
					SinBuff[indexSinCos] = sin((Angle / 180.0) * 3.1415926);
					CosBuff[indexSinCos] = cos((Angle / 180.0) * 3.1415926);
					indexSinCos++;
//					SeismoResults.alarm_flags &= ~AFL_PELENGOK;
					return;
				}
				else
				{
					_flagSinCosBuffReady = true;
					indexSinCos = 4;
				}
			}

			if(indexSinCos == SinCosBuffSize)
			{
				for(indexSinCos = 0; indexSinCos < SinCosBuffSize; indexSinCos ++)
				{
					SinBuff[indexSinCos] = SinBuff[indexSinCos + 1];
					CosBuff[indexSinCos] = CosBuff[indexSinCos + 1];
				}
				indexSinCos = 4;
			}

			// линейная аппроксимация a + bx, единица по х - шаг пеленгации, 1 сек
			SinBuff[indexSinCos] = sin((Angle / 180.0) * 3.1415926);
			CosBuff[indexSinCos] = cos((Angle / 180.0) * 3.1415926);
			indexSinCos ++;

			float sumSinArray;
			float sumCosArray;
			sumSinArray = 0.0;
			sumCosArray = 0.0;
			float a[2];
			float b[2];
			a[0] = 0.0;
			a[1] = 0.0;
			b[0] = 0.0;
			b[1] = 0.0;

			int sumIndexSize = 0;
			for(int i = 0; i < SinCosBuffSize; i++)
			{
				sumSinArray += SinBuff[i];
				sumCosArray += CosBuff[i];
				a[0] += SinBuff[i] * (i + 1);
				a[1] += CosBuff[i] * (i + 1);
				sumIndexSize += (i + 1);

			}
			
			a[0]*= SinCosBuffSize;
			a[0]-= sumSinArray * sumIndexSize;
			a[0]/= ( 275.0 - 225.0);
//			a[0] =  (5.0 * (SinBuff[0] + SinBuff[1] * 2.0 + SinBuff[2] * 3.0 + SinBuff[3] * 4.0 + SinBuff[4] * 5.0) - (SumSinArray) * 15.0) / ( 275.0 - 225.0);
			a[1]*= SinCosBuffSize;
			a[1]-= sumCosArray * sumIndexSize;
			a[1]/= ( 275.0 - 225.0);
//			a[1] =  (5.0 * (CosBuff[0] + CosBuff[1] * 2.0 + CosBuff[2] * 3.0 + CosBuff[3] * 4.0 + CosBuff[4] * 5.0) - (SumCosArray) * 15.0) / ( 275.0 - 225.0);
			b[0] = ( sumSinArray - (a[0] * sumIndexSize)) / SinCosBuffSize;
			b[1] = ( sumCosArray - (a[1] * sumIndexSize)) / SinCosBuffSize;
			
			float Se[2];
			Se[0] = 0.0;
			Se[1] = 0.0;
			for(int i = 0; i < SinCosBuffSize; i++)
			{
				Se[0] += (SinBuff[i] - a[0] * (i + 1) - b[0]) * (SinBuff[i] - a[0] * (i + 1) - b[0]);
				Se[1] += (CosBuff[i] - a[1] * (i + 1) - b[1]) * (CosBuff[i] - a[1] * (i + 1) - b[1]);
			}
			Peleng_Error = 360.0 * sqrt((Se[0]+Se[1]) / (sumSinArray * sumSinArray + sumCosArray * sumCosArray ));
//		}
//		else
//		{
//			Peleng_Error = 0.0;
//		}

			
////////////////////////////////////////////////////////////////////////////////			
			ValueTypeOut OutValue;
			OutValue.Peleng = (unsigned short)Angle;
			OutValue.PelengError = (unsigned short)Peleng_Error;
			Base::Output(OutValue);

			//Base::Output(Angle);
   		}
		
		virtual void Reset()
		{
			Base::Reset();
			_indexPeleng = 0;
			_flagIntegrBuffReady = false;
			_flagSinCosBuffReady = false;
			for(int i = 0; i < IntegrZYSumBuffSize; i++)
			{
				_integrZYSumBuff[i] = 0.0;
				_integrZXSumBuff[i] = 0.0;
			}
			///////////////
			indexSinCos = 0;
			///////////////////
		}
	protected:
	};
	
}
}
}
}
#endif