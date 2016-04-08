///////////////////////////////////////////////////////////////////////////////
//	Строитель пакетов
//	Протокол FirTwig2 специальный для ответов по кабелю
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETBUILDERFORMODEM_FIRTWIG2_H
#define PACKETBUILDERFORMODEM_FIRTWIG2_H

#include "PacketBuilder.h"
#include "DispatchParams.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{
/// Класс построителя пакетов для отправки в эфир (модем).
class PacketBuilderForModem : public PacketBuilder
{
public:

	/// Конструктор по умолчанию.
	PacketBuilderForModem()
	{
		_defaultParams.Source = AddrExt;
		_defaultParams.Dest = AddrModem;
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
			return false;

		if (cmd->DispatchParams && cmd->DispatchParams->Protocol != ProtocolFirTwig2)
			return false;
		DispatchParams* dispatchParams = (DispatchParams*)cmd->DispatchParams;

		ExternalCmdInfo externalCmd;

		if (ConvertToExternal(cmd, &externalCmd))
		{
			if (!externalCmd.ModemCmd)
			{
				signed char netAddr = (dispatchParams) ? dispatchParams->NetAddr : currentParams->NetAddress;
				unsigned char sendMode = (dispatchParams) ? dispatchParams->SendMode : DispatchParams::SendModeStd;

				// костыль для отправки информармации о модеме на адрес по умолчанию
				if (cmd->Cmd == CmdModemNotifySelfInfo)
				{
					netAddr = 0x00; // отправить на нулевой
				}

				if (netAddr >= 0)
				{
					// костыль для отправки информармации о модеме на адрес по умолчанию
					if (cmd->Cmd == CmdModemNotifySelfInfo)
					{
						netAddr |= 0x80; // целевой модем должен обработать пакет сам
						// сразу выше выставить этот бит нельзя, т.к. netAddr имеет знаковый тип
					}
					
					SetCmd(ModemCmdSendAddr);
					SetNetAddress(netAddr);
					SetSendMode(sendMode);
					SetSubCmd(externalCmd.Cmd, externalCmd.NestingLevel, sizeof(ModemSendAddrHeader));
					if (externalCmd.Data)
						SetData(sizeof(ModemSendAddrHeader) + externalCmd.NestingLevel, cmd->Data, cmd->DataSize);
					else
						SetLen(externalCmd.NestingLevel + sizeof(ModemSendAddrHeader));
				}
				else
				{
					SetCmd(ModemCmdSendDef);
					SetSubCmd(externalCmd.Cmd, externalCmd.NestingLevel);
					if (externalCmd.Data)
						SetData(externalCmd.NestingLevel, cmd->Data, cmd->DataSize);
					else
						SetLen(externalCmd.NestingLevel);
				}
			}
			else
			{
				SetCmd(externalCmd.Cmd, externalCmd.NestingLevel);

				if (externalCmd.Data)
					SetData(externalCmd.NestingLevel - 1, cmd->Data, cmd->DataSize);
				else
					SetLen(externalCmd.NestingLevel - 1);
			}

			SetAck(externalCmd.Ack);
			// когда говорим версию ПО модему, нужно Request выставить в 1
			if (cmd->Cmd == CmdModemNotifyExtDevSoftwareVersion ||
          cmd->Cmd == CmdNotifySoftwareVersion)
			{
				SetRequest(1);
			}
			else
			{
				SetRequest(0);
			}
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
			if (currentParams->NetAddress >= 0)
			{
				SetCmd(ModemCmdSendAddr);
				SetNetAddress(currentParams->NetAddress);
				SetData(sizeof(ModemSendAddrHeader),
						currentParams->Data[0] + sizeof(ModemCmdDataHeader),
						currentParams->DataSize[0] - sizeof(ModemCmdDataHeader));
			}
			else
			{
				SetCmd(ModemCmdSendDef);
				SetData(currentParams->Data[0] + sizeof(ModemCmdDataHeader),
						currentParams->DataSize[0] - sizeof(ModemCmdDataHeader));
			}
		}
		else
		{
		  	SetCmd(currentParams->Cmd[0]);
			SetData(currentParams->Data[0], currentParams->DataSize[0]);
		}
		
		SetRequest(1);
		SetAck(1);	// ошибка при выполнении
		SetDest(currentParams->Source);
		SetSource(currentParams->Dest);
		
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
		  externalCmd->ModemCmd = true;
		  break;
		case CmdModemPing:
			externalCmd->Cmd[0] = ModemCmdPing;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 0;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
		case CmdModemSetType:
			externalCmd->Cmd[0] = ModemCmdDo;
			externalCmd->Cmd[1] = ModemCmdDoSetXtyp;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
		case CmdModemGetVersion:
			externalCmd->Cmd[0] = ModemCmdWhoAreYou;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 0;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;		
#ifndef RBLIB_BOOTLOADER
		case CmdModemNotifySelfInfo:
			externalCmd->Cmd[0] = 0x03; // смысл команды выяснить не удалось, знаем только её код
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			break;
		case CmdModemGetTempAndPower:
			externalCmd->Cmd[0] = ModemCmdDo;
			externalCmd->Cmd[1] = ModemCmdDoGetTempPower;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
		case CmdModemGetSelfInfo:
			externalCmd->Cmd[0] = ModemCmdDo;
			externalCmd->Cmd[1] = ModemCmdDoShowStatus;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
		case CmdModemGetNetTime:
			externalCmd->Cmd[0] = ModemCmdDo;
			externalCmd->Cmd[1] = ModemCmdDoGetCTime;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
		case CmdModemGetRoute:
		  externalCmd->Cmd[0] = ModemCmdDo;
		  externalCmd->Cmd[1] = ModemCmdDoShowRoute;
		  externalCmd->NestingLevel = 2;
		  externalCmd->Request = 0;
		  externalCmd->Data = false;
		  externalCmd->Ack = 0;
		  externalCmd->ModemCmd = true;
		  break;
		case CmdModemSetMode:
		  externalCmd->Cmd[0] = ModemCmdDo;
		  externalCmd->Cmd[1] = ModemCmdDoSetMode;
		  externalCmd->NestingLevel = 2;
		  externalCmd->Request = 0;
		  externalCmd->Data = true;
		  externalCmd->Ack = 0;
		  externalCmd->ModemCmd = true;
		  break;
		case CmdModemGetRemoteModemInfo:
		  externalCmd->Cmd[0] = ModemCmdDo;
		  externalCmd->Cmd[1] = ModemCmdDoAskStatus;
		  externalCmd->NestingLevel = 2;
		  externalCmd->Request = 0;
		  externalCmd->Data = true;
		  externalCmd->Ack = 0;
		  externalCmd->ModemCmd = true;
		  break;
		case CmdModemGetRemoteModemVersion:
		  externalCmd->Cmd[0] = ModemCmdDo;
		  externalCmd->Cmd[1] = ModemCmdDoGetModemVersionRemote;
		  externalCmd->NestingLevel = 2;
		  externalCmd->Request = 0;
		  externalCmd->Data = true;
		  externalCmd->Ack = 0;
		  externalCmd->ModemCmd = true;
		  break;
		case CmdModemGetRemoteDeviceVersion:
		  externalCmd->Cmd[0] = ModemCmdDo;
		  externalCmd->Cmd[1] = ModemCmdDoGetDeviceVersionRemote;
		  externalCmd->NestingLevel = 2;
		  externalCmd->Request = 0;
		  externalCmd->Data = true;
		  externalCmd->Ack = 0;
		  externalCmd->ModemCmd = true;
		  break;
		case CmdModemSetStandbyMode:
			externalCmd->Cmd[0] = ModemCmdDo;
			externalCmd->Cmd[1] = ModemCmdDoStbyMode;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
		case CmdModemSetNormalMode:
			externalCmd->Cmd[0] = ModemCmdDo;
			externalCmd->Cmd[1] = ModemCmdDoNormalMode;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
		case CmdModemTestStop:
			externalCmd->Cmd[0] = ModemCmdTest;
			externalCmd->Cmd[1] = ModemCmdTestStop;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
		case CmdModemTestMeasureTempAndVoltage:
			externalCmd->Cmd[0] = ModemCmdTest;
			externalCmd->Cmd[1] = ModemCmdTestMeasureVT;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
		case CmdRsuAlarmFlagsNotify:
			externalCmd->Cmd[0] = DevCmdRsuSeismicInveroment;
			externalCmd->Cmd[1] = DevCmdRsuSIAlarmFlagsNotify;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
		case CmdModemGetNetInfo:
			externalCmd->Cmd[0] = ModemCmdDo;
			externalCmd->Cmd[1] = ModemCmdDoGetNetInfo;
			externalCmd->NestingLevel = 2;
			externalCmd->Request = 0;
			externalCmd->Data = false;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
#endif // RBLIB_BOOTLOADER
		case CmdModemNotifyExtDevSoftwareVersion:
			externalCmd->Cmd[0] = DevCmdGetFirmwareVer;
			externalCmd->NestingLevel = 1;
			externalCmd->Request = 1;
			externalCmd->Data = true;
			externalCmd->Ack = 0;
			externalCmd->ModemCmd = true;
			break;
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
