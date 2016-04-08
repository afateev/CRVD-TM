///////////////////////////////////////////////////////////////////////////////
//	Реализация скалярного сейсмического алгоритма
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_SEISMIC_SCALARALGORITHM_H
#define MATH_ALGORITHM_SEISMIC_SCALARALGORITHM_H

#include "../AlgorithmBlockBase.h"
#include "../MovingAverage.h"
#include "../Thinning.h"
#include "HighFrecuencyFilter.h"
#include "LowFrecuencyFilter.h"
#include "../Abs.h"
#include "AnomalyDetector.h"
#include "DetectorHuman.h"
#include "DetectorPeak.h"
#include "DetectorAuto.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
namespace Seismic
{
	struct ScalarAlgorithmOutputStruct
	{
		bool HumanAlarm;
		bool VehicleAlarm;
		
		ScalarAlgorithmOutputStruct()
		{
			Reset();
		}
		
		void Reset()
		{
			HumanAlarm = false;
			VehicleAlarm = false;
		}
	};
	
	class ScalarAlgorithm : public AlgorithmBlockBase</*float*/unsigned short, ScalarAlgorithmOutputStruct>
	{
	public:
		typedef /*float*/ unsigned short SampleValueType;
		typedef ScalarAlgorithmOutputStruct ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		
		typedef MovingAverage<SampleValueType, SampleValueType, 4> MovingAverage4Type;
		typedef MovingAverage<SampleValueType, SampleValueType, 7> MovingAverage7Type;
		typedef MovingAverage<SampleValueType, SampleValueType, 10> MovingAverage10Type;
		typedef Thinning<SampleValueType, SampleValueType, 4> Thinning4Type;
		typedef Thinning<SampleValueType, SampleValueType, 8> Thinning8Type;
		typedef HighFrequencyFilter<SampleValueType, int> HighFrequencyFilterType;
		typedef Abs<int, SampleValueType> AbsType;
		typedef LowFrequencyFilter<SampleValueType, SampleValueType> LowFrequencyFilterType;
		
		typedef AnomalyDetector<SampleValueType> AnomalyDetectorType;
		typedef DetectorHuman<SampleValueType> DetectorHumanType;
		typedef DetectorPeak<SampleValueType> DetectorPeakType;
		typedef DetectorAuto<SampleValueType> DetectorVehicleType;
		
		struct TestPointOutputStruct
		{
			float AnomalyHumanLevel;
			float AnomalyVehicleLevel;
			float AnomalyGate;
			bool AnomalyHumanDetected;
			bool AnomalyVehicleDetected;
			float PeakLevel;
			bool HumanDetected;
			float HumanCorelationLevel;
			bool VehicleDetected;
			float MeanLevel;
			float MaxContrast;
			float SpectralContrast;

			
			TestPointOutputStruct()
			{
				Reset();
			}
			
			void Reset()
			{
				AnomalyHumanLevel = 0;
				AnomalyVehicleLevel = 0;
				AnomalyGate = 0;
				AnomalyHumanDetected = false;
				AnomalyVehicleDetected = false;
				PeakLevel = 0;
				HumanDetected = false;
				HumanCorelationLevel = 0;
				VehicleDetected = false;
				MeanLevel = 0;
				MaxContrast = 0;
				SpectralContrast = 0;
			}
		};
		
		typedef CallbackWrapper<unsigned int> NoiseLevelOutNrb3Type;
		typedef CallbackWrapper<float> PeakDetectorLevelOutType;
		typedef CallbackWrapper<TestPointOutputStruct> TestPointCallbackType;

		struct AnomalyOutputStruct
		{
			bool AnomalyHumanDetected;
			bool AnomalyVehicleDetected;
			
			AnomalyOutputStruct()
			{
				Reset();
			}
			
			void Reset()
			{
				AnomalyHumanDetected = false;
				AnomalyVehicleDetected = false;
			}
		};
		typedef CallbackWrapper<AnomalyOutputStruct> AnomalyOutCallbackType;
		
		struct HumanDecisionVariablesStruct
		{
			bool AnomalyDetected;
			bool AnomalyDetectedUpdated;
			bool HumanDetected;
			bool HumanDetectedUpdated;
			
