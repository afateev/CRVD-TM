///////////////////////////////////////////////////////////////////////////////
//	Класс драйвера батареи питания
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
/// Драйвер батареи питания
/// \details Предоставляет интерфейс для доступа к используемой устройством батареи питания.\n
/// Тип батареи определяется автоматически.
template<class i2cBus, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, class communicationModule>
class Battary
{
protected:
	// Состояния драйвера батарейки
	enum State
	{
		StateIdle = 0,				// Не пронициализировано
		StateInited,				// Проинициализировано
		StateSearch,				// Поиск батарейки
		StateDetected,				// Батарейка найдена
		StateWtiteConsumerInfo,		// Отправка информации о потребителе
		StateWaitWtiteConsumerInfo, // Ожидание отправки информации о потребителе
		StateRunning,				// Нормальная работа
		StatePaused					// Пауза, не будет обрабатывать команды
	};

	// Состояния автомата отправки дапросов
	enum RequestProcessState
	{
		RequestProcessStateIdle = 0,
		RequestProcessStateWaitAnswer
	};
	
	// Информация о входящем запросе
	struct RequestInfo
	{
		Protocol::CmdInternal Cmd;	// Команда
		unsigned char InterfaceId;	// Интерфейс, с которого пришла команда
		unsigned char Requester;	// Сетевой адрес источника запроса
		unsigned char ParamChar;	// Некий параметр запроса
		
		// Конструктор по умолчанию
		RequestInfo() : Cmd(Protocol::CmdUnknown), InterfaceId(-1), Requester(-1), ParamChar(-1)
		{
		}
		// Конструктор с параметрами
		RequestInfo(Protocol::CmdInternal cmd, unsigned char interfaceId, unsigned char requester) : Cmd(cmd), InterfaceId(interfaceId), Requester(requester), ParamChar(-1)
		{
		}
	};

public:
	/// Инициализация драйвера
	static void Init()
	{
		_battary = _battaryFactory::Create(BattaryTypeUnknown);
		_state = StateInited;
		_detectRepeateCounter = 0;
		while(!_requestQueue.empty())
			_requestQueue.pop();
		Singletone<EventDispetcher>::Instance().AddHandler(tickEvent, EventDispetcher::EventFunctor(EventHandler), false);
	}
	
	// приостановить обработку входных команд
	inline static void Pause()
	{
		_stateBeforePause = _state;
		_state = StatePaused;
	}

	// возобновить обработку входных команд
	inline static void Resume()
	{
		if (_state != StatePaused)
			return;
		_state = _stateBeforePause;
	}

	// тип батареи
	inline static unsigned char Type()
	{
		return _battary ? _battary->GetType() : BattaryTypeUnknown;
	}

	static void SetConsumerInfo(BattaryConsumerInfoStruct consumerInfo)
	{
		_consumerInfo = consumerInfo;
		_hasConsumerInfo = true;
	}

	/// Обработчик команд
	static void CommandHandler(Protocol::CmdInfo *cmd, Rblib::Communication::InterfaceBase *outputInterface)
	{
		if (0 == _battary)
			return;
		// Идет поиск или инициализация, нельзя выполнять команды
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
				// ответ формируем на основе пришедшей команды
	  			Protocol::CmdInfo answer(*cmd);
				// ответ
				answer.Cmd = Protocol::CmdBattaryNotifyType;
				// данные ответа
				unsigned char battaryType = _battary->GetType();
				
				// особый случай, когда класс универсальный и обслуживает несколько типов
				switch(battaryType)
				{
				case BattaryTypeVip1026:
					{
						BattaryVip1026<i2cBus> *battary = (BattaryVip1026<i2cBus> *)_battary;
						battaryType = battary->GetRealType();
					}
					break;
				}
				// откуда брать данные
				answer.Data = (unsigned char *)&battaryType;
				// размер данных
				answer.DataSize = sizeof(battaryType);
				// отправляем ответ
				if (outputInterface)
					outputInterface->SendCommand(&answer);
				// помечаем команду как обработанную
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
								
				// основные данные запроса
				RequestInfo request(cmd->Cmd, interfaceId, requester);
				
				// флаг корректности параметров входящей команды
				bool paramsValid = false;
				
				// у некоторых команд бывают параметры
				switch(cmd->Cmd)
				{
				case Protocol::CmdBattaryGetPowerConsumersList:
					{
						// номер записи
						unsigned char recordNumber;
						// требуемый размер данных
						unsigned char requiredDataSize = sizeof(recordNumber);
						// если есть данные и размер подходящий
						if (0 != cmd->Data && cmd->DataSize >= requiredDataSize)
						{
							// получаем данные
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

				// помечаем команду как обработанную
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
					// Сообщаем информацию о потребителе
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
					// если надо сообщить информацию о потребителе
					if (_hasConsumerInfo && _requestProcessState == RequestProcessStateIdle)
					{
						_state = StateWtiteConsumerInfo;
					}
					else
					{
						// если ничего срочного, занимаемся основной работой
						ProcessNextRequest();
					}
				}
				break;
			}
		}
	}

