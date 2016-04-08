///////////////////////////////////////////////////////////////////////////////
//	���������� ������� ������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERNALCOMMANDS_H
#define INTERNALCOMMANDS_H

#include "Protocol.h"
#include "DispatchParamsBase.h"
#include "RequestParamsBase.h"

namespace Rblib
{
namespace Protocol
{
	// ���������� ������ ������
	enum CmdInternal
	{
		CmdUnknown,						// ����������� �������
		CmdDevicePing,					// Ping ����������
		CmdDevicePong,					// ����� �� Ping �� ����������
		CmdModemPing,					// Ping ������
		CmdModemPongNormalMode,			// ����� �� Ping �� ������ � ���������� ������
		CmdModemPongStandbyMode,		// ����� �� Ping �� ������ � ������ ��������
		CmdGetSoftwareVersion,			// ������ ������ ��
		CmdNotifySoftwareVersion,		// ����� �� CmdGetSoftwareVersion
		CmdSetBaudRate,					// ��������� �������� ������ �� �����
		CmdSetBaudRateAck,				// ����� "�������" �� CmdSetBaudRate
		CmdSetBaudRateNak,				// ����� "�����" �� CmdSetBaudRate
		CmdLogData,						// ��������������� ���������� ����������
		CmdGetDebugOutFormat,			// ������ �������� ������� ������ ��������� ������
		CmdNotifyDebugOutFormat,		// ����� �� CmdGetDebugOutFormat
		CmdSetDebugOutFormat,			// ��������� �������� ������� ������ ��������� ������
		CmdSetDebugOutFormatAck,		// ����� "�������" �� CmdSetDebugOutFormat
		CmdSetDebugOutFormatNak,		// ����� "�����" �� CmdSetDebugOutFormat
		CmdDebugOut,					// ���������� ������
		CmdAlarm,						// �������
		CmdGetAlarmState,				// ������ �������� ��������� �������
		CmdNotifyAlarmState,			// ����� �� CmdGetAlarmState
		CmdNotifyNetworkTime,			// ������� ������� ����� (������������ ��� ����������� ������������ � �.�.)
		CmdNotifyPowerOff,				// ����������� � ���������� �������
		CmdGetCoordinates,				// ������ �������������� ���������
		CmdNotifyCoordinates,			// ����� �� CmdGetCoordinates
		CmdSetCoordinates,				// ���������� �������������� ���������
		CmdSetCoordinatesAck,			// ����� "�������" �� CmdSetCoordinates
		CmdSetCoordinatesNak,			// ����� "�����" �� CmdSetCoordinates
		CmdGetCoordinatesDirection,		// ������ �������������� ��������� �����������
		CmdNotifyCoordinatesDirection,	// ����� �� CmdGetCoordinatesDirection
		CmdSetCoordinatesDirection,		// ���������� �������������� ��������� �����������
		CmdSetCoordinatesDirectionAck,	// ����� "�������" �� CmdSetCoordinatesDirection
		CmdSetCoordinatesDirectionNak,	// ����� "�����" �� CmdSetCoordinatesDirection
		
		CmdVmGetFirmwareVersion,
		CmdVmNotifyFirmwareVersion,
		CmdVmSetDeviceNumber,
		CmdVmSetDeviceNumberAck,
		CmdVmSetDeviceNumberNak,		
		CmdVmGetDeviceNumber,
		CmdVmNotifyDeviceNumber,
		CmdVmSetNetLevel,
		CmdVmSetNetLevelAck,
		CmdVmSetNetLevelNak,
		CmdVmGetNetLevel,		
		CmdVmNotifyNetLevel,
		CmdVmSetTxPower,
		CmdVmSetTxPowerAck,
		CmdVmSetTxPowerNak,		
		CmdVmGetTxPower,
		CmdVmNotifyTxPower,
		CmdVmGetDeviceStatus,		
		CmdVmNotifyDeviceStatus,
		CmdVmSaveAndRestart,
		CmdVmDiscardChanges,
		
		CmdSetLoaderState,				// ������������ ���������/���������������� ���������
		CmdGetLoaderState,				// ������ ������� ���������� (��� ��������: ��������� ��� ���������������� ���������)
		CmdGetLoaderStateNotify,        // ����� �� CmdGetLoaderState
		CmdStartLoadFirmware,           // ������ �������� ��������
		CmdStartLoadFirmwareNotify,     // ����� �mdStartLoadFirmware
		CmdFirmwareSection,				// ����� ��������
		CmdFinishLoadFirmware,          // ����� ��������
		CmdFinishLoadFirmwareNotify,    // ����� �� CmdFinishLoadFirmware
		CmdCheckLoadingStage,			// �������� ��� �� ������ �� ������ ����� ���� �������
		CmdCheckLoadingStageNotify,		// ����� �� CmdCheckLoadingStage

