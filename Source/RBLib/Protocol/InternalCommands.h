///////////////////////////////////////////////////////////////////////////////
//	Внутренняя система команд
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
	// Внутренний список команд
	enum CmdInternal
	{
		CmdUnknown,						// Неизвестная команда
		CmdDevicePing,					// Ping устройства
		CmdDevicePong,					// ответ на Ping от устройства
		CmdModemPing,					// Ping модема
		CmdModemPongNormalMode,			// ответ на Ping от модема в нормальном режиме
		CmdModemPongStandbyMode,		// ответ на Ping от модема в режиме ожидания
		CmdGetSoftwareVersion,			// Запрос версии ПО
		CmdNotifySoftwareVersion,		// Ответ на CmdGetSoftwareVersion
		CmdSetBaudRate,					// Установка скорости обмена по порту
		CmdSetBaudRateAck,				// Ответ "успешно" на CmdSetBaudRate
		CmdSetBaudRateNak,				// Ответ "отказ" на CmdSetBaudRate
		CmdLogData,						// Диагностическая информация устройства
		CmdGetDebugOutFormat,			// Запрос текущего формата вывода служебных данных
		CmdNotifyDebugOutFormat,		// Ответ на CmdGetDebugOutFormat
		CmdSetDebugOutFormat,			// Установка текущего формата вывода служебных данных
		CmdSetDebugOutFormatAck,		// Ответ "успешно" на CmdSetDebugOutFormat
		CmdSetDebugOutFormatNak,		// Ответ "отказ" на CmdSetDebugOutFormat
		CmdDebugOut,					// Отладочные данные
		CmdAlarm,						// Тревога
		CmdGetAlarmState,				// Запрос текущего состояния сенсора
		CmdNotifyAlarmState,			// Ответ на CmdGetAlarmState
		CmdNotifyNetworkTime,			// Текущее сетевое время (используется для автономного регистратора и т.п.)
		CmdNotifyPowerOff,				// Уведомление о выключении питания
		CmdGetCoordinates,				// Запрос географических координат
		CmdNotifyCoordinates,			// Ответ на CmdGetCoordinates
		CmdSetCoordinates,				// Устанновка географических координат
		CmdSetCoordinatesAck,			// Ответ "успешно" на CmdSetCoordinates
		CmdSetCoordinatesNak,			// Ответ "отказ" на CmdSetCoordinates
		CmdGetCoordinatesDirection,		// Запрос географических координат направления
		CmdNotifyCoordinatesDirection,	// Ответ на CmdGetCoordinatesDirection
		CmdSetCoordinatesDirection,		// Устанновка географических координат направления
		CmdSetCoordinatesDirectionAck,	// Ответ "успешно" на CmdSetCoordinatesDirection
		CmdSetCoordinatesDirectionNak,	// Ответ "отказ" на CmdSetCoordinatesDirection
		
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
		
		CmdSetLoaderState,				// Переключение загрузчик/пользовательская программа
		CmdGetLoaderState,				// Запрос статуса загрузчика (что работает: загрузчик или пользовательская программа)
		CmdGetLoaderStateNotify,        // Ответ на CmdGetLoaderState
		CmdStartLoadFirmware,           // Начать отправку прошивки
		CmdStartLoadFirmwareNotify,     // Ответ СmdStartLoadFirmware
		CmdFirmwareSection,				// Кусок прошивки
		CmdFinishLoadFirmware,          // Конец загрузки
		CmdFinishLoadFirmwareNotify,    // Ответ на CmdFinishLoadFirmware
		CmdCheckLoadingStage,			// проверка все ли пакеты на данном жтапе были приняты
		CmdCheckLoadingStageNotify,		// ответ на CmdCheckLoadingStage

		CmdSetRsvSupervisorMode,
		CmdGetRsvSupervisorMode,
		CmdGetRsvSupervisorModeAck,
		CmdGetRsvSupervisorModeNak,
		CmdRsvSupervisorModeNotify,
		CmdSetRsvSupervisorModeAck,
		CmdSetRsvSupervisorModeNak,

		CmdRsuGetFunctionsList,			// Запрос текущей конфигурации РСУ
		CmdRsuNotifyFunctionsList,		// Ответ на CmdRsuGetFunctionsList
		CmdRsuSetFunctionsList,			// Установка текущей конфигурации РСУ
		CmdRsuSetFunctionsListAck,		// Ответ "успешно" на CmdRsuSetFunctionsList
		CmdRsuSetFunctionsListNak,		// Ответ "отказ" на CmdRsuSetFunctionsList
		CmdRsuGetSeismicParam,			// Запрос группы параметров (коэффициент усиления, порог срабатывания по пешеходу) сейсмического датчика
		CmdRsuNotifySeismicParam,		// Ответ на CmdRsuGetSeismicParam
		CmdRsuSetSeismicParam,			// Установка группы параметров (коэффициент усиления, порог срабатывания по пешеходу) сейсмического датчика
		CmdRsuSetSeismicParamAck,		// Ответ "успешно" на CmdRsuSetSeismicParam
		CmdRsuSetSeismicParamNak,		// Ответ "отказ" на CmdRsuSetSeismicParam
		CmdRsuGetAvarageNoise,			// Запрос текущего уровня сигнала усредненного за несколько минут
		CmdRsuNotifyAvarageNoise,		// Ответ на CmdRsuGetAvarageNoise
		CmdRsuGetDetectOptions,			// Запрос опций детектора (разрешение / запрет отдельных модулей детектора)
		CmdRsuNotifyDetectOptions,		// Ответ на CmdRsuGetDetectOptions
		CmdRsuSetDetectOptions,			// Установка опций работы детектора
		CmdRsuSetDetectOptionsAck,		// Ответ "успешно" на CmdRsuSetDetectOptions
		CmdRsuSetDetectOptionsNak,		// Ответ "отказ" на CmdRsuSetDetectOptions
		CmdRsuGetVehicleThreshold,		// Запрос порога срабатывания по транспортному средству
		CmdRsuNotifyVehicleThreshold,	// Ответ на CmdRsuGetVehicleThreshold
		CmdRsuSetVehicleThreshold,		// Установка порога срабатывания по транспортному средству
		CmdRsuSetVehicleThresholdAck,	// Ответ "успешно" на CmdRsuSetVehicleThreshold
		CmdRsuSetVehicleThresholdNak,	// Ответ "отказ" на CmdRsuSetVehicleThreshold
		CmdRsuNotifySeismoResult,		// Результат анализа сейсмодетектора
		CmdRsuGetPelengThreshold,		// Запрос порога пеленгации
		CmdRsuNotifyPelengThreshold,	// Ответ на CmdRsuGetPelengThreshold
		CmdRsuSetPelengThreshold,		// Установка порога срабатывания по транспортному средству
		CmdRsuSetPelengThresholdAck,	// Ответ "успешно" на CmdRsuSetPelengThreshold
		CmdRsuSetPelengThresholdNak,	// Ответ "отказ" на CmdRsuSetPelengThreshold
		CmdRsuNotifyConfigChange,		// Уведомление о изменениях
		CmdRsuGetBandwidth,				// Запрос текущей полосы пропускания
		CmdRsuNotifyBandwidth,			// Ответ на CmdRsuGetBandwidth
		CmdRsuSetBandwidth,				// Установка полосы пропускания
		CmdRsuSetBandwidthAck,			// Ответ "успешно" на CmdRsuSetBandwidth
		CmdRsuSetBandwidthNak,			// Ответ "отказ" на CmdRsuSetBandwidth
		CmdRsuGetGain,					// Запрос текущего коэффициента усиления
		CmdRsuNotifyGain,				// Ответ на CmdRsuGetGain
		CmdRsuSetGain,					// Установка коэффициента усиления
		CmdRsuSetGainAck,				// Ответ "успешно" на CmdRsuSetGain
		CmdRsuSetGainNak,				// Ответ "отказ" на CmdRsuSetGain
		CmdRsuSetBanForAzimutIndefiniteType,	// Установить запрет отправки азимутов неопределенного типа.
		CmdRsuSetBanForAzimutIndefiniteTypeAck,	// Ответ "успешно" на CmdRsuSetBanForAzimutIndefiniteType
		CmdRsuSetBanForAzimutIndefiniteTypeNak,	// Ответ "отказ"  на CmdRsuSetBanForAzimutIndefiniteType
		CmdRsuGetBanForAzimutIndefiniteType,	// Запрос флага запрета выдачи азимутов неопределенного типа
		CmdRsuNotifyBanForAzimutIndefiniteType,	// Ответ на CmdRsuGetBanForAzimutIndefiniteType
		CmdRsuGetProtectiveSectors,		// Запрос флагов охраняемых секторов
		CmdRsuProtectiveSectorsNotify,	// Ответ на CmdRsuGetProtectiveSectors
		CmdRsuSetProtectiveSectors,		// Запрос на установку секторов под охрану
		CmdRsuSetProtectiveSectorsAck,	// Ответ "успешно" на CmdRsuSetProtectiveSectors
		CmdRsuSetProtectiveSectorsNak,	// Ответ "отказ" на CmdRsuSetProtectiveSectors
		CmdRsuGetRecognitionTimeSector,		// Запрос времени определения сектора
		CmdRsuRecognitionTimeSectorNotify,	// Ответ на CmdRsuGetRecognitionTimeSector
		CmdRsuSetRecognitionTimeSector,		// Запрос на установку времени определения сектора
		CmdRsuSetRecognitionTimeSectorAck,	// Ответ "успешно" на CmdRsuSetRecognitionTimeSector
		CmdRsuSetRecognitionTimeSectorNak,	// Ответ "отказ" на CmdRsuSetRecognitionTimeSector

		CmdRsuGetYellowAlarmParams,		// Запрос параметров жёлтых тревог
		CmdRsuNotifyYellowAlarmParams,	// Ответ на запрос параметров жёлтых тревог
		CmdRsuSetYellowAlarmParams,		// Установка параметров жёлтых тревог
		CmdRsuSetYellowAlarmParamsAck,	// Ответ "успешно" на установку параметров жёлтых тревог
		CmdRsuSetYellowAlarmParamsNak,	// Ответ "отказ" на установку параметров жёлтых тревог
		CmdRsuYellowAlarmNotify,		// Жёлтая тревога

		CmdRsuTestAmplifierNoise,		// Самотестирование 1
		CmdRsuTestAmplifierNoiseAck,	// Ответ "успешно" на CmdRsuTestAmplifierNoise
		CmdRsuTestAmplifierNoiseNak,	// Ответ "отказ" на CmdRsuTestAmplifierNoise
		CmdRsuTestAmplifierNoiseNotify, // Результат теста CmdRsuTestAmplifierNoise
		CmdRsuTestRelativeDelay,		// Самотестирование 2
		CmdRsuTestRelativeDelayAck,		// Ответ "успешно" на CmdRsuTestRelativeDelay
		CmdRsuTestRelativeDelayNak,		// Ответ "отказ" на CmdRsuTestRelativeDelay
		CmdRsuTestRelativeDelayNotify,	// Результат теста CmdRsuTestRelativeDelay
		CmdRsuTestAmplifierResponse,	// Самотестирование 3
		CmdRsuTestAmplifierResponseAck,	// Ответ "успешно" на CmdRsuTestAmplifierResponse
		CmdRsuTestAmplifierResponseNak,	// Ответ "отказ" на CmdRsuTestAmplifierResponse
		CmdRsuTestAmplifierResponseNotify, // Результат теста CmdRsuTestAmplifierResponse
		CmdRsuSelfTest,					// Запрос на самотестирование
		CmdRsuSelfTestAck,				// Ответ "успешно" на CmdRsuSelfTest
		CmdRsuSelfTestNak,				// Ответ "отказ" на CmdRsuSelfTest
		CmdRsuSelfTestNotify,			// Результаты CmdRsuSelfTest

		CmdRsuSelfTestSetPeriod,		// Установить период запуска автоматического самотестирования
		CmdRsuSelfTestSetPeriodAck,
		CmdRsuSelfTestSetPeriodNak,
		CmdRsuSelfTestGetPeriod,		// Запросить период запуска автоматического самотестирования
		CmdRsuSelfTestGetPeriodAck,
		CmdRsuSelfTestGetPeriodNak,
		CmdRsuSelfTestPeriodNotify,

		CmdRsuGetSelfTestReport,		// Запрос подробных результатов самотестирования
		CmdRsuNotifySelfTestReport,		// Ответ на CmdRsuGetSelfTestReport
		CmdRsuSetTestReportEnable,		// Разрешение выдачи результатов самотестирования
		CmdRsuSetTestReportEnableAck,	// Ответ "успешно" на CmdRsuSetTestReportEnable
		CmdRsuSetTestReportEnableNak,	// Ответ "отказ" на CmdRsuSetTestReportEnable
		CmdRsuSetTestReportDisable,		// Запрещение выдачи результатов самотестирования
		CmdRsuSetTestReportDisableAck,	// Ответ "успешно" на CmdRsuSetTestReportDisable
		CmdRsuSetTestReportDisableNak,	// Ответ "отказ" на CmdRsuSetTestReportDisable
		CmdRsuNotifyWirelessState,		// Уведомление о состоянии беспроводной гарнитуры

		CmdRsuSetShuntShorted,			// Закоротить измерительный шунт
		CmdRsuSetShuntShortedAck,		// Ответ "успешно" на CmdRsuSetShuntShorted
		CmdRsuSetShuntShortedNak,		// Ответ "отказ" на CmdRsuSetShuntShorted
		CmdRsuSetShuntOpen,				// Открыть измерительный шунт
		CmdRsuSetShuntOpenAck,			// Ответ "успешно" на
		CmdRsuSetShuntOpenNak,			// Ответ "отказ" на 

		CmdRsuTestCurrent,					// Запрос тока потребления
		CmdRsuTestCurrentAck,				// Ответ "успешно" на CmdRsuTestCurrent
		CmdRsuTestCurrentNak,				// Ответ "отказ" на CmdRsuTestCurrent
		CmdRsuTestCurrentNotify,			// Результаты CmdRsuTestCurrent
		CmdRsuTestExtAmplifierPowerOn,		// Включить питание внешних геофонов
		CmdRsuTestExtAmplifierPowerOnAck,	// Ответ "успешно" на CmdRsuTestExtAmplifierPowerOn
		CmdRsuTestExtAmplifierPowerOnNak,	// Ответ "отказ" на CmdRsuTestExtAmplifierPowerOn
		CmdRsuTestExtAmplifierPowerOff,		// Выключить питание внешних геофонов
		CmdRsuTestExtAmplifierPowerOffAck,	// Ответ "успешно" на CmdRsuTestExtAmplifierPowerOff
		CmdRsuTestExtAmplifierPowerOffNak,	// Ответ "отказ" на CmdRsuTestExtAmplifierPowerOff
		CmdRsuTestI2C0PullUpEnable,			// Включить подтягивающие резисторы I2C 0
		CmdRsuTestI2C0PullUpEnableAck,		// Ответ "успешно" на CmdRsuTestI2C0PullUpEnable
		CmdRsuTestI2C0PullUpEnableNak,		// Ответ "отказ" на CmdRsuTestI2C0PullUpEnable
		CmdRsuTestI2C0PullUpDisable,		// Включить подтягивающие резисторы I2C 0
		CmdRsuTestI2C0PullUpDisableAck,		// Ответ "успешно" на CmdRsuTestI2C0PullUpDisable
		CmdRsuTestI2C0PullUpDisableNak,		// Ответ "отказ" на CmdRsuTestI2C0PullUpDisable
		CmdRsuTestI2C1PullUpEnable,			// Включить подтягивающие резисторы I2C 1
		CmdRsuTestI2C1PullUpEnableAck,		// Ответ "успешно" на CmdRsuTestI2C1PullUpEnable
		CmdRsuTestI2C1PullUpEnableNak,		// Ответ "отказ" на CmdRsuTestI2C1PullUpEnable
		CmdRsuTestI2C1PullUpDisable,		// Включить подтягивающие резисторы I2C 1
		CmdRsuTestI2C1PullUpDisableAck,		// Ответ "успешно" на CmdRsuTestI2C1PullUpDisable
		CmdRsuTestI2C1PullUpDisableNak,		// Ответ "отказ" на CmdRsuTestI2C1PullUpDisable
		CmdRsuTestVibroOn,					// Включить вибромотор
		CmdRsuTestVibroOnAck,				// Ответ "успешно" на CmdRsuTestVibroOn
		CmdRsuTestVibroOnNak,				// Ответ "отказ" на CmdRsuTestVibroOn
		CmdRsuTestVibroOff,					// Выключить вибромотор
		CmdRsuTestVibroOffAck,				// Ответ "успешно" на CmdRsuTestVibroOff
		CmdRsuTestVibroOffNak,				// Ответ "отказ" на CmdRsuTestVibroOff
		CmdRsuTestLBEnable,					// Разрешить работу селектора питания
		CmdRsuTestLBEnableAck,				// Ответ "успешно" на CmdRsuTestLBEnable
		CmdRsuTestLBEnableNak,				// Ответ "отказ" на CmdRsuTestLBEnable
		CmdRsuTestLBDisable,				// Запретить работу селектора питания
		CmdRsuTestLBDisableAck,				// Ответ "успешно" на CmdRsuTestLBDisable
		CmdRsuTestLBDisableNak,				// Ответ "отказ" на CmdRsuTestLBDisable
		CmdRsuSelfTestChangeTypeEnable,		// Разрешить смену типа при неудачном завершении самотестирования
		CmdRsuSelfTestChangeTypeEnableAck,	// Ответ "успешно" на CmdRsuSelfTestChangeTypeEnable
		CmdRsuSelfTestChangeTypeEnableNak,	// Ответ "отказ" на CmdRsuSelfTestChangeTypeEnable
		CmdRsuSelfTestChangeTypeDisable,	// Запретить смену типа при неудачном завершении самотестирования
		CmdRsuSelfTestChangeTypeDisableAck,	// Ответ "успешно" на CmdRsuSelfTestChangeTypeDisable
		CmdRsuSelfTestChangeTypeDisableNak,	// Ответ "отказ" на CmdRsuSelfTestChangeTypeDisable
		CmdRsuSelfTestGetChangeType,		// Запрос текущего состояния разрешения смены типа при неудачном завершении самотестирования
		CmdRsuNotifySelfTestChangeType,		// Ответ на CmdRsuGetSelfTestChangeType

		CmdRsuSetRelayOn,				// Включение БУВ
		CmdRsuSetRelayOnNak,			// Ответ "отказ" на CmdRsuSetRelayOn
		CmdRsuSetRelayOff,				// Выключение БУВ
		CmdRsuSetRelayOffNak,			// Ответ "отказ" на CmdRsuSetRelayOff
		CmdRsuGetRelayTime,				// Запрос времени работы БУВ
		CmdRsuNotifyRelayTime,			// Ответ на CmdRsuGetRelayTime
		CmdRsuSetRelayTime,				// Установка времени работы БУВ
		CmdRsuSetRelayTimeAck,			// Ответ "успешно" на CmdRsuSetRelayTime
		CmdRsuSetRelayTimeNak,			// Ответ "отказ" на CmdRsuSetRelayTime
		CmdRsuGetRelayEnableByOwnSensor,	// Запрос разрешения включать БУВ от собственного сенсора
		CmdRsuNotifyRelayEnableByOwnSensor,	// Ответ на CmdRsuGetRelayEnableByOwnSensor
		CmdRsuSetRelayEnableByOwnSensor,	// Установка разрешения включать БУВ от собственного сенсора
		CmdRsuSetRelayEnableByOwnSensorAck,	// Ответ "успешно" на CmdRsuSetRelayEnableByOwnSensor
		CmdRsuSetRelayEnableByOwnSensorNak,	// Ответ "отказ" на CmdRsuSetRelayEnableByOwnSensor
		CmdRsuGetRelayModuleSwitchOnPreventFlag,	// Запрос флага блокировки включения модуля БУВ
		CmdRsuNotifyRelayModuleSwitchOnPreventFlag, // Ответ на CmdRsuGetRelayModuleSwitchOnPreventFlag
		CmdRsuSetRelayModuleSwitchOnPreventFlag,	// Установка флага блокировки включения модуля БУВ
		CmdRsuSetRelayModuleSwitchOnPreventFlagAck,	// Ответ "успешно" на CmdRsuSetRelayModuleSwitchOnPreventFlag
		CmdRsuSetRelayModuleSwitchOnPreventFlagNak,	// Ответ "отказ" на CmdRsuSetRelayModuleSwitchOnPreventFlag
    	CmdRsuGetAlarmFlags, // Запрос флагов состояния сейсмического алгоритма
    	CmdRsuAlarmFlagsNotify, // Ответ на CmdRsuGetAlarmFlags
		
		CmdRsuStartCalibration, // Запуск процедуры калибровки (автоматическая настройка порогов)
		CmdRsuStartCalibrationAck, // Ответ на команду CmdRsuStartCalibration - успешно
		CmdRsuStartCalibrationNak, // Ответ на команду CmdRsuStartCalibration - ошибка
		CmdRsuCalibrationNotify, // Результат калибровки

		CmdModemInputData,				// Приняты данные
		CmdModemSendData,				// Отправить данные на адрес по умолчанию
		CmdModemAskType,				// Запрос типа внешнего устройства
		CmdModemSetType,				// Установить тип внешнего устройства в модеме
		CmdModemGetTempAndPower,		// Запрос температуры и напряжения питания с модема
		CmdModemNotifyTempAndPower,		// Ответ на CmdModemGetTempAndPower
		CmdModemGetSelfInfo,			// Запрос состояния "своего" модема
		CmdModemNotifySelfInfo,			// Ответ на CmdModemGetMyInfo, или отправка информации о модеме со стороны датчика
		CmdModemGetNetTime,				// Запрос сетевого времени с модема
		CmdModemNotifyNetTime,			// Ответ на CmdModemGetNetTime
		CmdModemNotifyExtDevSoftwareVersion,	// сообщить модему версию ПО внешнего устройства
		CmdModemNotifyRoute,
		CmdModemNotifyRoute2,
		CmdModemGetNetInfo,
		CmdModemNotifyNetInfo,
		CmdModemGetRoute,
		CmdModemSetMode,
		CmdModemNotifyLinx,
		CmdModemGetRemoteModemInfo,			// Запрос состояния удёлённого модема
		CmdModemoteRemoteModemInfoNotify,	// Состояния удёлённого модема
		CmdModemGetRemoteModemVersion,
		CmdModemGetRemoteDeviceVersion,
		CmdModemRemoteModemVersionNotify,
		CmdModemRemoteDeviceVersionNotify,
		CmdModemSetStandbyMode,				// Перевод модема в режим ожидания
		CmdModemSetNormalMode,				// Перевод модема в нормальный режим работы
		CmdModemGetVersion,					// Запрос верси ПО модема
		CmdModemVersionNotify,				// Ответ на CmdModemGetVersion
		CmdModemTestStop,					// Прервать тестирование
		CmdModemTestMeasureTempAndVoltage,	// Измерение температры и напряжения питания

		CmdSetSensorOnOff,				// Включение / выключение сенсора
		CmdRsIrSetLedOnOff,				// Включение / выключение режима индикации тревоги с помощью светодиода.
		CmdRsIrSetLedOnOffAck,			// Ответ "успешно" на CmdRsIrSetLedOnOff
		CmdRsIrSetLedOnOffNak,			// Ответ "отказ" на CmdRsIrSetLedOnOff
		CmdRsIrSetCustomThresh, // Установка пользовтельского значения для порога
		CmdRsIrSetCustomThreshAck, // Ответ на CmdRsIrSetCustomThresh (успех)
		CmdRsIrSetCustomThreshNak, // Ответ на CmdRsIrSetCustomThresh (ошибка)
		CmdRsIrSetSensivity,			// Установка уровня чувствительности
		CmdRsIrSetSensivityAck,			// Ответ "успешно" на CmdRsIrSetSensivity
		CmdRsIrSetSensivityNak,			// Ответ "отказ" на CmdRsIrSetSensivity
		CmdRsIrGetParams,				// Запрос параметров "режим индикации тревоги с помощью светодиода" и "уровень чувствительности"
		CmdRsIrNotifyParams,			// Ответ на CmdRsIrGetParams
		CmdRsIrGetAlgorithmParams,		// Запрос параметров алгоритма обработки сигнала
		CmdRsIrNotifyAlgorithmParams,	// Ответ на CmdRsIrGetAlgorithmParams
		CmdRsIrSetAlgorithmParams,		// Установка параметров алгоритма обработки сигнала
		CmdRsIrSetAlgorithmParamsAck,	// Ответ "успешно" на CmdRsIrSetAlgorithmParams
		CmdRsIrSetAlgorithmParamsNak,	// Ответ "отказ" на CmdRsIrSetAlgorithmParams
		CmdGetSystemInfo,				// Запрос информации о состоянии системы
		CmdNotifySystemInfo,			// Ответ на CmdGetSystemInfo
    CmdGetDeviceType, // Запрос типа для всех датчиков, кроме РСУ (GET_SENS_CONFIG)
    CmdDeviceTypeNotify, // Ответ на CmdGetDeviceType
		CmdBattaryGetType,				// Запрос типа подключенной батареи
		CmdBattaryNotifyType,			// Ответ на CmdBattaryGetType
		CmdBattaryGetRemainingLifeTime,		// Запрос оставшегося времени работы от батареи
		CmdBattaryNotifyRemainingLifeTime,	// Ответ на CmdBattaryGetRemainingLifeTime
		CmdBattaryGetInfo,				// Запрос общей информации о батареи
		CmdBattaryNotifyInfo,			// Ответ на CmdBattaryGetInfo
		CmdBattaryGetState,				// Запрос состояния батареи
		CmdBattaryNotifyState,			// Ответ на CmdBattaryGetState
		CmdBattaryGetOperatingTime,		// Запрос времени наработки
		CmdBattaryNotifyOperatingTime,	// Ответ на CmdBattaryGetOperatingTime
		CmdBattaryGetPowerConsumersCount,		// Запрос размера списка потребителей
		CmdBattaryNotifyPowerConsumersCount,	// Ответ на CmdBattaryGetPowerConsumersCount
		CmdBattaryGetPowerConsumersList,		// Запрос списка потребителей
		CmdBattaryNotifyPowerConsumersList,		// Ответ на CmdBattaryGetPowerConsumersList
		CmdBattaryGetResetCounters,				// Запрос счетчиков перезагрузоп ПО батареи
		CmdBattaryNotifyResetCounters,			// Ответ на CmdBattaryGetResetCounters
		CmdBattaryGetRemainingLifeTimeReportPeriod,		// Запрос периода отправки уведомений об оставшемся времени работы от батареи
		CmdBattaryNotifyRemainingLifeTimeReportPeriod,	// Ответ на CmdBattaryGetRemainingLifeTimeReportPeriod
		CmdBattarySetRemainingLifeTimeReportPeriod,		// Установка периода отправки уведомений об оставшемся времени работы от батареи
		CmdBattarySetRemainingLifeTimeReportPeriodAck,	// Ответ "успешно" на CmdBattarySetRemainingLifeTimeReportPeriod
		CmdBattarySetRemainingLifeTimeReportPeriodNak,	// Ответ "отказ" на CmdBattarySetRemainingLifeTimeReportPeriod

		CmdRslTransmitterSetSerial,				// Записать серийник в передатчик РС-Л
		CmdRslTransmitterSetSerialAck,			// Ответ "успешно" на CmdRslTransmitterSetSerial
		CmdRslTransmitterSetSerialNak,			// Ответ "отказ" на CmdRslTransmitterSetSerial
		CmdRslTransmitterGetSerial,				// Прочитать серийник в передатчик РС-Л
		CmdRslTransmitterNotifySerial,			// Ответ на CmdRslTransmitterGetSerial
		CmdRslTransmitterSetSignalConfig,		// Записать настройки сигнала передатчик РС-Л
		CmdRslTransmitterSetSignalConfigAck,	// Ответ "успешно" на CmdRslTransmitterSetSignalConfig
		CmdRslTransmitterSetSignalConfigNak,	// Ответ "отказ" на CmdRslTransmitterSetSignalConfig
		CmdRslTransmitterGetSignalConfig,		// Прочитать настройки сигнала передатчик РС-Л
		CmdRslTransmitterNotifySignalConfig,	// Ответ на CmdRslTransmitterGetSignalConfig
		CmdRslTransmitterSetHFConfig,			// Записать настройки ВЧ геню передатчик РС-Л
		CmdRslTransmitterSetHFConfigAck,		// Ответ "успешно" на CmdRslTransmitterSetHFConfig
		CmdRslTransmitterSetHFConfigNak,		// Ответ "отказ" на CmdRslTransmitterSetHFConfig
		CmdRslTransmitterGetHFConfig,			// Прочитать настройки ВЧ геню передатчик РС-Л
		CmdRslTransmitterNotifyHFConfig,		// Ответ на CmdRslTransmitterGetHFConfig
		CmdRslTransmitterSetHFPowerAmplifier,	// Записать мощность ВЧ генератора передатчика РС-Л
		CmdRslTransmitterSetHFPowerAmplifierAck,// Ответ "успешно" на CmdRslTransmitterSetHFPower
		CmdRslTransmitterSetHFPowerAmplifierNak,// Ответ "отказ" на CmdRslTransmitterSetHFPower
		CmdRslTransmitterGetHFPowerAmplifier,	// Прочитать мощность ВЧ генератора передатчика РС-Л
		CmdRslTransmitterNotifyHFPowerAmplifier,// Ответ на CmdRslTransmitterGetHFPower
		CmdRslTransmitterSetTypeHFGeneration,	// Записать тип генерации (импульсный или непрерывный)
		CmdRslTransmitterSetTypeHFGenerationAck,// Ответ "успешно" на CmdRslTransmitterSetTypeHFGeneration
		CmdRslTransmitterSetTypeHFGenerationNak,// Ответ "отказ" на CmdRslTransmitterSetTypeHFGeneration
		CmdRslTransmitterGetTypeHFGeneration,	// Прочитать тип генерации (импульсный или непрерывный)
		CmdRslTransmitterNotifyTypeHFGeneration,// Ответ на CmdRslTransmitterGetTypeHFGeneration


		CmdRsTvGetWorkTime,						// Запрос времени работы камеры
		CmdRsTvNotifyWorkTime,					// Ответ на CmdRsTvGetWorkTime
		CmdRsTvSetWorkTime,						// Установка времени работы камеры
		CmdRsTvSetWorkTimeAck,					// Ответ "успешно" на CmdRsTvSetWorkTime
		CmdRsTvSetWorkTimeNak,					// Ответ "отказ" на CmdRsTvSetWorkTime
		CmdRsTvSetOn,							// Включить камеру
		CmdRsTvSetOnNak,						// Ответ "отказ" на CmdRsTvSetOn
		CmdRsTvSetOff,							// Включить камеру
		CmdRsTvSetOffNak,						// Ответ "отказ" на CmdRsTvSetOff
		CmdRsTvGetConfig,						// Запрос конфигурации камеры
		CmdRsTvNotifyConfig,					// Ответ на CmdRsTvGetConfig
		CmdRsTvSetConfig,						// Установка конфигурации камеры
		CmdRsTvSetConfigAck,					// Ответ "успешно" на CmdRsTvSetConfig
		CmdRsTvSetConfigNak,					// Ответ "отказ" на CmdRsTvSetConfig 
		CmdRsTvVrGetCommonStatus,					// Запрос статуса обобщенного видорегистратора
		CmdRsTvVrNotifyCommonStatus,			// Ответ на CmdRsTvVrGetCommonStatus
		CmdRsTvVrGetGsmStatus,					// Запрос статуса GSM видорегистратора
		CmdRsTvVrNotifyGsmStatus,				// Ответ на CmdRsTvVrGetGsmStatus
		CmdRsTvVrNotifyCoreStart,				// Уведомление о старте ядра видеорегистратора
		CmdRsTvVrNotifyRecordComplete,			// Уведомление об окончании записи
		CmdRsTvVrStartRecord,					// старт записи видео
		CmdRsTvVrNotifyStartRecord,				// Уведомление о старте записи видео
		CmdRsTvVrStopRecord,					// завершить запись видео
		CmdRsTvVrGetSerial,						// Прочитать серийник из РС-ВР
		CmdRsTvVrNotifySerial,					// Ответ на CmdRsTvVrGetSerial
		CmdRsTvVrSetSerial,						// записать серийник в РС-ВР
		CmdRsTvVrSetSerialAck,					// Ответ "успешно" на CmdRsTvVrSetSerial
		CmdRsTvVrSetSerialNak					// Ответ "отказ" на CmdRsTvVrSetSerial
	};

	// Описатель команды
	struct CmdInfo
	{
		CmdInternal Cmd;			// Код команды
		const unsigned char * Data;		// Данные связанные с командой
		unsigned int DataSize;		// Размер данных
		bool Handled;				// Команда была обработана
		const RequestParamsBase* RequestParams;
		DispatchParamsBase* DispatchParams;
		// Конструктор по-умолчанию
		CmdInfo() : Cmd(CmdUnknown), Data(0), DataSize(0), Handled(false), DispatchParams(0), RequestParams(0)
		{}
		// Конструктор по коду команды
		CmdInfo(CmdInternal cmd) : Cmd(cmd), Data(0), DataSize(0), Handled(false), DispatchParams(0), RequestParams(0)
		{}
		// Конструктор по коду команды и данным
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
