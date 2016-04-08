
#ifndef COMMAND_CONVERTER_FIRTWIG2_H
#define COMMAND_CONVERTER_FIRTWIG2_H

#include "..\InternalCommands.h"
#include "Parser.h"
#include "PacketFormat.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{
	///
	/// \brief Класс для конвертации команд FirTwig2 во внутреннюю систему команд
	///
	class CommandConverter
	{
	public:

		struct CmdData
		{
			unsigned char* Buffer;
			unsigned int Length;
		};
		
		struct CmdCodes
		{
			unsigned char Cmd[Parser::MaxNestingLevel];
			unsigned char NestingLevel;
		};

		enum CmdSendType
		{
			SendTypeCable = 0,
			SendTypeRadioDef = 1,
			SendTypeRadioAddr = 2
		};

	public:

		/// \brief Основной метод, осуществляет преобразование команды
		/// \param [in] summaryInfo Полная информация о пакете, полученная от парсера
		/// \param [in] commandSet Текущая система команд (зависит от типа датчика)
		/// \param [out] cmdData Вложенные данные
		/// \return Код команды во внутреннем представлении
		static CmdInternal Convert(const Parser::PacketSummaryInfo& summaryInfo, DevCommandSet commandSet, CmdData& cmdData)
		{
			// команда в терминах библиотеки
			CmdInternal cmdInternal = CmdUnknown;

			unsigned char cmdLevel = 0; // уровень вложенности текущей команды

			if (summaryInfo.Source == AddrModem) // если пакет от модема
			{
				switch(summaryInfo.Cmd[0])
				{
				// код команды = UCMD_DATA
				case ModemCmdData:
					{
						if (!summaryInfo.Request)
						{
							cmdLevel = 1; // команда для обработки будет лежать на 1-м уровне вложенности
						}
						else
						{
							// 0x88 это pong модема в режиме ожидания
							return ConvertCommandSetModem(summaryInfo, cmdData); // конвертируем на основе набора команд модема
						}
					}
					break;
				// модем может спросить версию ПО у ВУ
				case DevCmdGetFirmwareVer:
					{
						cmdLevel = 0; // команда для обработки будет лежать на 0-м уровне вложенности
					}
					break;
				// в противном случае это системная команда от модема
				default:
          if (ModemCmdForSpecificCmdSet(summaryInfo))
          {
            cmdLevel = 0;
          }
          else
          {
					  return ConvertCommandSetModem(summaryInfo, cmdData); // конвертируем на основе набора команд модема
          }
				}
			}

#ifndef RBLIB_BOOTLOADER
			// Ищем код команды в терминах протокола в заданном наборе команд
			switch(commandSet)
			{
			case DevCommandSetRsu:
				cmdInternal = ConvertCommandSetRsu(summaryInfo, cmdLevel, cmdData);
				break;
			case DevCommandSetRsIr:
				cmdInternal = ConvertCommandSetRsIr(summaryInfo, cmdLevel, cmdData);
				break;
			case DevCommandSetRslTransmitter:
				cmdInternal = ConvertCommandSetRslTransmitter(summaryInfo, cmdLevel, cmdData);
				break;
			case DevCommandSetRsTv:
				cmdInternal = ConvertCommandSetRsTv(summaryInfo, cmdLevel, cmdData);
				break;
			}
#endif // RBLIB_BOOTLOADER

			// Если команда не нашлась в наборах выше
			if (CmdUnknown == cmdInternal)
			{
				// ищем в наборе "по-умолчанию"
				cmdInternal = ConvertCommandSetDefault(summaryInfo, cmdLevel, cmdData);
			}

			return cmdInternal;
		}

	protected:

    static bool ModemCmdForSpecificCmdSet(const Parser::PacketSummaryInfo& summaryInfo)
    {
      if (summaryInfo.NestingLevel >= 2 &&
          summaryInfo.Cmd[0] == DevCmdRsuSeismicInveroment &&
          summaryInfo.Cmd[1] == DevCmdRsuSIGetAlarmFlags)
      {
        return true;
      }
      return false;
    }

		/// \brief Преобразование команды на основе CommandSetDefault
		/// \param [in] summaryInfo Полная информация о пакете, полученная от парсера
		/// \param [in] cmdLevel Уровень вложенности команды
		/// \param [out] cmdData Вложенные данные
		/// \return Код команды во внутреннем представлении
		static CmdInternal ConvertCommandSetDefault(const Parser::PacketSummaryInfo& summaryInfo, unsigned char cmdLevel, CmdData& cmdData)
		{
			CmdInternal cmdInternal = CmdUnknown;

			// текущий уровень вложенности команд в пакете, без учёта UCMD_DATA
			unsigned char actualNestingLevel = summaryInfo.NestingLevel - cmdLevel;

			switch(summaryInfo.Cmd[cmdLevel])
			{
			case DevCmdPing:
				if (summaryInfo.Request)
					cmdInternal = CmdDevicePong;
				else
					cmdInternal = CmdDevicePing;
			break;
			case DevCmdGetFirmwareVer:
				if (summaryInfo.Request)
					cmdInternal = CmdNotifySoftwareVersion;
				else
					cmdInternal = CmdGetSoftwareVersion;
			break;
			case DevCmdSetBaudRate:
				cmdInternal = CmdSetBaudRate;
			break;
			case DevCmdSystemControl:
				{
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case DevCmdSysCtrlGetSystemInfo:
						cmdInternal = CmdGetSystemInfo;
						break;
					case DevCmdSysCtrlBattaryGetType:
						cmdInternal = CmdBattaryGetType;
						break;
					case DevCmdSysCtrlBattaryGetReminingLifeTime:
						cmdInternal = CmdBattaryGetRemainingLifeTime;
						break;
					case DevCmdSysCtrlBattaryGetInfo:
						cmdInternal = CmdBattaryGetInfo;
						break;
					case DevCmdSysCtrlBattaryGetState:
						cmdInternal = CmdBattaryGetState;
						break;
					case DevCmdSysCtrlBattaryGetOperatingTime:
						cmdInternal = CmdBattaryGetOperatingTime;
						break;
					case DevCmdSysCtrlBattaryGetPowerConsumersCount:
						cmdInternal = CmdBattaryGetPowerConsumersCount;
						break;
					case DevCmdSysCtrlBattaryGetPowerConsumersList:
						cmdInternal = CmdBattaryGetPowerConsumersList;
						break;
					case DevCmdSysCtrlBattaryGetResetCounters:
						cmdInternal = CmdBattaryGetResetCounters;
						break;
					case DevCmdSysCtrlGetRemainingLifeTimeReportPeriod:
						cmdInternal = CmdBattaryGetRemainingLifeTimeReportPeriod;
						break;
					case DevCmdSysCtrlSetRemainingLifeTimeReportPeriod:
						cmdInternal = CmdBattarySetRemainingLifeTimeReportPeriod;
						break;
					/// video_modem_cmd_per_test
					case DevCmdSysCtrlVmGetSwVer:
						cmdInternal = CmdVmGetFirmwareVersion;
						break;
					case DevCmdSysCtrlVmSetDeviceNumber:
						cmdInternal = CmdVmSetDeviceNumber;
						break;
					case DevCmdSysCtrlVmGetDeviceNumber:
						cmdInternal = CmdVmGetDeviceNumber;
						break;
					case DerCmdSysCtrlVmSetNetLevel:
						cmdInternal = CmdVmSetNetLevel;
						break;						
					case DerCmdSysCtrlVmGetNetLevel:
						cmdInternal = CmdVmGetNetLevel;
						break;						
					case DerCmdSysCtrlVmSetTxPower:
						cmdInternal = CmdVmSetTxPower;
						break;							
					case DerCmdSysCtrlVmGetTxPower:
						cmdInternal = CmdVmGetTxPower;
						break;	
					case DerCmdSysCtrlVmGetDeviceStatus:
						cmdInternal = CmdVmGetDeviceStatus;
						break;							
					case DerCmdSysCtrlVmApplyChanges:
						cmdInternal = CmdVmSaveAndRestart;
						break;
					case DerCmdSysCtrlVmDiscardChanges:
						cmdInternal = CmdVmDiscardChanges;
						break;						
					}
				}
				break;
			case DevCmdGetOutputMode:
				cmdInternal = CmdGetDebugOutFormat;
			break;
			case DevCmdSetOutputMode:
				cmdInternal = CmdSetDebugOutFormat;
			break;
			case DevCmdSensStateNotify:
				cmdInternal = CmdAlarm;
			break;
			case DevCmdGetSensSate:
				cmdInternal = CmdGetAlarmState;
				break;
			case DevCmdSetSensorOnOff:
				cmdInternal = CmdSetSensorOnOff;
				break;
			case DevCmdExtended:
				{
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case DevCmdExtGetCoord:
						cmdInternal = CmdGetCoordinates;
						break;
					case DevCmdExtSetCoord:
						cmdInternal = CmdSetCoordinates;
						break;
					case DevCmdExtGetLoaderState:
						cmdInternal = CmdGetLoaderState;
						break;
					case DevCmdExtSetLoaderState:
						cmdInternal = CmdSetLoaderState;
						break;
					case DevCmdExtGetLoaderStateNotify:
						cmdInternal = CmdGetLoaderStateNotify;
						break;
					case DevCmdExtStartLoadeFirmaware:
						cmdInternal = CmdStartLoadFirmware;
						break;
					case DevCmdExtStartLoadeFirmawareNotify:
						cmdInternal = CmdStartLoadFirmwareNotify;
						break;
					case DevCmdExtFirmawareSection:
						cmdInternal = CmdFirmwareSection;
						break;
					case DevCmdExtFinishLoadFirmware:
						cmdInternal = CmdFinishLoadFirmware;
						break;
					case DevCmdExtFinishLoadFirmwareNotify:
						cmdInternal = CmdFinishLoadFirmwareNotify;
						break;
					case DevCmdExtCheckLoadingStage:
						cmdInternal = CmdCheckLoadingStage;
						break;
					case DevCmdExtCheckLoadingStageNotify:
						cmdInternal = CmdCheckLoadingStageNotify;
						break;
					case DevCmdExtGetCoordDirection:
						cmdInternal = CmdGetCoordinatesDirection;
						break;
					case DevCmdExtSetCoordDirection:
						cmdInternal = CmdSetCoordinatesDirection;
						break;

#ifndef RBLIB_BOOTLOADER

					case DevCmdExtSetRsvSupervisorMode:
						if (summaryInfo.Request)
						{
							if (summaryInfo.Ack)
							{
								cmdInternal = CmdSetRsvSupervisorModeNak;
							}
							else
							{
								cmdInternal = CmdSetRsvSupervisorModeAck;
							}
						}
						else
						{
							cmdInternal = CmdSetRsvSupervisorMode;
						}
						break;
					case DevCmdExtGetRsvSupervisorMode:
						if (summaryInfo.Request)
						{
							if (summaryInfo.Ack)
							{
								cmdInternal = CmdGetRsvSupervisorModeNak;
							}
							else
							{
								cmdInternal = CmdGetRsvSupervisorModeAck;
							}
						}
						else
						{
							cmdInternal = CmdGetRsvSupervisorMode;
						}
						
						break;

					case DevCmdExtRsvSupervisorModeNotify:

						cmdInternal = CmdRsvSupervisorModeNotify;
						break;

					// Период самотестирования
					case DevCmdExtSetSelfTestPeriod:

						if (summaryInfo.Request)
						{
							if (summaryInfo.Ack)
							{
								cmdInternal = CmdRsuSelfTestSetPeriodNak;
							}
							else
							{
								cmdInternal = CmdRsuSelfTestSetPeriodAck;
							}
						}
						else
						{
							cmdInternal = CmdRsuSelfTestSetPeriod;
						}

						break;

					case DevCmdExtGetSelfTestPeriod:

						if (summaryInfo.Request)
						{
							if (summaryInfo.Ack)
							{
								cmdInternal = CmdRsuSelfTestGetPeriodNak;
							}
							else
							{
								cmdInternal = CmdRsuSelfTestGetPeriodAck;
							}
						}
						else
						{
							cmdInternal = CmdRsuSelfTestGetPeriod;
						}

						break;
					case DevCmdExtSelfTestPeriodNotify:
						
						cmdInternal = CmdRsuSelfTestPeriodNotify;
						break;

						// Автокалибровка
					case DevCmdExtStartCalibration:

						if (summaryInfo.Request)
						{
							if (summaryInfo.Ack)
							{
								cmdInternal = CmdRsuStartCalibrationNak;
							}
							else
							{
								cmdInternal = CmdRsuStartCalibrationAck;
							}
						}
						else
						{
							cmdInternal = CmdRsuStartCalibration;
						}

						break;

					case DevCmdExtCalibrationNotify:

						cmdInternal = CmdRsuCalibrationNotify;
						break;

#endif // RBLIB_BOOTLOADER

					}
				}
				break;
			}

			// Если команда нашлась
			if (CmdUnknown != cmdInternal)
			{
				// выдаем результат наружу (наиболее глубоко вложенные данные)
				cmdData.Buffer = summaryInfo.Data[summaryInfo.NestingLevel - 1];
				cmdData.Length = summaryInfo.DataSize[summaryInfo.NestingLevel - 1];
			}

			return cmdInternal;
		}
        
