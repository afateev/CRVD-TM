///////////////////////////////////////////////////////////////////////////////
//	Строитель пакетов
//	Протокол FirTwig2 специальный для ответов по кабелю
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETBUILDERFORCABLE_FIRTWIG2_H
#define PACKETBUILDERFORCABLE_FIRTWIG2_H

#include "PacketBuilder.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{
/// Класс построителя пакетов для отправки по проводу (в ПК).
class PacketBuilderForCable : public PacketBuilder
{
public:

	/// Конструктор по умолчанию
	PacketBuilderForCable()
	{
		_defaultParams.Source = AddrExt;
		_defaultParams.Dest = AddrPc;
	}

	/// Построить пакет FirTwig2 на основе информации о команде.
	/// \param cmd Информация о команде.
	/// \return @c true - успешно, @c false - ошибка.
	bool Build(CmdInfo *cmd)
	{
		if (cmd == 0)
			return false;

		const Parser::PacketSummaryInfo* currentParams = GetCurrentCmdParams(cmd);
		if (currentParams == 0)
			return 0;

		ExternalCmdInfo externalCmd;
		if (ConvertToExternal(cmd, &externalCmd))
		{
			SetCmd(externalCmd.Cmd[0]);

			if (externalCmd.NestingLevel > 1)
			{
				SetSubCmd(&externalCmd.Cmd[1], externalCmd.NestingLevel - 1);
			}

			if (externalCmd.Data)
				SetData(externalCmd.NestingLevel - 1, cmd->Data, cmd->DataSize);
			else
				SetLen(externalCmd.NestingLevel - 1);

			SetAck(externalCmd.Ack);
			SetRequest(externalCmd.Request);
			SetSource(externalCmd.Source);
			SetDest(externalCmd.Dest);

			return true;
		}

		// не знаем, что строить
		// если что-то спрашивали, параметры будут,
		// иначе не спрашивали, получается, что вообще не знаем что строить
		if (CheckRequestParams(cmd) == false)
			return false;

		// Отвечам то что спросили, но с флагом Ack = "ошибка при выполнении"
		if (currentParams->IsModemData())
		{
			if (currentParams->NestingLevel > 1)
			{
				SetCmd(currentParams->Cmd[1]);
				SetData(currentParams->Data[1], currentParams->DataSize[1]);
			}
			else
				return false;
		}
		else
		{
			SetCmd(currentParams->Cmd[0]);
			SetData(currentParams->Data[0], currentParams->DataSize[0]);
		}

		SetDest(currentParams->Source);
		SetSource(currentParams->Dest);
		SetRequest(1);
		SetAck(1);	// ошибка при выполнении

		return true;
	}

protected:

