///////////////////////////////////////////////////////////////////////////////
//	Подкоды событий сети
//
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_NETWORK_EVENTCODE_H
#define G1_NETWORK_EVENTCODE_H

namespace Rblib
{
namespace StdDevice
{
namespace G1
{
	enum EventCode
	{
		EventCodeUnknown = 0,
		EventCodeGlobalOffline,
		EventCodeGlobalSync,
		EventCodeGlobalOnline,
		EventCodeDeviceOnline,
		EventCodeDeviceOffline,
		EventCodeFunctionEnabled,
		EventCodeFunctionDisabled,
		EventCodeAlarm,
		EventCodeAlarmCleared,
		EventCodeActuatorOn,		// исполнительное устройство такое как БУВ / Камера / и т.п. включено (не тревога)
		EventCodeActuatorOff,		// исполнительное устройство такое как БУВ / Камера / и т.п. включено (не тревога)
		EventCodeParamsReceived,
		EventCodeCameraStartCaptureResultReceived,
		EventCodeCameraCaptureFinished,
		EventCodeCameraDownloadComplete,
		EventCodeStorageFileReadResult,
		EventCodeStoragePushFileResult,
		EventCodeStoragePushDone
	};
}
}
}

#endif