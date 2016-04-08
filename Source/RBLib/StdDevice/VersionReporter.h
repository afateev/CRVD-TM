///////////////////////////////////////////////////////////////////////////////
//	Класс стандартного функционала датчика
//	Функция "Сообщить версию ПО"
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef VERSIONREPORTER_H
#define VERSIONREPORTER_H

#include "..\Protocol\Protocol.h"
#include "..\Protocol\InternalCommands.h"
#include "..\Communication\InterfaceBase.h"
#include "..\Protocol\FirTwig2\RequestParams.h"

namespace Rblib
{
namespace StdDevice
{

class VersionReporter
{
protected:
	char *_versionString;
	unsigned int _versionStringSize;
public:
	// Конструктор по умолчанию
	VersionReporter() : _versionString("Version string is not specified"), _versionStringSize(31)
	{}
	
	// Конструктор с передачей строки с версией ПО и размера строки
	VersionReporter(char *versionString, unsigned int versionStringSize): _versionString(versionString), _versionStringSize(versionStringSize)
	{}

	// Обработчик команды запроса версии (вызывать из общего обработчика команд)
	void VersionCommandHandler(Rblib::Protocol::CmdInfo *cmd, Rblib::Communication::InterfaceBase *outputInterface)
	{
		switch(cmd->Cmd)
		{
		case Rblib::Protocol::CmdGetSoftwareVersion:
		{
	  		// правильный запрос
			bool validRequest = false;
			// проверяем правильность
			if (cmd->RequestParams)
			{
				if (Rblib::Protocol::ProtocolFirTwig2 == cmd->RequestParams->Protocol)
				{
					Rblib::Protocol::FirTwig2::RequestParams *request = (Rblib::Protocol::FirTwig2::RequestParams *)cmd->RequestParams;
					// запрос правильный, если он не вкладывался в модемную команду
					validRequest = request->PacketInfo.NestingLevel == 1;
					
					if (validRequest)
					{
						if (request->PacketInfo.Source == Rblib::Protocol::FirTwig2::AddrModem)
						{
							// ответ формируем на основе запроса
							Rblib::Protocol::CmdInfo answer(*cmd);
							// смециальная команда ответа медему
							answer.Cmd = Rblib::Protocol::CmdModemNotifyExtDevSoftwareVersion;
							// в поле данных строка версии
							answer.Data = (unsigned char *)_versionString;
							answer.DataSize = _versionStringSize;
							// отправить ответ
							outputInterface->SendCommand(&answer);
						}
						else
						{
							// ответ формируем на основе запроса
							Rblib::Protocol::CmdInfo answer(*cmd);
							// команда ответа
							answer.Cmd = Rblib::Protocol::CmdNotifySoftwareVersion;
							// в поле данных строка версии
							answer.Data = (unsigned char *)_versionString;
							answer.DataSize = _versionStringSize;
							// отправить ответ
							outputInterface->SendCommand(&answer);
						}
					}
				}
			}
			// команду обработали
			cmd->Handled = true;
		}
		break;
		}
	}
};
}
}

#endif
