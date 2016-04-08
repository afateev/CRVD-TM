///////////////////////////////////////////////////////////////////////////////
//	Формат пакетов и константы уровня протокола
//	Протокол FirTwig2
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
	/// \brief Адреса устройств, указываемые в пакете в полях Dest и Source.
	enum DeviceAddress
	{
		/// ПК.
		AddrPc		= 0x00,
		/// Внешнее устройство.
		AddrExt		= 0x01,
		/// Модем.
		AddrModem	= 0x02,
		/// Любое устройство.
		AddrAll		= 0x0F
	};
	
	/// \brief Перечисление наборов команд, зависящих от типа устройства.
	enum DevCommandSet
	{
		/// Набор команд по умолчанию, когда тип устройства неизвестен.
		DevCommandSetDefault,
		/// Набор команд модема.
		DevCommandSetModem,
		/// Набор команд РС-У.
		DevCommandSetRsu,
		/// Набор команд РС-ИК.
		DevCommandSetRsIr,
		/// Набор команд передатчика РС-Л
		DevCommandSetRslTransmitter,
		/// Набор команд РС-ТВ
		DevCommandSetRsTv
	};

	/// \brief Список команд модема.
	enum ModemCmd
	{
		/// Пинг.
		ModemCmdPing		= 0x00,
		/// Запрос версии ПО.
		ModemCmdWhoAreYou	= 0x01,
		/// Данные принятые по радиоканалу (UCMD_DATA).
		ModemCmdData		= 0x08,
		/// Запрос версии ПО.
		ModemCmdText	= 0x09,
		/// Системная информация (UCMD_SYS).
		ModemCmdSys			= 0x0A,
		/// Отправить данные в эфир, адресат по умолчанию (UCMD_SENDDEF).
		ModemCmdSendDef		= 0x10,
		/// Отправить данные в эфир, на указанный адрес (UCMD_SENDADR).
		ModemCmdSendAddr	= 0x11,
		/// Команда модему (UCMD_DO).
		ModemCmdDo			= 0x12,
		/// Тестовые команды модема (UCMD_TEST)
		ModemCmdTest		= 0x13
	};

	/// \brief Список команд ВУ общий для всех устройств.
	enum DevCmd
	{
		/// Проверка связи.
		DevCmdPing				= 0x00,
		/// Запрос версии прошивки устройства.
		DevCmdGetFirmwareVer	= 0x01,
		/// Установка скорости обмена по порту.
		DevCmdSetBaudRate		= 0x02,
		/// Группа системных команд
		DevCmdSystemControl		= 0x03,
		/// Диагностическая информация устройства.
		DevCmdLogData			= 0x05,
		/// Включение / выключение сенсора.
		DevCmdSetSensorOnOff	= 0x07,
		/// Ответ на DevCmdGetFirmwareVer.
		DevCmdFirmwareVerNotify	= 0x08,
		/// Запрос текущего формата вывода служебных данных.
		DevCmdGetOutputMode		= 0x17,
		/// Установка текущего формата вывода служебных данных.
		DevCmdSetOutputMode		= 0x18,
		/// Ответ на DevCmdGetOutputMode.
		DevCmdOutputModeNotify	= 0x19,
		/// Отладочные данные.
		DevCmdOutData			= 0x1A,
		/// Запрос состояния сенсора.
		DevCmdGetSensSate		= 0x1B,
		/// Состояние сенсора.
		DevCmdSensStateNotify	= 0x1C,
		/// Группа дополнительных команд.
		DevCmdExtended			= 0x1E
	};

	/// \brief Список подкоманд группы DevCmdSystemControl
	enum DevCmdGroupSystemControl
	{
		/// Запрос информации о состоянии системы
		DevCmdSysCtrlGetSystemInfo						= 0x01,
		/// Ответ на DevCmdSysCtrlGetSystemInfo
		DevCmdSysCtrlSystemInfoNotify					= 0x81,
		/// Запрос типа подключенной батареи
		DevCmdSysCtrlBattaryGetType						= 0x10,
		/// Ответ на DevCmdSysCtrlBattaryGetType
		DevCmdSysCtrlBattaryTypeNotify					= 0x90,
		/// Запрос оставшегося времени работы от батареи
		DevCmdSysCtrlBattaryGetReminingLifeTime			= 0x11,
		/// Ответ на DevCmdSysCtrlBattaryGetReminingLifeTime
		DevCmdSysCtrlBattaryReminingLifeTimeNotify		= 0x91,
		/// Запрос общей информации о батареи
		DevCmdSysCtrlBattaryGetInfo						= 0x12,
		/// Ответ на DevCmdSysCtrlBattaryGetInfo
		DevCmdSysCtrlBattaryInfoNotify					= 0x92,
		/// Запрос состояния батареи
		DevCmdSysCtrlBattaryGetState					= 0x13,
		/// Ответ на DevCmdSysCtrlBattaryGetState
		DevCmdSysCtrlBattaryStateNotify					= 0x93,
		/// Запрос времени наработки
		DevCmdSysCtrlBattaryGetOperatingTime			= 0x14,
		/// Ответ на DevCmdSysCtrlBattaryGetOperatingTime
		DevCmdSysCtrlBattaryOperatingTimeNotify			= 0x94,
		/// Запрос размера списка потребителей
		DevCmdSysCtrlBattaryGetPowerConsumersCount		= 0x15,
		/// Ответ на DevCmdSysCtrlBattaryGetPowerConsumersCount
		DevCmdSysCtrlBattaryPowerConsumersCountNotify	= 0x95,
		/// Запрос списка потребителей
		DevCmdSysCtrlBattaryGetPowerConsumersList		= 0x16,
		/// Ответ на DevCmdSysCtrlBattaryGetPowerConsumersList
		DevCmdSysCtrlBattaryPowerConsumersListNotify	= 0x96,
		/// Запрос счетчиков перезагрузоп ПО батареи
		DevCmdSysCtrlBattaryGetResetCounters			= 0x17,
		/// Ответ на DevCmdSysCtrlBattaryGetResetCounters
		DevCmdSysCtrlBattaryResetCountersNotify			= 0x97,
		/// Запрос периода отправки уведомений об оставшемся времени работы от батареи
		DevCmdSysCtrlGetRemainingLifeTimeReportPeriod		= 0x18,
		/// Ответ на CmdBattaryGetRemainingLifeTimeReportPeriod
		DevCmdSysCtrlRemainingLifeTimeReportPeriodNotify	= 0x98,
		/// Установка периода отправки уведомений об оставшемся времени работы от батареи
		DevCmdSysCtrlSetRemainingLifeTimeReportPeriod		= 0x19,
		// Запрос версии ПО видео-модема					
		DevCmdSysCtrlVmGetSwVer								= 0x02,
		// Данные с версией ПО
		DevCmdSysCtrlVmSwVerNotify							= 0x03,
		// Установка номера устройства в сети УниНет
		DevCmdSysCtrlVmSetDeviceNumber 						= 0x04,
		// Чтение номера устройства в сети					
		DevCmdSysCtrlVmGetDeviceNumber 						= 0x05,
		// Данные с номером устройства в сети УниНет
		DevCmdSysCtrlVmDeviceNumberNotify 					= 0x06,
		// Установка уровня устройства в сети УниНет
		DerCmdSysCtrlVmSetNetLevel							= 0x07,
		// Чтение уровня устройства в сети УниНет
		DerCmdSysCtrlVmGetNetLevel							= 0x08,		
		// Данные с уровнем устройства в сети УниНет
		DerCmdSysCtrlVmNetLevelNotify						= 0x09,		
		// Установка выходной мощности передатчика радиомодуля
		DerCmdSysCtrlVmSetTxPower							= 0x0A,		
		// Чтение выходной мощности передатчика радиомодуля
		DerCmdSysCtrlVmGetTxPower							= 0x0B,		
		// Данные с выходной мощностью передатчика радиомодуля
		DerCmdSysCtrlVmTxPowerNotify						= 0x0C,		
		// Чтение статуса видео-модема
		DerCmdSysCtrlVmGetDeviceStatus						= 0x0D,		
		// Данные со статусом видео-модема
		DerCmdSysCtrlVmDeviceStatusNotify					= 0x0E,		
		// Команд сохранения и применения сделанных изменений
		DerCmdSysCtrlVmApplyChanges							= 0x0F,		
		// Команда отмены сделанных изменений
		DerCmdSysCtrlVmDiscardChanges						= 0x20		
	};

	/// \brief Список подкоманд группы DevCmdExtended.
	enum DevCmdGroupExtended
	{
		/// Сетевое время.
		DevCmdExtNetTimeNotify		= 0x01,
		/// Уведомление о выключении питания.
		DevCmdExtPowerOffNotify		= 0x02,
		/// Запрос географических координат.
		DevCmdExtGetCoord			= 0x03,
		/// Устанновка географических координат.
		DevCmdExtSetCoord			= 0x04,
		/// Ответ на DevCmdGetCoord.
		DevCmdExtCoordNotify		= 0x05,
		/// Запрос состояния загрузчика (в загрузчике или нет устройство).
		DevCmdExtGetLoaderState = 0x06,
		/// Установка состояния загрузчик.
		DevCmdExtSetLoaderState = 0x07,
		/// Ответ на DevCmdExtGetLoaderState.
		DevCmdExtGetLoaderStateNotify = 0x08,
		/// Начать загрузку.
		DevCmdExtStartLoadeFirmaware = 0x09,
		/// Ответ на DevCmdExtStartLoadeFirmaware.
		DevCmdExtStartLoadeFirmawareNotify = 0x0A,
		/// Секция данных для записи во флеш.
		DevCmdExtFirmawareSection = 0x0B,
		/// Завершение отправки прошивки
		DevCmdExtFinishLoadFirmware = 0x0C,
		/// Ответ на DevCmdExtFinishLoadFirmware
		DevCmdExtFinishLoadFirmwareNotify = 0x0D,
		/// Проверить все ли пакетына данном этапе риняты.
		DevCmdExtCheckLoadingStage = 0x0E,
    /// Ответ на DevCmdExtCheckLoadingStage
		DevCmdExtCheckLoadingStageNotify = 0x0F,
		///Включить режим наблюдения для РСВ
		DevCmdExtSetRsvSupervisorMode = 0x10,
		///Запросить режим наблюдения для РСВ
		DevCmdExtGetRsvSupervisorMode = 0x11,
		///Ответ на DevCmdExtGetRsvSupervisorMode
		DevCmdExtRsvSupervisorModeNotify = 0x12,
		///Задать период автоматического самотестирования
		DevCmdExtSetSelfTestPeriod = 0x13,
		///Запросить период автоматического самотестирования
		DevCmdExtGetSelfTestPeriod = 0x14,
		///Ответ на DevCmdExtSetSelfTestPeriod
		DevCmdExtSelfTestPeriodNotify = 0x15,
		///Запустить автоматическую настройку порогов
		DevCmdExtStartCalibration = 0x16,
		///Результат калибровки
		DevCmdExtCalibrationNotify = 0x17,
		/// Запрос географических координат направления.
		DevCmdExtGetCoordDirection			= 0x18,
		/// Устанновка географических координат направления.
		DevCmdExtSetCoordDirection			= 0x19,
		/// Ответ на DevCmdGetCoordDirection.
		DevCmdExtCoordDirectionNotify		= 0x1A,

	};

	/// \brief Список команд РСУ.
	enum DevCmdRsu
	{
		/// Запрос текущей конфигурации РСУ.
		DevCmdRsuGetSensConfig		= 0x06,
		/// Установка текущей конфигурации РСУ.
		DevCmdRsuSetSensConfig		= 0x07,
		/// Ответ на DevCmdRsuGetSensConfig.
		DevCmdRsuSensConfigNotify	= 0x08,
		/// Группа команд настройки сейсмической части.
		DevCmdRsuSeismicParamEx		= 0x09,
		/// Группа команд настройки БУВ.
		DevCmdRsuExtDevParam		= 0x0B,
		/// Запрос времени работы БУВ.
		DevCmdRsuGetExtDevTime		= 0x0D,
		/// Установка времени работы БУВ.
		DevCmdRsuSetExtDevTime		= 0x0E,
		/// Ответ на DevCmdRsuGetExtDevTime.
		DevCmdRsuExtDevTimeNotify	= 0x0F,
		/// Группа команд управления БУВ.
		DevCmdRsuExtDevControl		= 0x10,
		/// Запрос параметров сейсмического датчика.
		DevCmdRsuGetSeismicParam	= 0x11,
		/// Установка параметров сейсмического датчика.
		DevCmdRsuSetSeismicParam	= 0x12,
		/// Ответ на DevCmdRsuGetSeismicParam.
		DevCmdRsuSeismicParamNotify	= 0x13,
		/// Группа команд для самотестирования.
		DevCmdRsuSelfTest			= 0x14,
		/// Результаты самотестирования (группа команд).
		DevCmdRsuTestResultNotify	= 0x15,
		/// Результаты самотестирования (группа команд).
		DevCmdRsuSeismicInveroment	= 0x1D
	};

	/// \brief Список подкоманд группы DevCmdRsuTestResultNotify.
	enum DevCmdRsuGroupTestResultNotify
	{
		DevCmdRsuTRNCheckNoise = 0x01,
		DevCmdRsuTRNCheckSensor = 0x02,
		DevCmdRsuTRNCheckResponse = 0x03,
		DevCmdRsuTRNCurrent = 0x022,
		DevCmdRsuTRNWirlessState = 0x31
	};

	// \brief Список подкоманд группы DevCmdRsuSelfTest.
	enum DevCmdGroupRsuSelfTest
	{
		DevCmdRsuSTCheckNoise			= 0x01,
		DevCmdRsuSTCheckSensor			= 0x02,
		DevCmdRsuSTCheckResponse		= 0x03,
		/// Запрос на самотестирование.
		DevCmdRsuSTCheckAll				= 0x04,
		/// Запрос подробных результатов самотестирования.
		DevCmdRsuSTGetCheckAllReport	= 0x05,
		/// Разрешение выдачи результатов самотестирования.
		DevCmdRsuSTCheckReportEnable	= 0x06,
		/// Запрещение выдачи результатов самотестирования.
		DevCmdRsuSTCheckReportDisable	= 0x07,
		/// Закоротить измерительный шунт.
		DevCmdRsuSTShortShunt			= 0x20,
		/// Открыть измерительный шунт.
		DevCmdRsuSTOpenShunt			= 0x21,
		/// Запрос тока потребления
		DevCmdRsuSTGetCurrent			= 0x22,
		/// Включить питание внешних геофонов
		DevCmdRsuSTExtAmplifierPowerOn	= 0x23,
		/// Выключить питание внешних геофонов
		DevCmdRsuSTExtAmplifierPowerOff	= 0x24,
		/// Включить подтягивающие резисторы I2C 0
		DevCmdRsuSTPullUpEnableI2C0		= 0x25,
		/// Выключить подтягивающие резисторы I2C 0
		DevCmdRsuSTPullUpDisableI2C0	= 0x26,
		/// Включить подтягивающие резисторы I2C 1
		DevCmdRsuSTPullUpEnableI2C1		= 0x27,
		/// Выключить подтягивающие резисторы I2C 1
		DevCmdRsuSTPullUpDisableI2C1	= 0x28,
		/// Включить вибромотор
		DevCmdRsuSTVibroOn				= 0x29,
		/// Выключить вибромотор
		DevCmdRsuSTVibroOff				= 0x2A,
		/// Разрешить работу селектора питания
		DevCmdRsuSTLBEnable				= 0x2B,
		/// Запретить работу селектора питания
		DevCmdRsuSTLBDisable			= 0x2C,
		/// Разрешить смену типа при неудачном завершении самотестирования
		DevCmdRsuSTChangeTypeEnable		= 0x2D,
		/// Запретить смену типа при неудачном завершении самотестирования
		DevCmdRsuSTChangeTypeDisable	= 0x2E,
		/// Запрос текущего состояния разрешения смены типа при неудачном завершении самотестирования
		DevCmdRsuSTGetChangeType		= 0x2F,
		/// Ответ на DevCmdRsuSTGetChangeType
		DevCmdRsuSTChangeTypeNotify		= 0x30
	};

	/// \brief Список подкоманд группы DevCmdRsuSeismicParamEx.
	enum DevCmdGroupRsuSeismicParamEx
	{
		/// Запрос полосы пропускания.
		DevCmdRsuSPGetBandwidth		= 0x01,
		/// Ответ на DevCmdRsuSPGetBandwidth.
		DevCmdRsuSPBandwidthNotify	= 0x81,
		/// Установка полосы пропускания.
		DevCmdRsuSPSetBandwidth		= 0x02,
		/// Запрос текущего коэффициента усиления.
		DevCmdRsuSPGetGain			= 0x03,
		/// Ответ на DevCmdRsuSPGetGain.
		DevCmdRsuSPGainNotify		= 0x83,
		/// Установка коэффициента усиления.
		DevCmdRsuSPSetGain			= 0x04,
		/// Запрос охраняемых секторов.
		DevCmdRsuSPGetProtectiveSectors		= 0x05,
		/// Ответ на DevCmdRsuSPGetProtectiveSectors.
		DevCmdRsuSPProtectiveSectorsNotify	= 0x85,
		/// Установка охраняемых секторов.
		DevCmdRsuSPSetProtectiveSectors		= 0x06,
		/// Запрос охраняемых секторов.
		DevCmdRsuSPGetRecognitionTimeSector		= 0x07,
		/// Ответ на DevCmdRsuSPGetRecognitionTimeSector.
		DevCmdRsuSPRecognitionTimeSectorNotify		= 0x87,
		/// Установка охраняемых секторов.
		DevCmdRsuSPSetRecognitionTimeSector			= 0x08,
	};

	/// \brief Список подкоманд группы DevCmdRsuExtDevParam.
	enum DevCmdGroupRsuExtDevParam
	{
		/// Запросить разрешение включать БУВ от собственного сенсора.
		DevCmdRsuEDPGetParam		= 0x01,
		/// Ответ на DevCmdRsuEDPGetParam.
		DevCmdRsuEDPParamNotify		= 0x81,
		/// Установка разрешения включать БУВ от собственного сенсора.
		DevCmdRsuEDPSetParam		= 0x02,
		/// Запрос флага блокировки включения модуля БУВ
		DevCmdRsuEDPGetSwitchOnPreventFlag	= 0x03,
		/// Установка флага блокировки включения модуля БУВ
		DevCmdRsuEDPSetSwitchOnPreventFlag	= 0x04,
	};

	/// \brief Список подкоманд группы DevCmdRsuSeismicInveroment.
	enum DevCmdGroupRsuSeismicInveroment
	{
		/// Запрос текущего уровня сигнала усредненного за несколько минут.
		DevCmdRsuSIGetSeismicNoise				= 0x01,
		/// Ответ на DevCmdRsuSIGetSeismicNoise.
		DevCmdRsuSISeismicNoiseNotify			= 0x81,
		
		/// Запрос опций детектора.
		DevCmdRsuSIGetSeismicAlarmOptions		= 0x05,
		/// Ответ на DevCmdRsuSIGetSeismicAlarmOptions.
		DevCmdRsuSISeismicAlarmOptionsNotify	= 0x85,
		/// Установка опций детектора.
		DevCmdRsuSISetSeismicAlarmOptions		= 0x06,
		
		/// Запрос порога срабатывания по транспортному средству.
		DevCmdRsuSIGetMashineLevel				= 0x07,
		/// Ответ на DevCmdRsuSIGetMashineLevel.
		DevCmdRsuSIMashineLevelNotify			= 0x87,
		/// Установка порога срабатывания по транспортному средству.
		DevCmdRsuSISetMashineLevel				= 0x08,

		/// Результат анализа сейсмодетектора.
		DevCmdRsuSISeismoResultlNotify			= 0x8A,

		/// Запрос порога пеленгации.
		DevCmdRsuSIGetPelengLevel				= 0x0B,
		/// Ответ на DevCmdRsuSIGetPelengLevel.
		DevCmdRsuSIPelengLevelNotify			= 0x8C,
		/// Установка порога пеленгации.
		DevCmdRsuSISetPelengLevel				= 0x0C,
		
		/// Уведомление о изменение параметров.
		DevCmdRsuConfigChangeNotify				= 0x0D,
		
		/// Установить запрет отправки азимутов неопределенного типа.
		DevCmdRsuSISetBanForAzimutIndefiniteType= 0x0E,
		/// Запрос флага запрета выдачи азимутов неопределенного типа
		DevCmdRsuSIGetBanForAzimutIndefiniteType= 0x0F,
		/// Ответ на CmdRsuGetBanForAzimutIndefiniteType
		DevCmdRsuSIBanForAzimutIndefiniteTypeNotify= 0x10,
			
		/// Запрос параметров жёлтых тревог.
		DevCmdRsuSIGetYellowAlarmParams			= 0x03,
		/// Ответ на запрос параметров желтых тревог.
		DevCmdRsuSIYellowAlarmParamsNotify		= 0x83,
		/// Установка параметров желтых тревог.
		DevCmdRsuSISetYellowAlarmParams			= 0x04,
		/// Жёлтая тревога.
		DevCmdRsuSIYellowAlarmNotify			= 0x82,
    /// Запрос alarm_flags
    DevCmdRsuSIGetAlarmFlags = 0x09,
    /// Ответ на DevCmdRsuSIGetAlarmFlags
    DevCmdRsuSIAlarmFlagsNotify = 0x89
	};

	/// Список подкоманд группы DevCmdRsuExtDev.
	enum DevCmdGroupRsuExtDev
	{
		/// Выключить БУВ.
		DevCmdRsuEDOff	= 0x00,
		/// Включить БУВ.
		DevCmdRsuEDOn	= 0x01,
	};

	/// \brief Список команд РС-ИК.
	enum DevCmdRsIr
	{
		/// Включение / выключение режима индикации тревоги с помощью светодиода.
		DevCmdRsIrSetLedOnOff				= 0x11,
		/// Установка пользовательского значения для порога
		DevCmdRsIrSetCustomThresh = 0x12,
		/// Установка уровня чувствительности.
		DevCmdRsIrSetSensivity				= 0x14,
		/// Запрос параметров "режим индикации тревоги с помощью светодиода" и "уровень чувствительности".
		DevCmdRsIrGetParams					= 0x15,
		/// Ответ на DevCmdRsIrGetParams.
		DevCmdRsIrParamsNotify				= 0x16,
		/// Запрос параметров алгоритма обработки сигнала.
		DevCmdRsIrGetAlgorithmParams		= 0x20,
		/// Установка параметров алгоритма обработки сигнала.
		DevCmdRsIrSetAlgorithmParams		= 0x21
	};

	/// \brief Список подкоманд группы ModemCmdSys.
	enum ModemCmdSysGroup
	{
		/// Данные о слышимости для определённого узла
		ModemCmdSysLinx = 0x11,
		/// Состояние своего модема.
		ModemCmdSysMyStatus		= 0x12,
		/// Маршрутная таблица
		ModemCmdSysRout = 0x15,
		/// Маршрутная таблица в режиме совместимости
		ModemCmdSysRout2 = 0x1B,
		/// Информация о статусе сесоров (в режиме совместимости)
		ModemCmdSysNetInfo = 0x1A,
		/// Запрос радиомодемом типа ВУ.
		ModemCmdSysAskXtyp		= 0x17,
		/// Состояние удадённого модема.
		ModemCmdSysRemoteModemStatus = 0x19,
		/// Версия удалённого модема.
		ModemCmdSysModemVersionRemote = 0x1F,
		/// Температура и напряжение питания.
		ModemCmdSysTempPower	= 0x20,
		/// Сетевое время.
		ModemCmdSysCTime		= 0x25,
		/// Версия удалённого устройства.
		ModemCmdSysDeviceVersionRemote = 0x26,
	};

	/// \brief Список подкоманд группы ModemCmdDo.
	enum ModemCmdDoGroup
	{
		/// Перевод модема в режим ожидания
		ModemCmdDoStbyMode = 0x01,
		/// Запрос состояния "своего" модема.
		ModemCmdDoShowStatus	= 0x02,
		/// Запрос маршрутной таблицы
		ModemCmdDoShowRoute	= 0x07,
		/// Установка типа ВУ модему.
		ModemCmdDoSetXtyp		= 0x08,
		/// Перевод модема в нормальный режим работы
		ModemCmdDoNormalMode = 0x09,
		/// Установка редима работы модема.
		ModemCmdDoSetMode = 0x0A,
		/// Запрос состояния удалённого модема.
		ModemCmdDoAskStatus	= 0x0C,
		/// Запрос версии удалённого модема
		ModemCmdDoGetModemVersionRemote = 0x0F,
		/// Запрос сетевого времени.
		ModemCmdDoGetCTime		= 0x1E,
		/// Запрос температуры и напряжения питания.
		ModemCmdDoGetTempPower	= 0x20,
		/// Запрос версии удалённого устройства
		ModemCmdDoGetDeviceVersionRemote = 0x23,
		/// Запрос статусов устройств сети (в режиме совместимости)
		ModemCmdDoGetNetInfo = 0x0E
	};

	/// \brief Список подкоманд группы ModemCmdTest.
	enum ModemCmdTest
	{
		/// Прервать тестирование
		ModemCmdTestStop = 0x00,
		/// Измерение температуры и напряжения питания
		ModemCmdTestMeasureVT	= 0x12,
	};

	/// \brief Список команд передатчика РС-Л.
	enum RslTrasmitterCmd
	{
		/// Группа команд передатчика РС-Л.
		// вот так вот получилось, что они все вложены в подкоманду
		RslTrasmitterCmdTransmitter			= 0x1E
	};

	/// \brief Список подкоманд группы RslTrasmitterCmd.
	enum RslTrasmitterCmdGroupTransmitter
	{
		/// Записать серийник в передатчик РС-Л
		RslTrasmitterCmdTransmitterSetSerial = 0x10,
		/// Прочитать серийник в передатчик РС-Л
		RslTrasmitterCmdTransmitterGetSerial = 0x11,
		/// Записать настройки сигнала передатчик РС-Л
		RslTrasmitterCmdTransmitterSetSignal = 0x12,
		/// Прочитать настройки сигнала передатчик РС-Л
		RslTrasmitterCmdTransmitterGetSignal = 0x13,
		/// Записать настройки ВЧ геню передатчик РС-Л
		RslTrasmitterCmdTransmitterSetHFConfig = 0x14,
		/// Прочитать настройки ВЧ геню передатчик РС-Л
		RslTrasmitterCmdTransmitterGetHFConfig = 0x15,
		/// Записать мощность ВЧ генератора передатчика РС-Л
		RslTrasmitterCmdTransmitterSetHFPowerAmplifier = 0x16,
		/// Прочитать мощность ВЧ генератора передатчика РС-Л
		RslTrasmitterCmdTransmitterGetHFPowerAmplifier = 0x17,
		/// Записать тип генерации (импульсный или непрерывный)
		RslTrasmitterCmdTransmitterSetTypeHFGeneration = 0x18,
		/// Прочитать тип генерации (импульсный или непрерывный)
		RslTrasmitterCmdTransmitterGetTypeHFGeneration = 0x19

	};

	/// \brief Список команд РС-ТВ.
	enum DevCmdRsTv
	{
		/// Запрос времени работы камеры.
		DevCmdRsTvGetWorkTime		= 0x0D,
		/// Установка времени работы камеры.
		DevCmdRsTvSetWorkTime		= 0x0E,
		/// Ответ на DevCmdRsTvGetWorkTime.
		DevCmdRsTvWorkTimeNotify	= 0x0F,
		/// Группа команд работы видеорегистратора
		DevCmdRsTvVrControl			= 0x09,
		/// Группа команд управления камерой.
		DevCmdRsTvWorkControl		= 0x10,
		/// Группа команд конфигурирования камеры.
		DevCmdRsTvCameraControl		= 0x16
	};

	/// Список подкоманд группы DevCmdRsTvVrControl.
	enum DevCmdGroupRsTvVrControl
	{
		/// Запрос статуса обобщенного видорегистратора
		DevCmdRsTvVrControlGetCommonStatus		= 0x00,
		/// Ответ на DevCmdRsTvVrControlGetCommonStatus
		DevCmdRsTvVrControlNotifyCommonStatus	= 0x01,
		/// Запрос статуса GSM видорегистратора
		DevCmdRsTvVrControlGetGsmStatus			= 0x02,
		/// Ответ на DevCmdRsTvVrControlGetGsmStatus
		DevCmdRsTvVrControlNotifyGsmStatus		= 0x03,
		/// Уведомление о старте ядра видеорегистратора
		DevCmdRsTvVrControlNotifyCoreStart		= 0x04,
		/// Уведомление об окончании записи
		DevCmdRsTvVrControlNotifyRecordComplete	= 0x05,
		/// Начать запись видео
		DevCmdRsTvVrControlStartRecord			= 0x06,
		/// Уведомление о старте записи видео
		DevCmdRsTvVrControlNotifyStartRecord	= 0x07,
		/// Завершить запись видео
		DevCmdRsTvVrControlStopRecord			= 0x08,
		/// Прочитать серийник из РС-ВР
		DevCmdRsTvVrControlGetSerial			= 0x09,
		/// Записать серийник в РС-ВР
		DevCmdRsTvVrControlSetSerial			= 0x10,
	};
	
	/// Список подкоманд группы DevCmdRsTvWorkControl.
	enum DevCmdGroupRsTvWorkControl
	{
		/// Выключить камеру.
		DevCmdRsTvWorkOff	= 0x00,
		/// Включить камеру.
		DevCmdRsTvWorkOn	= 0x01,
	};

	/// Список подкоманд группы DevCmdRsTvCameraControl.
	enum DevCmdGroupRsTvCameraControl
	{
		/// Запрос конфигурации камеры.
		DevCmdRsTvCameraControlGetConfig	= 0x01,
		/// Ответ на DevCmdRsTvCameraControlGetConfig
		DevCmdRsTvCameraControlNotigyConfig	= 0x81,
		/// Установка конфигурации камеры
		DevCmdRsTvCameraControlSetConfig	= 0x02,
	};

	/// Параметры команды ModemCmdDoSetMode.
	enum DevMode
	{
		DevModeDebugLog = 0x01,		// включение постоянного лога
		DevModeSHNews = 0x02,		//оповещение ВУ об изменении статуса и слышимости
		DevModeSHBad = 0x04,		//отображать в логе сбойные пакеты
		DevModeEcoDisable = 0x08,	//запрет экономичного режима приема
		DevModeOverHear = 0x20,		//пересылка на ВУ всех принятых пакетов
		DevModeRFProtect = 0x10,	//отключение линейного автомата статистики слыш.
		DevModeAntBad = 0x40,		//показывает обрыв антенны РМ (флаг только на чтение)
		DevModePeleng = 0x80		//включен или выключен режим пеленгации РМ (флаг только на чтение)
	};

	/// \brief заголовое команды UCMD_SENDADR.
	struct ModemSendAddrHeader
	{
		/// Сетевой адрес.
		unsigned char NetAddr;
		/// Способ отправки.
		unsigned char SendMode;
	};

	/// \brief Заголовок команды UCMD_DATA.
	struct ModemCmdDataHeader
	{
		/// Адрес получателя при последней ретрансляции (0..0x3F).
		unsigned char Retranslator;
		/// Адрес конечного получателя (0..0x3F).
		unsigned char Receiver;
		/// Сетевой адрес отправителя пакета.
		/// Может быть установлен 6ой бит (0x40) — признак пересылки (переадресован «ведущим» узлом).
		unsigned char Sender;
		/// Индекс пакета в реестре отправителя.
		unsigned char Index;
	};

	/// \brief Поле команды с Request и Ack.
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

	/// \brief Поле заголовка пакета.
	union HeaderField
	{
		struct
		{
			/// Адрес приемника.
			unsigned long Dest : 4;
			/// Адрес источника.
			unsigned long Source : 4;
			/// Код команды.
			unsigned long Cmd : 5;
			/// Код подтверждения.
			unsigned long Ack : 2;
			/// Флаг "Запрос"/"Ответ" (0 - запрос / уведомление, 1 - ответ).
			unsigned long Request : 1;
			/// Длина поля данных.
			unsigned long Len : 8;
			/// Контрольная сумма заголовка.
			unsigned long Crc : 8;
		};
		unsigned char Data[4];

		/// \brief Конструктор по-умолчанию.
		HeaderField()
		{
			for (unsigned char i = 0; i < sizeof(Data); i++)
				Data[i] = 0;
			UpdateCrc();
		}

		/// \brief Пересчитать контрольную сумму.
		void UpdateCrc()
		{
			Crc = 0x89;
			for (unsigned char i = 0; i < sizeof(Data) - 1; i++)
				Crc += Data[i];
		}

		/// \brief Установить адрес получателя.
		void SetDest(unsigned char value)
		{
			Dest = value;
			UpdateCrc();
		}

		/// \brief Установка адрес отправителя.
		void SetSource(unsigned char value)
		{
			Source = value;
			UpdateCrc();
		}

		/// \brief Установить код команды верхнего уровня.
		void SetCmd(unsigned char value)
		{
			Cmd = value;
			UpdateCrc();
		}

		/// \brief Флаг ошибки.
		void SetAck(unsigned char value)
		{
			Ack = value;
			UpdateCrc();
		}

		/// \brief Флаг запроса.
		void SetRequest(unsigned char value)
		{
			Request = value;
			UpdateCrc();
		}

		/// \brief Длина вложенных данных.
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
