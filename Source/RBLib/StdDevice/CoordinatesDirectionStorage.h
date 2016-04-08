///////////////////////////////////////////////////////////////////////////////
//	Класс хранения и выдачи географических координат направления устройства
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef COORDINATESDIRECTIONSTRORAGE_H
#define COORDINATESDIRECTIONSTRORAGE_H

namespace Rblib
{
namespace StdDevice
{

///////////////////////////////////////////////////////////////
///	\brief Структура для хранения координат направления устройства в энергонезависимой памяти
///	
///////////////////////////////////////////////////////////////
struct CoordinatesDirectionStorageDataType
{
	struct DataStruct
	{
		float Lat;			// Широта
		float Lon;			// Долгота
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
///	\brief Класс хранения и выдачи географических координат направления устройства
///	
///	\tparam Container	Тип контейнера (должен быть Rblib::Flash либо аналогичный)
///
///////////////////////////////////////////////////////////////
template<class Container>
class CoordinatesDirectionStorage
{
protected:
	// формат данных передаваемых внутри команд
	#pragma pack(push, 1)
	struct CmdDataFormat
	{
		unsigned char DeviceNumber;		// номер устройства
		float Lat;						// Широта
		float Lon;						// Долгота
	};
	#pragma pack(pop)
	
	unsigned char _myNumber;	// номер устройства в сети, которое обрабатывает данную команду

	static const unsigned char _myNumberInvalid = 0xFF;

	CmdDataFormat _cmdData;
public:
	
	// Конструктор по умолчанию
	CoordinatesDirectionStorage() : _myNumber(_myNumberInvalid)
	{
	
	}
	
	// Обновить "свой номер"
	///	Установить номер "своего" устройства
	/*!
		\param myNumber Номер устройства в сети (адрес).
	*/
	void CoordinatesDirectionStorageSetMyNumber(unsigned char myNumber)
	{
		_myNumber = myNumber;
	}
	