			void Reset()
			{
				AnomalyDetected = 0;
				AnomalyDetectedUpdated = false;
				HumanDetected = 0;
				HumanDetectedUpdated = false;
			}
			
			bool All()
			{
				return AnomalyDetectedUpdated && HumanDetectedUpdated;
			}
		};
		
		struct VehicleDecisionVariablesStruct
		{
			bool AnomalyDetected;
			bool AnomalyDetectedUpdated;
			bool VehicleDetected;
			bool VehicleDetectedUpdated;
			
			void Reset()
			{
				AnomalyDetected = 0;
				AnomalyDetectedUpdated = false;
				VehicleDetected = 0;
				VehicleDetectedUpdated = false;
			}
			
			bool All()
			{
				return AnomalyDetectedUpdated && VehicleDetectedUpdated;
			}
		};
	public:
		MovingAverage4Type MovingAverage4;
		MovingAverage7Type MovingAverage7;
		MovingAverage10Type MovingAverage10;
		Thinning4Type Thinning4;
		HighFrequencyFilterType HighFrequencyFilter;
		AbsType Abs;
		LowFrequencyFilterType LowFrequencyFilter;
		Thinning8Type Thinning8;
		
		AnomalyDetectorType AnomalyDetector;
		DetectorHumanType DetectorHuman;
		DetectorPeakType DetectorPeak;
		DetectorVehicleType DetectorVehicle;
	
		HumanDecisionVariablesStruct HumanDecisionVariables;
		VehicleDecisionVariablesStruct VehicleDecisionVariables;
		
		NoiseLevelOutNrb3Type NoiseLevelOutNrb3;
		PeakDetectorLevelOutType PeakDetectorLevelOut;
		TestPointCallbackType TestPoint;
		AnomalyOutCallbackType AnomalyOutCallback;
	protected:
		bool _dibaleAnomalyBufferUpdateByHumanDetectedFlag;
		bool _dibaleAnomalyBufferUpdateByVehicleDetectedFlag;
		bool _enableVehicleAlgorithmFlag;
		bool _enableHumanAlgorithmFlag;
		bool _enableVehicleAlgorithmByAnomalyFlag;
		bool _resetVehicleDetector;
		
		ValueTypeOut _currentOutputValue;
		
		TestPointOutputStruct _testPointOutputValue;
		AnomalyOutputStruct _anomalyOutputStruct;
	public:
		ScalarAlgorithm()
		{
			Reset();
			
			MovingAverage4.Out.Set(MovingAverage7.InCallback, &MovingAverage7);
			MovingAverage7.Out.Set(MovingAverage10.InCallback, &MovingAverage10);
			MovingAverage10.Out.Set(Thinning4.InCallback, &Thinning4);
			Thinning4.Out.Set(Spliter128Hz, this);
			
			HighFrequencyFilter.Out.Set(Abs.InCallback, &Abs);
			Abs.Out.Set(LowFrequencyFilter.InCallback, &LowFrequencyFilter);
			LowFrequencyFilter.Out.Set(Thinning8.InCallback, &Thinning8);
			Thinning8.Out.Set(Spliter16Hz, this);
			
			AnomalyDetector.Out.Set(AnomalyDetectorOutput, this);
			AnomalyDetector.AnomalyThreshold.NoiseLevelOutNrb3.Set(NoiseLevelOutputNrb3, this);
			AnomalyDetector.DetectorHuman.TestPoint.Set(AnomalyHumanDetectorTestPointOutput, this);
			AnomalyDetector.DetectorVehicle.TestPoint.Set(AnomalyVehicleDetectorTestPointOutput, this);
			DetectorHuman.Out.Set(HumanDetectorOutput, this);
			DetectorHuman.AnalyzeAutocorrelationFunction.TestPoint.Set(DetectorHumanAnalyzeAutocorrelationFunctionTestPointOut, this);
			DetectorPeak.Out.Set(PeakDetectorOutput, this);
			DetectorVehicle.Out.Set(VehicleDetectorOutput, this);
			DetectorVehicle.TestPoint.Set(VehicleDetectorTestPointOutput, this);
		}
		