		CmdSetRsvSupervisorMode,
		CmdGetRsvSupervisorMode,
		CmdGetRsvSupervisorModeAck,
		CmdGetRsvSupervisorModeNak,
		CmdRsvSupervisorModeNotify,
		CmdSetRsvSupervisorModeAck,
		CmdSetRsvSupervisorModeNak,

		CmdRsuGetFunctionsList,			// ������ ������� ������������ ���
		CmdRsuNotifyFunctionsList,		// ����� �� CmdRsuGetFunctionsList
		CmdRsuSetFunctionsList,			// ��������� ������� ������������ ���
		CmdRsuSetFunctionsListAck,		// ����� "�������" �� CmdRsuSetFunctionsList
		CmdRsuSetFunctionsListNak,		// ����� "�����" �� CmdRsuSetFunctionsList
		CmdRsuGetSeismicParam,			// ������ ������ ���������� (����������� ��������, ����� ������������ �� ��������) ������������� �������
		CmdRsuNotifySeismicParam,		// ����� �� CmdRsuGetSeismicParam
		CmdRsuSetSeismicParam,			// ��������� ������ ���������� (����������� ��������, ����� ������������ �� ��������) ������������� �������
		CmdRsuSetSeismicParamAck,		// ����� "�������" �� CmdRsuSetSeismicParam
		CmdRsuSetSeismicParamNak,		// ����� "�����" �� CmdRsuSetSeismicParam
		CmdRsuGetAvarageNoise,			// ������ �������� ������ ������� ������������ �� ��������� �����
		CmdRsuNotifyAvarageNoise,		// ����� �� CmdRsuGetAvarageNoise
		CmdRsuGetDetectOptions,			// ������ ����� ��������� (���������� / ������ ��������� ������� ���������)
		CmdRsuNotifyDetectOptions,		// ����� �� CmdRsuGetDetectOptions
		CmdRsuSetDetectOptions,			// ��������� ����� ������ ���������
		CmdRsuSetDetectOptionsAck,		// ����� "�������" �� CmdRsuSetDetectOptions
		CmdRsuSetDetectOptionsNak,		// ����� "�����" �� CmdRsuSetDetectOptions
		CmdRsuGetVehicleThreshold,		// ������ ������ ������������ �� ������������� ��������
		CmdRsuNotifyVehicleThreshold,	// ����� �� CmdRsuGetVehicleThreshold
		CmdRsuSetVehicleThreshold,		// ��������� ������ ������������ �� ������������� ��������
		CmdRsuSetVehicleThresholdAck,	// ����� "�������" �� CmdRsuSetVehicleThreshold
		CmdRsuSetVehicleThresholdNak,	// ����� "�����" �� CmdRsuSetVehicleThreshold
		CmdRsuNotifySeismoResult,		// ��������� ������� ���������������
		CmdRsuGetPelengThreshold,		// ������ ������ ����������
		CmdRsuNotifyPelengThreshold,	// ����� �� CmdRsuGetPelengThreshold
		CmdRsuSetPelengThreshold,		// ��������� ������ ������������ �� ������������� ��������
		CmdRsuSetPelengThresholdAck,	// ����� "�������" �� CmdRsuSetPelengThreshold
		CmdRsuSetPelengThresholdNak,	// ����� "�����" �� CmdRsuSetPelengThreshold
		CmdRsuNotifyConfigChange,		// ����������� � ����������
		CmdRsuGetBandwidth,				// ������ ������� ������ �����������
		CmdRsuNotifyBandwidth,			// ����� �� CmdRsuGetBandwidth
		CmdRsuSetBandwidth,				// ��������� ������ �����������
		CmdRsuSetBandwidthAck,			// ����� "�������" �� CmdRsuSetBandwidth
		CmdRsuSetBandwidthNak,			// ����� "�����" �� CmdRsuSetBandwidth
		CmdRsuGetGain,					// ������ �������� ������������ ��������
		CmdRsuNotifyGain,				// ����� �� CmdRsuGetGain
		CmdRsuSetGain,					// ��������� ������������ ��������
		CmdRsuSetGainAck,				// ����� "�������" �� CmdRsuSetGain
		CmdRsuSetGainNak,				// ����� "�����" �� CmdRsuSetGain
		CmdRsuSetBanForAzimutIndefiniteType,	// ���������� ������ �������� �������� ��������������� ����.
		CmdRsuSetBanForAzimutIndefiniteTypeAck,	// ����� "�������" �� CmdRsuSetBanForAzimutIndefiniteType
		CmdRsuSetBanForAzimutIndefiniteTypeNak,	// ����� "�����"  �� CmdRsuSetBanForAzimutIndefiniteType
		CmdRsuGetBanForAzimutIndefiniteType,	// ������ ����� ������� ������ �������� ��������������� ����
		CmdRsuNotifyBanForAzimutIndefiniteType,	// ����� �� CmdRsuGetBanForAzimutIndefiniteType
		CmdRsuGetProtectiveSectors,		// ������ ������ ���������� ��������
		CmdRsuProtectiveSectorsNotify,	// ����� �� CmdRsuGetProtectiveSectors
		CmdRsuSetProtectiveSectors,		// ������ �� ��������� �������� ��� ������
		CmdRsuSetProtectiveSectorsAck,	// ����� "�������" �� CmdRsuSetProtectiveSectors
		CmdRsuSetProtectiveSectorsNak,	// ����� "�����" �� CmdRsuSetProtectiveSectors
		CmdRsuGetRecognitionTimeSector,		// ������ ������� ����������� �������
		CmdRsuRecognitionTimeSectorNotify,	// ����� �� CmdRsuGetRecognitionTimeSector
		CmdRsuSetRecognitionTimeSector,		// ������ �� ��������� ������� ����������� �������
		CmdRsuSetRecognitionTimeSectorAck,	// ����� "�������" �� CmdRsuSetRecognitionTimeSector
		CmdRsuSetRecognitionTimeSectorNak,	// ����� "�����" �� CmdRsuSetRecognitionTimeSector