	///	Обработчик команд запроса и сохранения координат
	/*!
		\param cmd Команда.
		\param outputInterface Интерфейс для отправки ответа.
	*/
	void CoordinatesDirectionStorageCommandHandler(Rblib::Protocol::CmdInfo *cmd, Rblib::Communication::InterfaceBase *outputInterface)
	{
		switch(cmd->Cmd)
		{
		// Запрос координат направления
		case Rblib::Protocol::CmdGetCoordinatesDirection:
			{
  				// если не знаем свой номер, то ничего не можем сделать
				if (_myNumberInvalid == _myNumber)
					return;

				// ответ формируем на основе запроса
				Rblib::Protocol::CmdInfo answer(*cmd);
				
				// номер устройства для которого запрашиваются координаты
				unsigned char deviceNumber;
				// требуемый размер данных
				unsigned char requiredDataSize = sizeof(deviceNumber);
				// если есть данные и размер подходящий
				if (0 != cmd->Data && cmd->DataSize >= requiredDataSize)
				{
					// получаем данные
					memcpy(&deviceNumber, cmd->Data, requiredDataSize);
					// номер должен быть _myNumber, т.е. спрашивают нас
					if (_myNumber == deviceNumber)
					{
						float Lat = 0;
						float Lon = 0;
						// если координаты есть
						if (GetFromContainer(Lat, Lon))
						{
							// подготавливаем данные для отправки
							_cmdData.DeviceNumber = _myNumber;
							_cmdData.Lat = Lat;
							_cmdData.Lon = Lon;
						  // команда ответа
							answer.Cmd = Rblib::Protocol::CmdNotifyCoordinatesDirection;
							// поле данных
							answer.Data = (unsigned char *)&_cmdData;
							answer.DataSize = sizeof(CmdDataFormat);
							// отправить ответ
							outputInterface->SendCommand(&answer);
						}
						// команду обработали
						cmd->Handled = true;
					}
				}
			}
			break;
		// Установка координат направления
		case Rblib::Protocol::CmdSetCoordinatesDirection:
			{
				// если не знаем свой номер, то ничего не можем сделать
				if (_myNumberInvalid == _myNumber)
					return;
				
				// ответ формируем на основе пришедшей команды
				Rblib::Protocol::CmdInfo answer(*cmd);
				// требуемый размер данных
				unsigned char requiredDataSize = sizeof(CmdDataFormat);
				// по-умолчанию ответим отказом
				answer.Cmd = Rblib::Protocol::CmdSetCoordinatesDirectionNak;
				// если есть данные и размер подходящий
				if (0 != cmd->Data && cmd->DataSize >= requiredDataSize)
				{
					// получаем данные
					memcpy(&_cmdData, cmd->Data, requiredDataSize);
					
					// проверяем входящие данные
					// номер должен быть _myNumber, т.е. координаты наши, а не чьи-то ещё
					if (_myNumber == _cmdData.DeviceNumber)
					{
						// сохраняем новые значения
						SetToContainer(_cmdData.Lat, _cmdData.Lon);
						// ответ "успешно"
			  			answer.Cmd = Rblib::Protocol::CmdSetCoordinatesDirectionAck;
						// отправляем ответ "подтверждение"
						if (outputInterface)
							outputInterface->SendCommand(&answer);
						// помечаем команду как обработанную
						cmd->Handled = true;
						return;
					}
					// если номер не наш, то ответ не будет отправлен и команда может быть обработана другим модулем
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
protected:
	// Получить данные из контейнера
	bool GetFromContainer(float &Lat, float &Lon)
	{
		// Читаем
		CoordinatesDirectionStorageDataType data = Container::Read();
		// Проверяем целостность
		if (data.CalcCrc() != data.Crc)
			return false;
		
		// возвращаем
		Lat = data.Data.Lat;
		Lon = data.Data.Lon;
		return true;
	}

	// Поместить данные в контейнер
	void SetToContainer(float Lat, float Lon)
	{
		// Подготавливаем данные
		CoordinatesDirectionStorageDataType data;

		data.Data.Lat = Lat;
		data.Data.Lon = Lon;
		data.Crc = data.CalcCrc();

		// Записываем в контейнер
		Container::Write(data);
	}
};

/*!
\class CoordinatesStorage

Класс позволяет добавить к реализации пользовательского устройства стандартную
функциональность по сохранению и выдаче географических координат.

Для хранения координат класс использует контейнер, назначаемый через параметр шаблона <b>Container</b>.

Контейнером должен быть класс Rblib::Flash или аналогичный, хранящий данные в виде структуры CoordinatesStorageDataType, например:
\code
typedef Rblib::Flash<Rblib::StdDevice::CoordinatesStorageDataType, 0> MyCoordContainer;
\endcode

Для добавления функций работы с координатами необходимо унаследовать класс пользовательского устройства от класса CoordinatesStorage:
\code
// Класс пользовательского устройства
class DeviceClass: public Rblib::StdDevice::CoordinatesStorage<MyCoordContainer>
{
	...
};
\endcode

Далее в обработчике команд класса пользовательского устройства необходимо вызывать метод CoordinatesStorageCommandHandler():
\code
// Обработчик команд
void DeviceClass::CommandHandler(CmdInfo *cmd, InterfaceBase *outputInterface)
{
	...
	CoordinatesStorageCommandHandler(cmd, outputInterface);
	...
}
\endcode

Команды запроса и сохранения координат содержита номер устройства, для которого передаются эти координаты. CoordinatesStorageCommandHandler() проверяет этот номер и выполняет команды
только для тех команд, у которых поле номера устройства совпадает с заранее заданным номером "своего" устройства. Этот номер задается вызовом метода CoordinatesStorageSetMyNumber().
Пользователь должен вызывать его при получении номера устройства от модема:
\code
// Обработчик команд
void DeviceClass::CommandHandler(CmdInfo *cmd, InterfaceBase *outputInterface)
{
	...
	case CmdModemNotifySelfInfo:
	{
		...
		netAddress = xxx;
		...
		CoordinatesStorageSetMyNumber(netAddress);
		...
	}
	break;
	...
}
\endcode



*/

/*!
\class CoordinatesStorageDataType

Состоит из поля данных, содержащего координаты, и поля контрольной суммы.

Для проверки целостности данных, необходимо сравнить <b>Crc</b> со значением, которое вернёт CalcCrc().

Если координаты не были ни разу записаны, то очевидно, что контрольная сумма не пройдет проверку.

\sa CoordinatesStorage

*/

}
}

#endif