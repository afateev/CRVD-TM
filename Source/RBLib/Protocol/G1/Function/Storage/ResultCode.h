///////////////////////////////////////////////////////////////////////////////
//	Код результата выполнения команды работы с хранилищем
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_STORAGE_RESULTCODE_H
#define PROTOCOL_G1_FUNCTION_STORAGE_RESULTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Storage
{
	enum ResultCode
	{
		ResultUnknown			= 0,
		ResultOk				= 1,
		ResultInvalidRequest	= 2,
		ResultFileNotExist		= 3,
		ResultBusy				= 4,
	};
}
}
}
}
}

#endif