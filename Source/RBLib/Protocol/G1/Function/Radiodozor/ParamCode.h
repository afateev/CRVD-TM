///////////////////////////////////////////////////////////////////////////////
//	Коды параметров
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_RADIODOZOR_PARAMCODE_H
#define PROTOCOL_G1_FUNCTION_RADIODOZOR_PARAMCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Radiodozor
{
	enum ParamCode
	{
		ParamCodeInvalid									= 0, // недопустимый код параметра
		ParamCodeModeSwitchTimeout							= 1, // таймаут переключения режима
	};
}
}
}
}
}

#endif