///////////////////////////////////////////////////////////////////////////////
//	‘ильтр заголовков пакетов
//	ѕротокол FirTwig2
//
//	‘ильтр дл€ порта обмена с ѕ 
//	ѕримен€етс€, когда требуетс€ обрабатывать пакеты предназначенные дл€ ¬”
//	и пересылать пакеты предназначенные дл€ модема
//
//	–ешение принимаетс€ на основе полей Dest. и Source заголовка пакета
//	ѕравила работы фильтра следующие:
//	Dest		Source		–езультат
//	ADDR_PC		ADDR_PC		игнорировать
//	ADDR_PC		ADDR_EXT	игнорировать
//	ADDR_PC		ADDR_MODEM	игнорировать
//	ADDR_PC		ADDR_ALL	игнорировать
//	ADDR_EXT	ADDR_PC		обработать
//	ADDR_EXT	ADDR_EXT	обработать
//	ADDR_EXT	ADDR_MODEM	обработать
//	ADDR_EXT	ADDR_ALL	игнорировать
//	ADDR_MODEM	ADDR_PC		переслать
//	ADDR_MODEM	ADDR_EXT	переслать
//	ADDR_MODEM	ADDR_MODEM	переслать
//	ADDR_MODEM	ADDR_ALL	игнорировать
//	ADDR_ALL	ADDR_PC		обработать и переслать
//	ADDR_ALL	ADDR_EXT	обработать и переслать
//	ADDR_ALL	ADDR_MODEM	обработать и переслать
//	ADDR_ALL	ADDR_ALL	игнорировать
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETHEADERFILTER_PORTPC_FIRTWIG2_H
#define PACKETHEADERFILTER_PORTPC_FIRTWIG2_H

#include "Parser.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{
/// \brief  ласс фильтра пакетов от ѕ .
/// \details ќпредел€ет, как библиотека поступит с пакетом: обработает, перешлЄт или проигнорирует. \n
/// –ешение принимаетс€ на основе полей Dest и Source заголовка пакета.\n
/// ѕравила работы фильтра следующие:\n
/// <table>
/// <tr><td>Dest</td><td>Source</td><td>–езультат</td></tr>\n
/// <tr><td>ADDR_PC</td><td>ADDR_PC	</td><td>игнорировать</td></tr>\n
/// <tr><td>ADDR_PC</td><td>ADDR_EXT</td><td>игнорировать</td></tr>\n
/// <tr><td>ADDR_PC</td><td>ADDR_MODEM</td><td>игнорировать</td></tr>\n
/// <tr><td>ADDR_PC</td><td>ADDR_ALL</td><td>игнорировать</td></tr>\n
/// <tr><td>ADDR_EXT</td><td>ADDR_PC</td><td>обработать</td></tr>\n
/// <tr><td>ADDR_EXT</td><td>ADDR_EXT</td><td>обработать</td></tr>\n
/// <tr><td>ADDR_EXT</td><td>ADDR_MODEM</td><td>обработать</td></tr>\n
/// <tr><td>ADDR_EXT</td><td>ADDR_ALL</td><td>игнорировать</td></tr>\n
/// <tr><td>ADDR_MODEM</td><td>ADDR_PC</td><td>переслать</td></tr>\n
/// <tr><td>ADDR_MODEM</td><td>ADDR_EXT</td><td>переслать</td></tr>\n
/// <tr><td>ADDR_MODEM</td><td>ADDR_MODEM</td><td>переслать</td></tr>\n
/// <tr><td>ADDR_MODEM</td><td>ADDR_ALL</td><td>игнорировать</td></tr>\n
/// <tr><td>ADDR_ALL</td><td>ADDR_PC</td><td>обработать и переслать</td></tr>\n
/// <tr><td>ADDR_ALL</td><td>ADDR_EXT</td><td>обработать и переслать</td></tr>\n
/// <tr><td>ADDR_ALL</td><td>ADDR_MODEM</td><td>обработать и переслать</td></tr>\n
/// <tr><td>ADDR_ALL</td><td>ADDR_ALL</td><td>игнорировать</td></tr>\n
/// </table>
class PacketHeaderFilterPortPc
{
public:
	/// ѕроверка надо ли обрабатывать и (или) пересылать пакет.
	/// \param [in] packetBuffer Ѕуфер пакета.
	/// \param [in] packetLength ƒлина пакета.
	/// \param [out] process ¬ыставл€етс€ в \p true если требуетс€ обработать пакет, иначе \p false.
	/// \param [out] resend ¬ыставл€етс€ в \p true если требуетс€ переслать пакет, иначе \p false.
	static inline void Check(unsigned char* packetBuffer, unsigned int packetLength, bool *process, bool *resend)
	{
		Parser parse;
		parse.SelectPacket(packetBuffer, packetLength);

		if( (parse.GetDstAddress() == AddrAll) && (parse.GetSrcAddress() != AddrAll) )
		{
			*process = true;
			*resend = true;
			return;
		}
		if( (parse.GetDstAddress() == AddrExt) && (parse.GetSrcAddress() != AddrAll) )
		{
			*process = true;
			*resend = false;
			return;
		}
		if( (parse.GetDstAddress() == AddrModem) && (parse.GetSrcAddress() != AddrAll) )
		{
			*process = false;
			*resend = true;
			return;
		}
		*process = false;
		*resend = false;
	}
};
}
}
}

#endif