		CmdRsuGetYellowAlarmParams,		// ������ ���������� ����� ������
		CmdRsuNotifyYellowAlarmParams,	// ����� �� ������ ���������� ����� ������
		CmdRsuSetYellowAlarmParams,		// ��������� ���������� ����� ������
		CmdRsuSetYellowAlarmParamsAck,	// ����� "�������" �� ��������� ���������� ����� ������
		CmdRsuSetYellowAlarmParamsNak,	// ����� "�����" �� ��������� ���������� ����� ������
		CmdRsuYellowAlarmNotify,		// Ƹ���� �������

		CmdRsuTestAmplifierNoise,		// ���������������� 1
		CmdRsuTestAmplifierNoiseAck,	// ����� "�������" �� CmdRsuTestAmplifierNoise
		CmdRsuTestAmplifierNoiseNak,	// ����� "�����" �� CmdRsuTestAmplifierNoise
		CmdRsuTestAmplifierNoiseNotify, // ��������� ����� CmdRsuTestAmplifierNoise
		CmdRsuTestRelativeDelay,		// ���������������� 2
		CmdRsuTestRelativeDelayAck,		// ����� "�������" �� CmdRsuTestRelativeDelay
		CmdRsuTestRelativeDelayNak,		// ����� "�����" �� CmdRsuTestRelativeDelay
		CmdRsuTestRelativeDelayNotify,	// ��������� ����� CmdRsuTestRelativeDelay
		CmdRsuTestAmplifierResponse,	// ���������������� 3
		CmdRsuTestAmplifierResponseAck,	// ����� "�������" �� CmdRsuTestAmplifierResponse
		CmdRsuTestAmplifierResponseNak,	// ����� "�����" �� CmdRsuTestAmplifierResponse
		CmdRsuTestAmplifierResponseNotify, // ��������� ����� CmdRsuTestAmplifierResponse
		CmdRsuSelfTest,					// ������ �� ����������������
		CmdRsuSelfTestAck,				// ����� "�������" �� CmdRsuSelfTest
		CmdRsuSelfTestNak,				// ����� "�����" �� CmdRsuSelfTest
		CmdRsuSelfTestNotify,			// ���������� CmdRsuSelfTest

		CmdRsuSelfTestSetPeriod,		// ���������� ������ ������� ��������������� ����������������
		CmdRsuSelfTestSetPeriodAck,
		CmdRsuSelfTestSetPeriodNak,
		CmdRsuSelfTestGetPeriod,		// ��������� ������ ������� ��������������� ����������������
		CmdRsuSelfTestGetPeriodAck,
		CmdRsuSelfTestGetPeriodNak,
		CmdRsuSelfTestPeriodNotify,

