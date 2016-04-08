///////////////////////////////////////////////////////////////////////////////
//	������� ����� ����������
//	��������� ��������� ���������� ��������� � �������� �������
//	� ������������ ������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERFACEBASE_H
#define INTERFACEBASE_H

#include "..\Protocol\PacketQueue.h"
#include "..\Protocol\InternalCommands.h"
#include "..\Protocol\InternalCommands.h"

namespace Rblib
{
namespace Communication
{

/// \brief ��������� ���������� :)
class InterfaceBase
{
public:
	/// \brief �������� ���������� ������������� ����������.
	virtual unsigned char GetId(void) = 0;
	/// \brief ��������� ���������� ���������, �������� ������ � �������� �� � �������.
	virtual void ReadInterface(void) = 0;
	/// \brief ���������� �����.
	virtual void ProcessPacket(PacketQueueInfo* packetInfo) = 0;
	/// \brief ���������� ��������� ����� (��������� � ���������� ���������).
	virtual void ProcessOutputPacket(PacketQueueInfo* packetInfo) = 0;
	/// \brief ��������� �����.
	virtual void SendPacket(unsigned char *data, unsigned int length) = 0;
	/// \brief ��������� �������.
	virtual unsigned char *BuildCommand(Protocol::CmdInfo *cmd, unsigned int *packetSize) = 0;
	/// \brief ��������� �������.
	virtual void SendCommand(Protocol::CmdInfo *cmd) = 0;
};

}
}

#endif