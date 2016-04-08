///////////////////////////////////////////////////////////////////////////////
//	Класс простого интерфейса
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERFACESIMPLE_H
#define INTERFACESIMPLE_H

#include "..\Protocol\Protocol.h"
#include "..\Protocol\PacketExtractor.h"
#include "..\Common.h"

namespace Rblib
{
namespace Communication
{
/// \brief Реализация простого интерфейса.
/// \details Не зависит от протокола.
///	Протоколозависымые составляющие передаются в качестве параметров шаблона.
/// \tparam InterfaceId Уникальный идентификатор интерфейса.
/// \tparam HardwareInterface Физический интерфейс (например Uart).
/// \tparam InputQueueType Тип очереди входящих пакетов.
/// \tparam inputQueue Указатель на очередь входящих пакетов.
/// \tparam OutputQueueType Тип очереди исходящих пакетов.
/// \tparam outputQueue Указатель на очередь исходящих пакетов.
/// \tparam p Протокол по которому работает интерфейс.
/// \tparam PacketFilter Фильтр заголовков пакетов.
/// \tparam PacketBuilder Пакетопостроитель.
/// \tparam PacketHandler Обработчик команд.
template <
	unsigned char InterfaceId,
	class HardwareInterface,
	class InputQueueType,
	InputQueueType *inputQueue,
	class OutputQueueType,
	OutputQueueType *outputQueue,
	Protocol::ProtocolType p,
	class PacketFilter,
	class PacketBuilder,
	class PacketHandler
	>
class InterfaceSimple : public InterfaceBase
{
protected:

	/// Тип экстрактора пакетов.
	typedef Protocol::PacketExtractor<p> ExtractorType;

public:	

protected:

	/// Экземпляр экстрактора пакетов.
	ExtractorType _extractor;
	
	/// Экземпляр строителя пакетов.
	RBLIB_NO_INIT static PacketBuilder _packetBuilder;
	/// Экземпляр обработчика пакетов.
	PacketHandler _packetHandler;

	/// Интерфейс пересылки пакетов.
	InterfaceBase *_resendInterface;

	/// Возвращает указатель на экземпляр обработчика команд.
	PacketHandler* GetPacketHandler()
	{
		return &_packetHandler;
	}

	/// Функция помещения пакета в очередь (вызывается из экстрактора пакетов).
	static void PushQueue(unsigned char *data, unsigned int length)
	{
		if (0 == inputQueue)
			return;
		if (0 == data)
			return;
		if (0 == length)
			return;
		// поместить пакет в очередь указав свой идентификатор
		inputQueue->Push(InterfaceId, data, length);
	}
public:

	/// Конструткор по-умолчанию.
	InterfaceSimple()
	{
		_resendInterface = 0;
	}
	
	/// Получить уникальный идентификатор интерфейса.
	virtual unsigned char GetId(void)
	{
		return InterfaceId;
	}
	
	/// Возвращает указатель на экземпляр пакетопостроителя.
	PacketBuilder* GetPacketBuilder()
	{
		return &_packetBuilder;
	}
	
	/// Прочитать физический интерфейс, выделить пакеты и положить их в очередь.
	virtual void ReadInterface(void)
	{
		unsigned char data[64]; //локальный буфер
		
		while(1)
		{
			// прочитать физический интерфейс
			int length = HardwareInterface::Read(data, 64);
			if(length == 0)
				break;
			// вызвать экстрактор
			_extractor.ExtractPackets(data, length, PushQueue);
		}
	}
	
	/// Обработать пакет.
	virtual void ProcessPacket(PacketQueueInfo* packetInfo)
	{
		bool process = false;
		bool resend = false;
		PacketFilter::Check(packetInfo->Data, packetInfo->Length, &process, &resend);

		if (process)
		{
			// Вызвать обработчик команд
			_packetHandler.PreProcessCommand(packetInfo->Data, packetInfo->Length, this);
		}

		if (resend)
		{
			// переслать пакет
			_resendInterface->SendPacket(packetInfo->Data, packetInfo->Length);
		}
	}

	/// Обработать исходящий пакет (отправить в физический интерфейс).
	virtual void ProcessOutputPacket(PacketQueueInfo* packetInfo)
	{
		if (0 == packetInfo)
			return;
		if (0 == packetInfo->Data)
			return;
		if (0 == packetInfo->Length)
			return;
		// Пишем в физический интерфейс
		HardwareInterface::Write(packetInfo->Data, packetInfo->Length);
	}

	/// Назначить интерфейс для пересылки пакетов.
	///\param interfaceFroResend Интерфейс для пересылки.
	void SetResendInterface(InterfaceBase *interfaceFroResend)
	{
		_resendInterface = interfaceFroResend;
	}

	/// Отправить пакет.
	/// \details Cтавит готовый к отправке пакет в очередь (копирует).
	/// \param data Указатель на буфер пакета.
	/// \param length Длина пакета.
	virtual void SendPacket(unsigned char *data, unsigned int length)
	{
		if (0 == outputQueue)
			return;
		if (0 == data)
			return;
		if (0 == length)
			return;
		// Поместить пакет в очередь на отправку
		outputQueue->Push(InterfaceId, data, length);
	}

	/// Построить пакет из команды.
	/// \param [in] cmd Указатель на структуры команды.
	/// \param [out] packetSize Размер полученного пакета.
	/// \return указатель на внутренний буфер, в котором лежит сформированный пакет.
	unsigned char *BuildCommand(Protocol::CmdInfo *cmd, unsigned int *packetSize)
	{
		if (0 == cmd)
			return 0;
		// Сбросить строитель пакетов
		_packetBuilder.Reset();
		// Построить ответ на команду
		bool builded = _packetBuilder.Build(cmd);
		if (!builded)
			return 0;
		// Получаем то, что приготовил строитель пакетов
		return _packetBuilder.GetPacket(packetSize);
	}

	/// Отправить команду.
	/// \details Формирует пакет в формате протокола, определяемого пакетопостроителем. Затем пакет ставится в очередь на отправку.
	virtual void SendCommand(Protocol::CmdInfo *cmd)
	{
		// новый пакет
		unsigned char *packet = 0;
		// размер нового пакета
		unsigned int packetSize = 0;
		// Строим пакет из команду с помощью строителя пакетов.
		packet = BuildCommand(cmd, &packetSize);
		// Отправляем
		SendPacket(packet, packetSize);
	}

	virtual void SendCommandDirect(Protocol::CmdInfo *cmd)
	{
		unsigned char *packet = 0;
		// размер нового пакета
		unsigned int packetSize = 0;
		// Строим пакет из команду с помощью строителя пакетов.
		packet = BuildCommand(cmd, &packetSize);
		if (packetSize == 0)
			return;
		// Отправляем
		HardwareInterface::Write(packet, packetSize);
	}

};

// Инстанцирование статических переменных
template <unsigned char InterfaceId, class HardwareInterface, class InputQueueType, InputQueueType *inputQueue, class OutputQueueType, OutputQueueType *outputQueue,
	Protocol::ProtocolType p, class PacketFilter, class PacketBuilder, class PacketHandler>
PacketBuilder InterfaceSimple<InterfaceId, HardwareInterface, InputQueueType, inputQueue, OutputQueueType, outputQueue, p, PacketFilter, PacketBuilder, PacketHandler>::
	_packetBuilder;

}
}

#endif
