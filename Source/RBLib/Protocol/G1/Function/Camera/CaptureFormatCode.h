///////////////////////////////////////////////////////////////////////////////
//	Варианты форматов съемки
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_CAMERA_CAPTUREFORMATCODE_H
#define PROTOCOL_G1_FUNCTION_CAMERA_CAPTUREFORMATCODE_H

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
	enum CaptureFormatCode
	{
		CaptureFormatUnknown		= -1,
		CaptureFormatDefault		= 0,
		CaptureFormatJpeg1280x960	= 1,
	};
}
}
}
}
}

#endif