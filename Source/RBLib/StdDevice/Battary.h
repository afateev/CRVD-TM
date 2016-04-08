///////////////////////////////////////////////////////////////////////////////
//	����� �������� ������� �������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef BATTARY_H
#define BATTARY_H

#include "Battary\BattaryFactory.h"
#include "..\Protocol\InternalCommands.h"
#include "..\Protocol\FirTwig2\RequestParams.h"
#include "..\Protocol\FirTwig2\DispatchParams.h"

namespace Rblib
{
namespace StdDevice
{
/// ������� ������� �������
/// \details ������������� ��������� ��� ������� � ������������ ����������� ������� �������.\n
/// ��� ������� ������������ �������������.
template<class i2cBus, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, class communicationModule>
class Battary
{
protected:
	// ��������� �������� ���������
	enum State
	{
		StateIdle = 0,				// �� ������������������
		StateInited,				// �������������������
		StateSearch,				// ����� ���������
		StateDetected,				// ��������� �������
		StateWtiteConsumerInfo,		// �������� ���������� � �����������
		StateWaitWtiteConsumerInfo, // �������� �������� ���������� � �����������
		StateRunning,				// ���������� ������
		StatePaused					// �����, �� ����� ������������ �������
	};

	// ��������� �������� �������� ��������
	enum RequestProcessState
	{
		RequestProcessStateIdle = 0,
		RequestProcessStateWaitAnswer
	};
	
	// ���������� � �������� �������
	struct RequestInfo
	{
		Protocol::CmdInternal Cmd;	// �������
		unsigned char InterfaceId;	// ���������, � �������� ������ �������
		unsigned char Requester;	// ������� ����� ��������� �������
		unsigned char ParamChar;	// ����� �������� �������
		
		// ����������� �� ���������
		RequestInfo() : Cmd(Protocol::CmdUnknown), InterfaceId(-1), Requester(-1), ParamChar(-1)
		{
		}
		// ����������� � �����������
		RequestInfo(Protocol::CmdInternal cmd, unsigned char interfaceId, unsigned char requester) : Cmd(cmd), InterfaceId(interfaceId), Requester(requester), ParamChar(-1)
		{
		}
	};

public:
	/// ������������� ��������
	static void Init()
	{
		_battary = _battaryFactory::Create(BattaryTypeUnknown);
		_state = StateInited;
		_detectRepeateCounter = 0;
		while(!_requestQueue.empty())
			_requestQueue.pop();
		Singletone<EventDispetcher>::Instance().AddHandler(tickEvent, EventDispetcher::EventFunctor(EventHandler), false);
	}
	
	// ������������� ��������� ������� ������
	inline static void Pause()
	{
		_stateBeforePause = _state;
		_state = StatePaused;
	}

	// ����������� ��������� ������� ������
	inline static void Resume()
	{
		if (_state != StatePaused)
			return;
		_state = _stateBeforePause;
	}

	// ��� �������
	inline static unsigned char Type()
	{
		return _battary ? _battary->GetType() : BattaryTypeUnknown;
	}

	static void SetConsumerInfo(BattaryConsumerInfoStruct consumerInfo)
	{
		_consumerInfo = consumerInfo;
		_hasConsumerInfo = true;
	}

