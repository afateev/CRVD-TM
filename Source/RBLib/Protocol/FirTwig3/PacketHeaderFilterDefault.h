///////////////////////////////////////////////////////////////////////////////
//	������ ���������� �������
//	�������� FirTwig3
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETHEADERFILTER_DEFAULT_FIRTWIG3_H
#define PACKETHEADERFILTER_DEFAULT_FIRTWIG3_H

#include "Parser.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig3
{
// ����� ������� �������
class PacketHeaderFilterDefault
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