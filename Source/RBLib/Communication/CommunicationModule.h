///////////////////////////////////////////////////////////////////////////////
//	Класс обмена информацией
//	Регистрирует интерфейсы
//	Вызывает необходимые методы интерфейсов
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMUNICATIONMODULE_H
#define COMMUNICATIONMODULE_H

#include "InterfaceBase.h"
#include "..\Utils\Singletone.h"

namespace Rblib
{
namespace Communication
{
/// Класс дял работы с набором интерфесов.
/// \tparam InputQueueType Тип очереди входящих пакетов.
/// \tparam inputQueue Указатель на очередь входящих пакетов.
/// \tparam OutputQueueType Тип очереди исходящих пакетов.
/// \tparam outputQueue Указатель на очередь исходящих пакетов.
template <
	class InputQueueType,		// тип очереди входящих пакетов
	InputQueueType *inputQueue,	// указатель на очередь входящих пакетов
	class OutputQueueType,		// тип очереди исходящих пакетов
	OutputQueueType *outputQueue// указатель на очередь исходящих пакетов
	>
class CommunicationModule
{
protected:
	//максимальное количество обробатываемых интерфейсов 
	static const unsigned char _size = 5;	
	//количество зарегистрированных интерфейсов
	unsigned char _count;	
	// контейнер списка интерфейсов
	InterfaceBase* _interfaceArray[_size];
	
public:
	/// Конструктор.
	CommunicationModule()
	{
		_count = 0;
	}
	
	/// Зарегистрировать интерфейс.
	/// \param transport указатель на интерфейс.
	void RegisterInterface(InterfaceBase* transport)
	{
		//если превышен лимит на количество интерфейсов
		if((_count == _size) || (transport == 0))
			return;
		
		// проверка на уникальность идентификатора
		for(int i = 0; i < _count; i++)
		{
			if(_interfaceArray[i]->GetId() == transport->GetId())
				return;
		}
		
		// проверка на уникальность указателя
		for(int i = 0; i < _count; i++)
		{
			if(_interfaceArray[i] == transport)
				return;
		}
		
		//добавить интерфейс
		_interfaceArray[_count++] = transport;
	}

	/// Прочитать данные из всех физических интерфейсов.
	/// Пакеты попадут в очередь входящих пакетов.
	void ReadInterfaces(void)
	{
		for(unsigned char i = 0; i < _count; i++)
			_interfaceArray[i]->ReadInterface();
	}

	/// Вызвать обработчики пакетов всех интерфейсов из очереди входящих пакетов.
	void ProcessPackets(void)
	{	
		if (0 == inputQueue)
			return;
		// пока есть пакеты
		while(!inputQueue->Empty())
		{
			// взять пакет из очереди
			PacketQueueInfo info;
			inputQueue->Front(info);
		
			// передать пакет в нужный интерфейс в зависимости от идентификатора интерфейса,
			// который указан в информации о пакете взятом из очереди
			for(unsigned char i = 0; i < _count; i++)
			{
				if(_interfaceArray[i]->GetId() == info.InterfaceId)
				{
					_interfaceArray[i]->ProcessPacket(&info);
					break;
				}
			}

			// удалить обработанный пакет из очереди
			inputQueue->Pop();
		}
	}

	/// Вызвать отправку всех пакетов из очереди исходящих пакетов.
	void SendPackets(void)
	{
		if (0 == outputQueue)
			return;
		
		// пока есть пакеты
		while(!outputQueue->Empty())
		{
			// взять пакет из очереди
			PacketQueueInfo info;
			outputQueue->Front(info);
		
			// передать пакет в нужный интерфейс в зависимости от идентификатора интерфейса,
			// который указан в информации о пакете взятом из очереди
			for(unsigned char i = 0; i < _count; i++)
			{
				if(_interfaceArray[i]->GetId() == info.InterfaceId)
				{
					_interfaceArray[i]->ProcessOutputPacket(&info);
					break;
				}
			}

			// удалить обработанный пакет из очереди
			outputQueue->Pop();
		}
	}

	/// Отправить команду.
	/// \details Построить пакет и положить его в очередь исходящих пакетов.
	/// \param interfaceId Идентификатор интерфейса, через который следует отправлять команду.
	/// \param cmd Информация о команде.
	void SendCommand(unsigned char interfaceId, Protocol::CmdInfo *cmd)
	{
		if (0 == cmd)
			return;
		for(unsigned char i = 0; i < _count; i++)
		{
			if(_interfaceArray[i]->GetId() == interfaceId)
			{
				_interfaceArray[i]->SendCommand(cmd);
				break;
			}
		}
	}
};

}
}

#endif
