///////////////////////////////////////////////////////////////////////////////
//	Строитель пакетов
//	Протокол FirTwig2
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETBUILDER_FIRTWIG2_H
#define PACKETBUILDER_FIRTWIG2_H

#include "PacketFormat.h"
#include "Parser.h"
#include "..\InternalCommands.h"
#include "RequestParams.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{

	///
	/// \brief Формирует пакеты в формате протокола FirTwig2 на основе кода команды внутреннего протокола.
	///
	class PacketBuilder
	{
	public:
		/// Максимальный размер пакета.
		static const unsigned int MaxPacketSize = 256;
	protected:

		/// Тип данных для поля CRC.
		typedef unsigned char CrcType;
		/// Буфер пакета.
		unsigned char _data[MaxPacketSize];
		/// Указатель на заголовок пакета.
		HeaderField *_header;
		/// Указатель на поле CRC
		CrcType *_packetCrc;
		/// Параметры по умолчанию (если не было запроса)
		Parser::PacketSummaryInfo _defaultParams;

	protected:

		/// \brief Параметры команды протокола FirTwig2.
		struct ExternalCmdInfo
		{
			/// Коды команд (массив по уровням вложенности).
			unsigned char Cmd[Parser::MaxNestingLevel];
			/// Уровень вложенности команд.
			unsigned char NestingLevel;
			/// Флаг запроса.
			bool Request;
			/// Флаг ошибки.
			unsigned char Ack;
			/// Источник.
			unsigned char Source;
			/// Получатель.
			unsigned char Dest;
			/// Флаг наличия обязательных вложенных данных.
			bool Data;
			/// Флаг команды модема.
			bool ModemCmd;
		};

	public:

		/// Конструктор по-умолчанию.
		PacketBuilder()
		{
			_header = (HeaderField *)_data;
			Reset();
		}

		void SetDefaultAddresses(DeviceAddress src, DeviceAddress dst)
		{
			_defaultParams.Source = src;
			_defaultParams.Dest = dst;
		}

		/// Сброс текущих параметров.
		void Reset()
		{
			ResetData();
			_packetCrc = 0;
			_header->UpdateCrc();
		}

		/// Получить параметры по умолчанию.
		/// \return Указатель на структуру параметров.
		Parser::PacketSummaryInfo* GetDefaultParams()
		{
			return &_defaultParams;
		}

		/// Установить адрес приемника.
		/// \param value Адрес получателя.
		void SetDest(unsigned char value)
		{
			_header->SetDest(value);
			UpdatePacketCrc();
		}

		/// Установить адрес источника.
		/// \param value Адрес источника.
		void SetSource(unsigned char value)
		{
			_header->SetSource(value);
			UpdatePacketCrc();
		}
		
		/// Установить код команды.
		/// \param value Код команды.
		void SetCmd(unsigned char value)
		{
			_header->SetCmd(value);
			UpdatePacketCrc();
		}

		/// Установить код подтверждения.
		/// \param value Флаг ошибки.
		void SetAck(unsigned char value)
		{
			_header->SetAck(value);
			UpdatePacketCrc();
		}

		/// Установить флаг запроса.
		/// \param value Флаг запроса.
		void SetRequest(unsigned char value)
		{
			_header->SetRequest(value);
			UpdatePacketCrc();
		}

		/// Установить длину поля данных.
		/// \param value Длина поля данных.
		void SetLen(unsigned char value)
		{
			_header->SetLen(value);
			UpdatePacketCrc();
		}

		/// Прикрепить данные (вложенные данные команды).
		/// \param data указатель на буфер с данные.
		/// \param dataSize Длина данных.
		void SetData(const unsigned char *data, unsigned int dataSize)
		{
			SetData(0, data, dataSize);
		}
		
		/// Прикрепить данные со cмещением без изменения данных левее offset.
		///	\param offset Смещение относительно начала секции данных пакета.
		/// \param data Указатель на буфер с данные.
		/// \param dataSize Длина данных.
		void SetData(unsigned int offset, const unsigned char *data, unsigned int dataSize)
		{
			// вычисляем сколько есть места под данные
			unsigned int emptySpace = MaxPacketSize - sizeof(HeaderField) - sizeof(CrcType) - offset;
			// сколько надо скопировать
			unsigned int copySize = dataSize;
			// если не влезает, то будем копировать сколько влезает
			if (emptySpace < copySize)
				copySize = emptySpace;	
			// прописываем в заголовок длину
			_header->SetLen(offset + copySize);
			// начало поля данных в буфере
			unsigned int start = sizeof(HeaderField);
			// конец данных
			unsigned int end = start + _header->Len;
			// т.е. положение контрольной суммы пакета
			_packetCrc = &_data[end];
			for (unsigned int i = 0; i < copySize; i ++)
				_data[i + start + offset] = data[i];
			// Пересчитываем контрольную сумму пакета
			UpdatePacketCrc();
		}

		/// Заполнить поля составной команды.
		/// \param cmd Массив кодов по уровням вложенности.
		/// \param nestingLevel Уровень вложенности команд.
		/// \param packetDataOffset Смещение секции данных пакета относительно стандартного положения (бывает > 0 в случае команд верхнего уровня таких, как UCMD_SENDADR).
		void SetCmd(unsigned char* cmd, unsigned char nestingLevel, unsigned int packetDataOffset = 0)
		{
			if (nestingLevel < 1)
				return;

			_header->Cmd = cmd[0];

			unsigned char packetDataIndex = sizeof(HeaderField) + packetDataOffset;
			for (unsigned char i = 1; i < nestingLevel; i++)
			{
				_data[packetDataIndex] = cmd[i];
				packetDataIndex++;
			}
			
			if (nestingLevel > 1)
				_packetCrc = &_data[sizeof(HeaderField) + nestingLevel - 1];

			UpdatePacketCrc();
		}

		/// Заполнить поля составной команды, не затрагивая поле команды верхнего уровня.
		/// \param cmd Массив кодов по уровням вложенности.
		/// \param nestingLevel Уровень вложенности команд.
		/// \param packetDataOffset Смещение секции данных пакета относительно стандартного положения (бывает > 0 в случае команд верхнего уровня таких, как UCMD_SENDADR).
		void SetSubCmd(unsigned char* cmd, unsigned char nestingLevel, unsigned int packetDataOffset = 0)
		{
			unsigned char dataIndex = sizeof(HeaderField) + packetDataOffset;

			for (unsigned char i = 0; i < nestingLevel; i++)
			{
				_data[dataIndex] = cmd[i];
				dataIndex++;
			}
			
			if (nestingLevel > 0)
				_packetCrc = &_data[sizeof(HeaderField) + packetDataOffset + nestingLevel];

			UpdatePacketCrc();
		}

		/// Устанавливает сетевой адрес получателя пакета.
		/// \param netAddress Сетевой адрес получателя.
		void SetNetAddress(unsigned char netAddress)
		{
			((ModemSendAddrHeader*)&_data[sizeof(HeaderField)])->NetAddr = netAddress;
			UpdatePacketCrc();
		}

		/// Задаёт режим отправки пакета.
		/// \param sendMode Режим отправки.
		void SetSendMode(unsigned char sendMode)
		{
			((ModemSendAddrHeader*)&_data[sizeof(HeaderField)])->SendMode = sendMode;
			UpdatePacketCrc();
		}

		/// Получить готовый пакет.
		/// \param [out] packetSize Размер пакета.
		/// \return Указатель на построенный пакет.
		unsigned char *GetPacket(unsigned int *packetSize)
		{
			unsigned int pSize = sizeof(HeaderField) + _header->Len;
			if (_header->Len > 0)
				pSize += sizeof(CrcType);
			if (0 != packetSize)
				*packetSize = pSize;
			return _data;
		}
	protected:
	  
		/// Очистить буфер пакета.
		void ResetData()
		{
			for (unsigned int i = 0; i < MaxPacketSize; i++)
				_data[i] = 0;
		}

		/// Пересчитать контрольную сумму всего пакета.
		void UpdatePacketCrc()
		{
			if (_header->Len < 1)
				return;
			if (0 == _packetCrc)
				return;
			unsigned int i = sizeof(HeaderField);
			unsigned int end = i + _header->Len;
			*_packetCrc = _header->Crc;
			for (; i < end; i ++)
				*_packetCrc += _data[i];
		}

		/// Конвертация внутренней команды в набор вложенных команд в формате FirTwig2.
		/// \param [in] cmd Информация о внутренней команде.
		/// \param [out] externalCmd Информация о команде в формате FirTwig2.
		/// \return @c true конвертация прошла успешно, @c false - ошибка.
		bool ConvertToExternal(CmdInfo *cmd, ExternalCmdInfo* externalCmd)
		{
			if (0 == cmd)
				return false;

			const Parser::PacketSummaryInfo* currentParams = GetCurrentCmdParams(cmd);

			externalCmd->ModemCmd = false;

			// разбираем команды и строим пакеты
			switch(cmd->Cmd)
			{
			case CmdDevicePong:
				externalCmd->Cmd[0] = DevCmdPing;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				externalCmd->ModemCmd = true;
			break;
			// Есть особая реализация для кабеля
			/* TODO а вот нормального варианта как оказалось нет, по радио спрашиваем версию ВУ у модема
			case CmdNotifySoftwareVersion:
				SetCmd(DevCmdFirmwareVerNotify);
				SetData(cmd->Data, cmd->DataSize);
				SetRequest(1);
			break;
			*/
			// По радио отправлять нельзя, только по кабелю
			// Есть особая реализация для кабеля
			case CmdSetBaudRateAck:
				return false;
			// По радио отправлять нельзя, только по кабелю
			// Есть особая реализация для кабеля
			case CmdSetBaudRateNak:
				return false;
			case CmdLogData:
				externalCmd->Cmd[0] = DevCmdLogData;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
			break;
#ifndef RBLIB_BOOTLOADER            
			// Есть особая реализация для кабеля
			case CmdNotifyDebugOutFormat:
				externalCmd->Cmd[0] = DevCmdOutputModeNotify;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
			break;
			case CmdSetDebugOutFormatAck:
				return false;
			case CmdSetDebugOutFormatNak:
				return false;
			case CmdDebugOut:
				externalCmd->Cmd[0] = DevCmdOutData;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
			break;
			case CmdModemSendData:
				externalCmd->Cmd[0] = ModemCmdSendDef;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 0;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
			break;

			case CmdAlarm:
				externalCmd->Cmd[0] = DevCmdSensStateNotify;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdNotifyAlarmState:
				externalCmd->Cmd[0] = DevCmdSensStateNotify;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdNotifyNetworkTime:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtNetTimeNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdNotifyPowerOff:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtPowerOffNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdNotifyCoordinates:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtCoordNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdNotifyCoordinatesDirection:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtCoordDirectionNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
      case CmdDeviceTypeNotify:
			case CmdRsuNotifyFunctionsList:
				externalCmd->Cmd[0] = DevCmdRsuSensConfigNotify;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSetFunctionsListAck:
				return false;
			case CmdRsuSetFunctionsListNak:
				return false;
			case CmdRsuNotifySeismicParam:
				externalCmd->Cmd[0] = DevCmdRsuSeismicParamNotify;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSetSeismicParamAck:
				return false;
			case CmdRsuSetSeismicParamNak:
				return false;
			case CmdRsuNotifyAvarageNoise:
				externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
				externalCmd->Cmd[1] = DevCmdRsuSISeismicNoiseNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuNotifyDetectOptions:
				externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
				externalCmd->Cmd[1] = DevCmdRsuSISeismicAlarmOptionsNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSetDetectOptionsAck:
				return false;
			case CmdRsuSetDetectOptionsNak:
				return false;
			case CmdRsuNotifyVehicleThreshold:
				externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
				externalCmd->Cmd[1] = DevCmdRsuSIMashineLevelNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSetVehicleThresholdAck:
				return false;
			case CmdRsuSetVehicleThresholdNak:
				return false;
			case CmdRsuNotifySeismoResult:
				return false;
			case CmdRsuNotifyPelengThreshold:
				externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
				externalCmd->Cmd[1] = DevCmdRsuSIPelengLevelNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuNotifyBanForAzimutIndefiniteType:
				externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
				externalCmd->Cmd[1] = DevCmdRsuSIBanForAzimutIndefiniteTypeNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuNotifyConfigChange:
				externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
				externalCmd->Cmd[1] = DevCmdRsuConfigChangeNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSetPelengThresholdAck:
				return false;
			case CmdRsuSetPelengThresholdNak:
				return false;
			case CmdRsuNotifyBandwidth:
				externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
				externalCmd->Cmd[1] = DevCmdRsuSPBandwidthNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSetBandwidthAck:
				return false;
			case CmdRsuSetBandwidthNak:
				return false;
			case CmdRsuNotifyGain:
				externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
				externalCmd->Cmd[1] = DevCmdRsuSPGainNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsIrSetCustomThresh:
				externalCmd->Cmd[0] = DevCmdRsIrSetCustomThresh;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
			break;
			case CmdRsIrSetCustomThreshAck:
				externalCmd->Cmd[0] = DevCmdRsIrSetCustomThresh;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
			break;
			case CmdRsIrSetCustomThreshNak:
				externalCmd->Cmd[0] = DevCmdRsIrSetCustomThresh;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 1;
			break;
			case CmdRsuSetGainAck:
				return false;
			case CmdRsuSetGainNak:
				return false;
			case CmdRsuProtectiveSectorsNotify:
				externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
				externalCmd->Cmd[1] = DevCmdRsuSPProtectiveSectorsNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSetProtectiveSectorsAck:
				return false;
			case CmdRsuSetProtectiveSectorsNak:
				return false;
			case CmdRsuRecognitionTimeSectorNotify:
				externalCmd->Cmd[0] = DevCmdRsuSeismicParamEx;
				externalCmd->Cmd[1] = DevCmdRsuSPRecognitionTimeSectorNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSetRecognitionTimeSectorAck:
				return false;
			case CmdRsuSetRecognitionTimeSectorNak:
				return false;

			case CmdRsIrSetLedOnOffAck:
				return false;
			case CmdRsIrSetLedOnOffNak:
				return false;
			case CmdRsIrSetSensivityAck:
				return false;
			case CmdRsIrSetSensivityNak:
				return false;
			case CmdRsIrNotifyParams:
				externalCmd->Cmd[0] = DevCmdRsIrParamsNotify;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsIrNotifyAlgorithmParams:
				return false;
			case CmdRsIrSetAlgorithmParamsAck:
				return false;
			case CmdRsIrSetAlgorithmParamsNak:
				return false;
			case CmdRsuNotifyYellowAlarmParams:
				externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
				externalCmd->Cmd[1] = DevCmdRsuSIYellowAlarmParamsNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuYellowAlarmNotify:
				externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
				externalCmd->Cmd[1] = DevCmdRsuSIYellowAlarmNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSelfTestNotify:
				externalCmd->Cmd[0] = DevCmdRsuTestResultNotify;
				externalCmd->Cmd[1] = DevCmdRsuSTCheckAll;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuNotifyWirelessState:
				externalCmd->Cmd[0] = DevCmdRsuTestResultNotify;
				externalCmd->Cmd[1] = DevCmdRsuTRNWirlessState;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuNotifyRelayTime:
				externalCmd->Cmd[0] = DevCmdRsuExtDevTimeNotify;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuNotifyRelayEnableByOwnSensor:
				externalCmd->Cmd[0] = DevCmdRsuExtDevParam;
				externalCmd->Cmd[1] = DevCmdRsuEDPParamNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			// Группа System Control
			case CmdNotifySystemInfo:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlSystemInfoNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdBattaryNotifyType:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlBattaryTypeNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdBattaryNotifyRemainingLifeTime:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlBattaryReminingLifeTimeNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdBattaryNotifyInfo:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlBattaryInfoNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdBattaryNotifyState:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlBattaryStateNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdBattaryNotifyOperatingTime:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlBattaryOperatingTimeNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdBattaryNotifyPowerConsumersCount:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlBattaryPowerConsumersCountNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdBattaryNotifyPowerConsumersList:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlBattaryPowerConsumersListNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdBattaryNotifyResetCounters:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlBattaryResetCountersNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdBattaryNotifyRemainingLifeTimeReportPeriod:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlRemainingLifeTimeReportPeriodNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			/// Команды видео-модема
			case CmdVmNotifyFirmwareVersion:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlVmSwVerNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;

			case CmdVmSetDeviceNumberAck:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlVmSetDeviceNumber;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdVmSetDeviceNumberNak:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlVmSetDeviceNumber;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 1;
				break;
				
			case CmdVmNotifyDeviceNumber:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DevCmdSysCtrlVmDeviceNumberNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;			
				
			case CmdVmSetNetLevelAck:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DerCmdSysCtrlVmSetNetLevel;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
				
			case CmdVmSetNetLevelNak:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DerCmdSysCtrlVmSetNetLevel;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 1;
				break;
				
			case CmdVmNotifyNetLevel:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DerCmdSysCtrlVmNetLevelNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;				
				
			case CmdVmSetTxPowerAck:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DerCmdSysCtrlVmSetTxPower;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
				
			case CmdVmSetTxPowerNak:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DerCmdSysCtrlVmSetTxPower;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 1;
				break;				
				
			case CmdVmNotifyTxPower:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DerCmdSysCtrlVmTxPowerNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;					
				
			case CmdVmNotifyDeviceStatus:
				externalCmd->Cmd[0] = DevCmdSystemControl;
				externalCmd->Cmd[1] = DerCmdSysCtrlVmDeviceStatusNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;				
				
			// команды РС-ТВ
			case CmdRsTvNotifyWorkTime:
				externalCmd->Cmd[0] = DevCmdRsTvWorkTimeNotify;
				externalCmd->NestingLevel = 1;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsTvNotifyConfig:
				externalCmd->Cmd[0] = DevCmdRsTvCameraControl;
				externalCmd->Cmd[1] = DevCmdRsTvCameraControlNotigyConfig;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsTvVrNotifyCommonStatus:
				externalCmd->Cmd[0] = DevCmdRsTvVrControl;
				externalCmd->Cmd[1] = DevCmdRsTvVrControlNotifyCommonStatus;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsTvVrNotifyGsmStatus:
				externalCmd->Cmd[0] = DevCmdRsTvVrControl;
				externalCmd->Cmd[1] = DevCmdRsTvVrControlNotifyGsmStatus;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
				
#endif // RBLIB_BOOTLOADER
			case CmdSetLoaderState:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtSetLoaderState;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdGetLoaderState:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtGetLoaderState;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdGetLoaderStateNotify:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtGetLoaderStateNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdStartLoadFirmware:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtStartLoadeFirmaware;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdStartLoadFirmwareNotify:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtStartLoadeFirmawareNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdFirmwareSection:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtFirmawareSection;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdFinishLoadFirmware:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtFinishLoadFirmware;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdFinishLoadFirmwareNotify:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtFinishLoadFirmwareNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdCheckLoadingStage:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtCheckLoadingStage;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdCheckLoadingStageNotify:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtCheckLoadingStageNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
#ifndef RBLIB_BOOTLOADER
			case CmdSetRsvSupervisorMode:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtSetRsvSupervisorMode;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdGetRsvSupervisorMode:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtGetRsvSupervisorMode;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdGetRsvSupervisorModeAck:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtGetRsvSupervisorMode;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdGetRsvSupervisorModeNak:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtGetRsvSupervisorMode;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 1;
				break;
			case CmdRsvSupervisorModeNotify:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtRsvSupervisorModeNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdSetRsvSupervisorModeAck:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtSetRsvSupervisorMode;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdSetRsvSupervisorModeNak:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtSetRsvSupervisorMode;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 1;
				break;

			case CmdRsuSelfTestSetPeriod:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtSetSelfTestPeriod;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSelfTestSetPeriodAck:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtSetSelfTestPeriod;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSelfTestSetPeriodNak:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtSetSelfTestPeriod;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 1;
				break;

			case CmdRsuSelfTestGetPeriod:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtGetSelfTestPeriod;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSelfTestGetPeriodAck:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtGetSelfTestPeriod;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;
			case CmdRsuSelfTestGetPeriodNak:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtGetSelfTestPeriod;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 1;
				break;

			case CmdRsuSelfTestPeriodNotify:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtSelfTestPeriodNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;

			case CmdRsuStartCalibration:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtStartCalibration;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;

			case CmdRsuStartCalibrationNak:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtStartCalibration;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 1;
				break;

			case CmdRsuStartCalibrationAck:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtStartCalibration;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 1;
				externalCmd->Data = false;
				externalCmd->Ack = 0;
				break;

			case CmdRsuCalibrationNotify:
				externalCmd->Cmd[0] = DevCmdExtended;
				externalCmd->Cmd[1] = DevCmdExtCalibrationNotify;
				externalCmd->NestingLevel = 2;
				externalCmd->Request = 0;
				externalCmd->Data = true;
				externalCmd->Ack = 0;
				break;

			case CmdBattarySetRemainingLifeTimeReportPeriodAck:
				return false;
			case CmdBattarySetRemainingLifeTimeReportPeriodNak:
				return false;
#endif // RBLIB_BOOTLOADER
			default:
				return false;
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

		/// Возвращает параметры внутренней команды (берёт из запроса или подставляет стандартные)
		/// \param [in] cmd Информация о внутренней команде
		/// \return Структура параметров команды
		inline const Parser::PacketSummaryInfo* GetCurrentCmdParams(CmdInfo *cmd)
		{
			if (0 == cmd->RequestParams)
			{
				return &_defaultParams;
			}
			else
			{
				if (CheckRequestProtocol(cmd) == false)
					return 0;
				return &((RequestParams*)cmd->RequestParams)->PacketInfo;
			}
		}
		
		/// Проверяет наличие параметров запроса во внутренней команде.
		/// \param [in] cmd Информация о внутренней команде.
		/// \return @c true, если есть запрос, @c false, если нет.
		static inline bool CheckRequestParams(CmdInfo *cmd)
		{
		  return (0 != cmd->RequestParams);
		}

		/// Проверить, что протокол запроса - FirTwig2
		/// \param [in] cmd Информация о внутренней команде.
		/// \return @c true, если протокол правильный, @c false, если нет.
		static inline bool CheckRequestProtocol(CmdInfo *cmd)
		{
			return cmd->RequestParams->Protocol == ProtocolFirTwig2;
		}
		
	};

}
}
}

#endif
