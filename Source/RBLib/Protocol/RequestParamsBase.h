
#ifndef REQUEST_PARAMS_BASE_H
#define REQUEST_PARAMS_BASE_H

#include "Protocol.h"

namespace Rblib
{
namespace Protocol
{

	/// \brief ѕараметры запроса, базовый класс.
	/// \details Ѕазовый класс дл€ задани€ протоколозависимых параметров запроса.
	/// Ќужно определить наследника данного класса и указать используемый протокол при инициализации.
	struct RequestParamsBase
	{
		/// ѕротокол, которому соответствуют параметры.
		const ProtocolType Protocol;

		///  онструктор.
		/// \param protocol »спользуемый протокол.
		RequestParamsBase(ProtocolType protocol) : Protocol(protocol)
		{
		}
	};

}
}

#endif // REQUEST_PARAMS_BASE_H
