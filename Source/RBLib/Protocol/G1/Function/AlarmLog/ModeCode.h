///////////////////////////////////////////////////////////////////////////////
//	Коды режимов функции "Радиодозор"
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_RADIODOZOR_STATECODE_H
#define PROTOCOL_G1_FUNCTION_RADIODOZOR_STATECODE_H

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
	enum ModeCode
	{
		ModeCodeUnknown					= 0,
		ModeCodeDisabled				= 1,
		ModeCodeSync					= 2,
		ModeCodeAsync					= 3,
		ModeCodePur						= 4,
	};
}
}
}
}
}

#endif