		CmdRsuGetSelfTestReport,		// ������ ��������� ����������� ����������������
		CmdRsuNotifySelfTestReport,		// ����� �� CmdRsuGetSelfTestReport
		CmdRsuSetTestReportEnable,		// ���������� ������ ����������� ����������������
		CmdRsuSetTestReportEnableAck,	// ����� "�������" �� CmdRsuSetTestReportEnable
		CmdRsuSetTestReportEnableNak,	// ����� "�����" �� CmdRsuSetTestReportEnable
		CmdRsuSetTestReportDisable,		// ���������� ������ ����������� ����������������
		CmdRsuSetTestReportDisableAck,	// ����� "�������" �� CmdRsuSetTestReportDisable
		CmdRsuSetTestReportDisableNak,	// ����� "�����" �� CmdRsuSetTestReportDisable
		CmdRsuNotifyWirelessState,		// ����������� � ��������� ������������ ���������

		CmdRsuSetShuntShorted,			// ���������� ������������� ����
		CmdRsuSetShuntShortedAck,		// ����� "�������" �� CmdRsuSetShuntShorted
		CmdRsuSetShuntShortedNak,		// ����� "�����" �� CmdRsuSetShuntShorted
		CmdRsuSetShuntOpen,				// ������� ������������� ����
		CmdRsuSetShuntOpenAck,			// ����� "�������" ��
		CmdRsuSetShuntOpenNak,			// ����� "�����" �� 

		CmdRsuTestCurrent,					// ������ ���� �����������
		CmdRsuTestCurrentAck,				// ����� "�������" �� CmdRsuTestCurrent
		CmdRsuTestCurrentNak,				// ����� "�����" �� CmdRsuTestCurrent
		CmdRsuTestCurrentNotify,			// ���������� CmdRsuTestCurrent
		CmdRsuTestExtAmplifierPowerOn,		// �������� ������� ������� ��������
		CmdRsuTestExtAmplifierPowerOnAck,	// ����� "�������" �� CmdRsuTestExtAmplifierPowerOn
		CmdRsuTestExtAmplifierPowerOnNak,	// ����� "�����" �� CmdRsuTestExtAmplifierPowerOn
		CmdRsuTestExtAmplifierPowerOff,		// ��������� ������� ������� ��������
		CmdRsuTestExtAmplifierPowerOffAck,	// ����� "�������" �� CmdRsuTestExtAmplifierPowerOff
		CmdRsuTestExtAmplifierPowerOffNak,	// ����� "�����" �� CmdRsuTestExtAmplifierPowerOff
		CmdRsuTestI2C0PullUpEnable,			// �������� ������������� ��������� I2C 0
		CmdRsuTestI2C0PullUpEnableAck,		// ����� "�������" �� CmdRsuTestI2C0PullUpEnable
		CmdRsuTestI2C0PullUpEnableNak,		// ����� "�����" �� CmdRsuTestI2C0PullUpEnable
		CmdRsuTestI2C0PullUpDisable,		// �������� ������������� ��������� I2C 0
		CmdRsuTestI2C0PullUpDisableAck,		// ����� "�������" �� CmdRsuTestI2C0PullUpDisable
		CmdRsuTestI2C0PullUpDisableNak,		// ����� "�����" �� CmdRsuTestI2C0PullUpDisable
		CmdRsuTestI2C1PullUpEnable,			// �������� ������������� ��������� I2C 1
		CmdRsuTestI2C1PullUpEnableAck,		// ����� "�������" �� CmdRsuTestI2C1PullUpEnable
		CmdRsuTestI2C1PullUpEnableNak,		// ����� "�����" �� CmdRsuTestI2C1PullUpEnable
		CmdRsuTestI2C1PullUpDisable,		// �������� ������������� ��������� I2C 1
		CmdRsuTestI2C1PullUpDisableAck,		// ����� "�������" �� CmdRsuTestI2C1PullUpDisable
		CmdRsuTestI2C1PullUpDisableNak,		// ����� "�����" �� CmdRsuTestI2C1PullUpDisable
		CmdRsuTestVibroOn,					// �������� ����������
		CmdRsuTestVibroOnAck,				// ����� "�������" �� CmdRsuTestVibroOn
		CmdRsuTestVibroOnNak,				// ����� "�����" �� CmdRsuTestVibroOn
		CmdRsuTestVibroOff,					// ��������� ����������
		CmdRsuTestVibroOffAck,				// ����� "�������" �� CmdRsuTestVibroOff
		CmdRsuTestVibroOffNak,				// ����� "�����" �� CmdRsuTestVibroOff
		CmdRsuTestLBEnable,					// ��������� ������ ��������� �������
		CmdRsuTestLBEnableAck,				// ����� "�������" �� CmdRsuTestLBEnable
		CmdRsuTestLBEnableNak,				// ����� "�����" �� CmdRsuTestLBEnable
		CmdRsuTestLBDisable,				// ��������� ������ ��������� �������
		CmdRsuTestLBDisableAck,				// ����� "�������" �� CmdRsuTestLBDisable
		CmdRsuTestLBDisableNak,				// ����� "�����" �� CmdRsuTestLBDisable
		CmdRsuSelfTestChangeTypeEnable,		// ��������� ����� ���� ��� ��������� ���������� ����������������
		CmdRsuSelfTestChangeTypeEnableAck,	// ����� "�������" �� CmdRsuSelfTestChangeTypeEnable
		CmdRsuSelfTestChangeTypeEnableNak,	// ����� "�����" �� CmdRsuSelfTestChangeTypeEnable
		CmdRsuSelfTestChangeTypeDisable,	// ��������� ����� ���� ��� ��������� ���������� ����������������
		CmdRsuSelfTestChangeTypeDisableAck,	// ����� "�������" �� CmdRsuSelfTestChangeTypeDisable
		CmdRsuSelfTestChangeTypeDisableNak,	// ����� "�����" �� CmdRsuSelfTestChangeTypeDisable
		CmdRsuSelfTestGetChangeType,		// ������ �������� ��������� ���������� ����� ���� ��� ��������� ���������� ����������������
		CmdRsuNotifySelfTestChangeType,		// ����� �� CmdRsuGetSelfTestChangeType

