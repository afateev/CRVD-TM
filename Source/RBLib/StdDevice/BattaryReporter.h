///////////////////////////////////////////////////////////////////////////////
//	Класс отправки уведомлений батареи питания
//	Периодически сообщает оставшееся время работы
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef BATTARYREPORTER_H
#define BATTARYREPORTER_H

namespace Rblib
{
namespace StdDevice
{

///////////////////////////////////////////////////////////////
///	\brief Структура для хранения настроек класса
///	
///////////////////////////////////////////////////////////////
struct BattaryReporterStorageDataType
{
	struct DataStruct
	{
		unsigned long ReportPeriod;			// Период отсылки уведомлений об оставшемся времени работы батареи (минуты)
	};
	/// Данные
	DataStruct Data;
	/// Контрольная сумма
	unsigned char Crc;

	/// Расчёт контрольной сумму по полю данных
	unsigned char CalcCrc()
	{
		unsigned char *data = (unsigned char *)&Data;
		unsigned char crc = 0x89;

		for (unsigned char i = 0; i < sizeof(Data); i++)
			crc += data[i];

		return crc;
	}
};

///////////////////////////////////////////////////////////////
///	\brief Класс отправки уведомлений об оставшемся времени работы батареи
///	
///	\tparam Battary							Класс драйвера батареи
///	\tparam Container						Тип контейнера (должен быть Rblib::Flash либо аналогичный)
///	\tparam tickEvent						Тактирующее событие
///	\tparam BatickPeriodMilisecondsttary	Период тактирующего события (мс)
///	\tparam interfacePcId					Идентификатор интерфейса КСК
///	\tparam interfaceModemId				Идентификатор интерфейса модема
///
///////////////////////////////////////////////////////////////
template<class Battary, class Container, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, unsigned int interfacePcId, unsigned int interfaceModemId>
class BattaryReporter
{
protected:
	static bool _inited;

	static const unsigned long OneMinuteMaxCount = 60000 / tickPeriodMiliseconds;	// сколько тактирующих событий поступает в минуту (время в милисекундах / период поступления тиков)
	static const unsigned long DefaultReportPeriod = 1140;							// Период отправки уведомлений по умолчанию (1140 минут = сутки)

	static unsigned long _oneMinuteCounter;
	static unsigned long _reportCounter;
public:
	/// Инициализация
	static void Init()
	{
		if (_inited)
			return;
		
		_oneMinuteCounter = 0;
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
		// Запрос настроек
		case Rblib::Protocol::CmdBattaryGetRemainingLifeTimeReportPeriod:
			{
				unsigned long period = GetReportPeriod();
				
				// ответ формируем на основе запроса
				Rblib::Protocol::CmdInfo answer(*cmd);
				// команда ответа
				answer.Cmd = Rblib::Protocol::CmdBattaryNotifyRemainingLifeTimeReportPeriod;
				// в поле данных строка версии
				answer.Data = (unsigned char *)&period;
				answer.DataSize = sizeof(period);
				// отправить ответ
				outputInterface->SendCommand(&answer);
				// команду обработали
				cmd->Handled = true;
			}
			break;
		// Установка настроек
		case Rblib::Protocol::CmdBattarySetRemainingLifeTimeReportPeriod:
			{
				unsigned long period = 0;
				
				// ответ формируем на основе пришедшей команды
				Rblib::Protocol::CmdInfo answer(*cmd);
				// требуемый размер данных
				unsigned char requiredDataSize = sizeof(period);
				// по-умолчанию ответим отказом
				answer.Cmd = Rblib::Protocol::CmdBattarySetRemainingLifeTimeReportPeriodNak;
				// если есть данные и размер подходящий
				if (0 != cmd->Data && cmd->DataSize >= requiredDataSize)
				{
					// получаем данные
					memcpy(&period, cmd->Data, requiredDataSize);
					// сохраняем
					SetReportPeriod(period);
					// ответ "успешно"
					answer.Cmd = Rblib::Protocol::CmdBattarySetRemainingLifeTimeReportPeriodAck;
					// отправляем ответ "подтверждение"
					if (outputInterface)
						outputInterface->SendCommand(&answer);
					// помечаем команду как обработанную
					cmd->Handled = true;
					return;
				}
				else
				{
					// отправляем ответ "отказ"
					if (outputInterface)
						outputInterface->SendCommand(&answer);
					// помечаем команду как обработанную
					cmd->Handled = true;
				}
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
			_oneMinuteCounter++;
			if (_oneMinuteCounter >= OneMinuteMaxCount)
			{
				_oneMinuteCounter = 0;
				_reportCounter++;
				unsigned long reportPeriod = GetReportPeriod();
				if (_reportCounter >= reportPeriod)
				{
					_reportCounter = 0;
					if (reportPeriod > 0)
						Report();
				}
			}
		}
	}
protected:
	// Отправить уведомление
	static void Report()
	{
		// По кабелю
		Battary::SendRemainingLifeTimeReport(interfacePcId, -1);
		// По радио на адрес 0
		Battary::SendRemainingLifeTimeReport(interfaceModemId, 0);
	}

	// Получить сохраненный параметр "Период отпраки уведомлений"
	static unsigned long GetReportPeriod()
	{
		// Читаем из контейнера (флэшки)
		BattaryReporterStorageDataType data = Container::Read();
		// Проверяем целостность
		// Если во флэшке битые данные, то по умолчанию
		if (data.CalcCrc() != data.Crc)
			return DefaultReportPeriod;
		
		return data.Data.ReportPeriod;
	}

	// Сохранить параметр "Период отпраки уведомлений"
	static void SetReportPeriod(unsigned long period)
	{
		// Подготавливаем данные
		BattaryReporterStorageDataType data;

		data.Data.ReportPeriod = period;
		data.Crc = data.CalcCrc();

		// Записываем в контейнер
		Container::Write(data);
	}
};

// Инстанцирование статических переменных
template<class Battary, class Container, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, unsigned int interfacePcId, unsigned int interfaceModemId>
bool BattaryReporter<Battary, Container, tickEvent, tickPeriodMiliseconds, interfacePcId, interfaceModemId>::_inited = false;

template<class Battary, class Container, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, unsigned int interfacePcId, unsigned int interfaceModemId>
unsigned long BattaryReporter<Battary, Container, tickEvent, tickPeriodMiliseconds, interfacePcId, interfaceModemId>::_oneMinuteCounter = 0;

template<class Battary, class Container, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, unsigned int interfacePcId, unsigned int interfaceModemId>
unsigned long BattaryReporter<Battary, Container, tickEvent, tickPeriodMiliseconds, interfacePcId, interfaceModemId>::_reportCounter = 0;

}
}

#endif