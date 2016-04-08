///////////////////////////////////////////////////////////////////////////////
//	������ ������� � ��������� ������ ���������
//	�������� FirTwig2
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETFORMAT_FIRTWIG2_H
#define PACKETFORMAT_FIRTWIG2_H

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{
	/// \brief ������ ���������, ����������� � ������ � ����� Dest � Source.
	enum DeviceAddress
	{
		/// ��.
		AddrPc		= 0x00,
		/// ������� ����������.
		AddrExt		= 0x01,
		/// �����.
		AddrModem	= 0x02,
		/// ����� ����������.
		AddrAll		= 0x0F
	};
	
	/// \brief ������������ ������� ������, ��������� �� ���� ����������.
	enum DevCommandSet
	{
		/// ����� ������ �� ���������, ����� ��� ���������� ����������.
		DevCommandSetDefault,
		/// ����� ������ ������.
		DevCommandSetModem,
		/// ����� ������ ��-�.
		DevCommandSetRsu,
		/// ����� ������ ��-��.
		DevCommandSetRsIr,
		/// ����� ������ ����������� ��-�
		DevCommandSetRslTransmitter,
		/// ����� ������ ��-��
		DevCommandSetRsTv
	};

	/// \brief ������ ������ ������.
	enum ModemCmd
	{
		/// ����.
		ModemCmdPing		= 0x00,
		/// ������ ������ ��.
		ModemCmdWhoAreYou	= 0x01,
		/// ������ �������� �� ����������� (UCMD_DATA).
		ModemCmdData		= 0x08,
		/// ������ ������ ��.
		ModemCmdText	= 0x09,
		/// ��������� ���������� (UCMD_SYS).
		ModemCmdSys			= 0x0A,
		/// ��������� ������ � ����, ������� �� ��������� (UCMD_SENDDEF).
		ModemCmdSendDef		= 0x10,
		/// ��������� ������ � ����, �� ��������� ����� (UCMD_SENDADR).
		ModemCmdSendAddr	= 0x11,
		/// ������� ������ (UCMD_DO).
		ModemCmdDo			= 0x12,
		/// �������� ������� ������ (UCMD_TEST)
		ModemCmdTest		= 0x13
	};

	/// \brief ������ ������ �� ����� ��� ���� ���������.
	enum DevCmd
	{
		/// �������� �����.
		DevCmdPing				= 0x00,
		/// ������ ������ �������� ����������.
		DevCmdGetFirmwareVer	= 0x01,
		/// ��������� �������� ������ �� �����.
		DevCmdSetBaudRate		= 0x02,
		/// ������ ��������� ������
		DevCmdSystemControl		= 0x03,
		/// ��������������� ���������� ����������.
		DevCmdLogData			= 0x05,
		/// ��������� / ���������� �������.
		DevCmdSetSensorOnOff	= 0x07,
		/// ����� �� DevCmdGetFirmwareVer.
		DevCmdFirmwareVerNotify	= 0x08,
		/// ������ �������� ������� ������ ��������� ������.
		DevCmdGetOutputMode		= 0x17,
		/// ��������� �������� ������� ������ ��������� ������.
		DevCmdSetOutputMode		= 0x18,
		/// ����� �� DevCmdGetOutputMode.
		DevCmdOutputModeNotify	= 0x19,
		/// ���������� ������.
		DevCmdOutData			= 0x1A,
		/// ������ ��������� �������.
		DevCmdGetSensSate		= 0x1B,
		/// ��������� �������.
		DevCmdSensStateNotify	= 0x1C,
		/// ������ �������������� ������.
		DevCmdExtended			= 0x1E
	};

	/// \brief ������ ��������� ������ DevCmdSystemControl
	enum DevCmdGroupSystemControl
	{
		/// ������ ���������� � ��������� �������
		DevCmdSysCtrlGetSystemInfo						= 0x01,
		/// ����� �� DevCmdSysCtrlGetSystemInfo
		DevCmdSysCtrlSystemInfoNotify					= 0x81,
		/// ������ ���� ������������ �������
		DevCmdSysCtrlBattaryGetType						= 0x10,
		/// ����� �� DevCmdSysCtrlBattaryGetType
		DevCmdSysCtrlBattaryTypeNotify					= 0x90,
		/// ������ ����������� ������� ������ �� �������
		DevCmdSysCtrlBattaryGetReminingLifeTime			= 0x11,
		/// ����� �� DevCmdSysCtrlBattaryGetReminingLifeTime
		DevCmdSysCtrlBattaryReminingLifeTimeNotify		= 0x91,
		/// ������ ����� ���������� � �������
		DevCmdSysCtrlBattaryGetInfo						= 0x12,
		/// ����� �� DevCmdSysCtrlBattaryGetInfo
		DevCmdSysCtrlBattaryInfoNotify					= 0x92,
		/// ������ ��������� �������
		DevCmdSysCtrlBattaryGetState					= 0x13,
		/// ����� �� DevCmdSysCtrlBattaryGetState
		DevCmdSysCtrlBattaryStateNotify					= 0x93,
		/// ������ ������� ���������
		DevCmdSysCtrlBattaryGetOperatingTime			= 0x14,
		/// ����� �� DevCmdSysCtrlBattaryGetOperatingTime
		DevCmdSysCtrlBattaryOperatingTimeNotify			= 0x94,
		/// ������ ������� ������ ������������
		DevCmdSysCtrlBattaryGetPowerConsumersCount		= 0x15,
		/// ����� �� DevCmdSysCtrlBattaryGetPowerConsumersCount
		DevCmdSysCtrlBattaryPowerConsumersCountNotify	= 0x95,
		/// ������ ������ ������������
		DevCmdSysCtrlBattaryGetPowerConsumersList		= 0x16,
		/// ����� �� DevCmdSysCtrlBattaryGetPowerConsumersList
		DevCmdSysCtrlBattaryPowerConsumersListNotify	= 0x96,
		/// ������ ��������� ������������ �� �������
		DevCmdSysCtrlBattaryGetResetCounters			= 0x17,
		/// ����� �� DevCmdSysCtrlBattaryGetResetCounters
		DevCmdSysCtrlBattaryResetCountersNotify			= 0x97,
		/// ������ ������� �������� ���������� �� ���������� ������� ������ �� �������
		DevCmdSysCtrlGetRemainingLifeTimeReportPeriod		= 0x18,
		/// ����� �� CmdBattaryGetRemainingLifeTimeReportPeriod
		DevCmdSysCtrlRemainingLifeTimeReportPeriodNotify	= 0x98,
		/// ��������� ������� �������� ���������� �� ���������� ������� ������ �� �������
		DevCmdSysCtrlSetRemainingLifeTimeReportPeriod		= 0x19,
		// ������ ������ �� �����-������					
		DevCmdSysCtrlVmGetSwVer								= 0x02,
		// ������ � ������� ��
		DevCmdSysCtrlVmSwVerNotify							= 0x03,
		// ��������� ������ ���������� � ���� ������
		DevCmdSysCtrlVmSetDeviceNumber 						= 0x04,
		// ������ ������ ���������� � ����					
		DevCmdSysCtrlVmGetDeviceNumber 						= 0x05,
		// ������ � ������� ���������� � ���� ������
		DevCmdSysCtrlVmDeviceNumberNotify 					= 0x06,
		// ��������� ������ ���������� � ���� ������
		DerCmdSysCtrlVmSetNetLevel							= 0x07,
		// ������ ������ ���������� � ���� ������
		DerCmdSysCtrlVmGetNetLevel							= 0x08,		
		// ������ � ������� ���������� � ���� ������
		DerCmdSysCtrlVmNetLevelNotify						= 0x09,		
		// ��������� �������� �������� ����������� �����������
		DerCmdSysCtrlVmSetTxPower							= 0x0A,		
		// ������ �������� �������� ����������� �����������
		DerCmdSysCtrlVmGetTxPower							= 0x0B,		
		// ������ � �������� ��������� ����������� �����������
		DerCmdSysCtrlVmTxPowerNotify						= 0x0C,		
		// ������ ������� �����-������
		DerCmdSysCtrlVmGetDeviceStatus						= 0x0D,		
		// ������ �� �������� �����-������
		DerCmdSysCtrlVmDeviceStatusNotify					= 0x0E,		
		// ������ ���������� � ���������� ��������� ���������
		DerCmdSysCtrlVmApplyChanges							= 0x0F,		
		// ������� ������ ��������� ���������
		DerCmdSysCtrlVmDiscardChanges						= 0x20		
	};

	/// \brief ������ ��������� ������ DevCmdExtended.
	enum DevCmdGroupExtended
	{
		/// ������� �����.
		DevCmdExtNetTimeNotify		= 0x01,
		/// ����������� � ���������� �������.
		DevCmdExtPowerOffNotify		= 0x02,
		/// ������ �������������� ���������.
		DevCmdExtGetCoord			= 0x03,
		/// ���������� �������������� ���������.
		DevCmdExtSetCoord			= 0x04,
		/// ����� �� DevCmdGetCoord.
		DevCmdExtCoordNotify		= 0x05,
		/// ������ ��������� ���������� (� ���������� ��� ��� ����������).
		DevCmdExtGetLoaderState = 0x06,
		/// ��������� ��������� ���������.
		DevCmdExtSetLoaderState = 0x07,
		/// ����� �� DevCmdExtGetLoaderState.
		DevCmdExtGetLoaderStateNotify = 0x08,
		/// ������ ��������.
		DevCmdExtStartLoadeFirmaware = 0x09,
		/// ����� �� DevCmdExtStartLoadeFirmaware.
		DevCmdExtStartLoadeFirmawareNotify = 0x0A,
		/// ������ ������ ��� ������ �� ����.
		DevCmdExtFirmawareSection = 0x0B,
		/// ���������� �������� ��������
		DevCmdExtFinishLoadFirmware = 0x0C,
		/// ����� �� DevCmdExtFinishLoadFirmware
		DevCmdExtFinishLoadFirmwareNotify = 0x0D,
		/// ��������� ��� �� �������� ������ ����� ������.
		DevCmdExtCheckLoadingStage = 0x0E,
    /// ����� �� DevCmdExtCheckLoadingStage
		DevCmdExtCheckLoadingStageNotify = 0x0F,
		///�������� ����� ���������� ��� ���
		DevCmdExtSetRsvSupervisorMode = 0x10,
		///��������� ����� ���������� ��� ���
		DevCmdExtGetRsvSupervisorMode = 0x11,
		///����� �� DevCmdExtGetRsvSupervisorMode
		DevCmdExtRsvSupervisorModeNotify = 0x12,
		///������ ������ ��������������� ����������������
		DevCmdExtSetSelfTestPeriod = 0x13,
		///��������� ������ ��������������� ����������������
		DevCmdExtGetSelfTestPeriod = 0x14,
		///����� �� DevCmdExtSetSelfTestPeriod
		DevCmdExtSelfTestPeriodNotify = 0x15,
		///��������� �������������� ��������� �������
		DevCmdExtStartCalibration = 0x16,
		///��������� ����������
		DevCmdExtCalibrationNotify = 0x17,
		/// ������ �������������� ��������� �����������.
		DevCmdExtGetCoordDirection			= 0x18,
		/// ���������� �������������� ��������� �����������.
		DevCmdExtSetCoordDirection			= 0x19,
		/// ����� �� DevCmdGetCoordDirection.
		DevCmdExtCoordDirectionNotify		= 0x1A,

	};

	/// \brief ������ ������ ���.
	enum DevCmdRsu
	{
		/// ������ ������� ������������ ���.
		DevCmdRsuGetSensConfig		= 0x06,
		/// ��������� ������� ������������ ���.
		DevCmdRsuSetSensConfig		= 0x07,
		/// ����� �� DevCmdRsuGetSensConfig.
		DevCmdRsuSensConfigNotify	= 0x08,
		/// ������ ������ ��������� ������������ �����.
		DevCmdRsuSeismicParamEx		= 0x09,
		/// ������ ������ ��������� ���.
		DevCmdRsuExtDevParam		= 0x0B,
		/// ������ ������� ������ ���.
		DevCmdRsuGetExtDevTime		= 0x0D,
		/// ��������� ������� ������ ���.
		DevCmdRsuSetExtDevTime		= 0x0E,
		/// ����� �� DevCmdRsuGetExtDevTime.
		DevCmdRsuExtDevTimeNotify	= 0x0F,
		/// ������ ������ ���������� ���.
		DevCmdRsuExtDevControl		= 0x10,
		/// ������ ���������� ������������� �������.
		DevCmdRsuGetSeismicParam	= 0x11,
		/// ��������� ���������� ������������� �������.
		DevCmdRsuSetSeismicParam	= 0x12,
		/// ����� �� DevCmdRsuGetSeismicParam.
		DevCmdRsuSeismicParamNotify	= 0x13,
		/// ������ ������ ��� ����������������.
		DevCmdRsuSelfTest			= 0x14,
		/// ���������� ���������������� (������ ������).
		DevCmdRsuTestResultNotify	= 0x15,
		/// ���������� ���������������� (������ ������).
		DevCmdRsuSeismicInveroment	= 0x1D
	};

	/// \brief ������ ��������� ������ DevCmdRsuTestResultNotify.
	enum DevCmdRsuGroupTestResultNotify
	{
		DevCmdRsuTRNCheckNoise = 0x01,
		DevCmdRsuTRNCheckSensor = 0x02,
		DevCmdRsuTRNCheckResponse = 0x03,
		DevCmdRsuTRNCurrent = 0x022,
		DevCmdRsuTRNWirlessState = 0x31
	};

	// \brief ������ ��������� ������ DevCmdRsuSelfTest.
	enum DevCmdGroupRsuSelfTest
	{
		DevCmdRsuSTCheckNoise			= 0x01,
		DevCmdRsuSTCheckSensor			= 0x02,
		DevCmdRsuSTCheckResponse		= 0x03,
		/// ������ �� ����������������.
		DevCmdRsuSTCheckAll				= 0x04,
		/// ������ ��������� ����������� ����������������.
		DevCmdRsuSTGetCheckAllReport	= 0x05,
		/// ���������� ������ ����������� ����������������.
		DevCmdRsuSTCheckReportEnable	= 0x06,
		/// ���������� ������ ����������� ����������������.
		DevCmdRsuSTCheckReportDisable	= 0x07,
		/// ���������� ������������� ����.
		DevCmdRsuSTShortShunt			= 0x20,
		/// ������� ������������� ����.
		DevCmdRsuSTOpenShunt			= 0x21,
		/// ������ ���� �����������
		DevCmdRsuSTGetCurrent			= 0x22,
		/// �������� ������� ������� ��������
		DevCmdRsuSTExtAmplifierPowerOn	= 0x23,
		/// ��������� ������� ������� ��������
		DevCmdRsuSTExtAmplifierPowerOff	= 0x24,
		/// �������� ������������� ��������� I2C 0
		DevCmdRsuSTPullUpEnableI2C0		= 0x25,
		/// ��������� ������������� ��������� I2C 0
		DevCmdRsuSTPullUpDisableI2C0	= 0x26,
		/// �������� ������������� ��������� I2C 1
		DevCmdRsuSTPullUpEnableI2C1		= 0x27,
		/// ��������� ������������� ��������� I2C 1
		DevCmdRsuSTPullUpDisableI2C1	= 0x28,
		/// �������� ����������
		DevCmdRsuSTVibroOn				= 0x29,
		/// ��������� ����������
		DevCmdRsuSTVibroOff				= 0x2A,
		/// ��������� ������ ��������� �������
		DevCmdRsuSTLBEnable				= 0x2B,
		/// ��������� ������ ��������� �������
		DevCmdRsuSTLBDisable			= 0x2C,
		/// ��������� ����� ���� ��� ��������� ���������� ����������������
		DevCmdRsuSTChangeTypeEnable		= 0x2D,
		/// ��������� ����� ���� ��� ��������� ���������� ����������������
		DevCmdRsuSTChangeTypeDisable	= 0x2E,
		/// ������ �������� ��������� ���������� ����� ���� ��� ��������� ���������� ����������������
		DevCmdRsuSTGetChangeType		= 0x2F,
		/// ����� �� DevCmdRsuSTGetChangeType
		DevCmdRsuSTChangeTypeNotify		= 0x30
	};

	/// \brief ������ ��������� ������ DevCmdRsuSeismicParamEx.
	enum DevCmdGroupRsuSeismicParamEx
	{
		/// ������ ������ �����������.
		DevCmdRsuSPGetBandwidth		= 0x01,
		/// ����� �� DevCmdRsuSPGetBandwidth.
		DevCmdRsuSPBandwidthNotify	= 0x81,
		/// ��������� ������ �����������.
		DevCmdRsuSPSetBandwidth		= 0x02,
		/// ������ �������� ������������ ��������.
		DevCmdRsuSPGetGain			= 0x03,
		/// ����� �� DevCmdRsuSPGetGain.
		DevCmdRsuSPGainNotify		= 0x83,
		/// ��������� ������������ ��������.
		DevCmdRsuSPSetGain			= 0x04,
		/// ������ ���������� ��������.
		DevCmdRsuSPGetProtectiveSectors		= 0x05,
		/// ����� �� DevCmdRsuSPGetProtectiveSectors.
		DevCmdRsuSPProtectiveSectorsNotify	= 0x85,
		/// ��������� ���������� ��������.
		DevCmdRsuSPSetProtectiveSectors		= 0x06,
		/// ������ ���������� ��������.
		DevCmdRsuSPGetRecognitionTimeSector		= 0x07,
		/// ����� �� DevCmdRsuSPGetRecognitionTimeSector.
		DevCmdRsuSPRecognitionTimeSectorNotify		= 0x87,
		/// ��������� ���������� ��������.
		DevCmdRsuSPSetRecognitionTimeSector			= 0x08,
	};

	/// \brief ������ ��������� ������ DevCmdRsuExtDevParam.
	enum DevCmdGroupRsuExtDevParam
	{
		/// ��������� ���������� �������� ��� �� ������������ �������.
		DevCmdRsuEDPGetParam		= 0x01,
		/// ����� �� DevCmdRsuEDPGetParam.
		DevCmdRsuEDPParamNotify		= 0x81,
		/// ��������� ���������� �������� ��� �� ������������ �������.
		DevCmdRsuEDPSetParam		= 0x02,
		/// ������ ����� ���������� ��������� ������ ���
		DevCmdRsuEDPGetSwitchOnPreventFlag	= 0x03,
		/// ��������� ����� ���������� ��������� ������ ���
		DevCmdRsuEDPSetSwitchOnPreventFlag	= 0x04,
	};

	/// \brief ������ ��������� ������ DevCmdRsuSeismicInveroment.
	enum DevCmdGroupRsuSeismicInveroment
	{
		/// ������ �������� ������ ������� ������������ �� ��������� �����.
		DevCmdRsuSIGetSeismicNoise				= 0x01,
		/// ����� �� DevCmdRsuSIGetSeismicNoise.
		DevCmdRsuSISeismicNoiseNotify			= 0x81,
		
		/// ������ ����� ���������.
		DevCmdRsuSIGetSeismicAlarmOptions		= 0x05,
		/// ����� �� DevCmdRsuSIGetSeismicAlarmOptions.
		DevCmdRsuSISeismicAlarmOptionsNotify	= 0x85,
		/// ��������� ����� ���������.
		DevCmdRsuSISetSeismicAlarmOptions		= 0x06,
		
		/// ������ ������ ������������ �� ������������� ��������.
		DevCmdRsuSIGetMashineLevel				= 0x07,
		/// ����� �� DevCmdRsuSIGetMashineLevel.
		DevCmdRsuSIMashineLevelNotify			= 0x87,
		/// ��������� ������ ������������ �� ������������� ��������.
		DevCmdRsuSISetMashineLevel				= 0x08,

		/// ��������� ������� ���������������.
		DevCmdRsuSISeismoResultlNotify			= 0x8A,

		/// ������ ������ ����������.
		DevCmdRsuSIGetPelengLevel				= 0x0B,
		/// ����� �� DevCmdRsuSIGetPelengLevel.
		DevCmdRsuSIPelengLevelNotify			= 0x8C,
		/// ��������� ������ ����������.
		DevCmdRsuSISetPelengLevel				= 0x0C,
		
		/// ����������� � ��������� ����������.
		DevCmdRsuConfigChangeNotify				= 0x0D,
		
		/// ���������� ������ �������� �������� ��������������� ����.
		DevCmdRsuSISetBanForAzimutIndefiniteType= 0x0E,
		/// ������ ����� ������� ������ �������� ��������������� ����
		DevCmdRsuSIGetBanForAzimutIndefiniteType= 0x0F,
		/// ����� �� CmdRsuGetBanForAzimutIndefiniteType
		DevCmdRsuSIBanForAzimutIndefiniteTypeNotify= 0x10,
			
		/// ������ ���������� ����� ������.
		DevCmdRsuSIGetYellowAlarmParams			= 0x03,
		/// ����� �� ������ ���������� ������ ������.
		DevCmdRsuSIYellowAlarmParamsNotify		= 0x83,
		/// ��������� ���������� ������ ������.
		DevCmdRsuSISetYellowAlarmParams			= 0x04,
		/// Ƹ���� �������.
		DevCmdRsuSIYellowAlarmNotify			= 0x82,
    /// ������ alarm_flags
    DevCmdRsuSIGetAlarmFlags = 0x09,
    /// ����� �� DevCmdRsuSIGetAlarmFlags
    DevCmdRsuSIAlarmFlagsNotify = 0x89
	};

	/// ������ ��������� ������ DevCmdRsuExtDev.
	enum DevCmdGroupRsuExtDev
	{
		/// ��������� ���.
		DevCmdRsuEDOff	= 0x00,
		/// �������� ���.
		DevCmdRsuEDOn	= 0x01,
	};

	/// \brief ������ ������ ��-��.
	enum DevCmdRsIr
	{
		/// ��������� / ���������� ������ ��������� ������� � ������� ����������.
		DevCmdRsIrSetLedOnOff				= 0x11,
		/// ��������� ����������������� �������� ��� ������
		DevCmdRsIrSetCustomThresh = 0x12,
		/// ��������� ������ ����������������.
		DevCmdRsIrSetSensivity				= 0x14,
		/// ������ ���������� "����� ��������� ������� � ������� ����������" � "������� ����������������".
		DevCmdRsIrGetParams					= 0x15,
		/// ����� �� DevCmdRsIrGetParams.
		DevCmdRsIrParamsNotify				= 0x16,
		/// ������ ���������� ��������� ��������� �������.
		DevCmdRsIrGetAlgorithmParams		= 0x20,
		/// ��������� ���������� ��������� ��������� �������.
		DevCmdRsIrSetAlgorithmParams		= 0x21
	};

	/// \brief ������ ��������� ������ ModemCmdSys.
	enum ModemCmdSysGroup
	{
		/// ������ � ���������� ��� ������������ ����
		ModemCmdSysLinx = 0x11,
		/// ��������� ������ ������.
		ModemCmdSysMyStatus		= 0x12,
		/// ���������� �������
		ModemCmdSysRout = 0x15,
		/// ���������� ������� � ������ �������������
		ModemCmdSysRout2 = 0x1B,
		/// ���������� � ������� ������� (� ������ �������������)
		ModemCmdSysNetInfo = 0x1A,
		/// ������ ������������ ���� ��.
		ModemCmdSysAskXtyp		= 0x17,
		/// ��������� ��������� ������.
		ModemCmdSysRemoteModemStatus = 0x19,
		/// ������ ��������� ������.
		ModemCmdSysModemVersionRemote = 0x1F,
		/// ����������� � ���������� �������.
		ModemCmdSysTempPower	= 0x20,
		/// ������� �����.
		ModemCmdSysCTime		= 0x25,
		/// ������ ��������� ����������.
		ModemCmdSysDeviceVersionRemote = 0x26,
	};

	/// \brief ������ ��������� ������ ModemCmdDo.
	enum ModemCmdDoGroup
	{
		/// ������� ������ � ����� ��������
		ModemCmdDoStbyMode = 0x01,
		/// ������ ��������� "������" ������.
		ModemCmdDoShowStatus	= 0x02,
		/// ������ ���������� �������
		ModemCmdDoShowRoute	= 0x07,
		/// ��������� ���� �� ������.
		ModemCmdDoSetXtyp		= 0x08,
		/// ������� ������ � ���������� ����� ������
		ModemCmdDoNormalMode = 0x09,
		/// ��������� ������ ������ ������.
		ModemCmdDoSetMode = 0x0A,
		/// ������ ��������� ��������� ������.
		ModemCmdDoAskStatus	= 0x0C,
		/// ������ ������ ��������� ������
		ModemCmdDoGetModemVersionRemote = 0x0F,
		/// ������ �������� �������.
		ModemCmdDoGetCTime		= 0x1E,
		/// ������ ����������� � ���������� �������.
		ModemCmdDoGetTempPower	= 0x20,
		/// ������ ������ ��������� ����������
		ModemCmdDoGetDeviceVersionRemote = 0x23,
		/// ������ �������� ��������� ���� (� ������ �������������)
		ModemCmdDoGetNetInfo = 0x0E
	};

	/// \brief ������ ��������� ������ ModemCmdTest.
	enum ModemCmdTest
	{
		/// �������� ������������
		ModemCmdTestStop = 0x00,
		/// ��������� ����������� � ���������� �������
		ModemCmdTestMeasureVT	= 0x12,
	};

	/// \brief ������ ������ ����������� ��-�.
	enum RslTrasmitterCmd
	{
		/// ������ ������ ����������� ��-�.
		// ��� ��� ��� ����������, ��� ��� ��� ������� � ����������
		RslTrasmitterCmdTransmitter			= 0x1E
	};

	/// \brief ������ ��������� ������ RslTrasmitterCmd.
	enum RslTrasmitterCmdGroupTransmitter
	{
		/// �������� �������� � ���������� ��-�
		RslTrasmitterCmdTransmitterSetSerial = 0x10,
		/// ��������� �������� � ���������� ��-�
		RslTrasmitterCmdTransmitterGetSerial = 0x11,
		/// �������� ��������� ������� ���������� ��-�
		RslTrasmitterCmdTransmitterSetSignal = 0x12,
		/// ��������� ��������� ������� ���������� ��-�
		RslTrasmitterCmdTransmitterGetSignal = 0x13,
		/// �������� ��������� �� ���� ���������� ��-�
		RslTrasmitterCmdTransmitterSetHFConfig = 0x14,
		/// ��������� ��������� �� ���� ���������� ��-�
		RslTrasmitterCmdTransmitterGetHFConfig = 0x15,
		/// �������� �������� �� ���������� ����������� ��-�
		RslTrasmitterCmdTransmitterSetHFPowerAmplifier = 0x16,
		/// ��������� �������� �� ���������� ����������� ��-�
		RslTrasmitterCmdTransmitterGetHFPowerAmplifier = 0x17,
		/// �������� ��� ��������� (���������� ��� �����������)
		RslTrasmitterCmdTransmitterSetTypeHFGeneration = 0x18,
		/// ��������� ��� ��������� (���������� ��� �����������)
		RslTrasmitterCmdTransmitterGetTypeHFGeneration = 0x19

	};

	/// \brief ������ ������ ��-��.
	enum DevCmdRsTv
	{
		/// ������ ������� ������ ������.
		DevCmdRsTvGetWorkTime		= 0x0D,
		/// ��������� ������� ������ ������.
		DevCmdRsTvSetWorkTime		= 0x0E,
		/// ����� �� DevCmdRsTvGetWorkTime.
		DevCmdRsTvWorkTimeNotify	= 0x0F,
		/// ������ ������ ������ �����������������
		DevCmdRsTvVrControl			= 0x09,
		/// ������ ������ ���������� �������.
		DevCmdRsTvWorkControl		= 0x10,
		/// ������ ������ ���������������� ������.
		DevCmdRsTvCameraControl		= 0x16
	};

	/// ������ ��������� ������ DevCmdRsTvVrControl.
	enum DevCmdGroupRsTvVrControl
	{
		/// ������ ������� ����������� ����������������
		DevCmdRsTvVrControlGetCommonStatus		= 0x00,
		/// ����� �� DevCmdRsTvVrControlGetCommonStatus
		DevCmdRsTvVrControlNotifyCommonStatus	= 0x01,
		/// ������ ������� GSM ����������������
		DevCmdRsTvVrControlGetGsmStatus			= 0x02,
		/// ����� �� DevCmdRsTvVrControlGetGsmStatus
		DevCmdRsTvVrControlNotifyGsmStatus		= 0x03,
		/// ����������� � ������ ���� �����������������
		DevCmdRsTvVrControlNotifyCoreStart		= 0x04,
		/// ����������� �� ��������� ������
		DevCmdRsTvVrControlNotifyRecordComplete	= 0x05,
		/// ������ ������ �����
		DevCmdRsTvVrControlStartRecord			= 0x06,
		/// ����������� � ������ ������ �����
		DevCmdRsTvVrControlNotifyStartRecord	= 0x07,
		/// ��������� ������ �����
		DevCmdRsTvVrControlStopRecord			= 0x08,
		/// ��������� �������� �� ��-��
		DevCmdRsTvVrControlGetSerial			= 0x09,
		/// �������� �������� � ��-��
		DevCmdRsTvVrControlSetSerial			= 0x10,
	};
	
	/// ������ ��������� ������ DevCmdRsTvWorkControl.
	enum DevCmdGroupRsTvWorkControl
	{
		/// ��������� ������.
		DevCmdRsTvWorkOff	= 0x00,
		/// �������� ������.
		DevCmdRsTvWorkOn	= 0x01,
	};

	/// ������ ��������� ������ DevCmdRsTvCameraControl.
	enum DevCmdGroupRsTvCameraControl
	{
		/// ������ ������������ ������.
		DevCmdRsTvCameraControlGetConfig	= 0x01,
		/// ����� �� DevCmdRsTvCameraControlGetConfig
		DevCmdRsTvCameraControlNotigyConfig	= 0x81,
		/// ��������� ������������ ������
		DevCmdRsTvCameraControlSetConfig	= 0x02,
	};

	/// ��������� ������� ModemCmdDoSetMode.
	enum DevMode
	{
		DevModeDebugLog = 0x01,		// ��������� ����������� ����
		DevModeSHNews = 0x02,		//���������� �� �� ��������� ������� � ����������
		DevModeSHBad = 0x04,		//���������� � ���� ������� ������
		DevModeEcoDisable = 0x08,	//������ ������������ ������ ������
		DevModeOverHear = 0x20,		//��������� �� �� ���� �������� �������
		DevModeRFProtect = 0x10,	//���������� ��������� �������� ���������� ����.
		DevModeAntBad = 0x40,		//���������� ����� ������� �� (���� ������ �� ������)
		DevModePeleng = 0x80		//������� ��� �������� ����� ���������� �� (���� ������ �� ������)
	};

	/// \brief ��������� ������� UCMD_SENDADR.
	struct ModemSendAddrHeader
	{
		/// ������� �����.
		unsigned char NetAddr;
		/// ������ ��������.
		unsigned char SendMode;
	};

	/// \brief ��������� ������� UCMD_DATA.
	struct ModemCmdDataHeader
	{
		/// ����� ���������� ��� ��������� ������������ (0..0x3F).
		unsigned char Retranslator;
		/// ����� ��������� ���������� (0..0x3F).
		unsigned char Receiver;
		/// ������� ����� ����������� ������.
		/// ����� ���� ���������� 6�� ��� (0x40) � ������� ��������� (������������� �������� �����).
		unsigned char Sender;
		/// ������ ������ � ������� �����������.
		unsigned char Index;
	};

	/// \brief ���� ������� � Request � Ack.
	union SubCommandField
	{
		struct SubCommandStruct
		{
			unsigned char Cmd : 5;
			unsigned char Ack : 2;
			unsigned char Request : 1;
		};
		unsigned char CmdRow;
	};

	/// \brief ���� ��������� ������.
	union HeaderField
	{
		struct
		{
			/// ����� ���������.
			unsigned long Dest : 4;
			/// ����� ���������.
			unsigned long Source : 4;
			/// ��� �������.
			unsigned long Cmd : 5;
			/// ��� �������������.
			unsigned long Ack : 2;
			/// ���� "������"/"�����" (0 - ������ / �����������, 1 - �����).
			unsigned long Request : 1;
			/// ����� ���� ������.
			unsigned long Len : 8;
			/// ����������� ����� ���������.
			unsigned long Crc : 8;
		};
		unsigned char Data[4];

		/// \brief ����������� ��-���������.
		HeaderField()
		{
			for (unsigned char i = 0; i < sizeof(Data); i++)
				Data[i] = 0;
			UpdateCrc();
		}

		/// \brief ����������� ����������� �����.
		void UpdateCrc()
		{
			Crc = 0x89;
			for (unsigned char i = 0; i < sizeof(Data) - 1; i++)
				Crc += Data[i];
		}

		/// \brief ���������� ����� ����������.
		void SetDest(unsigned char value)
		{
			Dest = value;
			UpdateCrc();
		}

		/// \brief ��������� ����� �����������.
		void SetSource(unsigned char value)
		{
			Source = value;
			UpdateCrc();
		}

		/// \brief ���������� ��� ������� �������� ������.
		void SetCmd(unsigned char value)
		{
			Cmd = value;
			UpdateCrc();
		}

		/// \brief ���� ������.
		void SetAck(unsigned char value)
		{
			Ack = value;
			UpdateCrc();
		}

		/// \brief ���� �������.
		void SetRequest(unsigned char value)
		{
			Request = value;
			UpdateCrc();
		}

		/// \brief ����� ��������� ������.
		void SetLen(unsigned char value)
		{
			Len = value;
			UpdateCrc();
		}
	};
}
}
}

#endif