		CmdRsuSetRelayOn,				// ��������� ���
		CmdRsuSetRelayOnNak,			// ����� "�����" �� CmdRsuSetRelayOn
		CmdRsuSetRelayOff,				// ���������� ���
		CmdRsuSetRelayOffNak,			// ����� "�����" �� CmdRsuSetRelayOff
		CmdRsuGetRelayTime,				// ������ ������� ������ ���
		CmdRsuNotifyRelayTime,			// ����� �� CmdRsuGetRelayTime
		CmdRsuSetRelayTime,				// ��������� ������� ������ ���
		CmdRsuSetRelayTimeAck,			// ����� "�������" �� CmdRsuSetRelayTime
		CmdRsuSetRelayTimeNak,			// ����� "�����" �� CmdRsuSetRelayTime
		CmdRsuGetRelayEnableByOwnSensor,	// ������ ���������� �������� ��� �� ������������ �������
		CmdRsuNotifyRelayEnableByOwnSensor,	// ����� �� CmdRsuGetRelayEnableByOwnSensor
		CmdRsuSetRelayEnableByOwnSensor,	// ��������� ���������� �������� ��� �� ������������ �������
		CmdRsuSetRelayEnableByOwnSensorAck,	// ����� "�������" �� CmdRsuSetRelayEnableByOwnSensor
		CmdRsuSetRelayEnableByOwnSensorNak,	// ����� "�����" �� CmdRsuSetRelayEnableByOwnSensor
		CmdRsuGetRelayModuleSwitchOnPreventFlag,	// ������ ����� ���������� ��������� ������ ���
		CmdRsuNotifyRelayModuleSwitchOnPreventFlag, // ����� �� CmdRsuGetRelayModuleSwitchOnPreventFlag
		CmdRsuSetRelayModuleSwitchOnPreventFlag,	// ��������� ����� ���������� ��������� ������ ���
		CmdRsuSetRelayModuleSwitchOnPreventFlagAck,	// ����� "�������" �� CmdRsuSetRelayModuleSwitchOnPreventFlag
		CmdRsuSetRelayModuleSwitchOnPreventFlagNak,	// ����� "�����" �� CmdRsuSetRelayModuleSwitchOnPreventFlag
    	CmdRsuGetAlarmFlags, // ������ ������ ��������� ������������� ���������
    	CmdRsuAlarmFlagsNotify, // ����� �� CmdRsuGetAlarmFlags
		
		CmdRsuStartCalibration, // ������ ��������� ���������� (�������������� ��������� �������)
		CmdRsuStartCalibrationAck, // ����� �� ������� CmdRsuStartCalibration - �������
		CmdRsuStartCalibrationNak, // ����� �� ������� CmdRsuStartCalibration - ������
		CmdRsuCalibrationNotify, // ��������� ����������

