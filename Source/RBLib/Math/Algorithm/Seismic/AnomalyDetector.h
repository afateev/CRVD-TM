///////////////////////////////////////////////////////////////////////////////
//	Блок алгоритма обработки сигнала - определение аномалии
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MATH_ALGORITHM_ANOMALYDETECTOR_H
#define MATH_ALGORITHM_ANOMALYDETECTOR_H

#include "../AlgorithmBlockBase.h"

namespace Rblib
{
namespace Math
{
namespace Algorithm
{
	template <
				typename valueType,	// тип входных значений
				int bufferSize		// размер буфера
			 >
	class BackFonBuffer : public AlgorithmBlockBase<valueType, valueType *>
	{
	public:
		typedef valueType ValueTypeIn;
		typedef valueType *ValueTypeOut;
		typedef AlgorithmBlockBase<valueType, ValueTypeOut> Base;

		static const unsigned char MaxFreqSampleIndexValue = 32;	// максимальное значение прореживание элементов для записи в буфер
		static const int BufferSize = bufferSize;					// размер буфера фона
		static const int OutputMultiplicity = 16;					// Кратность вывода данных
		static const int StartFrequency = 4;
		static const int StartOutputFrequency = StartFrequency * 2;
	protected:
		unsigned int _indexBufferElement;		// индекс элемента в буфере
		unsigned char _valueFreqSample;			// величина частоты прореживания
		unsigned char _index;					// индекс элемента для прореживания
		ValueTypeIn _bufferBackFon[BufferSize];	// буфер хранения элементов фона
		int _outputCounter;
		bool _disableUpdate;
	public:
		BackFonBuffer()
		{
			Reset();
		}
		
		virtual ~BackFonBuffer()
		{
		}
		
		virtual void Reset()
		{
			Base::Reset();
			_indexBufferElement = 0;
			_valueFreqSample = StartFrequency;
			_index = 0;
			_outputCounter = 0;
			_disableUpdate = false;
		}

		virtual void In(ValueTypeIn sample)
		{
			if (!_disableUpdate || _valueFreqSample < StartOutputFrequency)
			{
				_index++;
				if(_index >= _valueFreqSample)	// прореживание входного сигнала
				{
					_index = 0;
					_bufferBackFon[_indexBufferElement] = sample;	// запись значения сигнала в буфер фона
				
					_indexBufferElement++;
					// заполнили буфер
					if (_indexBufferElement >= BufferSize)
					{
						_indexBufferElement = 0;
					
						if (_valueFreqSample < MaxFreqSampleIndexValue)
						{
							_valueFreqSample *= 2;	// удвоение частоты прореживания
						}
					}
				}
			}
			
			_outputCounter++;
			if (_outputCounter >= OutputMultiplicity)
			{
				_outputCounter = 0;
				if(_valueFreqSample >= StartOutputFrequency)
				{
					Base::Output(_bufferBackFon);
				}
			}
		}

		void InDisableUpdate(bool val)
		{
			_disableUpdate = val;
		}
	};

	template <
				typename valueTypeIn,	// тип входных значений
		  		typename valueTypeOut,	// тип выходных значений
				int bufferSize		// размер буфера
			 >
	class AnomalyThreshold : public AlgorithmBlockBase<valueTypeIn*, valueTypeOut>
	{
	public:
		typedef valueTypeIn* ValueTypeIn;
		typedef valueTypeIn ValueType;
		typedef valueTypeOut ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		
		typedef CallbackWrapper <unsigned int> NoiseLevelOutNrb3CallbackType;
		static const int NoiseLevelMax = 1000;		// Максимальное значение шума
		
		static const int BufferSize = bufferSize;					// размер буфера сортировки
	protected:
		ValueTypeOut _anomalyGate;					// порог аномалии
	public:
		// вывод шума в диапазоне от 0 до 1000, для алгоритма НРБ3
		NoiseLevelOutNrb3CallbackType NoiseLevelOutNrb3;

	public:
		AnomalyThreshold()
		{
			Reset();
		}
		
		virtual ~AnomalyThreshold()
		{
		}