	/// Конвертация внутренней команды в набор вложенных команд в формате FirTwig2
	/// \param [in] cmd Информация о внутренней команде
	/// \param [out] externalCmd Информация о команде в формате FirTwig2
	/// \return @c true конвертация прошла успешно, @c false - ошибка.
	bool ConvertToExternal(CmdInfo *cmd, ExternalCmdInfo* externalCmd)
	{
		if (0 == cmd)
			return false;

		const Parser::PacketSummaryInfo* currentParams = GetCurrentCmdParams(cmd);

		externalCmd->ModemCmd = false;
		
		switch(cmd->Cmd)
		{
		case CmdNotifySoftwareVersion:
			externalCmd->Cmd[0] = DevCmdGetFirmwareVer;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdSetBaudRateAck:
			externalCmd->Cmd[0] = DevCmdSetBaudRate;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdSetBaudRateNak:
			externalCmd->Cmd[0] = DevCmdSetBaudRate;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
#ifndef RBLIB_BOOTLOADER
		case CmdDevicePing:
			externalCmd->Cmd[0] = DevCmdPing;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 0;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdNotifyDebugOutFormat:
			externalCmd->Cmd[0] = DevCmdGetOutputMode;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdSetDebugOutFormatAck:
			externalCmd->Cmd[0] = DevCmdSetOutputMode;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdSetDebugOutFormatNak:
			externalCmd->Cmd[0] = DevCmdSetOutputMode;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdSetCoordinatesAck:
			externalCmd->Cmd[0] = DevCmdExtended;
			externalCmd->Cmd[1] = DevCmdExtSetCoord;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdSetCoordinatesNak:
			externalCmd->Cmd[0] = DevCmdExtended;
			externalCmd->Cmd[1] = DevCmdExtSetCoord;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdSetCoordinatesDirectionAck:
			externalCmd->Cmd[0] = DevCmdExtended;
			externalCmd->Cmd[1] = DevCmdExtSetCoordDirection;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdSetCoordinatesDirectionNak:
			externalCmd->Cmd[0] = DevCmdExtended;
			externalCmd->Cmd[1] = DevCmdExtSetCoordDirection;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdNotifyAlarmState:
			externalCmd->Cmd[0] = DevCmdGetSensSate;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
    case CmdDeviceTypeNotify:
		case CmdRsuNotifyFunctionsList:
			externalCmd->Cmd[0] = DevCmdRsuGetSensConfig;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetFunctionsListAck:
			externalCmd->Cmd[0] = DevCmdRsuSetSensConfig;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetFunctionsListNak:
			externalCmd->Cmd[0] = DevCmdRsuSetSensConfig;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuNotifySeismicParam:
			externalCmd->Cmd[0] = DevCmdRsuGetSeismicParam;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetSeismicParamAck:
			externalCmd->Cmd[0] = DevCmdRsuSetSeismicParam;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetSeismicParamNak:
			externalCmd->Cmd[0] = DevCmdRsuSetSeismicParam;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;

		// ответы SEISMIC_ENVIRONMENT
		case CmdRsuNotifyAvarageNoise:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSIGetSeismicNoise;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuNotifyDetectOptions:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSIGetSeismicAlarmOptions;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetDetectOptionsAck:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISetSeismicAlarmOptions;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetDetectOptionsNak:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISetSeismicAlarmOptions;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuNotifyVehicleThreshold:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSIGetMashineLevel;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetVehicleThresholdAck:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISetMashineLevel;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetVehicleThresholdNak:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISetMashineLevel;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuNotifySeismoResult:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISeismoResultlNotify;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuNotifyPelengThreshold:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSIGetPelengLevel;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetPelengThresholdAck:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISetPelengLevel;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetPelengThresholdNak:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISetPelengLevel;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuSetBanForAzimutIndefiniteTypeAck:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISetBanForAzimutIndefiniteType;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetBanForAzimutIndefiniteTypeNak:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISetBanForAzimutIndefiniteType;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
	
		// ответы SEISMIC_PARAM_EX
		case CmdRsuNotifyBandwidth:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPGetBandwidth;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetBandwidthAck:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPSetBandwidth;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetBandwidthNak:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPSetBandwidth;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuNotifyGain:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPGetGain;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetGainAck:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPSetGain;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetGainNak:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPSetGain;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuProtectiveSectorsNotify:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPGetProtectiveSectors;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetProtectiveSectorsAck:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPSetProtectiveSectors;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetProtectiveSectorsNak:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPSetProtectiveSectors;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuRecognitionTimeSectorNotify:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPGetRecognitionTimeSector;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetRecognitionTimeSectorAck:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPSetRecognitionTimeSector;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetRecognitionTimeSectorNak:
			externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
			externalCmd->Cmd[1] = DevCmdRsuSPSetRecognitionTimeSector;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;

		case CmdRsIrSetLedOnOffAck:
			externalCmd->Cmd[0] = DevCmdRsIrSetLedOnOff;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsIrSetLedOnOffNak:
			externalCmd->Cmd[0] = DevCmdRsIrSetLedOnOff;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsIrSetSensivityAck:
			externalCmd->Cmd[0] = DevCmdRsIrSetSensivity;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsIrSetSensivityNak:
			externalCmd->Cmd[0] = DevCmdRsIrSetSensivity;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsIrNotifyParams:
			externalCmd->Cmd[0] = DevCmdRsIrGetParams;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsIrNotifyAlgorithmParams:
			externalCmd->Cmd[0] = DevCmdRsIrGetAlgorithmParams;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsIrSetAlgorithmParamsAck:
			externalCmd->Cmd[0] = DevCmdRsIrSetAlgorithmParams;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsIrSetAlgorithmParamsNak:
			externalCmd->Cmd[0] = DevCmdRsIrSetAlgorithmParams;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;

		case CmdRsuSetYellowAlarmParamsAck:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISetYellowAlarmParams;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetYellowAlarmParamsNak:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSISetYellowAlarmParams;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuNotifyYellowAlarmParams:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSIGetYellowAlarmParams;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;

		case CmdRsuTestAmplifierNoiseAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckNoise;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestAmplifierNoiseNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckNoise;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestAmplifierNoiseNotify:
			externalCmd->Cmd[0] = DevCmdRsuTestResultNotify;
			externalCmd->Cmd[1] = DevCmdRsuTRNCheckNoise;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestRelativeDelayAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckSensor;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestRelativeDelayNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckSensor;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestRelativeDelayNotify:
			externalCmd->Cmd[0] = DevCmdRsuTestResultNotify;
			externalCmd->Cmd[1] = DevCmdRsuTRNCheckSensor;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestAmplifierResponseAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckResponse;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestAmplifierResponseNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckResponse;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestAmplifierResponseNotify:
			externalCmd->Cmd[0] = DevCmdRsuTestResultNotify;
			externalCmd->Cmd[1] = DevCmdRsuTRNCheckResponse;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSelfTestAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckAll;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSelfTestNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckAll;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuNotifySelfTestReport:
			externalCmd->Cmd[0] = DevCmdRsuTestResultNotify;
			externalCmd->Cmd[1] = DevCmdRsuSTGetCheckAllReport;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetTestReportEnableAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckReportEnable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetTestReportEnableNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckReportEnable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuSetTestReportDisableAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckReportDisable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetTestReportDisableNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTCheckReportDisable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuSetShuntShortedAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTShortShunt;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetShuntShortedNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTShortShunt;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuSetShuntOpenAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTOpenShunt;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetShuntOpenNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTOpenShunt;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestCurrentAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTGetCurrent;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestCurrentNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTGetCurrent;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestCurrentNotify:
			externalCmd->Cmd[0] = DevCmdRsuTestResultNotify;
			externalCmd->Cmd[1] = DevCmdRsuTRNCurrent;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestExtAmplifierPowerOnAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTExtAmplifierPowerOn;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestExtAmplifierPowerOnNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTExtAmplifierPowerOn;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestExtAmplifierPowerOffAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTExtAmplifierPowerOff;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestExtAmplifierPowerOffNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTExtAmplifierPowerOff;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestI2C0PullUpEnableAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTPullUpEnableI2C0;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestI2C0PullUpEnableNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTPullUpEnableI2C0;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestI2C0PullUpDisableAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTPullUpDisableI2C0;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestI2C0PullUpDisableNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTPullUpDisableI2C0;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestI2C1PullUpEnableAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTPullUpEnableI2C1;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestI2C1PullUpEnableNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTPullUpEnableI2C1;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestI2C1PullUpDisableAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTPullUpDisableI2C1;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestI2C1PullUpDisableNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTPullUpDisableI2C1;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestVibroOnAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTVibroOn;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestVibroOnNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTVibroOn;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestVibroOffAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTVibroOff;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestVibroOffNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTVibroOff;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestLBEnableAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTLBEnable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestLBEnableNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTLBEnable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuTestLBDisableAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTLBDisable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuTestLBDisableNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTLBDisable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuSelfTestChangeTypeEnableAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTChangeTypeEnable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSelfTestChangeTypeEnableNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTChangeTypeEnable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuSelfTestChangeTypeDisableAck:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTChangeTypeDisable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSelfTestChangeTypeDisableNak:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTChangeTypeDisable;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuNotifySelfTestChangeType:
			externalCmd->Cmd[0] = DevCmdRsuSelfTest;
			externalCmd->Cmd[1] = DevCmdRsuSTChangeTypeNotify;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		
		case CmdRsuSetRelayOnNak:
			externalCmd->Cmd[0] = DevCmdRsuExtDevControl;
			externalCmd->Cmd[1] = DevCmdRsuEDOn;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuSetRelayOffNak:
			externalCmd->Cmd[0] = DevCmdRsuExtDevControl;
			externalCmd->Cmd[1] = DevCmdRsuEDOff;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuNotifyRelayTime:
			externalCmd->Cmd[0] = DevCmdRsuGetExtDevTime;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetRelayTimeAck:
			externalCmd->Cmd[0] = DevCmdRsuSetExtDevTime;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetRelayTimeNak:
			externalCmd->Cmd[0] = DevCmdRsuSetExtDevTime;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuNotifyRelayEnableByOwnSensor:
			externalCmd->Cmd[0] = DevCmdRsuExtDevParam;
			externalCmd->Cmd[1] = DevCmdRsuEDPGetParam;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetRelayEnableByOwnSensorAck:
			externalCmd->Cmd[0] = DevCmdRsuExtDevParam;
			externalCmd->Cmd[1] = DevCmdRsuEDPSetParam;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetRelayEnableByOwnSensorNak:
			externalCmd->Cmd[0] = DevCmdRsuExtDevParam;
			externalCmd->Cmd[1] = DevCmdRsuEDPSetParam;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsuNotifyRelayModuleSwitchOnPreventFlag:
			externalCmd->Cmd[0] = DevCmdRsuExtDevParam;
			externalCmd->Cmd[1] = DevCmdRsuEDPGetSwitchOnPreventFlag;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetRelayModuleSwitchOnPreventFlagAck:
			externalCmd->Cmd[0] = DevCmdRsuExtDevParam;
			externalCmd->Cmd[1] = DevCmdRsuEDPSetSwitchOnPreventFlag;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuSetRelayModuleSwitchOnPreventFlagNak:
			externalCmd->Cmd[0] = DevCmdRsuExtDevParam;
			externalCmd->Cmd[1] = DevCmdRsuEDPSetSwitchOnPreventFlag;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		// Группа System Control
		case CmdNotifySystemInfo:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlGetSystemInfo;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdBattaryNotifyType:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlBattaryGetType;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdBattaryNotifyRemainingLifeTime:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlBattaryGetReminingLifeTime;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdBattaryNotifyInfo:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlBattaryGetInfo;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdBattaryNotifyState:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlBattaryGetState;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdBattaryNotifyOperatingTime:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlBattaryGetOperatingTime;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdBattaryNotifyPowerConsumersCount:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlBattaryGetPowerConsumersCount;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdBattaryNotifyPowerConsumersList:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlBattaryGetPowerConsumersList;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdBattaryNotifyResetCounters:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlBattaryGetResetCounters;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdBattaryNotifyRemainingLifeTimeReportPeriod:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlGetRemainingLifeTimeReportPeriod;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdBattarySetRemainingLifeTimeReportPeriodAck:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlSetRemainingLifeTimeReportPeriod;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdBattarySetRemainingLifeTimeReportPeriodNak:
			externalCmd->Cmd[0] = DevCmdSystemControl;
			externalCmd->Cmd[1] = DevCmdSysCtrlSetRemainingLifeTimeReportPeriod;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
		break;
		case CmdGetSoftwareVersion:
			externalCmd->Cmd[0] = DevCmdGetFirmwareVer;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 0;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsuAlarmFlagsNotify:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSIAlarmFlagsNotify;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;

		// команды передатчика РС-Л
		case CmdRslTransmitterSetSerialAck:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterSetSerial;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRslTransmitterSetSerialNak:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterSetSerial;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 1;
			break;
		case CmdRslTransmitterNotifySerial:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterGetSerial;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRslTransmitterSetSignalConfigAck:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterSetSignal;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRslTransmitterSetSignalConfigNak:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterSetSignal;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 1;
			break;
		case CmdRslTransmitterNotifySignalConfig:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterGetSignal;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRslTransmitterSetHFConfigAck:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterSetHFConfig;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRslTransmitterSetHFConfigNak:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterSetHFConfig;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 1;
			break;
		case CmdRslTransmitterNotifyHFConfig:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterGetHFConfig;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRslTransmitterSetHFPowerAmplifierAck:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterSetHFPowerAmplifier;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRslTransmitterSetHFPowerAmplifierNak:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterSetHFPowerAmplifier;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRslTransmitterNotifyHFPowerAmplifier:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterGetHFPowerAmplifier;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRslTransmitterSetTypeHFGenerationAck:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterSetTypeHFGeneration;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRslTransmitterSetTypeHFGenerationNak:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterSetTypeHFGeneration;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRslTransmitterNotifyTypeHFGeneration:
			externalCmd->Cmd[0] = RslTrasmitterCmdTransmitter;
			externalCmd->Cmd[1] = RslTrasmitterCmdTransmitterGetTypeHFGeneration;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;

		// команды РС-ТВ
		case CmdRsTvNotifyWorkTime:
			externalCmd->Cmd[0] = DevCmdRsTvGetWorkTime;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvSetWorkTimeAck:
			externalCmd->Cmd[0] = DevCmdRsTvSetWorkTime;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvSetWorkTimeNak:
			externalCmd->Cmd[0] = DevCmdRsTvSetWorkTime;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsTvSetOnNak:
			externalCmd->Cmd[0] = DevCmdRsTvWorkControl;
			externalCmd->Cmd[1] = DevCmdRsTvWorkOn;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsTvSetOffNak:
			externalCmd->Cmd[0] = DevCmdRsTvWorkControl;
			externalCmd->Cmd[1] = DevCmdRsTvWorkOff;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsTvNotifyConfig:
			externalCmd->Cmd[0] = DevCmdRsTvCameraControl;
			externalCmd->Cmd[1] = DevCmdRsTvCameraControlGetConfig;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvSetConfigAck:
			externalCmd->Cmd[0] = DevCmdRsTvCameraControl;
			externalCmd->Cmd[1] = DevCmdRsTvCameraControlSetConfig;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvSetConfigNak:
			externalCmd->Cmd[0] = DevCmdRsTvCameraControl;
			externalCmd->Cmd[1] = DevCmdRsTvCameraControlSetConfig;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsTvVrGetCommonStatus:
			externalCmd->Cmd[0] = DevCmdRsTvVrControl;
			externalCmd->Cmd[1] = DevCmdRsTvVrControlGetCommonStatus;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvVrGetGsmStatus:
			externalCmd->Cmd[0] = DevCmdRsTvVrControl;
			externalCmd->Cmd[1] = DevCmdRsTvVrControlGetGsmStatus;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvVrNotifyCoreStart:
			externalCmd->Cmd[0] = DevCmdRsTvVrControl;
			externalCmd->Cmd[1] = DevCmdRsTvVrControlNotifyCoreStart;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvVrNotifyRecordComplete:
			externalCmd->Cmd[0] = DevCmdRsTvVrControl;
			externalCmd->Cmd[1] = DevCmdRsTvVrControlNotifyRecordComplete;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvVrStartRecord:
			externalCmd->Cmd[0] = DevCmdRsTvVrControl;
			externalCmd->Cmd[1] = DevCmdRsTvVrControlStartRecord;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvVrNotifyStartRecord:
			externalCmd->Cmd[0] = DevCmdRsTvVrControl;
			externalCmd->Cmd[1] = DevCmdRsTvVrControlNotifyStartRecord;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvVrStopRecord:
			externalCmd->Cmd[0] = DevCmdRsTvVrControl;
			externalCmd->Cmd[1] = DevCmdRsTvVrControlStopRecord;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvVrSetSerialAck:
			externalCmd->Cmd[0] = DevCmdRsTvVrControl;
			externalCmd->Cmd[1] = DevCmdRsTvVrControlSetSerial;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			break;
		case CmdRsTvVrSetSerialNak:
			externalCmd->Cmd[0] = DevCmdRsTvVrControl;
			externalCmd->Cmd[1] = DevCmdRsTvVrControlSetSerial;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = false;
			externalCmd->Ack = 1;
			break;
		case CmdRsTvVrNotifySerial:
			externalCmd->Cmd[0] = DevCmdRsTvVrControl;
			externalCmd->Cmd[1] = DevCmdRsTvVrControlGetSerial;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
			
#endif // RBLIB_BOOTLOADER
		// остальные строим обычным строителем
		default:
			return PacketBuilder::ConvertToExternal(cmd, externalCmd);
		}

		if (CheckRequestParams(cmd))
		{
			externalCmd->Dest = currentParams->Source;
			externalCmd->Source = currentParams->Dest;
		}
		else
		{
			externalCmd->Dest = currentParams->Dest;
			externalCmd->Source = currentParams->Source;
		}

		return true;
	}

};

}
}
}

#endif