		virtual void In(SampleValueType sample)
		{
			if(_enableHumanAlgorithmFlag || _enableVehicleAlgorithmFlag)
			{
				MovingAverage4.In(sample * 10);
			}
		}
		// включение/отключение алгоритма авто
		void SetVehicleEnableAlgorithm(bool state)
		{
			_enableVehicleAlgorithmFlag = state;
		}
		bool GetVehicleEnableAlgorithm()
		{
			return _enableVehicleAlgorithmFlag;
		}
		// включение/отключение алгоритма человека
		void SetHumanEnableAlgorithm(bool state)
		{
			_enableHumanAlgorithmFlag = state;
		}
		bool GetHumanEnableAlgorithm()
		{
			return _enableHumanAlgorithmFlag;
		}
		
		// входное занчение в местной шкале
		void SetThresholdHuman(float val)
		{
			if (val < 0)
			{
				val = 0;
			}

			AnomalyDetector.DetectorHuman.SetThreshold(val);
		}

		// входное занчение в местной шкале
		void SetThresholdVehicle(float val)
		{
			if (val < 0)
			{
				val = 0;
			}

			AnomalyDetector.DetectorVehicle.SetThreshold(val);
		}
		
		// выходное занчение в местной шкале
		float GetThresholdHuman()
		{
			return AnomalyDetector.DetectorHuman.GetThreshold();
		}
		
		// выходное занчение в местной шкале
		float GetThresholdVehicle()
		{
			return AnomalyDetector.DetectorVehicle.GetThreshold();
		}
		
		virtual void Reset()
		{
			Base::Reset();
			MovingAverage4.Reset();
			MovingAverage7.Reset();
			MovingAverage10.Reset();
			Thinning4.Reset();
			
			AnomalyDetector.Reset();
			DetectorHuman.Reset();
			DetectorPeak.Reset();
			DetectorVehicle.Reset();
			
			HumanDecisionVariables.Reset();
			VehicleDecisionVariables.Reset();
			
			_dibaleAnomalyBufferUpdateByHumanDetectedFlag = false;
			_dibaleAnomalyBufferUpdateByVehicleDetectedFlag = false;
			_enableVehicleAlgorithmFlag = true;
			_enableHumanAlgorithmFlag = true;
			_enableVehicleAlgorithmByAnomalyFlag = false;
			_resetVehicleDetector = false;
			
			_currentOutputValue.Reset();
			_testPointOutputValue.Reset();
			_anomalyOutputStruct.Reset();
		}
	protected:
		static void Spliter128Hz(void *callbackParam, SampleValueType val)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->HighFrequencyFilter.In(val);

			if((im->_enableVehicleAlgorithmByAnomalyFlag == false) && (im->_enableVehicleAlgorithmFlag == true) && (im->_resetVehicleDetector == true))
			{
				im->DetectorVehicle.Reset();
				im->_resetVehicleDetector = false;
				im->DetectorVehicle.Out.Call(false);
			}
			if(im->_enableVehicleAlgorithmByAnomalyFlag && im->_enableVehicleAlgorithmFlag)
			{
				im->DetectorVehicle.In(val / 10);
				im->_resetVehicleDetector = true;
			}
		}
		
