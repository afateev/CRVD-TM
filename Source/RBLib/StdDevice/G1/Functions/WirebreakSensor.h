///////////////////////////////////////////////////////////////////////////////
//	 ласс реализации работы с функцие устройтсва "обрывной сенсор" по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_FUNCTIONS_WIREBREAK_SENSOR_H
#define G1_FUNCTIONS_WIREBREAK_SENSOR_H

#include "../Function.h"

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
	class WirebreakSensor : public Base
	{
	public:

		static const int TimerTickCountPerSecond = timerTickCountPerSecond;
	
	protected:

		enum State
		{
			StateDisabled = 0,
			StateEnabled,
			StateTxGetParams,
			StateTxGetParamsWaitDelivery,
			StateRxParamsWait,
			StateIdle
		};

	public:

		WirebreakSensor()
		: _needStateRequest(false)
		, _alarm(false)
		, _state(StateDisabled)
		{
		}
		
		//
		// "высокоуровневый" интерфейс
		//
				
		inline void UpdateState()
		{
			_needStateRequest = true;
		}

		virtual bool WaitResponseFlag()
		{
			bool res = Base::WaitResponseFlag();
			
			res |= _state > StateEnabled && _state < StateIdle;
			
			return res;
		}

		inline bool Alarm()
		{
			return _alarm;
		}

		//
		// "низкокоуровневый" интерфейс
		//

		virtual Protocol::G1::Device::FunctionCode FunctionCode()
		{
			return Protocol::G1::Device::FunctionCodeWirebreakSensor;
		}
		
		virtual void Reset()
		{
			Base::Reset();
		}

		virtual void SetEnabled()
		{
			Base::SetEnabled();
			
			_state = StateEnabled;
			
			UpdateState();
		}

		virtual void OnTick()
		{
			Base::OnTick();

			switch(_state)
			{
			case StateDisabled:
				break;

			case StateEnabled:

				if (_needStateRequest)
				{
					_state = StateTxGetParams;
				}

				break;

			case StateTxGetParams:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::WirebreakSensor::MessageCodeGetWireState, 0, 0, Base::DefaultTimeout);
					_state = StateTxGetParamsWaitDelivery;
				}

				break;

			case StateTxGetParamsWaitDelivery:
			case StateRxParamsWait:

				Base::_waitTimeout--;

				if (Base::_waitTimeout < 1)
				{
					_state = StateTxGetParams;
				}

				break;

			case StateIdle:

				_state = StateEnabled;
				break;
			}
		}

		virtual void OnTxComplete(int packetId, bool delivered)
		{
			Base::OnTxComplete(packetId, delivered);
			
			// дождались завершени€ передачи
			if (packetId != Base::_waitPacketId)
				return;

			if (_state != StateTxGetParamsWaitDelivery)
				return;

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

		virtual void OnCommandFromFunction(unsigned char *data, int dataCount)
		{
			Base::OnCommandFromFunction(data, dataCount);

			Protocol::G1::Function::WirebreakSensor::InputMessageFormat command(data, dataCount);

			if (command.Valid())
			{
				if (command.Data.Valid())
				{
					switch(command.Data.Code)
					{
					case Protocol::G1::Function::WirebreakSensor::MessageCodeWireState:
						HandleWireState((unsigned char*)command.Data.Data, command.Data.DataLen);
						break;
					case Protocol::G1::Function::WirebreakSensor::MessageCodeWireStateChanged:
						HandleWireStateChanged((unsigned char*)command.Data.Data, command.Data.DataLen);
						break;
					}
				}
			}
		}

		int SendCommandToFunction(Protocol::G1::Function::WirebreakSensor::MessageCode code, void *data, int dataCount, int timeout)
		{
			Protocol::G1::Function::WirebreakSensor::OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(Base::_functionData, sizeof(Base::_functionData));

			int packetId = 0;
			Base::SendCommandToFunctionCallback.Call(Base::_functionData, functionDataLen, timeout, packetId);

			return packetId;
		}
	
	protected:

		//
		// message handlers
		//

		// ответ на запрос состо€ни€
		void HandleWireState(unsigned char *data, int dataCount)
		{
			ParseState(data, dataCount);
			SendParamsReceivedEvent();

			if (_state == StateRxParamsWait)
			{
				_needStateRequest = false;
				_state = StateIdle;
			}
		}

		// состо€ние изменилось
		inline void HandleWireStateChanged(unsigned char *data, int dataCount)
		{
			ParseState(data, dataCount);
			SendParamsReceivedEvent();
		}

		//
		//
		//

		// парсинг состо€ни€ сенсора
		void ParseState(unsigned char *data, int dataCount)
		{
			if (dataCount < 2 || data[0] < 1) // мимимум один обрывной сенсор должен быть (в 1-м байте количество обрывников)
				return;

			for (int i = 1; i <= data[0]; i++) // если хоть один оборван, выставл€ем тревогу
			{
				if (!data[i]) // i-й шлейф оборван
				{
					SetAlarm(true);
					return;
				}
			}

			SetAlarm(false);
		}
		
		void SetAlarm(bool newState)
		{
			if (_alarm != newState)
			{
				_alarm = newState;
				
				if (newState)
				{
					Event e(Base::EventCode, EventCodeAlarm, Base::_deviceId, FunctionCode());
					Singletone<EventDispetcher>::Instance().PushEvent(e);
				}
				else
				{
					Event e(Base::EventCode, EventCodeAlarmCleared, Base::_deviceId, FunctionCode());
					Singletone<EventDispetcher>::Instance().PushEvent(e);
				}
			}
		}

		// уведомление о получении параметров
		inline void SendParamsReceivedEvent()
		{
			Event e(Base::EventCode, EventCodeParamsReceived, Base::_deviceId, FunctionCode());
			Singletone<EventDispetcher>::Instance().PushEvent(e);
		}

	protected:

		bool _needStateRequest;
		bool _alarm;

		State _state;

	};

}
}
}
}

#endif