		virtual void In(ValueTypeIn buff)
		{
			ValueType bufferForSort[BufferSize];	// буфер хранения элементов фона

			for(int i = 0; i < BufferSize; i++)	// копируем значения себе для фильтрации
			{
				bufferForSort[i] = buff[i];
			}
			
			// сортировка буфера методом пузырька
			bool work = false;
			
			do
			{
				work = false;
				
				for(int i = 0; i < (BufferSize - 1); i++)
				{
					if (bufferForSort[i] < bufferForSort[i + 1])
					{
						ValueType tmpElementForSort = bufferForSort[i];
						bufferForSort[i] = bufferForSort[i + 1];
						bufferForSort[i + 1] = tmpElementForSort;
						work = true;
					}
				}
			}
			while(work);

			//медиана
			unsigned int indexforMedian = 0;
			indexforMedian = BufferSize / 2;
			float median2 = bufferForSort[indexforMedian] + bufferForSort[indexforMedian - 1];
			// расчет порога аномалии
			float anomalyGate = 0;
			anomalyGate += median2;
			anomalyGate += median2 / 2.0;
			
			Base::Output((ValueTypeOut)anomalyGate);
			
			float noiseLevel = anomalyGate;
			if (noiseLevel > NoiseLevelMax)
			{
				noiseLevel = NoiseLevelMax;
			}
			// вывод без преобразования в проценты
			NoiseLevelOutNrb3.Call( (unsigned int)noiseLevel);
		}
		
		virtual void Reset()
		{
			Base::Reset();
		}
	};

	template <
				typename sampleValueType,
				typename anomalyGateValueType
			 >
	class Detector
	{
	public:
		typedef sampleValueType SampleValueType;
		typedef anomalyGateValueType AnomalyGateValueType;
		typedef bool OutputValueType;
		typedef int DetectorLevelValueType;
		typedef CallbackWrapper<OutputValueType> OutCallbackType;
		typedef CallbackWrapper<int, float> TestPointCallbackType;

		static const int LevelDetectorHi = 40;	// верхняя граница детектора
		static const int LevelDetectorLo = 0;	// нижняя граница детектора
		static const int LevelDetectorAnomalyThreshold = 30;	// граница детектирования аномалии
		static const int DetectorIncStep = 10;		//  положительный шаг детектора
		static const int DetectorDecStep = 1;		//  отрицательный шаг детектора
		static const int OutputMultiplicity = 16;	// Выдавать результат каждые OutputMultiplicity входных сэмплов
		static const int ThresholdMax = 1000;		// Максимальное значение порога
		
	public:
		OutCallbackType Out;
		OutCallbackType DebugOut;
		TestPointCallbackType TestPoint;
		float Threshold;							// Текущий порог, значение в перделах от 0 до 100, пересчитывается в нужный диапазон перед использованием
	protected:
		SampleValueType _maxSample;					// Последний пришедший семпл
		DetectorLevelValueType _currentLevel;		// текущий уровень детектора аномалии
	public:
		Detector()
		{
			Reset();
		}
		
		virtual ~Detector()
		{
		}

		// устанавливаем в местных попугаях
		void SetThreshold(float val)
		{
			Threshold = val;
		}
		
		// возвращаем в местных попугаях
		float GetThreshold()
		{
			return Threshold;
		}
		
		void InSample(SampleValueType sample)
		{
			if (sample > _maxSample)
			{
				_maxSample = sample;
			}
		}
		
		virtual void InGate(AnomalyGateValueType gateLevel)
		{
			AnomalyGateValueType level = gateLevel;
			// Threshold задается в местных попугаях преобразование не требуется
			float levelThreshold = Threshold;
			
			if (level < levelThreshold)	//ограничение снизу
			{
				level = (AnomalyGateValueType)levelThreshold;
			}

			if (_maxSample > level)	//превышение уровня
			{
				_currentLevel += DetectorIncStep;
			
				if (_currentLevel > LevelDetectorHi)
				{
					_currentLevel = LevelDetectorHi;
				}
			}
			else
			{
				_currentLevel -= DetectorDecStep;
			
				if (_currentLevel < LevelDetectorLo)
				{
					_currentLevel = LevelDetectorLo;
				}
			}

			TestPoint.Call(_currentLevel, gateLevel);

			bool result = _currentLevel > LevelDetectorAnomalyThreshold;
			Output(result);
			
			_maxSample = 0;
		}
		
