
#ifndef DISPATCH_PARAMS_FIRTWIG2_H
#define DISPATCH_PARAMS_FIRTWIG2_H

#include "../DispatchParamsBase.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{

	/// \brief Параметры отправки команды в формате FirTwig2
	struct DispatchParams : public DispatchParamsBase
	{
		/// Сетевой адрес получателя команды.
		signed char NetAddr;
		/// Способ отправки.
		unsigned char SendMode;

		/// Константы определяющие способ отправки.
		enum SendMode
		{
			/// Поставить в очередь на отправку, послать в соответствии с маршрутной таблицей.
			SendModeStd = 0x00,
			/// Пакет ставится в буфер отправки вне очереди.
			SendModeUrgent = 0x20,
			/// Делается всего одна попытка, передачи. Независимо от результата пакет после этого удаляется.
			SendModeOneShot = 0x40,
			/// Форсированная адресация. Непосредственный адрес отправки не ищется в маршрутной таблице,
			/// вместо этого делается 3 попытки послать пакет напрямую адресату без всякой ретрансляции.
			/// Если с 3 попыток подтверждения нет, то пакет удаляется из буфера и в очередь не возвращается.
			SendModeForced = 0x80
		};

		/// Недействительный сетевой адрес.
		static const signed char NetAddrNone = -1;

		/// Конструктор по умолчанию.
		/// \details Базовый класс инициализируется типом протокола FirTwig2.
		DispatchParams()
		: NetAddr(NetAddrNone)
		, SendMode(SendModeStd)
		, DispatchParamsBase(ProtocolFirTwig2)
		{
		}

		/// Конструктор.
		/// \details Базовый класс инициализируется типом протокола FirTwig2.
		/// \param netAddr Сетевой адрес.
		DispatchParams(signed char netAddr)
			: NetAddr(netAddr)
			, SendMode(SendModeStd)
			, DispatchParamsBase(ProtocolFirTwig2)
		{
		}

		/// Конструктор.
		/// \details Базовый класс инициализируется типом протокола FirTwig2.
		/// \param netAddr Сетевой адрес.
		/// \param sendMode Способ отправки.
		DispatchParams(signed char netAddr, unsigned char sendMode)
		: NetAddr(netAddr)
		, SendMode(sendMode)
		, DispatchParamsBase(ProtocolFirTwig2)
		{
		}

	};

}
}
}

#endif // DISPATCH_PARAMS_FIRTWIG2_H
