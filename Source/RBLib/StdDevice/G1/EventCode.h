///////////////////////////////////////////////////////////////////////////////
//	������� ������� ����
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
		EventCodeActuatorOn,		// �������������� ���������� ����� ��� ��� / ������ / � �.�. �������� (�� �������)
		EventCodeActuatorOff,		// �������������� ���������� ����� ��� ��� / ������ / � �.�. �������� (�� �������)
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