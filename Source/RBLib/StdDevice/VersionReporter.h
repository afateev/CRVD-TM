///////////////////////////////////////////////////////////////////////////////
//	����� ������������ ����������� �������
//	������� "�������� ������ ��"
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
	// ����������� �� ���������
	VersionReporter() : _versionString("Version string is not specified"), _versionStringSize(31)
	{}
	
	// ����������� � ��������� ������ � ������� �� � ������� ������
	VersionReporter(char *versionString, unsigned int versionStringSize): _versionString(versionString), _versionStringSize(versionStringSize)
	{}

	// ���������� ������� ������� ������ (�������� �� ������ ����������� ������)
	void VersionCommandHandler(Rblib::Protocol::CmdInfo *cmd, Rblib::Communication::InterfaceBase *outputInterface)
	{
		switch(cmd->Cmd)
		{
		case Rblib::Protocol::CmdGetSoftwareVersion:
		{
	  		// ���������� ������
			bool validRequest = false;
			// ��������� ������������
			if (cmd->RequestParams)
			{
				if (Rblib::Protocol::ProtocolFirTwig2 == cmd->RequestParams->Protocol)
				{
					Rblib::Protocol::FirTwig2::RequestParams *request = (Rblib::Protocol::FirTwig2::RequestParams *)cmd->RequestParams;
					// ������ ����������, ���� �� �� ����������� � �������� �������
					validRequest = request->PacketInfo.NestingLevel == 1;
					
					if (validRequest)
					{
						if (request->PacketInfo.Source == Rblib::Protocol::FirTwig2::AddrModem)
						{
							// ����� ��������� �� ������ �������
							Rblib::Protocol::CmdInfo answer(*cmd);
							// ����������� ������� ������ ������
							answer.Cmd = Rblib::Protocol::CmdModemNotifyExtDevSoftwareVersion;
							// � ���� ������ ������ ������
							answer.Data = (unsigned char *)_versionString;
							answer.DataSize = _versionStringSize;
							// ��������� �����
							outputInterface->SendCommand(&answer);
						}
						else
						{
							// ����� ��������� �� ������ �������
							Rblib::Protocol::CmdInfo answer(*cmd);
							// ������� ������
							answer.Cmd = Rblib::Protocol::CmdNotifySoftwareVersion;
							// � ���� ������ ������ ������
							answer.Data = (unsigned char *)_versionString;
							answer.DataSize = _versionStringSize;
							// ��������� �����
							outputInterface->SendCommand(&answer);
						}
					}
				}
			}
			// ������� ����������
			cmd->Handled = true;
		}
		break;
		}
	}
};
}
}

#endif