#ifndef RBLIB_BOOTLOADER

		/// \brief Преобразование команды на основе CommandSetRsu
		/// \param [in] summaryInfo Полная информация о пакете, полученная от парсера
		/// \param [in] cmdLevel Уровень вложенности команды
		/// \param [out] cmdData Вложенные данные
		/// \return Код команды во внутреннем представлении
		static CmdInternal ConvertCommandSetRsu(const Parser::PacketSummaryInfo& summaryInfo, unsigned char cmdLevel, CmdData& cmdData)
		{
			CmdInternal cmdInternal = CmdUnknown;

			// текущий уровень вложенности команд в пакете, без учёта UCMD_DATA
			unsigned char actualNestingLevel = summaryInfo.NestingLevel - cmdLevel;

			switch(summaryInfo.Cmd[cmdLevel])
			{
			case DevCmdRsuGetSensConfig:
				cmdInternal = CmdRsuGetFunctionsList;
				break;
			case DevCmdRsuSetSensConfig:
				cmdInternal = CmdRsuSetFunctionsList;
				break;
			case DevCmdRsuExtDevParam:
				{
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case DevCmdRsuEDPGetParam:
						cmdInternal = CmdRsuGetRelayEnableByOwnSensor;
						break;
					case DevCmdRsuEDPSetParam:
						cmdInternal = CmdRsuSetRelayEnableByOwnSensor;
						break;
					case DevCmdRsuEDPGetSwitchOnPreventFlag:
						cmdInternal = CmdRsuGetRelayModuleSwitchOnPreventFlag;
						break;
					case DevCmdRsuEDPSetSwitchOnPreventFlag:
						cmdInternal = CmdRsuSetRelayModuleSwitchOnPreventFlag;
						break;
					}
				}
				break;
			case DevCmdRsuGetExtDevTime:
				cmdInternal = CmdRsuGetRelayTime;
				break;
			case DevCmdRsuSetExtDevTime:
				cmdInternal = CmdRsuSetRelayTime;
				break;
			case DevCmdRsuGetSeismicParam:
				cmdInternal = CmdRsuGetSeismicParam;
				break;
			case DevCmdRsuSetSeismicParam:
				cmdInternal = CmdRsuSetSeismicParam;
				break;
			case DevCmdRsuSeismicInveroment:	// группа команд
				{	
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case DevCmdRsuSIGetSeismicNoise:
						cmdInternal = CmdRsuGetAvarageNoise;
						break;
					case DevCmdRsuSIGetSeismicAlarmOptions:
						cmdInternal = CmdRsuGetDetectOptions;
						break;
					case DevCmdRsuSISetSeismicAlarmOptions:
						cmdInternal = CmdRsuSetDetectOptions;
						break;
					case DevCmdRsuSIGetMashineLevel:
						cmdInternal = CmdRsuGetVehicleThreshold;
						break;
					case DevCmdRsuSISetMashineLevel:
						cmdInternal = CmdRsuSetVehicleThreshold;
						break;
					case DevCmdRsuSIGetPelengLevel:
						cmdInternal = CmdRsuGetPelengThreshold;
						break;
					case DevCmdRsuSISetPelengLevel:
						cmdInternal = CmdRsuSetPelengThreshold;
						break;
					case DevCmdRsuConfigChangeNotify:
						cmdInternal = CmdRsuNotifyConfigChange;
						break;
					case DevCmdRsuSIGetYellowAlarmParams:
						cmdInternal = CmdRsuGetYellowAlarmParams;
						break;
					case DevCmdRsuSISetYellowAlarmParams:
						cmdInternal = CmdRsuSetYellowAlarmParams;
						break;
					case DevCmdRsuSISetBanForAzimutIndefiniteType:
						cmdInternal = CmdRsuSetBanForAzimutIndefiniteType;
						break;
					case DevCmdRsuSIGetBanForAzimutIndefiniteType:
						cmdInternal = CmdRsuGetBanForAzimutIndefiniteType;
						break;
          case DevCmdRsuSIGetAlarmFlags:
            cmdInternal = CmdRsuGetAlarmFlags;
            break;
					}
				}
				break;
			case DevCmdRsuSeismicParamEx:
				{
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case DevCmdRsuSPGetBandwidth:
						cmdInternal = CmdRsuGetBandwidth;
						break;
					case DevCmdRsuSPSetBandwidth:
						cmdInternal = CmdRsuSetBandwidth;
						break;
					case DevCmdRsuSPGetGain:
						cmdInternal = CmdRsuGetGain;
						break;
					case DevCmdRsuSPSetGain:
						cmdInternal = CmdRsuSetGain;
						break;
					case DevCmdRsuSPGetProtectiveSectors:
						cmdInternal = CmdRsuGetProtectiveSectors;
						break;
					case DevCmdRsuSPSetProtectiveSectors:
						cmdInternal = CmdRsuSetProtectiveSectors;
						break;
					case DevCmdRsuSPGetRecognitionTimeSector:
						cmdInternal = CmdRsuGetRecognitionTimeSector;
						break;
					case DevCmdRsuSPSetRecognitionTimeSector:
						cmdInternal = CmdRsuSetRecognitionTimeSector;
						break;
					}				
				}
				break;
			case DevCmdRsuExtDevControl:
				{
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case DevCmdRsuEDOff:
						cmdInternal = CmdRsuSetRelayOff;
						break;
					case DevCmdRsuEDOn:
						cmdInternal = CmdRsuSetRelayOn;
						break;
					}				
				}
				break;
			case DevCmdRsuSelfTest:
				{
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case DevCmdRsuSTCheckNoise:
						cmdInternal = CmdRsuTestAmplifierNoise;
						break;
					case DevCmdRsuSTCheckSensor:
						cmdInternal = CmdRsuTestRelativeDelay;
						break;
					case DevCmdRsuSTCheckResponse:
						cmdInternal = CmdRsuTestAmplifierResponse;
						break;
					case DevCmdRsuSTCheckAll:
						cmdInternal = CmdRsuSelfTest;
						break;
					case DevCmdRsuSTGetCheckAllReport:
						cmdInternal = CmdRsuGetSelfTestReport;
						break;
					case DevCmdRsuSTCheckReportEnable:
						cmdInternal = CmdRsuSetTestReportEnable;
						break;
					case DevCmdRsuSTCheckReportDisable:
						cmdInternal = CmdRsuSetTestReportDisable;
						break;
					case DevCmdRsuSTShortShunt:
						cmdInternal = CmdRsuSetShuntShorted;
						break;
					case DevCmdRsuSTOpenShunt:
						cmdInternal = CmdRsuSetShuntOpen;
						break;
					case DevCmdRsuSTGetCurrent:
						cmdInternal = CmdRsuTestCurrent;
						break;
					case DevCmdRsuSTExtAmplifierPowerOn:
						cmdInternal = CmdRsuTestExtAmplifierPowerOn;
						break;
					case DevCmdRsuSTExtAmplifierPowerOff:
						cmdInternal = CmdRsuTestExtAmplifierPowerOff;
						break;
					case DevCmdRsuSTPullUpEnableI2C0:
						cmdInternal = CmdRsuTestI2C0PullUpEnable;
						break;
					case DevCmdRsuSTPullUpDisableI2C0:
						cmdInternal = CmdRsuTestI2C0PullUpDisable;
						break;
					case DevCmdRsuSTPullUpEnableI2C1:
						cmdInternal = CmdRsuTestI2C1PullUpEnable;
						break;
					case DevCmdRsuSTPullUpDisableI2C1:
						cmdInternal = CmdRsuTestI2C1PullUpDisable;
						break;
					case DevCmdRsuSTVibroOn:
						cmdInternal = CmdRsuTestVibroOn;
						break;
					case DevCmdRsuSTVibroOff:
						cmdInternal = CmdRsuTestVibroOff;
						break;
					case DevCmdRsuSTLBEnable:
						cmdInternal = CmdRsuTestLBEnable;
						break;
					case DevCmdRsuSTLBDisable:
						cmdInternal = CmdRsuTestLBDisable;
						break;
					case DevCmdRsuSTChangeTypeEnable:
						cmdInternal = CmdRsuSelfTestChangeTypeEnable;
						break;
					case DevCmdRsuSTChangeTypeDisable:
						cmdInternal = CmdRsuSelfTestChangeTypeDisable;
						break;
					case DevCmdRsuSTGetChangeType:
						cmdInternal = CmdRsuSelfTestGetChangeType;
						break;
					}
				}
				break;

				// остальные команды
				
			}

			// Если команда нашлась
			if (CmdUnknown != cmdInternal)
			{
				// выдаем результат наружу (наиболее глубоко вложенные данные)
				cmdData.Buffer = summaryInfo.Data[summaryInfo.NestingLevel - 1];
				cmdData.Length = summaryInfo.DataSize[summaryInfo.NestingLevel - 1];
			}

			return cmdInternal;
		}

		/// \brief Преобразование команды на основе CommandSetRsIr
		/// \param [in] summaryInfo Полная информация о пакете, полученная от парсера
		/// \param [in] cmdLevel Уровень вложенности команды
		/// \param [out] cmdData Вложенные данные
		/// \return Код команды во внутреннем представлении
		static CmdInternal ConvertCommandSetRsIr(const Parser::PacketSummaryInfo& summaryInfo, unsigned char cmdLevel, CmdData& cmdData)
		{
			CmdInternal cmdInternal = CmdUnknown;

			switch(summaryInfo.Cmd[cmdLevel])
			{
      case DevCmdRsuGetSensConfig:
        cmdInternal = CmdGetDeviceType;
        break;
			case DevCmdRsIrSetLedOnOff:
				cmdInternal = CmdRsIrSetLedOnOff;
				break;
			case DevCmdRsIrSetCustomThresh:
				cmdInternal = CmdRsIrSetCustomThresh;
				break;
			case DevCmdRsIrSetSensivity:
				cmdInternal = CmdRsIrSetSensivity;
				break;
			case DevCmdRsIrGetParams:
				cmdInternal = CmdRsIrGetParams;
				break;
			case DevCmdRsIrGetAlgorithmParams:
				cmdInternal = CmdRsIrGetAlgorithmParams;
				break;
			case DevCmdRsIrSetAlgorithmParams:
				cmdInternal = CmdRsIrSetAlgorithmParams;
				break;
			}

			// Если команда нашлась
			if (CmdUnknown != cmdInternal)
			{
				// выдаем результат наружу (наиболее глубоко вложенные данные)
				cmdData.Buffer = summaryInfo.Data[summaryInfo.NestingLevel - 1];
				cmdData.Length = summaryInfo.DataSize[summaryInfo.NestingLevel - 1];
			}

			return cmdInternal;
		}

		/// \brief Преобразование команды на основе CommandSetRslTransmitter
		/// \param [in] summaryInfo Полная информация о пакете, полученная от парсера
		/// \param [in] cmdLevel Уровень вложенности команды
		/// \param [out] cmdData Вложенные данные
		/// \return Код команды во внутреннем представлении
		static CmdInternal ConvertCommandSetRslTransmitter(const Parser::PacketSummaryInfo& summaryInfo, unsigned char cmdLevel, CmdData& cmdData)
		{
			CmdInternal cmdInternal = CmdUnknown;

			// текущий уровень вложенности команд в пакете, без учёта UCMD_DATA
			unsigned char actualNestingLevel = summaryInfo.NestingLevel - cmdLevel;

			switch(summaryInfo.Cmd[cmdLevel])
			{
			case RslTrasmitterCmdTransmitter:
				{
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case RslTrasmitterCmdTransmitterSetSerial:
						cmdInternal = CmdRslTransmitterSetSerial;
						break;
					case RslTrasmitterCmdTransmitterGetSerial:
						cmdInternal = CmdRslTransmitterGetSerial;
						break;
					case RslTrasmitterCmdTransmitterSetSignal:
						cmdInternal = CmdRslTransmitterSetSignalConfig;
						break;
					case RslTrasmitterCmdTransmitterGetSignal:
						cmdInternal = CmdRslTransmitterGetSignalConfig;
						break;
					case RslTrasmitterCmdTransmitterSetHFConfig:
						cmdInternal = CmdRslTransmitterSetHFConfig;
						break;
					case RslTrasmitterCmdTransmitterGetHFConfig:
						cmdInternal = CmdRslTransmitterGetHFConfig;
						break;
					case RslTrasmitterCmdTransmitterSetHFPowerAmplifier:
						cmdInternal = CmdRslTransmitterSetHFPowerAmplifier;
						break;
					case RslTrasmitterCmdTransmitterGetHFPowerAmplifier:
						cmdInternal = CmdRslTransmitterGetHFPowerAmplifier;
						break;
					case RslTrasmitterCmdTransmitterSetTypeHFGeneration:
						cmdInternal = CmdRslTransmitterSetTypeHFGeneration;
						break;
					case RslTrasmitterCmdTransmitterGetTypeHFGeneration:
						cmdInternal = CmdRslTransmitterGetTypeHFGeneration;
						break;
					}
				}
				break;

				// остальные команды

			}

			// Если команда нашлась
			if (CmdUnknown != cmdInternal)
			{
				// выдаем результат наружу (наиболее глубоко вложенные данные)
				cmdData.Buffer = summaryInfo.Data[summaryInfo.NestingLevel - 1];
				cmdData.Length = summaryInfo.DataSize[summaryInfo.NestingLevel - 1];
			}

			return cmdInternal;
		}

		/// \brief Преобразование команды на основе CommandSetRsTv
		/// \param [in] summaryInfo Полная информация о пакете, полученная от парсера
		/// \param [in] cmdLevel Уровень вложенности команды
		/// \param [out] cmdData Вложенные данные
		/// \return Код команды во внутреннем представлении
		static CmdInternal ConvertCommandSetRsTv(const Parser::PacketSummaryInfo& summaryInfo, unsigned char cmdLevel, CmdData& cmdData)
		{
			CmdInternal cmdInternal = CmdUnknown;

			// текущий уровень вложенности команд в пакете, без учёта UCMD_DATA
			unsigned char actualNestingLevel = summaryInfo.NestingLevel - cmdLevel;

			switch(summaryInfo.Cmd[cmdLevel])
			{
			case DevCmdRsTvGetWorkTime:
				cmdInternal = CmdRsTvGetWorkTime;
				break;
			case DevCmdRsTvSetWorkTime:
				cmdInternal = CmdRsTvSetWorkTime;
				break;
			case DevCmdRsTvWorkControl:
				{
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case DevCmdRsTvWorkOff:
						cmdInternal = CmdRsTvSetOff;
						break;
					case DevCmdRsTvWorkOn:
						cmdInternal = CmdRsTvSetOn;
						break;
					}				
				}
				break;
			case DevCmdRsTvCameraControl:
				{
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case DevCmdRsTvCameraControlGetConfig:
						cmdInternal = CmdRsTvGetConfig;
						break;
					case DevCmdRsTvCameraControlSetConfig:
						cmdInternal = CmdRsTvSetConfig;
						break;
					}				
				}
				break;
			case DevCmdRsTvVrControl:
				{
					if (actualNestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[cmdLevel + 1])
					{
					case DevCmdRsTvVrControlGetCommonStatus:
						cmdInternal = CmdRsTvVrGetCommonStatus;
						break;
					case DevCmdRsTvVrControlNotifyCommonStatus:
						cmdInternal = CmdRsTvVrNotifyCommonStatus;
						break;
					case DevCmdRsTvVrControlGetGsmStatus:
						cmdInternal = CmdRsTvVrGetGsmStatus;
						break;
					case DevCmdRsTvVrControlNotifyGsmStatus:
						cmdInternal = CmdRsTvVrNotifyGsmStatus;
						break;
					case DevCmdRsTvVrControlNotifyCoreStart:
						cmdInternal = CmdRsTvVrNotifyCoreStart;
						break;
					case DevCmdRsTvVrControlNotifyRecordComplete:
						cmdInternal = CmdRsTvVrNotifyRecordComplete;
						break;
					case DevCmdRsTvVrControlStartRecord:
						cmdInternal = CmdRsTvVrStartRecord;
						break;
					case DevCmdRsTvVrControlNotifyStartRecord:
						cmdInternal = CmdRsTvVrNotifyStartRecord;
						break;
					case DevCmdRsTvVrControlStopRecord:
						cmdInternal = CmdRsTvVrStopRecord;
						break;
					case DevCmdRsTvVrControlGetSerial:
						cmdInternal = CmdRsTvVrGetSerial;
						break;
					case DevCmdRsTvVrControlSetSerial:
						cmdInternal = CmdRsTvVrSetSerial;
						break;
					}
				}
				break;
				// остальные команды

			}

			// Если команда нашлась
			if (CmdUnknown != cmdInternal)
			{
				// выдаем результат наружу (наиболее глубоко вложенные данные)
				cmdData.Buffer = summaryInfo.Data[summaryInfo.NestingLevel - 1];
				cmdData.Length = summaryInfo.DataSize[summaryInfo.NestingLevel - 1];
			}

			return cmdInternal;
		}

