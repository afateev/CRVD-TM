///////////////////////////////////////////////////////////////////////////////
//	Класс реализации работы с функцие устройтсва "сейсмический сенсор" по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_FUNCTIONS_SEISMICSENSOR_H
#define G1_FUNCTIONS_SEISMICSENSOR_H

#include "../Function.h"
#include "../../../Protocol/G1/Function/SeismicSensor/FunctionData.h"

namespace Rblib
{
namespace StdDevice
{
namespace G1
{
namespace Functions
{
	template
		<
			class Base,
			int timerTickCountPerSecond
		>
	class SeismicSensor : public Base
	{
	public:
		static const int TimerTickCountPerSecond = timerTickCountPerSecond;
		static const int MaxParamsCount = 6;
		
		enum State
		{
			StateDisabled = 0,
			StateEnabled,
			StateTxGetParams,
			StateTxGetParamsWaitDelivery,
			StateRxParamsWait,
			StateTxSetParams,
			StateTxSetParamsWaitDelivery,
			StateRxSetParamsResultWait,
			StateTxAutotuningStart,
			StateTxAutotuningStartWaitDelivery,
			StateTxAutotuningStartResultWait,
			StateIdle
		};
		
		typedef Protocol::G1::Function::SeismicSensor::ParamCode ParamCodeType;
		typedef Protocol::G1::Function::SeismicSensor::ParamValueType ParamValueType;
		typedef map_static<ParamCodeType, ParamValueType, MaxParamsCount> ParamsListType;
		typedef Protocol::G1::Function::SeismicSensor::AlarmParamCode AlarmParamCodeType;
		typedef Protocol::G1::Function::SeismicSensor::MessageAlarm::ParamValueType AlarmParamValueType;
	protected:
		bool _alarmState;
		bool _humanDetected;
		bool _vehicleDetected;
		int _alarmCountDown;
		bool _getParams;
		bool _setParams;
		bool _autotuningStart;
		bool _autotuningWaitComplete;
		State _state;
		
		ParamsListType _paramsLatsReceived;
		ParamsListType _paramsToSet;
		float _signalLevel;
		
		static const int BufferSize = 1 + MaxParamsCount * (sizeof(ParamCodeType) + sizeof(ParamValueType));
		static unsigned char _buffer[BufferSize];
	public:
		SeismicSensor()
		{
			_alarmState = false;
			_humanDetected = false;
			_vehicleDetected = false;
			_alarmCountDown = 0;
			_getParams = false;
			_setParams = false;
			_signalLevel = 0;
			_autotuningStart = false;
			_autotuningWaitComplete = false;
		}
		
		virtual Protocol::G1::Device::FunctionCode FunctionCode()
		{
			return Protocol::G1::Device::FunctionCodeSeismicSensor;
		}
		
		virtual void Reset()
		{
			Base::Reset();
			
			_state = StateDisabled;
			_signalLevel = 0;
			_paramsLatsReceived.clear();
			_autotuningWaitComplete = false;
		}
		
		virtual void SetEnabled()
		{
			Base::SetEnabled();
			
			_state = StateEnabled;
			
			RequestParams();
		}
		
		virtual void OnTick()
		{
			Base::OnTick();
			
			if (_alarmCountDown)
			{
				_alarmCountDown--;
				
				if (_alarmCountDown < 1)
				{
					SetAlarm(false);
				}
			}
			
			switch(_state)
			{
			case StateDisabled:
				{
				}
				break;
			case StateEnabled:
				{
					if (_getParams)
					{
						_state = StateTxGetParams;
						break;
					}
					
					if (_setParams)
					{
						_state = StateTxSetParams;
						break;
					}
					
					if (_autotuningStart)
					{
						_state = StateTxAutotuningStart;
						break;
					}
				}
				break;
			case StateTxGetParams:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = Base::DefaultTimeout;
					
					unsigned char paramsCount = 0;
					
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::SeismicSensor::MessageCodeGetParams, &paramsCount, sizeof(paramsCount), deliveryTimeout);
					_state = StateTxGetParamsWaitDelivery;
				}
				break;
			case StateTxGetParamsWaitDelivery:
			case StateRxParamsWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						_state = StateTxGetParams;
					}
				}
				break;
			case StateTxSetParams:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = Base::DefaultTimeout;
					
