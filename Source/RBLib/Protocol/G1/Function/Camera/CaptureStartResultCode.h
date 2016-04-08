///////////////////////////////////////////////////////////////////////////////
//	Код результата выполнения команды "начать съемку"
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_CAMERA_CAPTURESTARTRESULTCODE_H
#define PROTOCOL_G1_FUNCTION_CAMERA_CAPTURESTARTRESULTCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Camera
{
	enum CaptureStartResultCode
	{
		CaptureStartResultUnknown	= 0,
		CaptureStartResultOk		= 1,
		CaptureStartResultBusy		= 2
	};
}
}
}
}
}

#endif