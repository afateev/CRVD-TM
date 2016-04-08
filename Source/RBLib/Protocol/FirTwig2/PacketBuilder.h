///////////////////////////////////////////////////////////////////////////////
//	��������� �������
//	�������� FirTwig2
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
	/// \brief ��������� ������ � ������� ��������� FirTwig2 �� ������ ���� ������� ����������� ���������.
	///
	class PacketBuilder
	{
	public:
		/// ������������ ������ ������.
		static const unsigned int MaxPacketSize = 256;
	protected:

		/// ��� ������ ��� ���� CRC.
		typedef unsigned char CrcType;
		/// ����� ������.
		unsigned char _data[MaxPacketSize];
		/// ��������� �� ��������� ������.
		HeaderField *_header;
		/// ��������� �� ���� CRC
		CrcType *_packetCrc;
		/// ��������� �� ��������� (���� �� ���� �������)
		Parser::PacketSummaryInfo _defaultParams;

	protected:

		/// \brief ��������� ������� ��������� FirTwig2.
		struct ExternalCmdInfo
		{
			/// ���� ������ (������ �� ������� �����������).
			unsigned char Cmd[Parser::MaxNestingLevel];
			/// ������� ����������� ������.
			unsigned char NestingLevel;
			/// ���� �������.
			bool Request;
			/// ���� ������.
			unsigned char Ack;
			/// ��������.
			unsigned char Source;
			/// ����������.
			unsigned char Dest;
			/// ���� ������� ������������ ��������� ������.
			bool Data;
			/// ���� ������� ������.
			bool ModemCmd;
		};

	public:

		/// ����������� ��-���������.
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

		/// ����� ������� ����������.
		void Reset()
		{
			ResetData();
			_packetCrc = 0;
			_header->UpdateCrc();
		}

		/// �������� ��������� �� ���������.
		/// \return ��������� �� ��������� ����������.
		Parser::PacketSummaryInfo* GetDefaultParams()
		{
			return &_defaultParams;
		}

		/// ���������� ����� ���������.
		/// \param value ����� ����������.
		void SetDest(unsigned char value)
		{
			_header->SetDest(value);
			UpdatePacketCrc();
		}

		/// ���������� ����� ���������.
		/// \param value ����� ���������.
		void SetSource(unsigned char value)
		{
			_header->SetSource(value);
			UpdatePacketCrc();
		}
		
		/// ���������� ��� �������.
		/// \param value ��� �������.
		void SetCmd(unsigned char value)
		{
			_header->SetCmd(value);
			UpdatePacketCrc();
		}

		/// ���������� ��� �������������.
		/// \param value ���� ������.
		void SetAck(unsigned char value)
		{
			_header->SetAck(value);
			UpdatePacketCrc();
		}

		/// ���������� ���� �������.
		/// \param value ���� �������.
		void SetRequest(unsigned char value)
		{
			_header->SetRequest(value);
			UpdatePacketCrc();
		}

		/// ���������� ����� ���� ������.
		/// \param value ����� ���� ������.
		void SetLen(unsigned char value)
		{
			_header->SetLen(value);
			UpdatePacketCrc();
		}

		/// ���������� ������ (��������� ������ �������).
		/// \param data ��������� �� ����� � ������.
		/// \param dataSize ����� ������.
		void SetData(const unsigned char *data, unsigned int dataSize)
		{
			SetData(0, data, dataSize);
		}
		
		/// ���������� ������ �� c�������� ��� ��������� ������ ����� offset.
		///	\param offset �������� ������������ ������ ������ ������ ������.
		/// \param data ��������� �� ����� � ������.
		/// \param dataSize ����� ������.
		void SetData(unsigned int offset, const unsigned char *data, unsigned int dataSize)
		{
			// ��������� ������� ���� ����� ��� ������
			unsigned int emptySpace = MaxPacketSize - sizeof(HeaderField) - sizeof(CrcType) - offset;
			// ������� ���� �����������
			unsigned int copySize = dataSize;
			// ���� �� �������, �� ����� ���������� ������� �������
			if (emptySpace < copySize)
				copySize = emptySpace;	
			// ����������� � ��������� �����
			_header->SetLen(offset + copySize);
			// ������ ���� ������ � ������
			unsigned int start = sizeof(HeaderField);
			// ����� ������
			unsigned int end = start + _header->Len;
			// �.�. ��������� ����������� ����� ������
			_packetCrc = &_data[end];
			for (unsigned int i = 0; i < copySize; i ++)
				_data[i + start + offset] = data[i];
			// ������������� ����������� ����� ������
			UpdatePacketCrc();
		}

		/// ��������� ���� ��������� �������.
		/// \param cmd ������ ����� �� ������� �����������.
		/// \param nestingLevel ������� ����������� ������.
		/// \param packetDataOffset �������� ������ ������ ������ ������������ ������������ ��������� (������ > 0 � ������ ������ �������� ������ �����, ��� UCMD_SENDADR).
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

		/// ��������� ���� ��������� �������, �� ���������� ���� ������� �������� ������.
		/// \param cmd ������ ����� �� ������� �����������.
		/// \param nestingLevel ������� ����������� ������.
		/// \param packetDataOffset �������� ������ ������ ������ ������������ ������������ ��������� (������ > 0 � ������ ������ �������� ������ �����, ��� UCMD_SENDADR).
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

		/// ������������� ������� ����� ���������� ������.
		/// \param netAddress ������� ����� ����������.
		void SetNetAddress(unsigned char netAddress)
		{
			((ModemSendAddrHeader*)&_data[sizeof(HeaderField)])->NetAddr = netAddress;
			UpdatePacketCrc();
		}

		/// ����� ����� �������� ������.
		/// \param sendMode ����� ��������.
		void SetSendMode(unsigned char sendMode)
		{
			((ModemSendAddrHeader*)&_data[sizeof(HeaderField)])->SendMode = sendMode;
			UpdatePacketCrc();
		}

		/// �������� ������� �����.
		/// \param [out] packetSize ������ ������.
		/// \return ��������� �� ����������� �����.
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
	  
		/// �������� ����� ������.
		void ResetData()
		{
			for (unsigned int i = 0; i < MaxPacketSize; i++)
				_data[i] = 0;
		}

		/// ����������� ����������� ����� ����� ������.
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

		/// ����������� ���������� ������� � ����� ��������� ������ � ������� FirTwig2.
		/// \param [in] cmd ���������� � ���������� �������.
		/// \param [out] externalCmd ���������� � ������� � ������� FirTwig2.
		/// \return @c true ����������� ������ �������, @c false - ������.
		bool ConvertToExternal(CmdInfo *cmd, ExternalCmdInfo* externalCmd)
		{
			if (0 == cmd)
				return false;

			const Parser::PacketSummaryInfo* currentParams = GetCurrentCmdParams(cmd);

			externalCmd->ModemCmd = false;

			// ��������� ������� � ������ ������
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
			// ���� ������ ���������� ��� ������
			/* TODO � ��� ����������� �������� ��� ��������� ���, �� ����� ���������� ������ �� � ������
			case CmdNotifySoftwareVersion:
				SetCmd(DevCmdFirmwareVerNotify);
				SetData(cmd->Data, cmd->DataSize);
				SetRequest(1);
			break;
			*/
			// �� ����� ���������� ������, ������ �� ������
			// ���� ������ ���������� ��� ������
			case CmdSetBaudRateAck:
				return false;
			// �� ����� ���������� ������, ������ �� ������
			// ���� ������ ���������� ��� ������
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
			// ���� ������ ���������� ��� ������
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
			// ������ System Control
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
			/// ������� �����-������
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
				
			// ������� ��-��
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

		/// ���������� ��������� ���������� ������� (���� �� ������� ��� ����������� �����������)
		/// \param [in] cmd ���������� � ���������� �������
		/// \return ��������� ���������� �������
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
		
		/// ��������� ������� ���������� ������� �� ���������� �������.
		/// \param [in] cmd ���������� � ���������� �������.
		/// \return @c true, ���� ���� ������, @c false, ���� ���.
		static inline bool CheckRequestParams(CmdInfo *cmd)
		{
		  return (0 != cmd->RequestParams);
		}

		/// ���������, ��� �������� ������� - FirTwig2
		/// \param [in] cmd ���������� � ���������� �������.
		/// \return @c true, ���� �������� ����������, @c false, ���� ���.
		static inline bool CheckRequestProtocol(CmdInfo *cmd)
		{
			return cmd->RequestParams->Protocol == ProtocolFirTwig2;
		}
		
	};

}
}
}

#endif
