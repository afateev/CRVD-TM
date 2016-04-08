///////////////////////////////////////////////////////////////////////////////
//	Код результата выполнения команды "очистить лог"
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_CAMERA_CLEARALARMLOGRESULTCODE_H
#define PROTOCOL_G1_FUNCTION_CAMERA_CLEARALARMLOGRESULTCODE_H

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
	enum ClearAlarmLogResultCode
	{
		ClearAlarmLogResultUnknown	= 0,
		ClearAlarmLogResultError	= 1,
		ClearAlarmLogResultOk		= 2
	};
}
}
}
}
}

#endif