	/// ���������� ������
	static void CommandHandler(Protocol::CmdInfo *cmd, Rblib::Communication::InterfaceBase *outputInterface)
	{
		if (0 == _battary)
			return;
		// ���� ����� ��� �������������, ������ ��������� �������
		if (StateRunning != _state)
			return;
		
		if (0 == cmd)
			return;
		
		if (cmd->Handled)
			return;
		
		switch(cmd->Cmd)
		{
		case Protocol::CmdBattaryGetType:
			{
				// ����� ��������� �� ������ ��������� �������
	  			Protocol::CmdInfo answer(*cmd);
				// �����
				answer.Cmd = Protocol::CmdBattaryNotifyType;
				// ������ ������
				unsigned char battaryType = _battary->GetType();
				
				// ������ ������, ����� ����� ������������� � ����������� ��������� �����
				switch(battaryType)
				{
				case BattaryTypeVip1026:
					{
						BattaryVip1026<i2cBus> *battary = (BattaryVip1026<i2cBus> *)_battary;
						battaryType = battary->GetRealType();
					}
					break;
				}
				// ������ ����� ������
				answer.Data = (unsigned char *)&battaryType;
				// ������ ������
				answer.DataSize = sizeof(battaryType);
				// ���������� �����
				if (outputInterface)
					outputInterface->SendCommand(&answer);
				// �������� ������� ��� ������������
				cmd->Handled = true;
			}
			break;
		case Protocol::CmdBattaryGetRemainingLifeTime:
		case Protocol::CmdBattaryGetInfo:
		case Protocol::CmdBattaryGetState:
		case Protocol::CmdBattaryGetOperatingTime:
		case Protocol::CmdBattaryGetPowerConsumersCount:
		case Protocol::CmdBattaryGetPowerConsumersList:
		case Protocol::CmdBattaryGetResetCounters:
			{
				unsigned char interfaceId = -1;
				unsigned char requester = -1;
				if (outputInterface)
					interfaceId = outputInterface->GetId();
				if (cmd->RequestParams)
				{
					if (Protocol::ProtocolFirTwig2 == cmd->RequestParams->Protocol)
					{
						Protocol::FirTwig2::RequestParams *request = (Protocol::FirTwig2::RequestParams *)cmd->RequestParams;
						requester = request->PacketInfo.NetAddress;
					}
				}
								
				// �������� ������ �������
				RequestInfo request(cmd->Cmd, interfaceId, requester);
				
				// ���� ������������ ���������� �������� �������
				bool paramsValid = false;
				
				// � ��������� ������ ������ ���������
				switch(cmd->Cmd)
				{
				case Protocol::CmdBattaryGetPowerConsumersList:
					{
						// ����� ������
						unsigned char recordNumber;
						// ��������� ������ ������
						unsigned char requiredDataSize = sizeof(recordNumber);
						// ���� ���� ������ � ������ ����������
						if (0 != cmd->Data && cmd->DataSize >= requiredDataSize)
						{
							// �������� ������
							memcpy(&recordNumber, cmd->Data, requiredDataSize);
							request.ParamChar = recordNumber;
							paramsValid = true;
						}
					}
					break;
				default:
					paramsValid = true;
				}
				
				if (paramsValid)
					PushRequest(request);

				// �������� ������� ��� ������������
				cmd->Handled = true;
			}
			break;
		}
	}

	static void EventHandler(const Event &event)
	{
		if (tickEvent == event.Code)
		{
			switch(_state)
			{
			case StateInited:
				{
					Detect();
				}
				break;
			case StateSearch:
				{
					_detectCounter++;
					if (_detectCounter >= DetectMaxCount)
					{
						if (_battary != _battaryDetector)
						{
							DestroyCurrentDetector();
						}
						if (_detectRepeateCounter >= DetectMaxRepeateCount)
						{
							_state = StateRunning;
						}
						else
						{
							_state = StateInited;
						}
					}
					else
					{
						if (_battaryDetector)
						{
							_battaryDetector->Wait();
						}
					}
				}
				break;
			case StateDetected:
				{
					if (_battaryDetector)
					{
						if (_battary)
							delete _battary;
						_battary = _battaryDetector;
						_battaryDetector = 0;
						_state = StateRunning;
					}
					else
					{
						_state = StateInited;
					}
				}
				break;
			case StateWtiteConsumerInfo:
				{
					// �������� ���������� � �����������
					SendConsumerInfo();
				}
				break;
			case StateWaitWtiteConsumerInfo:
				{
					_sendConsumerInfoCounter++;
					if (_sendConsumerInfoCounter >= SendConsumerInfoMaxCount)
					{
						_state = StateRunning;
					}
					else
					{
						if (_battary)
						{
							_battary->Wait();
						}
					}
				}
				break;
			case StateRunning:
				{
					// ���� ���� �������� ���������� � �����������
					if (_hasConsumerInfo && _requestProcessState == RequestProcessStateIdle)
					{
						_state = StateWtiteConsumerInfo;
					}
					else
					{
						// ���� ������ ��������, ���������� �������� �������
						ProcessNextRequest();
					}
				}
				break;
			}
		}
	}

