///////////////////////////////////////////////////////////////////////////////
//	���� ��������� ��������� ������� - ����������� �������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_DETECTORPELENG_PELENGSTEPSECTOR_H
#define MATH_ALGORITHM_DETECTORPELENG_PELENGSTEPSECTOR_H

#include "../../AlgorithmBlockBase.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
namespace DetectorPelengParts
{
	struct PelengStepSectorOutStruct
	{
		signed long int SumIntegrZY;
		signed long int SumIntegrZX;

		PelengStepSectorOutStruct()
		{
			Reset();
		}
		
		void Reset()
		{
			SumIntegrZY = 0.0;
			SumIntegrZX = 0.0;
		}

	};
	template <
				typename valueTypeIn	// ��� ������� ��������
			 >
	class PelengStepSector : public AlgorithmBlockBase<valueTypeIn*, PelengStepSectorOutStruct>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef valueTypeIn *ValueTypeInSample;
		typedef PelengStepSectorOutStruct ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeInSample, ValueTypeOut> Base;
		
	public:
		static const unsigned int PelengAnalizyPeriod = 128; // ������ ������ ����������� �������
	protected:
		int _outputCounter;
		unsigned int _indexBuffFor3dPeleng;	// ������ ��� ����������� ������� ���������� �������
		signed long int _integr_Z;	// ������������ ����� ������� Z
		signed long int _sumIntegrZY;
		signed long int _sumIntegrZX;

	public:
		PelengStepSector()
		{
			Reset();
		}
		
		virtual ~PelengStepSector()
		{
		}

		virtual void In(ValueTypeInSample samples)
		{
			// �������� �������� � ����� ��� ���������
			if(_indexBuffFor3dPeleng == 0)
			{
				_integr_Z = 0.0;
				_sumIntegrZY = 0.0;
				_sumIntegrZX = 0.0;
			}
			else
			{
				_integr_Z += samples[0];
				if(_integr_Z > 0)
				{
					_sumIntegrZY += samples[1];
					_sumIntegrZX += samples[2];
				}
			}
	
			_indexBuffFor3dPeleng ++;
			if(_indexBuffFor3dPeleng >= PelengAnalizyPeriod)
			{
			  _indexBuffFor3dPeleng = 0;
			}

			_outputCounter++;
			if (_outputCounter >= PelengAnalizyPeriod)
			{
				_outputCounter = 0;
				ValueTypeOut OutValue;
				OutValue.SumIntegrZY = _sumIntegrZY;
				OutValue.SumIntegrZX = _sumIntegrZX;
				Base::Output(OutValue);
			}
   		}
		
		virtual void Reset()
		{
			Base::Reset();

		}
	protected:
	};
}
}
}
}
#endif