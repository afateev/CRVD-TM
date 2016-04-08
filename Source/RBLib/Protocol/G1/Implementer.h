///////////////////////////////////////////////////////////////////////////////
//	Типовая реализация протокола G1 как устройтсва с набором функций
//
//	Для задания набора функций устройтсва использовать комбинацию определений:
//	#define RBLIB_PROTOCOL_G1_FUNCTION_TEXTLOG_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE
//	#define RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
//  #define RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_IMPLEMENTERWIRED_H
#define PROTOCOL_G1_IMPLEMENTERWIRED_H

#ifndef SOFTWARE_VERSION_INFO_A
#define SOFTWARE_VERSION_INFO_A 0
#endif

#ifndef SOFTWARE_VERSION_INFO_B
#define SOFTWARE_VERSION_INFO_B 0
#endif

#ifndef SOFTWARE_VERSION_INFO_C
#define SOFTWARE_VERSION_INFO_C 0
#endif

#ifndef SOFTWARE_VERSION_INFO_D
#define SOFTWARE_VERSION_INFO_D 0
#endif

#include "../../Utils/CallbackWrapper.h"
#include "Device/ImplementerWired.h"
#include "Device/ImplementerWireless.h"
#include "Function/Implementer.h"
#include "../../Modem/State/State.h"
#include "../../Modem/State/LockReasoneCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
	template
		<
			class ConfigType
		>
	class Implementer
	{
	public:
		typedef ConfigType Config;
		
		typedef Device::ImplementerWired<1024> DeviceWiredImplementerType;
		typedef Device::ImplementerWireless<1024> DeviceWirelessImplementerType;
		
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_TEXTLOG_ENABLE
		typedef Function::TextLog::Implementer<255> FunctionTextLogImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
		typedef typename Config::RadioType RadioType;

		static const unsigned int RadioTimeout = RadioType::LibConfig::RadioTimeout;
		typedef Function::WirelessModem::Implementer<RadioType::EventId, 1024, typename RadioType::EventDataType, false, true, true> FunctionWirelessModemImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE
		typedef typename Config::SeismicSensorType SeismicSensorType;
		typedef Function::SeismicSensor::Implementer<SeismicSensorType::EventId, 1024,
			SeismicSensorType::MaxChannelsCount, typename SeismicSensorType::InfoTypeType, 10> FunctionSeismicSensorImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
		typedef typename Config::WirebreakSensorType WirebreakSensorType;
		typedef Function::WirebreakSensor::Implementer<WirebreakSensorType::EventId, 1024> FunctionWirebreakSensorImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE
		typedef typename Config::RelayType RelayType;
		typedef Function::Relay::Implementer<RelayType::EventId, 1024, 10> FunctionRelayImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE
		typedef typename Config::CameraType CameraType;
		typedef Function::Camera::Implementer<CameraType::EventId, 1024, 10> FunctionCameraImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
		typedef typename Config::SubscriptionServiceType SubscriptionServiceType;
		typedef Function::SubscriptionService::Implementer<SubscriptionServiceType::EventId, 1024, SubscriptionServiceType::MaxSubscriptionsCount, SubscriptionServiceType::MaxSubscribersCount> FunctionSubscriptionServiceImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE
		typedef typename Config::StorageType StorageType;
		typedef Function::Storage::Implementer<StorageType::EventId, 1024> FunctionStorageImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
		typedef typename Config::RadiodozorType RadiodozorType;
		typedef Function::Radiodozor::Implementer<RadiodozorType::EventId, 1024, 10> FunctionRadiodozorImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
		typedef typename Config::DiagnosticsType DiagnosticsType;
		typedef Function::Diagnostics::Implementer<DiagnosticsType::EventId, 1024> FunctionDiagnosticsImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
		typedef typename Config::ActionServiceType ActionServiceType;
		typedef Function::ActionService::Implementer<ActionServiceType::EventId, 1024> FunctionActionServiceImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
		typedef typename Config::AlarmLogType AlarmLogType;
		typedef Function::AlarmLog::Implementer<AlarmLogType::EventId, 600> FunctionAlarmLogImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE
		typedef typename Config::ClockType ClockType;
		typedef Function::Clock::Implementer<100> FunctionClockImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE
		typedef typename Config::PowerMonitorType PowerMonitorType;
		typedef Function::PowerMonitor::Implementer<PowerMonitorType::EventId, 128, 32, 32> FunctionPowerMonitorImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
		typedef typename Config::RadiobeaconType RadiobeaconType;

		typedef Function::Radiobeacon::Implementer<RadiobeaconType::EventId, 1024, typename RadiobeaconType::EventDataType, false> FunctionRadiobeaconImplementerType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_PELENG_ENABLE
		typedef typename Config::PelengType PelengType;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE
		typedef typename Config::LocationType LocationType;
		typedef Function::Location::Implementer<LocationType::EventId, 128> FunctionLocationImplementerType;
#endif
		
#if defined(RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE) || defined(RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE)
		typedef typename Config::NodeConfigType NodeConfigType;
#endif
	
		typedef Rblib::CallbackWrapper<unsigned char *, unsigned int> SendDataCallbackType;
		typedef Rblib::CallbackWrapper<unsigned char, unsigned char *, int> OnRadioDataReceivedCallbackType;
	public:
		DeviceWiredImplementerType DeviceWired;
		DeviceWirelessImplementerType DeviceWireless;
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_TEXTLOG_ENABLE
		FunctionTextLogImplementerType FunctionTextLog;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
		FunctionWirelessModemImplementerType FunctionWirelessModem;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE	
		FunctionSeismicSensorImplementerType FunctionSeismicSensor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
		FunctionWirebreakSensorImplementerType FunctionWirebreakSensor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE
		FunctionRelayImplementerType FunctionRelay;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE
		FunctionCameraImplementerType FunctionCamera;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
		FunctionSubscriptionServiceImplementerType FunctionSubscriptionService;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE
		FunctionStorageImplementerType FunctionStorage;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
		FunctionRadiodozorImplementerType FunctionRadiodozor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
		FunctionDiagnosticsImplementerType FunctionDiagnostics;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
		FunctionActionServiceImplementerType FunctionActionService;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
		FunctionAlarmLogImplementerType FunctionAlarmLog;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE
		FunctionClockImplementerType FunctionClock;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE
		FunctionPowerMonitorImplementerType FunctionPowerMonitor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
		FunctionRadiobeaconImplementerType FunctionRadiobeacon;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE
		FunctionLocationImplementerType FunctionLocation;
#endif

		SendDataCallbackType SendDataCallback;
		OnRadioDataReceivedCallbackType OnRadioDataReceivedCallback;
	protected:

		ConfigType _config;
		bool _queueOverflow;

#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_DYNAMIC
		RadioType *_radio;
#endif
		typename RadioType::EventDataType _eventData;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE	
		SeismicSensorType *_seismicSensor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
		WirebreakSensorType *_wirebreakSensor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE
		RelayType *_relay;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE
		CameraType *_camera;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
		SubscriptionServiceType *_subscriptionService;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
		unsigned char _onlineNodes[RadioType::LibConfig::MaxNodeCount];
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE
		StorageType *_storage;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
		RadiodozorType *_radiodozor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
		DiagnosticsType *_diagnostics;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
		ActionServiceType *_actionService;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
		AlarmLogType *_alarmLog;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE
		ClockType *_clock;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE
		PowerMonitorType *_powerMonitor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
		RadiobeaconType *_radiobeacon;
		typename RadiobeaconType::EventDataType _radiobeaconEventData;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_PELENG_ENABLE
		PelengType *_peleng;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE
		LocationType *_location;
#endif
#if defined(RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE) || defined(RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE)
		NodeConfigType *_nodeConfig;
#endif
	public:
		Implementer() : _queueOverflow(false)
		{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_DYNAMIC
			_radio = 0;
#endif
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE
			_seismicSensor = _config.SeismicSensor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
			_wirebreakSensor = _config.WirebreakSensor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE
			_relay = _config.Relay;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE	
			_camera = _config.Camera;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
			_subscriptionService = _config.SubscriptionService;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE	
			_storage = _config.Storage;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
			_radiodozor = _config.Radiodozor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
			_diagnostics = _config.Diagnostics;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
			_actionService = _config.ActionService;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
			_alarmLog = _config.AlarmLog;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE
			_clock = _config.Clock;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE
			_powerMonitor = _config.PowerMonitor;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_DYNAMIC
			_radiobeacon = 0;
#endif
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE
			_location = _config.Location;
#endif

#if defined(RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE) || defined(RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE)
		_nodeConfig = _config.NodeConfig;
#endif

			DeviceWired.SendDataCallback.Set(SendData, this);
			DeviceWired.GetDeviceInfoCallback.Set(GetDeviceInfo, this);
			DeviceWired.GetSoftwareVersionCallback.Set(GetSoftwareVersion, this);
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
			DeviceWired.ToFunctionWirelessModemCallback.Set(FunctionWirelessModemImplementerType::ToFunction, &FunctionWirelessModem);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE
			DeviceWired.ToFunctionSeismicSensorCallback.Set(FunctionSeismicSensorImplementerType::ToFunction, &FunctionSeismicSensor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
			DeviceWired.ToFunctionWirebreakSensorCallback.Set(FunctionWirebreakSensorImplementerType::ToFunction, &FunctionWirebreakSensor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE
			DeviceWired.ToFunctionRelayCallback.Set(FunctionRelayImplementerType::ToFunction, &FunctionRelay);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE
			DeviceWired.ToFunctionCameraCallback.Set(FunctionCameraImplementerType::ToFunction, &FunctionCamera);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
			DeviceWired.ToFunctionSubscriptionServiceCallback.Set(FunctionSubscriptionServiceImplementerType::ToFunction, &FunctionSubscriptionService);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE
			DeviceWired.ToFunctionStorageCallback.Set(FunctionStorageImplementerType::ToFunction, &FunctionStorage);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
			DeviceWired.ToFunctionRadiodozorCallback.Set(FunctionRadiodozorImplementerType::ToFunction, &FunctionRadiodozor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
			DeviceWired.ToFunctionDiagnosticsCallback.Set(FunctionDiagnosticsImplementerType::ToFunction, &FunctionDiagnostics);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
			DeviceWired.ToFunctionActionServiceCallback.Set(FunctionActionServiceImplementerType::ToFunction, &FunctionActionService);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
			DeviceWired.ToFunctionAlarmLogCallback.Set(FunctionAlarmLogImplementerType::ToFunction, &FunctionAlarmLog);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE
			DeviceWired.ToFunctionClockCallback.Set(FunctionClockImplementerType::ToFunction, &FunctionClock);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE
			DeviceWired.ToFunctionPowerMonitorCallback.Set(FunctionPowerMonitorImplementerType::ToFunction, &FunctionPowerMonitor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
			DeviceWired.ToFunctionRadiobeaconCallback.Set(FunctionRadiobeaconImplementerType::ToFunction, &FunctionRadiobeacon);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE
			DeviceWired.ToFunctionLocationCallback.Set(FunctionLocationImplementerType::ToFunction, &FunctionLocation);
#endif
			
			DeviceWireless.SendDataByHimselfCallback.Set(WirelessSendData, this);
			DeviceWireless.SendDataAsResponseCallback.Set(WirelessSendData, this);
			DeviceWireless.GetDeviceInfoCallback.Set(GetDeviceInfo, this);
			DeviceWireless.GetSoftwareVersionCallback.Set(GetSoftwareVersion, this);
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
			DeviceWireless.ToFunctionWirelessModemCallback.Set(FunctionWirelessModemImplementerType::ToFunction, &FunctionWirelessModem);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE			
			DeviceWireless.ToFunctionSeismicSensorCallback.Set(FunctionSeismicSensorImplementerType::ToFunction, &FunctionSeismicSensor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
			DeviceWireless.ToFunctionWirebreakSensorCallback.Set(FunctionWirebreakSensorImplementerType::ToFunction, &FunctionWirebreakSensor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE
			DeviceWireless.ToFunctionRelayCallback.Set(FunctionRelayImplementerType::ToFunction, &FunctionRelay);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE
			DeviceWireless.ToFunctionCameraCallback.Set(FunctionCameraImplementerType::ToFunction, &FunctionCamera);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
			DeviceWireless.ToFunctionSubscriptionServiceCallback.Set(FunctionSubscriptionServiceImplementerType::ToFunction, &FunctionSubscriptionService);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE
			DeviceWireless.ToFunctionStorageCallback.Set(FunctionStorageImplementerType::ToFunction, &FunctionStorage);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
			DeviceWireless.ToFunctionRadiodozorCallback.Set(FunctionRadiodozorImplementerType::ToFunction, &FunctionRadiodozor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
			DeviceWireless.ToFunctionDiagnosticsCallback.Set(FunctionDiagnosticsImplementerType::ToFunction, &FunctionDiagnostics);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
			DeviceWireless.ToFunctionActionServiceCallback.Set(FunctionActionServiceImplementerType::ToFunction, &FunctionActionService);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
			DeviceWireless.ToFunctionAlarmLogCallback.Set(FunctionAlarmLogImplementerType::ToFunction, &FunctionAlarmLog);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE
			DeviceWireless.ToFunctionClockCallback.Set(FunctionClockImplementerType::ToFunction, &FunctionClock);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE
			DeviceWireless.ToFunctionPowerMonitorCallback.Set(FunctionPowerMonitorImplementerType::ToFunction, &FunctionPowerMonitor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
			DeviceWireless.ToFunctionRadiobeaconCallback.Set(FunctionRadiobeaconImplementerType::ToFunction, &FunctionRadiobeacon);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE
			DeviceWireless.ToFunctionLocationCallback.Set(FunctionLocationImplementerType::ToFunction, &FunctionLocation);
#endif

#ifdef RBLIB_PROTOCOL_G1_FUNCTION_TEXTLOG_ENABLE			
			FunctionTextLog.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionTextLog.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionTextLog.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionTextLog.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
			FunctionWirelessModem.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionWirelessModem.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionWirelessModem.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionWirelessModem.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionWirelessModem.GetConfigParamsCallback.Set(NodeConfigType::GetConfigParams, _nodeConfig);
			FunctionWirelessModem.SetConfigParamsCallback.Set(NodeConfigType::SetConfigParams, _nodeConfig);
			
			SetupRadio();
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE				
			FunctionSeismicSensor.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionSeismicSensor.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionSeismicSensor.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionSeismicSensor.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionSeismicSensor.GetSeismicInfoOutputModeCallback.Set(SeismicSensorType::GetSeismicInfoOutputMode, _seismicSensor);
			FunctionSeismicSensor.SetSeismicInfoOutputModeCallback.Set(SeismicSensorType::SetSeismicInfoOutputMode, _seismicSensor);
			FunctionSeismicSensor.GetParamsCallback.Set(SeismicSensorType::GetSeismicParams, _seismicSensor);
			FunctionSeismicSensor.SetParamsCallback.Set(SeismicSensorType::SetSeismicParams, _seismicSensor);
			FunctionSeismicSensor.GetChangedParamsCallback.Set(SeismicSensorType::GetChangedSeismicParams, _seismicSensor);
			FunctionSeismicSensor.GetAlarmParamsCallback.Set(SeismicSensorType::GetAlarmParams, _seismicSensor);
			FunctionSeismicSensor.GetFrequentAlarmParamsCallback.Set(SeismicSensorType::GetFrequentAlarmParams, _seismicSensor);
			FunctionSeismicSensor.AutotuningStartCallback.Set(SeismicSensorType::AutotuningStart, _seismicSensor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
			FunctionWirebreakSensor.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionWirebreakSensor.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionWirebreakSensor.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionWirebreakSensor.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionWirebreakSensor.GetWireStateCallback.Set(WirebreakSensorType::GetWireState, _wirebreakSensor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE
			FunctionRelay.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionRelay.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionRelay.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionRelay.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionRelay.GetRelayStateCallback.Set(RelayType::GetRelayState, _relay);
			FunctionRelay.SetRelayStateCallback.Set(RelayType::SetRelayState, _relay);
			FunctionRelay.GetParamsCallback.Set(RelayType::GetParams, _relay);
			FunctionRelay.SetParamsCallback.Set(RelayType::SetParams, _relay);
			FunctionRelay.GetChangedParamsCallback.Set(RelayType::GetChangedParams, _relay);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE
			FunctionCamera.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionCamera.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionCamera.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionCamera.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionCamera.StartCaptureCallback.Set(CameraType::ExecuteCaptureCommand, _camera);
			FunctionCamera.GetCurrentPhotoInfoCallback.Set(CameraType::GetCurrentImageInfo, _camera);
			FunctionCamera.GetAmbientLightLevelCallback.Set(CameraType::GetAmbientLightLevel, _camera);
			FunctionCamera.GetBacklightPowerLevelCallback.Set(CameraType::GetBacklightPowerLevel, _camera);
			FunctionCamera.GetParamsCallback.Set(CameraType::GetParams, _camera);
			FunctionCamera.SetParamsCallback.Set(CameraType::SetParams, _camera);
			FunctionCamera.GetChangedParamsCallback.Set(CameraType::GetChangedParams, _camera);
			FunctionCamera.SelectPhotoInfoCallback.Set(CameraType::SelectPhotoInfo, _camera);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
			FunctionSubscriptionService.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionSubscriptionService.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionSubscriptionService.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionSubscriptionService.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionSubscriptionService.GetSubscriptionsListCallback.Set(SubscriptionServiceType::GetSubscriptionsList, _subscriptionService);
			FunctionSubscriptionService.GetSubscribersListCallback.Set(SubscriptionServiceType::GetSubscribersList, _subscriptionService);
			FunctionSubscriptionService.AddSubscriptionCallback.Set(SubscriptionServiceType::AddSubscription, _subscriptionService);
			FunctionSubscriptionService.RemoveSubscriptionCallback.Set(SubscriptionServiceType::RemoveSubscription, _subscriptionService);
			FunctionSubscriptionService.OnKeepAliveTransmitedCallback.Set(SubscriptionServiceType::OnKeepAliveTransmited, _subscriptionService);
			FunctionSubscriptionService.AddSubscribersCallback.Set(SubscriptionServiceType::AddSubscribers, _subscriptionService);
			FunctionSubscriptionService.RemoveSubscribersCallback.Set(SubscriptionServiceType::RemoveSubscribers, _subscriptionService);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE
			FunctionStorage.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionStorage.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionStorage.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionStorage.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionStorage.ReadFileCallback.Set(StorageType::ReadFile, _storage);
			FunctionStorage.PushFileCallback.Set(StorageType::PushFile, _storage);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
			FunctionRadiodozor.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionRadiodozor.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionRadiodozor.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionRadiodozor.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionRadiodozor.GetModeCallback.Set(RadiodozorType::GetMode, _radiodozor);
			FunctionRadiodozor.SetModeCallback.Set(RadiodozorType::SetMode, _radiodozor);
			FunctionRadiodozor.GetParamsCallback.Set(RadiodozorType::GetParams, _radiodozor);
			FunctionRadiodozor.SetParamsCallback.Set(RadiodozorType::SetParams, _radiodozor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
			FunctionDiagnostics.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionDiagnostics.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionDiagnostics.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionDiagnostics.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionDiagnostics.GetStateCallback.Set(DiagnosticsType::GetState, _diagnostics);
			FunctionDiagnostics.GetSavedStateCallback.Set(DiagnosticsType::GetSavedState, _diagnostics);
			FunctionDiagnostics.ResetSavedStateCallback.Set(DiagnosticsType::ResetSavedState, _diagnostics);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
			FunctionActionService.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionActionService.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionActionService.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionActionService.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionActionService.GetSlaveListCallback.Set(ActionServiceType::GetSlaveList, _actionService);
			FunctionActionService.AddSlaveCallback.Set(ActionServiceType::AddSlave, _actionService);
			FunctionActionService.RemoveSlaveCallback.Set(ActionServiceType::RemoveSlave, _actionService);
			FunctionActionService.OnActionInitiatedCallback.Set(ActionServiceType::OnActionInitiated, _actionService);
			if (_actionService)
			{
				_actionService->SendDoActionCallback.Set(FunctionActionServiceImplementerType::SendDoAction, &FunctionActionService);
			}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
			FunctionAlarmLog.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionAlarmLog.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionAlarmLog.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionAlarmLog.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionAlarmLog.GetLogInfoCallback.Set(AlarmLogType::GetLogInfo, _alarmLog);
			FunctionAlarmLog.GenerateAlarmLogCallback.Set(AlarmLogType::GenerateAlarmLog, _alarmLog);
			FunctionAlarmLog.ClearAlarmLogCallback.Set(AlarmLogType::ClearAlarmLog, _alarmLog);
			FunctionAlarmLog.PushAlarmLogCallback.Set(AlarmLogType::PushFile, _alarmLog);
			FunctionAlarmLog.ReadAlarmLogCallback.Set(AlarmLogType::ReadFile, _alarmLog);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE
			FunctionClock.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionClock.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionClock.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionClock.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionClock.GetDateTimeCallback.Set(ClockType::GetDateTime, _clock);
			FunctionClock.SetDateTimeCallback.Set(ClockType::SetDateTime, _clock);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE				
			FunctionPowerMonitor.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionPowerMonitor.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionPowerMonitor.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionPowerMonitor.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionPowerMonitor.GetStateCallback.Set(PowerMonitorType::GetState, _powerMonitor);
			FunctionPowerMonitor.GetParamsCallback.Set(PowerMonitorType::GetParams, _powerMonitor);
			FunctionPowerMonitor.SetParamsCallback.Set(PowerMonitorType::SetParams, _powerMonitor);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
			FunctionRadiobeacon.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionRadiobeacon.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionRadiobeacon.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionRadiobeacon.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionRadiobeacon.GetConfigParamsCallback.Set(NodeConfigType::GetConfigParams, _nodeConfig);
			FunctionRadiobeacon.SetConfigParamsCallback.Set(NodeConfigType::SetConfigParams, _nodeConfig);
			
			SetupRadiobeacon();
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE
			FunctionLocation.FromFunctionCallback.Add(DeviceWiredImplementerType::FromFunction, &DeviceWired);
			FunctionLocation.FromFunctionCallback.Add(DeviceWirelessImplementerType::FromFunction, &DeviceWireless);
			FunctionLocation.FromFunctionToNodeCallback.Set(DeviceWirelessImplementerType::FromFunctionToNode, &DeviceWireless);
			FunctionLocation.ToFunctionToNodeCallback.Set(DeviceWirelessImplementerType::ToFunctionToNode, &DeviceWireless);
			FunctionLocation.GetDateTimeCallback.Set(LocationType::GetDateTime, _location);
			FunctionLocation.GetLocationCallback.Set(LocationType::GetLocation, _location);
			FunctionLocation.GetSatelliteInfoCallback.Set(LocationType::GetSatelliteInfo, _location);
			FunctionLocation.GetStateCallback.Set(LocationType::GetEnabled, _location);
			FunctionLocation.SetStateCallback.Set(LocationType::SetEnabled, _location);
			
			
																				// TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif

#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
			Singletone<EventDispetcher>::Instance().AddHandler(RadioType::EventId, EventDispetcher::EventFunctor(*this), false);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE
			Singletone<EventDispetcher>::Instance().AddHandler(SeismicSensorType::EventId, EventDispetcher::EventFunctor(*this), false);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
			Singletone<EventDispetcher>::Instance().AddHandler(WirebreakSensorType::EventId, EventDispetcher::EventFunctor(*this), false);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
			Singletone<EventDispetcher>::Instance().AddHandler(DiagnosticsType::EventId, EventDispetcher::EventFunctor(*this), false);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
			Singletone<EventDispetcher>::Instance().AddHandler(ActionServiceType::EventId, EventDispetcher::EventFunctor(*this), false);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
			Singletone<EventDispetcher>::Instance().AddHandler(AlarmLogType::EventId, EventDispetcher::EventFunctor(*this), false);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE
			Singletone<EventDispetcher>::Instance().AddHandler(ClockType::EventId, EventDispetcher::EventFunctor(*this), false);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE
			Singletone<EventDispetcher>::Instance().AddHandler(PowerMonitorType::EventId, EventDispetcher::EventFunctor(*this), false);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
			Singletone<EventDispetcher>::Instance().AddHandler(RadiobeaconType::EventId, EventDispetcher::EventFunctor(*this), false);
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_PELENG_ENABLE
			Singletone<EventDispetcher>::Instance().AddHandler(PelengType::EventId, EventDispetcher::EventFunctor(*this), false);
#endif
		}
		
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
		RadioType *Radio()
		{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_DYNAMIC
			return _radio;
#else
			return _config.Radio;
#endif
		}
#endif
		
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE		
		void SetupRadio()
		{
			FunctionWirelessModem.GetEventDataCallback.Set(RadioType::GetEventData, Radio());
			FunctionWirelessModem.GetStateCallback.Set(RadioType::GetState, Radio());
			FunctionWirelessModem.GetSelfActivityTableCallback.Set(RadioType::GetSelfActivityTable, Radio());
			FunctionWirelessModem.GetRemoteNodeActivityTableCallback.Set(RadioType::GetRemoteNodeActivityTable, Radio());
			FunctionWirelessModem.GetRouteTableCallback.Set(RadioType::GetRouteTable, Radio());
			FunctionWirelessModem.GetInputDataCallback.Set(RadioType::GetInputData, Radio());
			FunctionWirelessModem.GetDebugDataCallback.Set(RadioType::GetDebugData, Radio());
			FunctionWirelessModem.DataEnqueueCallback.Set(RadioType::EnqueueOutputData, Radio());
			FunctionWirelessModem.ApplyConfigParamsCallback.Set(RadioType::ApplyConfigParams, Radio());
			FunctionWirelessModem.EnableDebugDataOutputCallback.Set(RadioType::EnableDebugDataOutput, Radio());
			FunctionWirelessModem.TestCommandCallback.Set(RadioType::TestCommand, Radio());
			FunctionWirelessModem.GetTestDataCallback.Set(RadioType::GetTestData, Radio());
			if (Radio())
			{
				Radio()->SendTestDataToNodeCallback.Set(FunctionWirelessModemImplementerType::SendTestDataToNode, &FunctionWirelessModem);
			}
		}

#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_DYNAMIC
		void DynamicSetupRadio(
								   RadioType *radio
								   )
		{
			_radio = radio;
			SetupRadio();
		}
#endif
#endif
		
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
		RadiobeaconType *Radiobeacon()
		{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_DYNAMIC
			return _radiobeacon;
#else
			return _config.Radiobeacon;
#endif
		}
#endif
		
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE		
		void SetupRadiobeacon()
		{
			FunctionRadiobeacon.GetEventDataCallback.Set(RadiobeaconType::GetEventData, Radiobeacon());
			FunctionRadiobeacon.GetStateCallback.Set(RadiobeaconType::GetState, Radiobeacon());
			FunctionRadiobeacon.GetInputDataCallback.Set(RadiobeaconType::GetInputData, Radiobeacon());
			FunctionRadiobeacon.DataEnqueueCallback.Set(RadiobeaconType::EnqueueOutputData, Radiobeacon());
			FunctionRadiobeacon.ApplyConfigParamsCallback.Set(RadiobeaconType::ApplyConfigParams, Radiobeacon());
		}

#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_DYNAMIC
		void DynamicSetupRadiobeacon(
								   RadiobeaconType *radiobeacon
								   )
		{
			_radiobeacon = radiobeacon;
			SetupRadiobeacon();
		}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_PELENG_ENABLE
		void DynamicSetupPeleng(
								   PelengType *peleng
								   )
		{
			_peleng = peleng;
		}
#endif
#endif
		
		void operator()(const Event &event)
		{
			switch (event.Code)
			{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
			case RadioType::EventId:
				{
					typename RadioType::EventCode eventCode = (typename RadioType::EventCode)event.Param1;
					switch(eventCode)
					{
					case RadioType::EventDataUpdated:
						{
							if (!Radio())
							{
								break;
							}
							
							Radio()->LockEventData();
							_eventData = Radio()->GetEventData();
							Radio()->UnlockEventData();
							
							unsigned char eventDataId = event.Param2;
					
							if (eventDataId != _eventData.EventDataId())
							{
								break;
							}
							
							for (int i = 0; i < _eventData.StateHistorySize; i++)
							{
								Rblib::Modem::State::State modemState = (Rblib::Modem::State::State)_eventData.GetStateFromHistory(i);
						
								switch(modemState)
								{
								case Rblib::Modem::State::StateDisabled:
									{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
										if (_radiodozor)
										{
											_radiodozor->OnSyncDisabled();
										}
#endif
										TextLog("WiressModem StateDisabled\r\n");
									}
									break;
								case Rblib::Modem::State::StateLocked:
									{
										TextLog("WiressModem StateLocked\r\n");
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
										if (_diagnostics)
										{
											Rblib::Modem::State::LockReasoneCode lockReasoneCode = Radio()->GetLockReasone();
											switch(lockReasoneCode)
											{
											case Rblib::Modem::State::LockReasoneTransiverError:
												{
													_diagnostics->IncrementState(Rblib::Protocol::G1::Function::Diagnostics::DiagnosticCodeModemLockOnTransiverErrorCounter, 1);
												}
												break;
											case Rblib::Modem::State::LockReasoneNetConfigError:
												{
													_diagnostics->IncrementState(Rblib::Protocol::G1::Function::Diagnostics::DiagnosticCodeModemLockOnNetConfigErrorCounter, 1);
												}
												break;
											default:
												{
													_diagnostics->IncrementState(Rblib::Protocol::G1::Function::Diagnostics::DiagnosticCodeModemLockOnCommonErrorCounter, 1);
												}
												break;
											}
										}
#endif
									}
									break;
								case Rblib::Modem::State::StateInitializing:
									{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
										if (_radiodozor)
										{
											_radiodozor->OnSyncEnabled();
										}
#endif
										TextLog("WiressModem StateInitializing\r\n");
									}
									break;
								case Rblib::Modem::State::StateSearchigSyncSources:
									{
										TextLog("WiressModem StateSearchigSyncSources\r\n");
									}
									break;
								case Rblib::Modem::State::StateSynchronizing:
									{
										TextLog("WiressModem StateSynchronizing\r\n");
									}
									break;
								case Rblib::Modem::State::StateSync:
									{
										TextLog("WiressModem StateSync\r\n");
									}
									break;
								case Rblib::Modem::State::StatePrepareLinkTable:
									{
										TextLog("WiressModem StatePrepareLinkTable\r\n");
									}
									break;
								case Rblib::Modem::State::StatePrepareRouteTable:
									{
										TextLog("WiressModem StatePrepareRouteTable\r\n");
									}
									break;
								case Rblib::Modem::State::StateReady:
									{
										TextLog("WiressModem StateReady\r\n");
										//unsigned char testData[] = {3, 10, 00, 0, 0, 1, 3};
										//Radio()->TestCommand(2, 0, testData, sizeof(testData));
									}
									break;
								case Rblib::Modem::State::StateInfiniteListening:
									{
										TextLog("WiressModem StateInfiniteListening\r\n");
									}
									break;
								case Rblib::Modem::State::StateCarrierTransmit:
									{
										TextLog("WiressModem StateCarrierTransmit\r\n");
									}
									break;
								}
							}
							
							for (int i = 0; i < _eventData.DeliveryHistorySize; i++)
							{
								unsigned char packetId = -1;
								unsigned char node = -1;
								unsigned char deliveryResultCode = -1;
								
								if (_eventData.GetDeliveryHistory(i, packetId, node, deliveryResultCode))
								{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
									if (_subscriptionService)
									{
										_subscriptionService->OnTransmitComplete(node, packetId, deliveryResultCode == RadioType::DataTxResultDelivered);
									}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
									if (_actionService)
									{
										_actionService->OnTxComplete(node, packetId, deliveryResultCode == RadioType::DataTxResultDelivered);
									}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE
									if (_storage)
									{
										_storage->OnTxComplete(node, packetId, deliveryResultCode == RadioType::DataTxResultDelivered);
									}
#endif
								}
							}
							
							if (_eventData.DataReceived)
							{
								if (Radio() && Config::ProcessEventDataReceived)
								{
									// вычитываем все накопленные данные
									while(1)
									{
										unsigned int dataCount = 0;
										unsigned char sourceAddress = 0;
										unsigned char *buffer = DeviceWireless.GetInternalBufferPtr();
										int bufferSize = DeviceWireless.GetInternalBufferSize();
										
										typename RadioType::DataQueueItemType dataQueueItem;
										if (Radio()->GetInputData(dataQueueItem, buffer, bufferSize))
										{
											dataCount = dataQueueItem.DataCount;
											sourceAddress = dataQueueItem.SourceAddress;
											
											// ограничение на вывод в последовательный порт больших данных
											if (Config::EnableBigDataOutput || (!Config::EnableBigDataOutput && dataCount <= 128))
											{
												FunctionWirelessModem.SendDataReceived(sourceAddress, buffer, dataCount);
											}
											
											DeviceWireless.OnDataReceived(sourceAddress, buffer, dataCount);
											
											// перенаправление данных извлеченных из очереди модема наружу
											OnRadioDataReceivedCallback.Call(sourceAddress, buffer, dataCount);
										}
										else
										{
											// нет больше данных
											break;
										}
									}
								}
							}
						}
						break;	
					}
				}
				break;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE
			case SeismicSensorType::EventId:
				{
					typename Function::SeismicSensor::EventCode eventCode = (typename Function::SeismicSensor::EventCode)event.Param1;
					switch(eventCode)
					{
					case Function::SeismicSensor::EventCodeAlarm:
						{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
							if (_actionService)
							{
								int initiatorId = -1;
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
								if (Radio())
								{
									initiatorId = Radio()->MyAddress();
								}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
								if (Radiobeacon())
								{
									initiatorId = Radiobeacon()->MyAddress();
								}
#endif
								// запуск по тревоге (если это не радиодозор)
#ifndef RADIODOZOR
								_actionService->ThrowDoAction(initiatorId, Function::ActionService::ReasonCodeAlarm, -1);
#endif								
							}
#endif
						}
						break;
					}
				}
				break;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
			case WirebreakSensorType::EventId:
				{
					typename Function::WirebreakSensor::EventCode eventCode = (typename Function::WirebreakSensor::EventCode)event.Param1;
					switch(eventCode)
					{
					case Function::WirebreakSensor::EventCodeWireStateChanged:
						{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
							if (_actionService)
							{
								if (_wirebreakSensor)
								{
									// действие при обрыве шлейфа
									if (_wirebreakSensor->GetWireState() == false)
									{
										int initiatorId = -1;
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
										if (Radio())
										{
											initiatorId = Radio()->MyAddress();
										}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
										if (Radiobeacon())
										{
											initiatorId = Radiobeacon()->MyAddress();
										}
#endif
										// запуск по тревоге
										_actionService->ThrowDoAction(initiatorId, Function::ActionService::ReasonCodeAlarm, -1);
									}
								}
							}
#endif
						}
						break;
					}
				}
				break;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
			case DiagnosticsType::EventId:
				{
					typename Function::Diagnostics::EventCode eventCode = (typename Function::Diagnostics::EventCode)event.Param1;
					switch(eventCode)
					{
						case Function::Diagnostics::EventCodeDeviceStateChanged:
						{
							int deviceState = event.Param2;
							
							switch(deviceState)
							{
							case -1:
								{
									TextLog("Device diagnostics state: Diagnostics have not been performed.\r\n");
								}
								break;
							case 0:
								{
									TextLog("Device diagnostics state: OK.\r\n");
								}
								break;
							default:
								{
									TextLog("Device diagnostics state: FAILURE DETECTED.\r\n");
								}
								break;
							}
						}
						break;
					}
				}
				break;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
			case ActionServiceType::EventId:
				{
					typename Function::ActionService::EventCode eventCode = (typename Function::ActionService::EventCode)event.Param1;
					switch(eventCode)
					{
					case Function::ActionService::EventCodeSlaveListChanged:
						{
							TextLog("ActionService: Slave list changed.\r\n");
						}
						break;
					}
				}
				break;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
			case AlarmLogType::EventId:
				{
					typename Function::AlarmLog::EventCode eventCode = (typename Function::AlarmLog::EventCode)event.Param1;
					switch(eventCode)
					{
					case Function::AlarmLog::EventCodeAnomaly:
						{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
							if (_actionService)
							{
								int initiatorId = -1;
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
								if (Radio())
								{
									initiatorId = Radio()->MyAddress();
								}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
								if (Radiobeacon())
								{
									initiatorId = Radiobeacon()->MyAddress();
								}
#endif
								int reasonId = -1;
								if (_alarmLog)
								{
									reasonId = _alarmLog->GetAnomalyId();
								}
								// запуск по тревоге
								_actionService->ThrowDoAction(initiatorId, Function::ActionService::ReasonCodeAnomaly, reasonId);
							}
#endif
						}
						break;
					}
				}
				break;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
			case RadiobeaconType::EventId:
				{
					typename RadioType::EventCode eventCode = (typename RadioType::EventCode)event.Param1;
					switch(eventCode)
					{
					case RadiobeaconType::EventDataUpdated:
						{
							if (!Radiobeacon())
							{
								break;
							}
							
							_radiobeaconEventData = Radiobeacon()->GetEventData();
							
							unsigned char eventDataId = event.Param2;
					
							if (eventDataId != _radiobeaconEventData.EventDataId())
							{
								break;
							}
		
							for (int i = 0; i < _radiobeaconEventData.StateHistorySize; i++)
							{
								Rblib::Modem::State::State modemState = (Rblib::Modem::State::State)_radiobeaconEventData.GetStateFromHistory(i);
						
								switch(modemState)
								{
								case Rblib::Modem::State::StateDisabled:
									{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
										if (_radiodozor)
										{
											_radiodozor->OnAsyncDisabled();
										}
#endif
										TextLog("Radiobeacon StateDisabled\r\n");
									}
									break;
								case Rblib::Modem::State::StateLocked:
									{
										TextLog("Radiobeacon StateLocked\r\n");
										// TODO При блокировке перезагружаемся
										while(1)
										{
											;
										}
									}
									break;
								case Rblib::Modem::State::StateInitializing:
									{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
										if (_radiodozor)
										{
											_radiodozor->OnAsyncEnabled();
										}
#endif
										TextLog("Radiobeacon StateInitializing\r\n");
									}
									break;
								case Rblib::Modem::State::StateReady:
									{
										TextLog("Radiobeacon StateReady\r\n");
									}
									break;
								}
							}
							
							for (int i = 0; i < _radiobeaconEventData.DeliveryHistorySize; i++)
							{
								unsigned char packetId = -1;
								unsigned char node = -1;
								unsigned char deliveryResultCode = -1;
								
								if (_radiobeaconEventData.GetDeliveryHistory(i, packetId, node, deliveryResultCode))
								{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
									if (_subscriptionService)
									{
										_subscriptionService->OnTransmitComplete(node, packetId, deliveryResultCode == RadioType::DataTxResultDelivered);
									}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
									if (_actionService)
									{
										_actionService->OnTxComplete(node, packetId, deliveryResultCode == RadioType::DataTxResultDelivered);
									}
#endif
								}
							}
							
							if (_radiobeaconEventData.DataReceived)
							{
								if (Radiobeacon() && Config::ProcessEventDataReceived)
								{
									// вычитываем все накопленные данные
									while(1)
									{
										unsigned int dataCount = 0;
										unsigned char sourceAddress = 0;
										unsigned char *buffer = DeviceWireless.GetInternalBufferPtr();
										int bufferSize = DeviceWireless.GetInternalBufferSize();
										
										typename RadiobeaconType::DataQueueItemType dataQueueItem;
										if (Radiobeacon()->GetInputData(dataQueueItem, buffer, bufferSize))
										{
											dataCount = dataQueueItem.DataCount;
											sourceAddress = dataQueueItem.SourceAddress;
											
											// ограничение на вывод в последовательный порт больших данных
											if (Config::EnableBigDataOutput || (!Config::EnableBigDataOutput && dataCount <= 128))
											{
												FunctionRadiobeacon.SendDataReceived(sourceAddress, buffer, dataCount);
											}
											
											DeviceWireless.OnDataReceived(sourceAddress, buffer, dataCount);
											
											// перенаправление данных извлеченных из очереди модема наружу
											OnRadioDataReceivedCallback.Call(sourceAddress, buffer, dataCount);
										}
										else
										{
											// нет больше данных
											break;
										}
									}
								}
							}
						}
						break;	
					}
				}
				break;
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_PELENG_ENABLE
			case PelengType::EventId:
				{
					typename PelengType::EventCode eventCode = (typename PelengType::EventCode)event.Param1;
					switch(eventCode)
					{
					case PelengType::EventPelengEnabled:
						{
							if (_radiodozor)
							{
								_radiodozor->OnPelengEnabled();
							}
						}
						break;
					case PelengType::EventPelengDisabled:
						{
							if (_radiodozor)
							{
								_radiodozor->OnPelengDisabled();
							}
						}
						break;
					}
				}
				break;
#endif
			}
		}
		
		void OnTick(int tickFrequency)
		{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
			if (_subscriptionService)
			{
				_subscriptionService->OnTick(tickFrequency);
			}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
			if (Radio())
			{
				Radio()->ServiceTick(tickFrequency);
			}
#endif
		}
		
		void OnDataReceived(unsigned char *data, int dataCount)
		{
			DeviceWired.OnDataReceived(data, dataCount);
		}
		
		static void OnDataReceived(void *callbackParam, unsigned char *data, int dataCount)
		{
			Implementer *im = (Implementer *)callbackParam;
			
			if (!im)
			{
				return;
			}
			
			im->DeviceWired.OnDataReceived(data, dataCount);
		}

		void SetQueueOverflow()
		{
			_queueOverflow = true;
		}
		
		void SetQueueOverflow(bool everflow)
		{
			_queueOverflow = everflow;
		}
				
		void ClearQueueOverflow()
		{
			_queueOverflow = false;
		}

		inline bool QueueOverflow()
		{
			return _queueOverflow;
		}

		static void SignalLevelOutputCallback(void *callbackParam, float signalLevel)
		{
			Implementer *im = (Implementer *)callbackParam;
			if (!im)
			{
				return;
			}
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE				
			im->FunctionSeismicSensor.SendSignalLevel(signalLevel);
#endif
		}
		
		static void SeismicInfoOutputCallback(void *callbackParam, unsigned char channelCount, unsigned int *firstItemXValue, unsigned int *deltaX, Function::SeismicSensor::ChannelInfoTypeCode *infoType, unsigned int *itemsCount, unsigned short ** yValues)
		{
			Implementer *im = (Implementer *)callbackParam;
			if (!im)
			{
				return;
			}
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE				
			im->FunctionSeismicSensor.SendSeismicInfo(channelCount, firstItemXValue, deltaX, infoType, itemsCount, yValues);
#endif
		}
		
		void TextLog(char * str, unsigned int strLen)
		{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_TEXTLOG_ENABLE
			FunctionTextLog.Out(str, strLen);
#endif
		}
		
		void TextLog(char *str)
		{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_TEXTLOG_ENABLE
			FunctionTextLog.Out(str);
#endif
		}
	protected:
		static void GetDeviceInfo(void *callbackParam, Device::CommandDeviceInfoParams &devInfo)
		{
			Implementer *im = (Implementer *)callbackParam;
			if (!im)
			{
				return;
			}
			
			static Device::FunctionCode functionCodes[] =
				{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_TEXTLOG_ENABLE
					Device::FunctionCodeTextLog,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
					Device::FunctionCodeWirelessModem,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE
					Device::FunctionCodeSeismicSensor,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE
					Device::FunctionCodeWirebreakSensor,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE
					Device::FunctionCodeRelay,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE
					Device::FunctionCodeCamera,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
					Device::FunctionCodeSubscriptionService,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE
					Device::FunctionCodeStorage,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE
					Device::FunctionCodeRadiodozor,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE
					Device::FunctionCodeDiagnostics,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE
					Device::FunctionCodeActionService,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE
					Device::FunctionCodeAlarmLog,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE
					Device::FunctionCodeClock,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE
					Device::FunctionCodePowerMonitor,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
					Device::FunctionCodeRadiobeacon,
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE
					Device::FunctionCodeLocation,
#endif
#if !defined(RBLIB_PROTOCOL_G1_FUNCTION_TEXTLOG_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE)
					Device::FunctionCodeContainer
#endif
				};
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
			if (im->Radio())
			{
				devInfo.SerialNumber = im->Radio()->GetSerialNumber();
			}
#else
			devInfo.SerialNumber = 0xFFFFFFFF;
#endif
#if !defined(RBLIB_PROTOCOL_G1_FUNCTION_TEXTLOG_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_SEISMICSENSOR_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_RELAY_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_CAMERA_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_STORAGE_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_RADIODOZOR_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_DIAGNOSTICS_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_ALARMLOG_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_CLOCK_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_ACTIONSERVICE_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_POWERMONITOR_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE) &\
	!defined(RBLIB_PROTOCOL_G1_FUNCTION_LOCATION_ENABLE)
			devInfo.FunctionsCount = 0;
#else
			devInfo.FunctionsCount = sizeof(functionCodes) / sizeof(Device::FunctionCode);
#endif
			devInfo.FunctionsList = functionCodes;
		}
		
		static void GetSoftwareVersion(void *callbackParam, unsigned int &a,  unsigned int &b, unsigned int &c, unsigned int &d)
		{
			a = SOFTWARE_VERSION_INFO_A;
			b = SOFTWARE_VERSION_INFO_B;
			c = SOFTWARE_VERSION_INFO_C;
			d = SOFTWARE_VERSION_INFO_D;
		}
		
		static void SendData(void *callbackParam, unsigned char *data, unsigned int dataCount)
		{
			Implementer *im = (Implementer *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->SendDataCallback.Call(data, dataCount);
		}
		
		// отправка данных по радио по инициативе устройства, данные должны отправляться подписчикам
		static void WirelessSendData(void *callbackParam, unsigned char functionCode, unsigned char messageCode, unsigned char *data, unsigned int dataCount)
		{
			Implementer *im = (Implementer *)callbackParam;
			if (!im)
			{
				return;
			}
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
			if (im->Radio())
			{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
				if (im->_subscriptionService)
				{
					typename RadioType::AddressType dstList[SubscriptionServiceType::MaxSubscribersCount];
					int dstCount = im->_subscriptionService->GetSubscribers(functionCode, messageCode, dstList, sizeof(dstList) / sizeof(RadioType::AddressType));
					if (dstCount > 0)
					{
						int packetId = 0;
						im->Radio()->EnqueueOutputData(dstList, dstCount, data, dataCount, RadioTimeout, packetId);
					}
				}
#endif
			}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
			if (im->Radiobeacon())
			{
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_SUBSCRIPTIONSERVICE_ENABLE
				if (im->_subscriptionService)
				{
					typename RadiobeaconType::AddressType dstList[SubscriptionServiceType::MaxSubscribersCount];
					int dstCount = im->_subscriptionService->GetSubscribers(functionCode, messageCode, dstList, sizeof(dstList) / sizeof(RadiobeaconType::AddressType));
					if (dstCount > 0)
					{
						int packetId = 0;
						im->_radiobeacon->EnqueueOutputData(dstList, dstCount, data, dataCount, packetId);
		}
				}
#endif
			}
#endif
		}
		
		static void WirelessSendData(void *callbackParam, unsigned char addr, unsigned char *data, unsigned int dataCount)
		{
			bool enqueued;
			int packetId;
			WirelessSendData(callbackParam, addr, data, dataCount, enqueued, packetId);
		}

		static void WirelessSendData(void *callbackParam, unsigned char addr, unsigned char *data, unsigned int dataCount, bool& enqueued, int& packetId)
		{
			Implementer *im = (Implementer *)callbackParam;
			if (!im)
			{
				return;
			}
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_WIRELESSMODEM_ENABLE
			if (im->Radio())
			{
				typename RadioType::AddressType dstList[] = {addr};
				int dstCount = 1;
				enqueued = im->Radio()->EnqueueOutputData(dstList, dstCount, data, dataCount, RadioTimeout, packetId);
				if (!enqueued)
				{
					im->SetQueueOverflow(); // установить флаг переполнения, если ен удалось поставить пакет в очередь
				}
			}
#endif
#ifdef RBLIB_PROTOCOL_G1_FUNCTION_RADIOBEACON_ENABLE
			if (im->Radiobeacon())
			{
				typename RadiobeaconType::AddressType dstList[] = {addr};
				int dstCount = 1;
				enqueued = im->Radiobeacon()->EnqueueOutputData(dstList, dstCount, data, dataCount, packetId);
				if (!enqueued)
				{
					im->SetQueueOverflow(); // установить флаг переполнения, если не удалось поставить пакет в очередь
		}
			}
#endif
		}
	};
}
}
}

#endif