///////////////////////////////////////////////////////////////////////////////
//	Класс стандартного функционала датчика
//	Функция "Сообщить системную информацию (время работы и количество перезагрузок)"
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSINFOREPORTER_H
#define SYSINFOREPORTER_H

#include "..\Protocol\Protocol.h"
#include "..\Protocol\InternalCommands.h"

namespace Rblib
{
namespace StdDevice
{

///////////////////////////////////////////////////////////////
///	\brief Класс обработки запросов системной информации
///	
///	\tparam tickEvent						Тактирующее событие
///	\tparam tickPeriodMiliseconds			Периодичность тактирующих событий (мс)
///
///////////////////////////////////////////////////////////////
template<unsigned int tickEvent, unsigned int tickPeriodMiliseconds>
class SysInfoReporter
{
protected:
	#pragma pack(push, 1)
	struct ReportStruct 
	{
		unsigned long WorkTime;	// время работы
		unsigned int RebootCnt;	// счётчик перезагрузок
	};
	#pragma pack (pop)

	static bool _inited;

	static const unsigned long OneSecondMaxCount = 1000 / tickPeriodMiliseconds;	// сколько тактирующих событий поступает в секунду (время в милисекундах / период поступления тиков)
	
	static unsigned long _oneSecondCounter;

	static unsigned long _upTime; // время работы РС (сек.)
public:
	/// Инициализация
	static void Init()
	{
		if (_inited)
			return;

		_oneSecondCounter = 0;
		_upTime = 0;
		Singletone<EventDispetcher>::Instance().AddHandler(tickEvent, EventDispetcher::EventFunctor(EventHandler), false);
		_inited = true;
	}
	
	///	Обработчик команд запроса и сохранения координат
	/*!
		\param cmd Команда.
		\param outputInterface Интерфейс для отправки ответа.
	*/
	static void CommandHandler(Rblib::Protocol::CmdInfo *cmd, Rblib::Communication::InterfaceBase *outputInterface)
	{
		if (!_inited)
			return;

		switch(cmd->Cmd)
		{
		// Запрос
		case Rblib::Protocol::CmdGetSystemInfo:
			{
				ReportStruct data;
				data.WorkTime = _upTime;
				data.RebootCnt = Singletone<GlobalVars>::Instance().GetWatchdogResetCount();
				
				// ответ формируем на основе запроса
				Rblib::Protocol::CmdInfo answer(*cmd);
				// команда ответа
				answer.Cmd = Rblib::Protocol::CmdNotifySystemInfo;
				// в поле данных строка версии
				answer.Data = (unsigned char *)&data;
				answer.DataSize = sizeof(data);
				// отправить ответ
				outputInterface->SendCommand(&answer);
				// команду обработали
				cmd->Handled = true;
			}
			break;
		}
	}

	///	Обработчик событий
	/*!
		\param event Событие.
	*/
	static void EventHandler(const Event &event)
	{
		if (!_inited)
			return;

		if (tickEvent == event.Code)
		{
			_oneSecondCounter++;
			if (_oneSecondCounter >= OneSecondMaxCount)
			{
				_oneSecondCounter = 0;
				_upTime++;
			}
		}
	}
};

// инстанцирование статических переменных
template<unsigned int tickEvent, unsigned int tickPeriodMiliseconds>
bool SysInfoReporter<tickEvent, tickPeriodMiliseconds>::_inited = false;

template<unsigned int tickEvent, unsigned int tickPeriodMiliseconds>
unsigned long SysInfoReporter<tickEvent, tickPeriodMiliseconds>::_oneSecondCounter = 0;

template<unsigned int tickEvent, unsigned int tickPeriodMiliseconds>
unsigned long SysInfoReporter<tickEvent, tickPeriodMiliseconds>::_upTime = 0;

}
}

#endif