		CmdModemInputData,				// ������� ������
		CmdModemSendData,				// ��������� ������ �� ����� �� ���������
		CmdModemAskType,				// ������ ���� �������� ����������
		CmdModemSetType,				// ���������� ��� �������� ���������� � ������
		CmdModemGetTempAndPower,		// ������ ����������� � ���������� ������� � ������
		CmdModemNotifyTempAndPower,		// ����� �� CmdModemGetTempAndPower
		CmdModemGetSelfInfo,			// ������ ��������� "������" ������
		CmdModemNotifySelfInfo,			// ����� �� CmdModemGetMyInfo, ��� �������� ���������� � ������ �� ������� �������
		CmdModemGetNetTime,				// ������ �������� ������� � ������
		CmdModemNotifyNetTime,			// ����� �� CmdModemGetNetTime
		CmdModemNotifyExtDevSoftwareVersion,	// �������� ������ ������ �� �������� ����������
		CmdModemNotifyRoute,
		CmdModemNotifyRoute2,
		CmdModemGetNetInfo,
		CmdModemNotifyNetInfo,
		CmdModemGetRoute,
		CmdModemSetMode,
		CmdModemNotifyLinx,
		CmdModemGetRemoteModemInfo,			// ������ ��������� �������� ������
		CmdModemoteRemoteModemInfoNotify,	// ��������� �������� ������
		CmdModemGetRemoteModemVersion,
		CmdModemGetRemoteDeviceVersion,
		CmdModemRemoteModemVersionNotify,
		CmdModemRemoteDeviceVersionNotify,
		CmdModemSetStandbyMode,				// ������� ������ � ����� ��������
		CmdModemSetNormalMode,				// ������� ������ � ���������� ����� ������
		CmdModemGetVersion,					// ������ ����� �� ������
		CmdModemVersionNotify,				// ����� �� CmdModemGetVersion
		CmdModemTestStop,					// �������� ������������
		CmdModemTestMeasureTempAndVoltage,	// ��������� ���������� � ���������� �������

		CmdSetSensorOnOff,				// ��������� / ���������� �������
		CmdRsIrSetLedOnOff,				// ��������� / ���������� ������ ��������� ������� � ������� ����������.
		CmdRsIrSetLedOnOffAck,			// ����� "�������" �� CmdRsIrSetLedOnOff
		CmdRsIrSetLedOnOffNak,			// ����� "�����" �� CmdRsIrSetLedOnOff
		CmdRsIrSetCustomThresh, // ��������� ���������������� �������� ��� ������
		CmdRsIrSetCustomThreshAck, // ����� �� CmdRsIrSetCustomThresh (�����)
		CmdRsIrSetCustomThreshNak, // ����� �� CmdRsIrSetCustomThresh (������)
		CmdRsIrSetSensivity,			// ��������� ������ ����������������
		CmdRsIrSetSensivityAck,			// ����� "�������" �� CmdRsIrSetSensivity
		CmdRsIrSetSensivityNak,			// ����� "�����" �� CmdRsIrSetSensivity
		CmdRsIrGetParams,				// ������ ���������� "����� ��������� ������� � ������� ����������" � "������� ����������������"
		CmdRsIrNotifyParams,			// ����� �� CmdRsIrGetParams
		CmdRsIrGetAlgorithmParams,		// ������ ���������� ��������� ��������� �������
		CmdRsIrNotifyAlgorithmParams,	// ����� �� CmdRsIrGetAlgorithmParams
		CmdRsIrSetAlgorithmParams,		// ��������� ���������� ��������� ��������� �������
		CmdRsIrSetAlgorithmParamsAck,	// ����� "�������" �� CmdRsIrSetAlgorithmParams
		CmdRsIrSetAlgorithmParamsNak,	// ����� "�����" �� CmdRsIrSetAlgorithmParams
		CmdGetSystemInfo,				// ������ ���������� � ��������� �������
		CmdNotifySystemInfo,			// ����� �� CmdGetSystemInfo
    CmdGetDeviceType, // ������ ���� ��� ���� ��������, ����� ��� (GET_SENS_CONFIG)
    CmdDeviceTypeNotify, // ����� �� CmdGetDeviceType
		CmdBattaryGetType,				// ������ ���� ������������ �������
		CmdBattaryNotifyType,			// ����� �� CmdBattaryGetType
		CmdBattaryGetRemainingLifeTime,		// ������ ����������� ������� ������ �� �������
		CmdBattaryNotifyRemainingLifeTime,	// ����� �� CmdBattaryGetRemainingLifeTime
		CmdBattaryGetInfo,				// ������ ����� ���������� � �������
		CmdBattaryNotifyInfo,			// ����� �� CmdBattaryGetInfo
		CmdBattaryGetState,				// ������ ��������� �������
		CmdBattaryNotifyState,			// ����� �� CmdBattaryGetState
		CmdBattaryGetOperatingTime,		// ������ ������� ���������
		CmdBattaryNotifyOperatingTime,	// ����� �� CmdBattaryGetOperatingTime
		CmdBattaryGetPowerConsumersCount,		// ������ ������� ������ ������������
		CmdBattaryNotifyPowerConsumersCount,	// ����� �� CmdBattaryGetPowerConsumersCount
		CmdBattaryGetPowerConsumersList,		// ������ ������ ������������
		CmdBattaryNotifyPowerConsumersList,		// ����� �� CmdBattaryGetPowerConsumersList
		CmdBattaryGetResetCounters,				// ������ ��������� ������������ �� �������
		CmdBattaryNotifyResetCounters,			// ����� �� CmdBattaryGetResetCounters
		CmdBattaryGetRemainingLifeTimeReportPeriod,		// ������ ������� �������� ���������� �� ���������� ������� ������ �� �������
		CmdBattaryNotifyRemainingLifeTimeReportPeriod,	// ����� �� CmdBattaryGetRemainingLifeTimeReportPeriod
		CmdBattarySetRemainingLifeTimeReportPeriod,		// ��������� ������� �������� ���������� �� ���������� ������� ������ �� �������
		CmdBattarySetRemainingLifeTimeReportPeriodAck,	// ����� "�������" �� CmdBattarySetRemainingLifeTimeReportPeriod
		CmdBattarySetRemainingLifeTimeReportPeriodNak,	// ����� "�����" �� CmdBattarySetRemainingLifeTimeReportPeriod