#endif // RBLIB_BOOTLOADER

		/// \brief Преобразование команды на основе CommandSetRsIr
		/// \param [in] summaryInfo Полная информация о пакете, полученная от парсера
		/// \param [out] cmdData Вложенные данные
		/// \return Код команды во внутреннем представлении
		static CmdInternal ConvertCommandSetModem(const Parser::PacketSummaryInfo& summaryInfo, CmdData& cmdData)
		{
			CmdInternal cmdInternal = CmdUnknown;

			switch(summaryInfo.Cmd[0])
			{
      case ModemCmdText:
        cmdInternal = CmdModemVersionNotify;
        break;
			case ModemCmdPing:
				if (summaryInfo.Request)
					cmdInternal = CmdModemPongNormalMode;
				else
					cmdInternal = CmdModemPing;
				break;
			// ЖЕСТЬ!!! это pong от модена в режиме ожидания
			case ModemCmdData:
				if (summaryInfo.Request)
				{
					cmdInternal = CmdModemPongStandbyMode;
				}
				break;
			case ModemCmdSys:	// группа команд
				{	
					if (summaryInfo.NestingLevel < 2)
						break;
					switch(summaryInfo.Cmd[1])
					{
					case ModemCmdSysAskXtyp:
						cmdInternal = CmdModemAskType;
						break;
					case ModemCmdSysTempPower:
						cmdInternal = CmdModemNotifyTempAndPower;
						break;
					case ModemCmdSysMyStatus:
						cmdInternal = CmdModemNotifySelfInfo;
						break;
					case ModemCmdSysCTime:
						cmdInternal = CmdModemNotifyNetTime;
						break;
					case ModemCmdSysRout:
						cmdInternal = CmdModemNotifyRoute;
						break;
					case ModemCmdSysRout2:
						cmdInternal = CmdModemNotifyRoute2;
						break;
					case ModemCmdSysNetInfo:
						cmdInternal = CmdModemNotifyNetInfo;
						break;
					case ModemCmdSysLinx:
						cmdInternal = CmdModemNotifyLinx;
						break;
					case ModemCmdSysRemoteModemStatus:
						cmdInternal = CmdModemoteRemoteModemInfoNotify;
						break;
					case ModemCmdSysModemVersionRemote:
						cmdInternal = CmdModemRemoteModemVersionNotify;
						break;
					case ModemCmdSysDeviceVersionRemote:
						cmdInternal = CmdModemRemoteDeviceVersionNotify;
						break;
					}
				}
				break;
			}

			// Если команда нашлась
			if (CmdUnknown != cmdInternal)
			{
				// выдаем результат наружу (наиболее глубоко вложенные данные)
				cmdData.Buffer = summaryInfo.Data[summaryInfo.NestingLevel - 1];
				cmdData.Length = summaryInfo.DataSize[summaryInfo.NestingLevel - 1];
			}

			return cmdInternal;
		}

	};

}
}
}

#endif // COMMAND_CONVERTER_FIRTWIG2_H