	// ��������� ����������� �� ���������� ������� ������
	static void SendRemainingLifeTimeReport(unsigned char interfaceId, unsigned char netAddress)
	{
		RequestInfo request(Protocol::CmdBattaryGetRemainingLifeTime, interfaceId, netAddress);
		PushRequest(request);
	}

protected:
	/// ����� ������� � ����������� � ����
	static void Detect()
	{
		if (0 == _battary)
			return;
		if (_state < StateInited || _battaryDetector != 0)
			return;
		
		CreateNextDetector();
		DetectSendGetInfoCommand();
		_detectCounter = 0;
		_detectRepeateCounter++;
		_state = StateSearch;
	}

	// ������� ������ ��� ������ ���������
	static void CreateNextDetector()
	{
		DestroyCurrentDetector();
		_battaryDetector = _battaryFactory::Create(BattaryTypeVip1026);
	}
	
	// ���������� ������� ������ ������ ���������
	static void DestroyCurrentDetector()
	{
		if (_battaryDetector != 0)
		{
			delete _battaryDetector;
			_battaryDetector = 0;
		}
	}

	// ��������� ������� ����������� ������� ���������
	static void DetectSendGetInfoCommand()
	{
		if (0 == _battaryDetector)
			return;
		
		// � ����������� �� ���� ������� ��������� ���������� ������, ����� ��� �������� ���
		switch(_battaryDetector->GetType())
		{
		case BattaryTypeVip1026:
			{
				BattaryRequestVip1026Info request;
				_battaryDetector->Request(&request, DetectorOnResponse);
			}
			break;
		}
	}

	static void DetectorOnResponse(bool result, BattaryResponseBase *response)
	{
		if (result && response)
		{
			if (response->GetCommand() == BattaryCommandVip1026Info)
			{
				switch (_battaryDetector->GetType())
				{
				// ���-1026 ������������� ���, ��� ������ ���� ������������� ����� �������
				case BattaryTypeVip1026:
					{
						BattaryVip1026<i2cBus> *battaryDetector = (BattaryVip1026<i2cBus> *)_battaryDetector;
						// ��������� ������������� ����, ������� �������� ������� � ������� BattaryVip1026
						if (battaryDetector->TypeEqual(((BattaryResponseVip1026Info *)response)->Info.Type))
						{
							_state = StateDetected;
						}
					}
					break;
				// ������ "���������������" ���� �������
				default:
					{
						if (((BattaryResponseVip1026Info *)response)->Info.Type == _battaryDetector->GetType())
						{
							_state = StateDetected;
						}
					}
					break;
				}
			}
		}
	}
	
	static void SendConsumerInfo()
	{
		if (0 == _battary)
			return;
		
		switch(_battary->GetType())
		{
		case BattaryTypeVip1026:
			{
				BattaryRequestVip1026ConsumerInfo request;
				request.ConsumerInfo = _consumerInfo;
				_battary->Request(&request, OnResponse);
				// ������ ���� ���, �� �������� �� ���� ���������� ��������� ����� ������ ��� ���
				_hasConsumerInfo = false;
				_sendConsumerInfoCounter = 0;
				_state = StateWaitWtiteConsumerInfo;
			}
			break;
		default:
			_state = StateRunning;
		}
	}

	//
	// ������� ��������
	// 
	// ������� ��������� ������
	static bool PopFrontRequest()
	{
		CRITICAL_BLOCK;
		
		// ���� ���� ��������� ������, �� ������� ����� ��������� ��
		if (!_localRequestQueue.empty())
		{
			_currentRequest = _localRequestQueue.front();
			_localRequestQueue.pop();
			return true;
		}

		if (!_requestQueue.empty())
		{
			_currentRequest = _requestQueue.front();
			_requestQueue.pop();
			return true;
		}

		return false;
	}

	// ��������� ������ � �������
	static void PushRequest(const RequestInfo& request)
	{
		CRITICAL_BLOCK;
		_requestQueue.push(request);
	}

	static void ProcessNextRequest()
	{
		switch (_requestProcessState)
		{
		case RequestProcessStateIdle:
			{
				bool requestPresent = false;

				requestPresent = PopFrontRequest();

				if (requestPresent)
				{
					SendRequest(_currentRequest);
				}
			}
			break;
		case RequestProcessStateWaitAnswer:
			{
				if(_battary)
				{
					_battary->Wait();
				}
			}
			break;
		}
		
	}