		CmdRslTransmitterSetSerial,				// �������� �������� � ���������� ��-�
		CmdRslTransmitterSetSerialAck,			// ����� "�������" �� CmdRslTransmitterSetSerial
		CmdRslTransmitterSetSerialNak,			// ����� "�����" �� CmdRslTransmitterSetSerial
		CmdRslTransmitterGetSerial,				// ��������� �������� � ���������� ��-�
		CmdRslTransmitterNotifySerial,			// ����� �� CmdRslTransmitterGetSerial
		CmdRslTransmitterSetSignalConfig,		// �������� ��������� ������� ���������� ��-�
		CmdRslTransmitterSetSignalConfigAck,	// ����� "�������" �� CmdRslTransmitterSetSignalConfig
		CmdRslTransmitterSetSignalConfigNak,	// ����� "�����" �� CmdRslTransmitterSetSignalConfig
		CmdRslTransmitterGetSignalConfig,		// ��������� ��������� ������� ���������� ��-�
		CmdRslTransmitterNotifySignalConfig,	// ����� �� CmdRslTransmitterGetSignalConfig
		CmdRslTransmitterSetHFConfig,			// �������� ��������� �� ���� ���������� ��-�
		CmdRslTransmitterSetHFConfigAck,		// ����� "�������" �� CmdRslTransmitterSetHFConfig
		CmdRslTransmitterSetHFConfigNak,		// ����� "�����" �� CmdRslTransmitterSetHFConfig
		CmdRslTransmitterGetHFConfig,			// ��������� ��������� �� ���� ���������� ��-�
		CmdRslTransmitterNotifyHFConfig,		// ����� �� CmdRslTransmitterGetHFConfig
		CmdRslTransmitterSetHFPowerAmplifier,	// �������� �������� �� ���������� ����������� ��-�
		CmdRslTransmitterSetHFPowerAmplifierAck,// ����� "�������" �� CmdRslTransmitterSetHFPower
		CmdRslTransmitterSetHFPowerAmplifierNak,// ����� "�����" �� CmdRslTransmitterSetHFPower
		CmdRslTransmitterGetHFPowerAmplifier,	// ��������� �������� �� ���������� ����������� ��-�
		CmdRslTransmitterNotifyHFPowerAmplifier,// ����� �� CmdRslTransmitterGetHFPower
		CmdRslTransmitterSetTypeHFGeneration,	// �������� ��� ��������� (���������� ��� �����������)
		CmdRslTransmitterSetTypeHFGenerationAck,// ����� "�������" �� CmdRslTransmitterSetTypeHFGeneration
		CmdRslTransmitterSetTypeHFGenerationNak,// ����� "�����" �� CmdRslTransmitterSetTypeHFGeneration
		CmdRslTransmitterGetTypeHFGeneration,	// ��������� ��� ��������� (���������� ��� �����������)
		CmdRslTransmitterNotifyTypeHFGeneration,// ����� �� CmdRslTransmitterGetTypeHFGeneration


