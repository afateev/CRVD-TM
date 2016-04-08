
#ifndef REQUEST_PARAMS_FIRTWIG2_H
#define REQUEST_PARAMS_FIRTWIG2_H

#include "../RequestParamsBase.h"
#include "Parser.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{

	/// \brief Параметры запроса в формате FirTwig2
	struct RequestParams : public RequestParamsBase
	{
		/// Праметры запроса, определённые в парсере.
		const Parser::PacketSummaryInfo& PacketInfo;

		/// Конструктор.
		/// \details Базовый класс инициализируется типом протокола FirTwig2.
		/// \param packetInfo Структура параметров запроса, полученная от парсера.
		RequestParams(const Parser::PacketSummaryInfo& packetInfo)
		: PacketInfo(packetInfo)
		, RequestParamsBase(ProtocolFirTwig2)
		{
		}

	};

}
}
}

#endif // REQUEST_PARAMS_FIRTWIG2_H
