///////////////////////////////////////////////////////////////////////////////
//	������� ���������� ������
//	�������� FirTwig2
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDHANDLERBASE_FIRTWIG2_H
#define COMMANDHANDLERBASE_FIRTWIG2_H

#include "Parser.h"
#include "PacketFormat.h"
#include "CommandConverter.h"
#include "..\..\Communication\InterfaceBase.h"
#include "..\InternalCommands.h"
#include "..\CommandHandlerBase.h"
#include "RequestParams.h"


namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{

/// \brief ���������� ������, ����������� � ������� FirTwig2.
/// \details ������������ ������ ������ � ����������� ������ �� ���������� ������
///	� ������ �������� ������� ������ (CommandSet).
///	����� ��� ����������������� ������� ��������� � CommandHandlerBase 
///	��� ���������� ��������� �� ��������� � ������� ������� ���������������� ������������.
/// \tparam _commandSet ������� ������ (CommandSet) �� ������ ������� ����� �������������� 
///	����������� ������ �� �������� ������������� (FirTwig2) �� ����������.
template<DevCommandSet _commandSet>
class CommandHandler : public CommandHandlerBase
{

public:

	/// ���������� �������.
	/// \details ����� �������������� ������ ������, ����������� �������
	/// � ������ ���������� �������� ������ _CommandHandlerBase::ProcessCommand.
	inline void PreProcessCommand(unsigned char* data, unsigned char length, Communication::InterfaceBase *outputInterface)
	{
		// ������������� �������
		Parser parser;
		parser.SelectPacket(data, length);
		parser.SelectCommandSet(_commandSet);

		// ������ ������
		Parser::PacketSummaryInfo summaryInfo;
		parser.GetSummaryInfo(summaryInfo);

		// ������, ���������������� ������� �����, �� ����� ������������
		if (summaryInfo.NetAddress != -1 && (summaryInfo.NetAddress & 0x40))
			return;

		// ����������� �������
		CommandConverter::CmdData cmdData;
		CmdInternal cmdInternal = CommandConverter::Convert(summaryInfo, _commandSet, cmdData);

    RequestParams params(summaryInfo);
        
		// ��������� �������
		CmdInfo cmd(cmdInternal
					, cmdData.Buffer
					, cmdData.Length
					, params);

		ProcessCommand(&cmd, outputInterface);
	}

};

}
}
}

#endif