	// Отправить уведомление об оставшемся времени работы
	static void SendRemainingLifeTimeReport(unsigned char interfaceId, unsigned char netAddress)
	{
		RequestInfo request(Protocol::CmdBattaryGetRemainingLifeTime, interfaceId, netAddress);
		PushRequest(request);
	}

protected:
	/// Поиск батареи и определение её типа
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

	// Создать объект для поиска батарейки
	static void CreateNextDetector()
	{
		DestroyCurrentDetector();
		_battaryDetector = _battaryFactory::Create(BattaryTypeVip1026);
	}
	
	// Уничтожить текущий объект поиска батарейки
	static void DestroyCurrentDetector()
	{
		if (_battaryDetector != 0)
		{
			delete _battaryDetector;
			_battaryDetector = 0;
		}
	}

	// Отправить команду определения наличия батарейки
	static void DetectSendGetInfoCommand()
	{
		if (0 == _battaryDetector)
			return;
		
		// в зависимости от типа искомой батарейки отправляем запрос, чтобы она ответила тип
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
				// ВИП-1026 универсальный тип, там разные типы обслуживаются одним классом
				case BattaryTypeVip1026:
					{
						BattaryVip1026<i2cBus> *battaryDetector = (BattaryVip1026<i2cBus> *)_battaryDetector;
						// проверяем совместимость типа, который сообщила батарея с классом BattaryVip1026
						if (battaryDetector->TypeEqual(((BattaryResponseVip1026Info *)response)->Info.Type))
						{
							_state = StateDetected;
						}
					}
					break;
				// другие "неуниверсальные" типы батарей
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
				// Отправ один раз, не зависимо от того подтвердит батарейка прием данных или нет
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
	// очередь запросов
	// 
	// извлечь очередной запрос
	static bool PopFrontRequest()
	{
		CRITICAL_BLOCK;
		
		// если есть локальный запрос, то сначала будет обработан он
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

	// поставить запрос в очередь
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
					// если этот ответ соответсвует текущему запросу
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
					// если этот ответ соответсвует текущему запросу
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
					// если этот ответ соответсвует текущему запросу
					if (Protocol::CmdBattaryGetState == _currentRequest.Cmd)
					{
						BattaryStateStruct data;
						data = answer->State;
		
						Protocol::CmdInfo cmd(Protocol::CmdBattaryNotifyState);
						cmd.Data = (unsigned char*)&data;
						cmd.DataSize = sizeof(data);
		
						SendAnswer(&cmd);
					}
					// если этот ответ соответсвует текущему запросу
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
					// если этот ответ соответсвует текущему запросу
					if (Protocol::CmdBattaryGetPowerConsumersCount == _currentRequest.Cmd)
					{
						unsigned char data;
						data = answer->RecordsCount;
		
						Protocol::CmdInfo cmd(Protocol::CmdBattaryNotifyPowerConsumersCount);
						cmd.Data = (unsigned char*)&data;
						cmd.DataSize = sizeof(data);
		
						SendAnswer(&cmd);
					}
					
					// если этот ответ соответсвует текущему запросу
					if (Protocol::CmdBattaryGetPowerConsumersList == _currentRequest.Cmd)
					{
						// здесь ответ состоит их двух частей
						
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
					// если этот ответ соответсвует текущему запросу
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

	// фабрика батареек различных типов
	typedef BattaryFactory<i2cBus> _battaryFactory;

	static BattaryBase *_battary;			// текущая батарейка
	static BattaryBase *_battaryDetector;	// временный объект для поиска батарейки

	static const unsigned int DetectMaxCount = 1000 / tickPeriodMiliseconds;			// максимальное время ожидания в тиках (время в милисекундах / период поступления тиков)
	static const unsigned int DetectMaxRepeateCount = 3;								// максимальное количество попиток детектирования батарейки
	static const unsigned char MaxRequestCount = 10;									// максимальная длина очереди запросов
	static const unsigned int SendConsumerInfoMaxCount = 1000 / tickPeriodMiliseconds;	// максимальное время ожидания отправки информации о потребителе в тиках

	static unsigned int _detectCounter;					// счётчик таймаута детектирования батарейки
	static unsigned int _detectRepeateCounter;			// счётчик попыток детектирования батарейки
	static unsigned int _sendConsumerInfoCounter;		// счётчик таймаута отправки информации о потребителе

	static State _state;								// текущий статус драйвера
	static RequestProcessState _requestProcessState;	// статус обработки текущего запроса

	// тип очереди запросов
	typedef queue_static<RequestInfo, MaxRequestCount> RequestQueueType;
	static RequestQueueType _requestQueue;				// очередь запросов
	static RequestQueueType _localRequestQueue;			// очередь локальных запросов
	static RequestInfo _currentRequest;					// текущий обрабатываемый запрос

	static bool _hasConsumerInfo;						// была задана информация об устройстве
	static BattaryConsumerInfoStruct _consumerInfo;	// информация об устройстве

	static State _stateBeforePause;
};

// вспомогательные макросы, используются ниже
#define BATTARY_TEMPLATE_HEAD template<class i2cBus, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, class communicationModule>
#define BATTARY_CLASS Battary<i2cBus, tickEvent, tickPeriodMiliseconds, communicationModule>

// Инстанцирование статических переменных
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

