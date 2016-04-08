///////////////////////////////////////////////////////////////////////////////
//	Фильтр заголовков пакетов
//	Протокол FirTwig3
//
//	Фильтр для порта обмена с ПК
//	Применяется, когда требуется обрабатывать пакеты предназначенные для ВУ
//	и пересылать пакеты предназначенные для модема
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETHEADERFILTER_PORTPC_FIRTWIG3_H
#define PACKETHEADERFILTER_PORTPC_FIRTWIG3_H

#include "Parser.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig3
{
// Класс фильтра пакетов
class PacketHeaderFilterPortPc
{
public:
	// Проверка надо ли обрабатывать и (или) пересылать пакет
	// на момент вызова гарантируется, что в парсере "выделен" пакет
	// process - выставляется в true если требуется обработать пакет, иначе false
	// resend - выставляется в true если требуется переслать пакет, иначе false
	static inline void Check(unsigned char* packetBuffer, unsigned int packetLength, bool *process, bool *resend)
	{
	}
};
}
}
}

#endif