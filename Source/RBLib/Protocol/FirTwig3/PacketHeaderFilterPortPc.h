///////////////////////////////////////////////////////////////////////////////
//	������ ���������� �������
//	�������� FirTwig3
//
//	������ ��� ����� ������ � ��
//	�����������, ����� ��������� ������������ ������ ��������������� ��� ��
//	� ���������� ������ ��������������� ��� ������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETHEADERFILTER_PORTPC_FIRTWIG3_H
#define PACKETHEADERFILTER_PORTPC_FIRTWIG3_H

#include "Parser.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig3
{
// ����� ������� �������
class PacketHeaderFilterPortPc
{
public:
	// �������� ���� �� ������������ � (���) ���������� �����
	// �� ������ ������ �������������, ��� � ������� "�������" �����
	// process - ������������ � true ���� ��������� ���������� �����, ����� false
	// resend - ������������ � true ���� ��������� ��������� �����, ����� false
	static inline void Check(unsigned char* packetBuffer, unsigned int packetLength, bool *process, bool *resend)
	{
	}
};
}
}
}

#endif