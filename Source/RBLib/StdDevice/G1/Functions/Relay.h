///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ������ � ������� ���������� "���" �� ��������� G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_FUNCTIONS_RELAY_H
#define G1_FUNCTIONS_RELAY_H

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
	class Relay : public Base
	{
	public:

		static const int TimerTickCountPerSecond = timerTickCountPerSecond;
	
	protected:

		enum State
		{
			StateDisabled = 0,
			StateEnabled,
			StateTxGetState,
			StateTxGetStateWaitDelivery,
			StateRxStateWait,
			StateTxGetParams,
			StateTxGetParamsWaitDelivery,
			StateRxParamsWait,
			StateTxSetState,
			StateTxSetStateWaitDelivery,
			StateRxSetStateResultWait,
			StateTxSetParams,
			StateTxSetParamsWaitDelivery,
			StateRxSetParamsResultWait,
			StateIdle
		};

		bool _needStateRequest;
		bool _needStateSet;
		bool _paramsRequest;
		bool _paramsSet;

		bool _currentState;
		float _operationTime;

		// ��������� �� ����������

		bool _onToSet;
		float _operationTimeToSet;

		// ������� ���������

		State _state;

	public:

		Relay()
		: _needStateRequest(false)
		, _needStateSet(false)
		, _paramsRequest(false)
		, _paramsSet(false)
		, _currentState(false)
		, _operationTime(0)
		, _state(StateDisabled)
		{
		}

		//
		// "���������������" ���������
		//

		inline void UpdateState()
		{
			_needStateRequest = true;
		}

		inline void UpdateParams()
		{
			_paramsRequest = true;
		}

		virtual bool WaitResponseFlag()
		{
			bool res = Base::WaitResponseFlag();
			
			res |= _state > StateEnabled && _state < StateIdle;
			
			return res;
		}

		inline bool On()
		{
			return _currentState;
		}

		inline bool Off()
		{
			return !_currentState;
		}

		inline void SetOn()
		{
			SetState(true);
		}

		inline void SetOff()
		{
			SetState(false);
		}

		inline void SetState(bool on)
		{
			_onToSet = on;
			_needStateSet = true;
		}

		inline float OperationTime()
		{
			return _operationTime;
		}

		inline void SetOperationTime(float time)
		{
			_operationTimeToSet = time;
			_paramsSet = true;
		}

		//
		// "����������������" ���������
		//

		virtual Protocol::G1::Device::FunctionCode FunctionCode()
		{
			return Protocol::G1::Device::FunctionCodeRelay;
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
			UpdateParams();
		}

		// �������� ������
		virtual void OnTick()
		{
			Base::OnTick();

			RunState();
		}

		// ������ ����� �� ����������
		virtual void OnCommandFromFunction(unsigned char *data, int dataCount)
		{
			Base::OnCommandFromFunction(data, dataCount);

			Protocol::G1::Function::Relay::InputMessageFormat command(data, dataCount);

			if (command.Valid())
			{
				if (command.Data.Valid())
				{
					switch(command.Data.Code)
					{
					case Protocol::G1::Function::Relay::MessageCodeRelayState:
						HandleRelayState((unsigned char*)command.Data.Data, command.Data.DataLen);
						break;
					case Protocol::G1::Function::Relay::MessageCodeRelayStateChanged:
						HandleRelayStateChanged((unsigned char*)command.Data.Data, command.Data.DataLen);
						break;
					case Protocol::G1::Function::Relay::MessageCodeSetRelayStateResult:
						HandleSetRelayStateResult((unsigned char*)command.Data.Data, command.Data.DataLen);
						break;
					case Protocol::G1::Function::Relay::MessageCodeParams:
						HandleParams((unsigned char*)command.Data.Data, command.Data.DataLen);
						break;
					case Protocol::G1::Function::Relay::MessageCodeParamsChanged:
						HandleParamsChanged((unsigned char*)command.Data.Data, command.Data.DataLen);
						break;
					case Protocol::G1::Function::Relay::MessageCodeSetParamsResult:
						HandleSetParamsResult((unsigned char*)command.Data.Data, command.Data.DataLen);
						break;
					}
				}
			}
		}

		// ��������� ���������� ��������
		virtual void OnTxComplete(int packetId, bool delivered)
		{
			Base::OnTxComplete(packetId, delivered);

			// ��������� ���������� ��������
			if (packetId != Base::_waitPacketId)
				return;

			if (_state == StateTxGetStateWaitDelivery)
			{
				if (delivered)
				{
					Base::_waitTimeout = 5 * TimerTickCountPerSecond;
					_state = StateRxStateWait;
				}
				else
				{
					_state = StateTxGetState;
				}
			}
			else if (_state == StateTxSetStateWaitDelivery)
			{
				if (delivered)
				{
					Base::_waitTimeout = 5 * TimerTickCountPerSecond;
					_state = StateRxSetStateResultWait;
				}
				else
				{
					_state = StateTxSetState;
				}
			}
			else if (_state == StateTxGetParamsWaitDelivery)
			{
				if (delivered)
				{
					Base::_waitTimeout = 5 * TimerTickCountPerSecond;
					_state = StateRxParamsWait;
				}
				else
				{
					_state = StateTxGetState;
				}
			}
			else if (_state == StateTxSetParamsWaitDelivery)
			{
				if (delivered)
				{
					Base::_waitTimeout = 5 * TimerTickCountPerSecond;
					_state = StateRxSetParamsResultWait;
				}
				else
				{
					_state = StateTxSetState;
				}
			}
		}
	
	protected:

		//
		// ��� �������� ������
		//
		void RunState()
		{
			switch(_state)
			{
			case StateDisabled:
				break;

			case StateEnabled:

				if (_needStateRequest)
				{
					_state = StateTxGetState;
					_needStateRequest = false;
				}
				else if (_needStateSet)
				{
					_state = StateTxSetState;
					_needStateSet = false;
				}
				else if (_paramsRequest)
				{
					_state = StateTxGetParams;
					_paramsRequest = false;
				}
				else if (_paramsSet)
				{
					_state = StateTxSetParams;
					_paramsSet = false;
				}
				break; 

			//
			// ������ ���������
			// 
			case StateTxGetState:

				GetState();
				_state = StateTxGetStateWaitDelivery;

				break;

			case StateTxGetStateWaitDelivery:
			case StateRxStateWait:

				if (--Base::_waitTimeout < 1)
					_state = StateTxGetState;
				break;

			//
			// ������ ����������
			// 
			case StateTxGetParams:

				GetParams();
				_state = StateTxGetParamsWaitDelivery;

				break;

			case StateTxGetParamsWaitDelivery:
			case StateRxParamsWait:

				if (--Base::_waitTimeout < 1)
					_state = StateTxGetParams;
				break;

			//
			// ��������� ������ ���������
			// 
			case StateTxSetState:

				SetState();
				_state = StateTxSetStateWaitDelivery;

				break;

			case StateTxSetStateWaitDelivery:
			case StateRxSetStateResultWait:

				if (--Base::_waitTimeout < 1)
					_state = StateTxSetState;
				break;

			//
			// ��������� ����������
			// 
			case StateTxSetParams:

				SetParams();
				_state = StateTxSetParamsWaitDelivery;

				break;

			case StateTxSetParamsWaitDelivery:
			case StateRxSetParamsResultWait:

				if (--Base::_waitTimeout < 1)
					_state = StateTxSetParams;
				break;

			//
			// ���� �����������
			//
			case StateIdle:

				_state = StateEnabled;
				break;
			}
		}

		//
		// �������� ���������
		//

		// ������������� ����� ��� ��������
		int SendCommandToFunction(Protocol::G1::Function::Relay::MessageCode code, void *data, int dataCount, int timeout)
		{
			Protocol::G1::Function::Relay::OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(Base::_functionData, sizeof(Base::_functionData));

			int packetId = 0;
			Base::SendCommandToFunctionCallback.Call(Base::_functionData, functionDataLen, timeout, packetId);

			return packetId;
		}

		// ���������� �������� ���������
		inline void SetState()
		{
			Base::_waitTimeout = 60 * TimerTickCountPerSecond;
			Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::Relay::MessageCodeSetRelayState, &_onToSet, 1, Base::DefaultTimeout);
		}

		// ��������� ������� ���������
		inline void GetState()
		{
			Base::_waitTimeout = 60 * TimerTickCountPerSecond;
			Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::Relay::MessageCodeGetRelayState, 0, 0, Base::DefaultTimeout);
		}

		// ���������� �������� �������� ���������� (������ 1 ��������)
		void SetParams()
		{
			// ��������� �����
			Protocol::G1::Function::Relay::ParamCode code = Protocol::G1::Function::Relay::ParamCodeRelayWorkTime;
			Protocol::G1::Function::Relay::MessageParams messageParamsSet(&code, &_operationTimeToSet, 1);

			const unsigned char bufferSize = 1 + sizeof(Protocol::G1::Function::Relay::ParamCode) + sizeof(Protocol::G1::Function::Relay::MessageParams);
			unsigned char buffer[bufferSize];

			int sz = messageParamsSet.ToByteArray(buffer, bufferSize);

			// �������� ������
			Base::_waitTimeout = 60 * TimerTickCountPerSecond;
			Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::Relay::MessageCodeSetParams, buffer, sz, Base::DefaultTimeout);
		}

		// ������ ������� ���������� (������ 1 ��������)
		inline void GetParams()
		{
			unsigned char paramsCount = 0; // 0 - ����� ��� ���������

			// �������� ������
			Base::_waitTimeout = 60 * TimerTickCountPerSecond;
			Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::Relay::MessageCodeGetParams, &paramsCount, 1, Base::DefaultTimeout);
		}

		//
		// message handlers
		//

		// ����� �� ������ ���������
		void HandleRelayState(unsigned char *data, int dataCount)
		{
			ParseState(data, dataCount);
			//SendParamsReceivedEvent();

			if (_state == StateRxStateWait)
				_state = StateIdle;
		}

		// ��������� ����������
		inline void HandleRelayStateChanged(unsigned char *data, int dataCount)
		{
			ParseState(data, dataCount);
			//SendParamsReceivedEvent();
		}

		// ��������� ��������� ������ ����������
		inline void HandleSetRelayStateResult(unsigned char *data, int dataCount)
		{
			if (_state == StateRxSetStateResultWait)
				_state = StateIdle; 
		}

		// ����������� ���������
		inline void HandleParams(unsigned char *data, int dataCount)
		{
			ParseParams(data, dataCount);
			
			if (_state == StateRxParamsWait)
				_state = StateIdle;
		}

		// ��������� ����������
		inline void HandleParamsChanged(unsigned char *data, int dataCount)
		{
			ParseParams(data, dataCount);
		}

		// ��������� ��������� ����������
		inline void HandleSetParamsResult(unsigned char *data, int dataCount)
		{
			if (_state == StateRxSetParamsResultWait)
				_state = StateIdle; 
		}

		//
		// ��������� ������� ������
		//

		// ������� ��������� �������
		void ParseState(unsigned char *data, int dataCount)
		{
			if (dataCount < 1)
				return;
			
			bool newState = data[0];
			if (_currentState != newState)
			{
				if (newState)
				{
					Event e(Base::EventCode, EventCodeActuatorOn, Base::_deviceId, FunctionCode());
					Singletone<EventDispetcher>::Instance().PushEvent(e);
				}
				else
				{
					Event e(Base::EventCode, EventCodeActuatorOff, Base::_deviceId, FunctionCode());
					Singletone<EventDispetcher>::Instance().PushEvent(e);
				}
				
				_currentState = newState;
			}
		}

		// ������� ����������
		void ParseParams(unsigned char *data, int dataCount)
		{
			Protocol::G1::Function::Relay::MessageParams params(data, dataCount);

			if (params.Count < 1)
				return;

			for (int p = 0; p < params.Count; p++)
			{
				if (params.Ids[p] == Protocol::G1::Function::Relay::ParamCodeRelayWorkTime)
				{
					_operationTime = params.Values[p];
				}
			}
		}

		//
		// ����������� � ��������� ����������
		//
		inline void SendParamsReceivedEvent()
		{
			Event e(Base::EventCode, EventCodeParamsReceived, Base::_deviceId, FunctionCode());
			Singletone<EventDispetcher>::Instance().PushEvent(e);
		}
	};

}
}
}
}

#endif