					unsigned char paramsCount = _paramsToSet.size();
					
					_buffer[0] = paramsCount;
					
					ParamsListType::iterator p;
					int i = 0;
					for (p = _paramsToSet.begin(); p != _paramsToSet.end(); p++, i++)
					{
						_buffer[1 + i] = p->first;
						ParamValueType paramValue = p->second;
						for (int j = 0; j < sizeof(ParamValueType); j++)
						{
							_buffer[1 + paramsCount + sizeof(ParamValueType) * i + j] = ((unsigned char *)&paramValue)[j];
						}
					}
					
					int dataLen = 1 + paramsCount * (sizeof(ParamCodeType) + sizeof(ParamValueType));
					
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::SeismicSensor::MessageCodeSetParams, _buffer, dataLen, deliveryTimeout);
					// начали отправлять, если пользователь за это время что то поменяет, то флаг опять выставится и процесс начнется по новой
					_setParams = false;
					_state = StateTxSetParamsWaitDelivery;
				}
				break;
			case StateTxSetParamsWaitDelivery:
			case StateRxSetParamsResultWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						_state = StateTxSetParams;
					}
				}
				break;
			case StateTxAutotuningStart:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = Base::DefaultTimeout;
					
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::SeismicSensor::MessageCodeAutotuningStart, 0, 0, deliveryTimeout);
					_state = StateTxAutotuningStartWaitDelivery;
				}
				break;
			case StateTxAutotuningStartWaitDelivery:
			case StateTxAutotuningStartResultWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						_state = StateTxAutotuningStart;
					}
				}
				break;
			case StateIdle:
				{
					_state = StateEnabled;
				}
				break;
			}
		}
		
		int SendCommandToFunction(Protocol::G1::Function::SeismicSensor::MessageCode code, void *data, int dataCount, int timeout)
		{
			Protocol::G1::Function::SeismicSensor::OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(Base::_functionData, sizeof(Base::_functionData));
			
			int packetId = 0;
			Base::SendCommandToFunctionCallback.Call(Base::_functionData, functionDataLen, timeout, packetId);
			return packetId;
		}
		
		virtual void OnTxComplete(int packetId, bool delivered)
		{
			Base::OnTxComplete(packetId, delivered);
			
			bool waitComplete = packetId == Base::_waitPacketId;
			// дождались завершения передачи
			if (waitComplete)
			{
				switch(_state)
				{
				case StateTxGetParamsWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxParamsWait;
						}
						else
						{
							_state = StateTxGetParams;
						}
					}
					break;
				case StateTxSetParamsWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxSetParamsResultWait;
						}
						else
						{
							_state = StateTxSetParams;
						}
					}
					break;
				case StateTxAutotuningStartWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateTxAutotuningStartResultWait;
						}
						else
						{
							_state = StateTxAutotuningStart;
						}
					}
					break;
				}
			}
		}
		
		virtual void OnCommandFromFunction(unsigned char *data, int dataCount)
		{
			Base::OnCommandFromFunction(data, dataCount);
			
			Protocol::G1::Function::SeismicSensor::InputMessageFormat command(data, dataCount);
			if (command.Valid())
			{
				if (command.Data.Valid())
				{
					switch(command.Data.Code)
					{
					case Protocol::G1::Function::SeismicSensor::MessageCodeParams:
						{
							// Обновляем хранящийся у нас список параметров
							_paramsLatsReceived.clear();
							
							if (command.Data.DataLen >= 1)
							{
								unsigned char *data = (unsigned char *)command.Data.Data;
								if (data)
								{
									int paramsCount = data[0];
									
									for (int i = 0; i < paramsCount; i++)
									{
										ParamCodeType paramId = (ParamCodeType)data[1 + i];
										
										ParamValueType paramValue = 0;
										for (int j = 0; j < sizeof(ParamValueType); j++)
										{
											((unsigned char *)&paramValue)[j] = data[1 + paramsCount + sizeof(ParamValueType) * i + j];
										}
										
										ParamsListType::iterator p = _paramsLatsReceived.find(paramId);
										if (p == _paramsLatsReceived.end())
										{
											_paramsLatsReceived.insert(ParamsListType::value_type(paramId, paramValue));
										}
										else
										{
											p->second = paramValue;
										}
									}
								}
							}
							
							switch(_state)
							{
							case StateRxParamsWait:
								{
									_getParams = false;
									_state = StateIdle;
								}
								break;
							}
							
							// уведомление о получении параметров
							Event e(Base::EventCode, EventCodeParamsReceived, Base::_deviceId, FunctionCode());
							Singletone<EventDispetcher>::Instance().PushEvent(e);
						}
						break;
					case Protocol::G1::Function::SeismicSensor::MessageCodeSetParamsResult:
						{
							switch(_state)
							{
							case StateRxSetParamsResultWait:
								{
									if (command.Data.DataLen >= 1)
									{
										unsigned char *data = (unsigned char *)command.Data.Data;
										if (data)
										{
											int paramsCount = data[0];
											
											for (int i = 0; i < paramsCount; i++)
											{
												ParamCodeType paramId = (ParamCodeType)data[1 + i];
												
												ParamsListType::iterator p = _paramsToSet.find(paramId);
												if (p != _paramsToSet.end())
												{
													_paramsToSet.erase(p);
												}
											}
										}
									}
									
									_state = StateIdle;
								}
								break;
							}
						}
						break;
					case Protocol::G1::Function::SeismicSensor::MessageCodeAlarm:
						{
							if (command.Data.DataLen >= 1)
							{
								unsigned char *data = (unsigned char *)command.Data.Data;
								if (data)
								{
									int paramsCount = data[0];
									_humanDetected = false;
									_vehicleDetected = false;
									
									for (int i = 0; i < paramsCount; i++)
									{
										AlarmParamCodeType paramId = (AlarmParamCodeType)data[1 + i];
										
										AlarmParamValueType paramValue = 0;
										for (int j = 0; j < sizeof(AlarmParamValueType); j++)
										{
											((unsigned char *)&paramValue)[j] = data[1 + paramsCount + sizeof(AlarmParamValueType) * i + j];
										}
										
										switch(paramId)
										{
										case Protocol::G1::Function::SeismicSensor::AlarmParamCodeHumanDetectedFlag:
											{
												_humanDetected = paramValue > 0;
											}
											break;
										case Protocol::G1::Function::SeismicSensor::AlarmParamCodeVehicleDetectedFlag:
											{
												_vehicleDetected = paramValue > 0;
											}
											break;
										}
									}
								}
							}
							
							SetAlarm(true);
						}
						break;
					case Protocol::G1::Function::SeismicSensor::MessageCodeParamsChanged:
						{
							if (command.Data.DataLen >= 1)
							{
								unsigned char *data = (unsigned char *)command.Data.Data;
								if (data)
								{
									int paramsCount = data[0];
									
									for (int i = 0; i < paramsCount; i++)
									{
										ParamCodeType paramId = (ParamCodeType)data[1 + i];
										
										ParamValueType paramValue = 0;
										for (int j = 0; j < sizeof(ParamValueType); j++)
										{
											((unsigned char *)&paramValue)[j] = data[1 + paramsCount + sizeof(ParamValueType) * i + j];
										}
										
										ParamsListType::iterator p = _paramsLatsReceived.find(paramId);
										if (p == _paramsLatsReceived.end())
										{
											_paramsLatsReceived.insert(ParamsListType::value_type(paramId, paramValue));
										}
										else
										{
											p->second = paramValue;
										}
									}
								}
							}

							// уведомление о получении параметров
							Event e(Base::EventCode, EventCodeParamsReceived, Base::_deviceId, FunctionCode());
							Singletone<EventDispetcher>::Instance().PushEvent(e);
						}
						break;
					case Protocol::G1::Function::SeismicSensor::MessageCodeSignalLevel:
						{
							if (command.Data.DataLen >= 4)
							{
								unsigned char *data = (unsigned char *)command.Data.Data;
								if (data)
								{
									float signalLevel = 0;
									
									for (int j = 0; j < sizeof(signalLevel); j++)
									{
										((unsigned char *)&signalLevel)[j] = data[j];
									}
									
									_signalLevel = signalLevel;
								}
							}
						}
						break;
					case Protocol::G1::Function::SeismicSensor::MessageCodeAutotuningStartResult:
						{
							switch(_state)
							{
							case StateTxAutotuningStartResultWait:
								{
									_autotuningStart = false;
									_state = StateIdle;
									_autotuningWaitComplete = true;
								}
								break;
							}
						}
						break;
					case Protocol::G1::Function::SeismicSensor::MessageCodeAutotuningComplete:
						{
							_autotuningWaitComplete = false;
						}
						break;
					}
				}
			}
		}
		
		void RequestParams()
		{
			_getParams = true;
		}
		
		bool GetParamValue(ParamCodeType paramId, ParamValueType &value)
		{
			ParamsListType::iterator p = _paramsLatsReceived.find(paramId);
			if (p != _paramsLatsReceived.end())
			{
				value = p->second;
				return true;
			}
			
			return false;
		}
		
		void SetParamValue(ParamCodeType paramId, ParamValueType value)
		{
			ParamsListType::iterator p = _paramsToSet.find(paramId);
			if (p == _paramsToSet.end())
			{
				_paramsToSet.insert(ParamsListType::value_type(paramId, value));
			}
			else
			{
				p->second = value;
			}
			
			_setParams = true;
		}
		
		bool Alarm()
		{
			return _alarmState;
		}
		
		virtual bool WaitResponseFlag()
		{
			bool res = Base::WaitResponseFlag();
			
			res |= _state > StateEnabled && _state < StateIdle;
			
			return res;
		}
		
		bool HumanDetected()
		{
			return _humanDetected;
		}
		
		bool VehicleDetected()
		{
			return _vehicleDetected;
		}
		
		bool SignalLevel(float &value)
		{
			value = _signalLevel;
			return StateDisabled != _state;
		}
		
		bool GetThresholdHuman(float &value)
		{
			return GetParamValue(Protocol::G1::Function::SeismicSensor::ParamCodeThresholdHuman, value);
		}
		
		bool GetThresholdVehicle(float &value)
		{
			return GetParamValue(Protocol::G1::Function::SeismicSensor::ParamCodeThresholdVehicle, value);
		}
		
		void SetThresholdHuman(float value)
		{
			SetParamValue(Protocol::G1::Function::SeismicSensor::ParamCodeThresholdHuman, value);
		}
		
		void SetThresholdVehicle(float value)
		{
			SetParamValue(Protocol::G1::Function::SeismicSensor::ParamCodeThresholdVehicle, value);
		}

		bool ThresholdHumanSetting()
		{
			ParamsListType::iterator p = _paramsToSet.find(Protocol::G1::Function::SeismicSensor::ParamCodeThresholdHuman);
			return p != _paramsToSet.end();
		}

		bool ThresholdVehicleSetting()
		{
			ParamsListType::iterator p = _paramsToSet.find(Protocol::G1::Function::SeismicSensor::ParamCodeThresholdVehicle);
			return p != _paramsToSet.end();
		}
		
		void AutotuningStart()
		{
			_autotuningStart = true;
		}
		
		bool AutotuningWaitStart()
		{
			return _autotuningStart;
		}
		bool AutotuningWaitComplete()
		{
			return _autotuningWaitComplete;
		}
	protected:
		int SecondsToTimerTicks(int seconds)
		{
			return TimerTickCountPerSecond * seconds;
		}
		
		void SetAlarm(bool newState)
		{
			if (_alarmState != newState)
			{
				_alarmState = newState;
				
				if (newState)
				{
					_alarmCountDown = SecondsToTimerTicks(5);
					
					Event e(Base::EventCode, EventCodeAlarm, Base::_deviceId, FunctionCode());
					Singletone<EventDispetcher>::Instance().PushEvent(e);
				}
				else
				{
					_humanDetected = false;
					_vehicleDetected = false;
					Event e(Base::EventCode, EventCodeAlarmCleared, Base::_deviceId, FunctionCode());
					Singletone<EventDispetcher>::Instance().PushEvent(e);
				}
			}
		}
	};
	
	template <class Base, int timerTickCountPerSecond>
		unsigned char SeismicSensor<Base,timerTickCountPerSecond>::_buffer[SeismicSensor<Base, timerTickCountPerSecond>::BufferSize];
}
}
}
}

#endif