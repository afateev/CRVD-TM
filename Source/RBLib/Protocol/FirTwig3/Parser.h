///////////////////////////////////////////////////////////////////////////////
//	Парсер пакетов
//	Протокол FirTwig3
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PARSER_FIRTWIG3_H
#define PARSER_FIRTWIG3_H

namespace Rblib
{
namespace Protocol
{
namespace FirTwig3
{

class Parser
{
public:
	// Выбрать пакет
	// data - указатель на пакет
	// length - размер пакета
	void SelectPacket(unsigned char *data, unsigned int length)
	{
	
	}

	// Функции обработки "выбранного" пакета

	// Получить адрес приемника
	unsigned char GetDstAddress(void)
	{
		return 0; //TODO
	}
	
	// Получить адрес источника
	unsigned char GetSrcAddress(void)
	{
		return 0; //TODO
	}

	// Получить номер команды
	unsigned int GetCommand(void)
	{
		return 0; //TODO
	}

	// Получить размер данных
	unsigned int GetDataSize(void)
	{
		return 0; //TODO
	}

	// Получить данные
	unsigned char * GetData()
	{
		return 0; //TODO
	}

	// Получить код подтверждения
	unsigned char GetAckCode(void)
	{
		return 0; //TODO
	}

	// Получить флаг запроса
	unsigned char GetRequestFlag(void)
	{
		return 0; //TODO
	}
};

}
}
}

#endif