		static void Spliter16Hz(void *callbackParam, SampleValueType val)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->AnomalyDetector.In(val);
			if(im->_enableHumanAlgorithmFlag)
			{
				im->DetectorHuman.In(val);
			}
			im->DetectorPeak.In(val);
		}
		
		static void AnomalyDetectorOutput(void *callbackParam, AnomalyDetectorType::ValueTypeOut val)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->_testPointOutputValue.AnomalyHumanDetected = val.HumanDetected;
			im->_testPointOutputValue.AnomalyVehicleDetected = val.VehicleDetected;
			
			im->HumanDecisionVariables.AnomalyDetected = val.HumanDetected;
			im->HumanDecisionVariables.AnomalyDetectedUpdated = true;
			im->VehicleDecisionVariables.AnomalyDetected = val.VehicleDetected;
			im->VehicleDecisionVariables.AnomalyDetectedUpdated = true;
			im->_enableVehicleAlgorithmByAnomalyFlag = val.VehicleDetected;

			im->_anomalyOutputStruct.AnomalyHumanDetected = val.HumanDetected;
			im->_anomalyOutputStruct.AnomalyVehicleDetected = val.VehicleDetected;
			im->AnomalyOutCallback.Call(im->_anomalyOutputStruct);

			im->Decide();
		}
		
		static void NoiseLevelOutputNrb3(void *callbackParam, unsigned int val)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->NoiseLevelOutNrb3.Call(val);
		}
		
		static void AnomalyHumanDetectorTestPointOutput(void *callbackParam, int level, float gate)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->_testPointOutputValue.AnomalyHumanLevel = level;
			im->_testPointOutputValue.AnomalyGate = gate;
		}

		static void AnomalyVehicleDetectorTestPointOutput(void *callbackParam, int level, float gate)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->_testPointOutputValue.AnomalyVehicleLevel = level;
			im->_testPointOutputValue.AnomalyGate = gate;
		}

		static void DetectorHumanAnalyzeAutocorrelationFunctionTestPointOut(void *callbackParam, float val)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}

			im->_testPointOutputValue.HumanCorelationLevel = val;
		}

		static void HumanDetectorOutput(void *callbackParam, bool val)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->_testPointOutputValue.HumanDetected = val;
			
			im->HumanDecisionVariables.HumanDetected = val;
			im->HumanDecisionVariables.HumanDetectedUpdated = true;
			im->_dibaleAnomalyBufferUpdateByHumanDetectedFlag = val;
			im->EnableDisableBackBufferUpdate();
			im->Decide();
		}
		
		static void PeakDetectorOutput(void *callbackParam, float val)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->_testPointOutputValue.PeakLevel = val;
			im->PeakDetectorLevelOut.Call(val);
		}
		
		static void VehicleDetectorOutput(void *callbackParam, bool val)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->_testPointOutputValue.VehicleDetected = val;
			
			im->VehicleDecisionVariables.VehicleDetected = val;
			im->VehicleDecisionVariables.VehicleDetectedUpdated = true;
			im->_dibaleAnomalyBufferUpdateByVehicleDetectedFlag = val;
			im->EnableDisableBackBufferUpdate();

			im->Decide();
		}
		
		static void VehicleDetectorTestPointOutput(void *callbackParam, float mean, float maxContrast, float spectralContrast)
		{
			ScalarAlgorithm *im = (ScalarAlgorithm *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->_testPointOutputValue.MeanLevel = mean;
			im->_testPointOutputValue.MaxContrast = maxContrast;
			im->_testPointOutputValue.SpectralContrast = spectralContrast;
		}
		
		void EnableDisableBackBufferUpdate()
		{
			AnomalyDetector.InDisableBackBufferUpdate(_dibaleAnomalyBufferUpdateByHumanDetectedFlag || _dibaleAnomalyBufferUpdateByVehicleDetectedFlag);
		}
		
		void Decide()
		{
			bool out = false;
			
			if (HumanDecisionVariables.All())
			{
				
				_currentOutputValue.HumanAlarm = HumanDecisionVariables.AnomalyDetected && HumanDecisionVariables.HumanDetected;
				
				HumanDecisionVariables.Reset();
				out = true;
			}
			
			if (VehicleDecisionVariables.All())
			{
				_currentOutputValue.VehicleAlarm = VehicleDecisionVariables.AnomalyDetected && VehicleDecisionVariables.VehicleDetected && !_currentOutputValue.HumanAlarm;
				
				VehicleDecisionVariables.Reset();
				out = true;
			}
			
			if (out)
			{
				if(!_enableHumanAlgorithmFlag)
				{
					_currentOutputValue.HumanAlarm = false;
				}
				if(!_enableVehicleAlgorithmFlag)
				{
					_currentOutputValue.VehicleAlarm = false;
				}
				if(_enableHumanAlgorithmFlag && _enableVehicleAlgorithmFlag)
				{
					if(_currentOutputValue.HumanAlarm)
					{
						_currentOutputValue.VehicleAlarm = false;
					}
				}
				TestPoint.Call(_testPointOutputValue);
				Output(_currentOutputValue);
			}
		}
	};
}
}
}
}

#endif