		static void InGateCallback(void *callbackParam, AnomalyGateValueType val)
		{
			Detector *im = (Detector *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->InGate(val);
		}
		
		virtual void Reset()
		{
			_currentLevel = LevelDetectorLo;
			Threshold = 0;
			_maxSample = 0;
		}
	protected:
		virtual void Output(OutputValueType val)
		{
			Out.Call(val);
			DebugOut.Call(val);
		}
	};

	struct AnomalyDetectorOutputStruct
	{
		bool HumanDetected;
		bool VehicleDetected;
		
		AnomalyDetectorOutputStruct()
		{
			Reset();
		}
		
		void Reset()
		{
			HumanDetected = false;
			VehicleDetected = false;
		}
	};

	
	
	template <
				typename valueTypeIn	// тип входных значений
			 >
	class AnomalyDetector : public AlgorithmBlockBase<valueTypeIn, AnomalyDetectorOutputStruct>
	{
	public:
		typedef valueTypeIn ValueTypeIn;
		typedef AnomalyDetectorOutputStruct ValueTypeOut;
		typedef AlgorithmBlockBase<ValueTypeIn, ValueTypeOut> Base;
		
		static const unsigned int BufferSize = 64;
		
		typedef BackFonBuffer<ValueTypeIn, BufferSize> BackFonBufferType;
		typedef AnomalyThreshold<ValueTypeIn, ValueTypeIn, BufferSize> AnomalyThresholdType;
		typedef Detector<ValueTypeIn, ValueTypeIn> DetectorType;
	public:
		BackFonBufferType BackFonBuffer;
		AnomalyThresholdType AnomalyThreshold;
		DetectorType DetectorHuman;
		DetectorType DetectorVehicle;
	protected:
		AnomalyDetectorOutputStruct _outputVal;
	public:
		AnomalyDetector()
		{
			BackFonBuffer.Out.Set(AnomalyThreshold.InCallback, &AnomalyThreshold);
			AnomalyThreshold.Out.Set(AnomalyThresholdOutCallback, this);
			DetectorHuman.Out.Set(DetectorHumanCallback, this);
			DetectorVehicle.Out.Set(DetectorVehicleCallback, this);
		}
		
		virtual ~AnomalyDetector()
		{
		}
		
		virtual void In(ValueTypeIn val)
		{
			BackFonBuffer.In(val);
			DetectorHuman.InSample(val);
			DetectorVehicle.InSample(val);
		}
		
		void InDisableBackBufferUpdate(bool val)
		{
			BackFonBuffer.InDisableUpdate(val);
		}
		
		virtual void Reset()
		{
			Base::Reset();
			BackFonBuffer.Reset();
			AnomalyThreshold.Reset();
			DetectorHuman.Reset();
			DetectorHuman.Threshold = 0;
			DetectorVehicle.Reset();
			DetectorVehicle.Threshold = 0;
			_outputVal.Reset();
		}
	protected:
		static void AnomalyThresholdOutCallback(void *callbackParam, typename AnomalyThresholdType::ValueTypeOut val)
		{
			AnomalyDetector *im = (AnomalyDetector *)callbackParam;
			if(!im)
			{
				return;
			}
			
			im-> DetectorHuman.InGate(val);
			im-> DetectorVehicle.InGate(val);
		}

		static void DetectorHumanCallback(void *callbackParam, bool val)
		{
			AnomalyDetector *im = (AnomalyDetector *)callbackParam;
			if(!im)
			{
				return;
			}
			im->_outputVal.HumanDetected = val;
		}

		static void DetectorVehicleCallback(void *callbackParam, bool val)
		{
			AnomalyDetector *im = (AnomalyDetector *)callbackParam;
			if(!im)
			{
				return;
			}
			im-> _outputVal.VehicleDetected = val;
			im-> Output(im->_outputVal);
		}
	};
}
}
}
#endif