	static void SendRequest(RequestInfo requestInfo)
	{
		if (_battary)
		{
			_requestProcessState = RequestProcessStateWaitAnswer;
			switch(requestInfo.Cmd)
			{
			case Protocol::CmdBattaryGetRemainingLifeTime:
				{
					BattaryRequestRemainingLifeTime request;
					_battary->Request(&request, OnResponse);
				}
				break;
			case Protocol::CmdBattaryGetInfo:
				{
					switch(_battary->GetType())
					{
					case BattaryTypeVip1026:
						{
							BattaryRequestVip1026Info request;
							_battary->Request(&request, OnResponse);
						}
						break;
					default:
						_requestProcessState = RequestProcessStateIdle;
					}
				}
				break;
			case Protocol::CmdBattaryGetState:
				{
					switch(_battary->GetType())
					{
					case BattaryTypeVip1026:
						{
							BattaryRequestVip1026State request;
							_battary->Request(&request, OnResponse);
						}
						break;
					default:
						_requestProcessState = RequestProcessStateIdle;
					}
				}
				break;
			case Protocol::CmdBattaryGetOperatingTime:
				{
					switch(_battary->GetType())
					{
					case BattaryTypeVip1026:
						{
							BattaryRequestVip1026State request;
							_battary->Request(&request, OnResponse);
						}
						break;
					default:
						_requestProcessState = RequestProcessStateIdle;
					}
				}
				break;
			case Protocol::CmdBattaryGetPowerConsumersCount:
				{
					switch(_battary->GetType())
					{
					case BattaryTypeVip1026:
						{
							BattaryRequestVip1026ConsumersList request;
							request.RecordNumber = 0;
							_battary->Request(&request, OnResponse);
						}
						break;
					default:
						_requestProcessState = RequestProcessStateIdle;
					}
				}
				break;
			case Protocol::CmdBattaryGetPowerConsumersList:
				{
					switch(_battary->GetType())
					{
					case BattaryTypeVip1026:
						{
							BattaryRequestVip1026ConsumersList request;
							request.RecordNumber = requestInfo.ParamChar;
							_battary->Request(&request, OnResponse);
						}
						break;
					default:
						_requestProcessState = RequestProcessStateIdle;
					}
				}
				break;
			case Protocol::CmdBattaryGetResetCounters:
				{
					switch(_battary->GetType())
					{
					case BattaryTypeVip1026:
						{
							BattaryRequestVip1026ResetCounters request;
							_battary->Request(&request, OnResponse);
						}
						break;
					default:
						_requestProcessState = RequestProcessStateIdle;
					}
				}
				break;
			default:
				_requestProcessState = RequestProcessStateIdle;
			}
		}
		else
		{
			_requestProcessState = RequestProcessStateIdle;
		}
	}

	static void SendAnswer(Protocol::CmdInfo *cmd)
	{
		if (0 == cmd)
			return;

		Protocol::FirTwig2::DispatchParams dispatchParams;

		if (_currentRequest.Requester != -1)
		{
			dispatchParams.NetAddr = _currentRequest.Requester;
			cmd->DispatchParams = &dispatchParams;
		}

		Singletone<communicationModule>::Instance().SendCommand(_currentRequest.InterfaceId, cmd);
	}

