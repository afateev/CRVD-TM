
#ifndef DISPATCH_PARAMS_BASE_H
#define DISPATCH_PARAMS_BASE_H

#include "Protocol.h"

namespace Rblib
{
namespace Protocol
{

	/// \brief ѕараметры отправки команды, базовый класс.
	/// \details Ѕазовый класс дл€ задани€ протоколозависимых параметров отпраки команды.
	/// Ќужно определить наследника данного класса и указать используемый протокол при инициализации.
	struct DispatchParamsBase
	{
		/// ѕротокол, которому соответствуют параметры.
		const ProtocolType Protocol;

		///  онструктор.
		/// \param protocol »спользуемый протокол.
		DispatchParamsBase(ProtocolType protocol) : Protocol(protocol)
		{
		}
	};

}
}

#endif // DISPATCH_PARAMS_BASE_H
