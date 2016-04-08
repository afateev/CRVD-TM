///////////////////////////////////////////////////////////////////////////////
//	Код результата выполнения команды работы с загрузкой лога тревог
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_ALARMLOG_RESULTCODE_H
#define PROTOCOL_G1_FUNCTION_ALARMLOG_RESULTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace AlarmLog
{
	enum ResultCode
	{
		ResultUnknown			= 0,
		ResultOk				= 1,
		ResultInvalidRequest	= 2,
		ResultAlarmLogNotExist	= 3,
		ResultBusy				= 4,
	};
}
}
}
}
}

#endif