		CmdRsTvGetWorkTime,						// ������ ������� ������ ������
		CmdRsTvNotifyWorkTime,					// ����� �� CmdRsTvGetWorkTime
		CmdRsTvSetWorkTime,						// ��������� ������� ������ ������
		CmdRsTvSetWorkTimeAck,					// ����� "�������" �� CmdRsTvSetWorkTime
		CmdRsTvSetWorkTimeNak,					// ����� "�����" �� CmdRsTvSetWorkTime
		CmdRsTvSetOn,							// �������� ������
		CmdRsTvSetOnNak,						// ����� "�����" �� CmdRsTvSetOn
		CmdRsTvSetOff,							// �������� ������
		CmdRsTvSetOffNak,						// ����� "�����" �� CmdRsTvSetOff
		CmdRsTvGetConfig,						// ������ ������������ ������
		CmdRsTvNotifyConfig,					// ����� �� CmdRsTvGetConfig
		CmdRsTvSetConfig,						// ��������� ������������ ������
		CmdRsTvSetConfigAck,					// ����� "�������" �� CmdRsTvSetConfig
		CmdRsTvSetConfigNak,					// ����� "�����" �� CmdRsTvSetConfig 
		CmdRsTvVrGetCommonStatus,					// ������ ������� ����������� ����������������
		CmdRsTvVrNotifyCommonStatus,			// ����� �� CmdRsTvVrGetCommonStatus
		CmdRsTvVrGetGsmStatus,					// ������ ������� GSM ����������������
		CmdRsTvVrNotifyGsmStatus,				// ����� �� CmdRsTvVrGetGsmStatus
		CmdRsTvVrNotifyCoreStart,				// ����������� � ������ ���� �����������������
		CmdRsTvVrNotifyRecordComplete,			// ����������� �� ��������� ������
		CmdRsTvVrStartRecord,					// ����� ������ �����
		CmdRsTvVrNotifyStartRecord,				// ����������� � ������ ������ �����
		CmdRsTvVrStopRecord,					// ��������� ������ �����
		CmdRsTvVrGetSerial,						// ��������� �������� �� ��-��
		CmdRsTvVrNotifySerial,					// ����� �� CmdRsTvVrGetSerial
		CmdRsTvVrSetSerial,						// �������� �������� � ��-��
		CmdRsTvVrSetSerialAck,					// ����� "�������" �� CmdRsTvVrSetSerial
		CmdRsTvVrSetSerialNak					// ����� "�����" �� CmdRsTvVrSetSerial
	};

	// ��������� �������
	struct CmdInfo
	{
		CmdInternal Cmd;			// ��� �������
		const unsigned char * Data;		// ������ ��������� � ��������
		unsigned int DataSize;		// ������ ������
		bool Handled;				// ������� ���� ����������
		const RequestParamsBase* RequestParams;
		DispatchParamsBase* DispatchParams;
		// ����������� ��-���������
		CmdInfo() : Cmd(CmdUnknown), Data(0), DataSize(0), Handled(false), DispatchParams(0), RequestParams(0)
		{}
		// ����������� �� ���� �������
		CmdInfo(CmdInternal cmd) : Cmd(cmd), Data(0), DataSize(0), Handled(false), DispatchParams(0), RequestParams(0)
		{}
		// ����������� �� ���� ������� � ������
		CmdInfo(CmdInternal cmd, const unsigned char* data, unsigned int dataSize)
		: Cmd(cmd)
		, Data(data)
		, DataSize(dataSize)
		, Handled(false)
		, DispatchParams(0)
		, RequestParams(0)
		{}

		CmdInfo(CmdInternal cmd, const unsigned char* data, unsigned int dataSize, const RequestParamsBase& requestParams) 
		: Cmd(cmd)
		, Data(data)
		, DataSize(dataSize)
		, Handled(false)
		, DispatchParams(0)
		, RequestParams(&requestParams)
		{}
	};
}
}

#endif
