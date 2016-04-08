///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий функции "Камера" устройтсва
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_CAMERA_EVENTCODE_H
#define PROTOCOL_G1_FUNCTION_CAMERA_EVENTCODE_H

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
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeCaptureComplete,
		EventCodePhotoCreated,
		EventCodeParamsChanged
	};
}
}
}
}
}

#endif