	static void OnResponse(bool result, BattaryResponseBase *response)
	{
		if (result && response)
		{
			switch(response->GetCommand())
			{
			case BattaryCommandRemainingLifeTime:
				{
					BattaryResponseRemainingLifeTime *answer = (BattaryResponseRemainingLifeTime *)response;
					// ���� ���� ����� ������������ �������� �������
					if (Protocol::CmdBattaryGetRemainingLifeTime == _currentRequest.Cmd)
					{
						BattaryRemainingLifeTimeInfo data;
						data = answer->Info;
		
						Protocol::CmdInfo cmd(Protocol::CmdBattaryNotifyRemainingLifeTime);
						cmd.Data = (unsigned char*)&data;
						cmd.DataSize = sizeof(data);
		
						SendAnswer(&cmd);
					}
				}
				break;
			case BattaryCommandVip1026ConsumerInfo:
				{
					BattaryResponseVip1026ConsumerInfo *answer = (BattaryResponseVip1026ConsumerInfo *)response;
					if (answer->Ack)
						_hasConsumerInfo = false;
				}
				break;
			case BattaryCommandVip1026Info:
				{
					BattaryResponseVip1026Info *answer = (BattaryResponseVip1026Info *)response;
					// ���� ���� ����� ������������ �������� �������
					if (Protocol::CmdBattaryGetInfo == _currentRequest.Cmd)
					{
						BattaryInfoStruct data;
						data = answer->Info;
		
						Protocol::CmdInfo cmd(Protocol::CmdBattaryNotifyInfo);
						cmd.Data = (unsigned char*)&data;
						cmd.DataSize = sizeof(data);
		
						SendAnswer(&cmd);
					}
				}
				break;
			case BattaryCommandVip1026State:
				{
					BattaryResponseVip1026State *answer = (BattaryResponseVip1026State *)response;
					// ���� ���� ����� ������������ �������� �������
					if (Protocol::CmdBattaryGetState == _currentRequest.Cmd)
					{
						BattaryStateStruct data;
						data = answer->State;
		
						Protocol::CmdInfo cmd(Protocol::CmdBattaryNotifyState);
						cmd.Data = (unsigned char*)&data;
						cmd.DataSize = sizeof(data);
		
						SendAnswer(&cmd);
					}
					// ���� ���� ����� ������������ �������� �������
					if (Protocol::CmdBattaryGetOperatingTime == _currentRequest.Cmd)
					{
						unsigned long data;
						data = answer->UpTime;
						
						Protocol::CmdInfo cmd(Protocol::CmdBattaryNotifyOperatingTime);
						cmd.Data = (unsigned char*)&data;
						cmd.DataSize = sizeof(data);
		
						SendAnswer(&cmd);
					}
				}
				break;
			case BattaryCommandVip1026ConsumersList:
				{
					BattaryResponseVip1026ConsumersList *answer = (BattaryResponseVip1026ConsumersList *)response;
					// ���� ���� ����� ������������ �������� �������
					if (Protocol::CmdBattaryGetPowerConsumersCount == _currentRequest.Cmd)
					{
						unsigned char data;
						data = answer->RecordsCount;
		
						Protocol::CmdInfo cmd(Protocol::CmdBattaryNotifyPowerConsumersCount);
						cmd.Data = (unsigned char*)&data;
						cmd.DataSize = sizeof(data);
		
						SendAnswer(&cmd);
					}
					
					// ���� ���� ����� ������������ �������� �������
					if (Protocol::CmdBattaryGetPowerConsumersList == _currentRequest.Cmd)
					{
						// ����� ����� ������� �� ���� ������
						
						BattaryConsumersListRecordPartOneStruct dataPartOne;
						dataPartOne = answer->PartOne;
						dataPartOne.RecordNumber = _currentRequest.ParamChar;
						
						BattaryConsumersListRecordPartTwoStruct dataPartTwo;
						dataPartTwo = answer->PartTwo;
						dataPartTwo.RecordNumber = _currentRequest.ParamChar;
		
						Protocol::CmdInfo cmdOne(Protocol::CmdBattaryNotifyPowerConsumersList);
						cmdOne.Data = (unsigned char*)&dataPartOne;
						cmdOne.DataSize = sizeof(dataPartOne);
						
						Protocol::CmdInfo cmdTwo(Protocol::CmdBattaryNotifyPowerConsumersList);
						cmdTwo.Data = (unsigned char*)&dataPartTwo;
						cmdTwo.DataSize = sizeof(dataPartTwo);
		
						SendAnswer(&cmdOne);
						SendAnswer(&cmdTwo);
					}
				}
				break;
				case BattaryCommandVip1026ResetCounters:
				{
					BattaryResponseVip1026ResetCounters *answer = (BattaryResponseVip1026ResetCounters *)response;
					// ���� ���� ����� ������������ �������� �������
					if (Protocol::CmdBattaryGetResetCounters == _currentRequest.Cmd)
					{
						BattaryResetCountersStruct data;
						data = answer->ResetCounters;
		
						Protocol::CmdInfo cmd(Protocol::CmdBattaryNotifyResetCounters);
						cmd.Data = (unsigned char*)&data;
						cmd.DataSize = sizeof(data);
		
						SendAnswer(&cmd);
					}
				}
				break;
			}
		}
		_requestProcessState = RequestProcessStateIdle;
	}

protected:

	// ������� �������� ��������� �����
	typedef BattaryFactory<i2cBus> _battaryFactory;

	static BattaryBase *_battary;			// ������� ���������
	static BattaryBase *_battaryDetector;	// ��������� ������ ��� ������ ���������

	static const unsigned int DetectMaxCount = 1000 / tickPeriodMiliseconds;			// ������������ ����� �������� � ����� (����� � ������������ / ������ ����������� �����)
	static const unsigned int DetectMaxRepeateCount = 3;								// ������������ ���������� ������� �������������� ���������
	static const unsigned char MaxRequestCount = 10;									// ������������ ����� ������� ��������
	static const unsigned int SendConsumerInfoMaxCount = 1000 / tickPeriodMiliseconds;	// ������������ ����� �������� �������� ���������� � ����������� � �����

	static unsigned int _detectCounter;					// ������� �������� �������������� ���������
	static unsigned int _detectRepeateCounter;			// ������� ������� �������������� ���������
	static unsigned int _sendConsumerInfoCounter;		// ������� �������� �������� ���������� � �����������

	static State _state;								// ������� ������ ��������
	static RequestProcessState _requestProcessState;	// ������ ��������� �������� �������

	// ��� ������� ��������
	typedef queue_static<RequestInfo, MaxRequestCount> RequestQueueType;
	static RequestQueueType _requestQueue;				// ������� ��������
	static RequestQueueType _localRequestQueue;			// ������� ��������� ��������
	static RequestInfo _currentRequest;					// ������� �������������� ������

	static bool _hasConsumerInfo;						// ���� ������ ���������� �� ����������
	static BattaryConsumerInfoStruct _consumerInfo;	// ���������� �� ����������

	static State _stateBeforePause;
};

// ��������������� �������, ������������ ����
#define BATTARY_TEMPLATE_HEAD template<class i2cBus, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, class communicationModule>
#define BATTARY_CLASS Battary<i2cBus, tickEvent, tickPeriodMiliseconds, communicationModule>

// ��������������� ����������� ����������
BATTARY_TEMPLATE_HEAD BattaryBase* BATTARY_CLASS::_battary = 0;
BATTARY_TEMPLATE_HEAD BattaryBase* Battary<i2cBus, tickEvent, tickPeriodMiliseconds, communicationModule>::_battaryDetector = 0;
BATTARY_TEMPLATE_HEAD unsigned int BATTARY_CLASS::_detectCounter = 0;
BATTARY_TEMPLATE_HEAD unsigned int BATTARY_CLASS::_detectRepeateCounter = 0;
BATTARY_TEMPLATE_HEAD unsigned int BATTARY_CLASS::_sendConsumerInfoCounter = 0;
BATTARY_TEMPLATE_HEAD typename BATTARY_CLASS::RequestQueueType BATTARY_CLASS::_requestQueue;
BATTARY_TEMPLATE_HEAD typename BATTARY_CLASS::State BATTARY_CLASS::_state = BATTARY_CLASS::StateIdle;
BATTARY_TEMPLATE_HEAD typename BATTARY_CLASS::State BATTARY_CLASS::_stateBeforePause = BATTARY_CLASS::StateIdle;
BATTARY_TEMPLATE_HEAD typename BATTARY_CLASS::RequestProcessState BATTARY_CLASS::_requestProcessState = BATTARY_CLASS::RequestProcessStateIdle;
BATTARY_TEMPLATE_HEAD typename BATTARY_CLASS::RequestInfo BATTARY_CLASS::_currentRequest;
BATTARY_TEMPLATE_HEAD bool BATTARY_CLASS::_hasConsumerInfo = false;
BATTARY_TEMPLATE_HEAD BattaryConsumerInfoStruct BATTARY_CLASS::_consumerInfo;
BATTARY_TEMPLATE_HEAD typename BATTARY_CLASS::RequestQueueType BATTARY_CLASS::_localRequestQueue;

}
}

#endif

