///////////////////////////////////////////////////////////////////////////////
//	������ ���������� �������
//	�������� FirTwig2
//
//	������ ��-���������
//	�����������, ����� ��������� ������������ ������ ��������������� ������ ��� ��
//	��������� ������� �� ������������
//
//	������� ����������� �� ������ ����� Dest. � Source ��������� ������
//	������� ������ ������� ���������:
//	Dest		Source		���������
//	ADDR_PC		ADDR_PC		������������
//	ADDR_PC		ADDR_EXT	������������
//	ADDR_PC		ADDR_MODEM	������������
//	ADDR_PC		ADDR_ALL	������������
//	ADDR_EXT	ADDR_PC		����������
//	ADDR_EXT	ADDR_EXT	����������
//	ADDR_EXT	ADDR_MODEM	����������
//	ADDR_EXT	ADDR_ALL	������������
//	ADDR_MODEM	ADDR_PC		������������
//	ADDR_MODEM	ADDR_EXT	������������
//	ADDR_MODEM	ADDR_MODEM	������������
//	ADDR_MODEM	ADDR_ALL	������������
//	ADDR_ALL	ADDR_PC		������������
//	ADDR_ALL	ADDR_EXT	������������
//	ADDR_ALL	ADDR_MODEM	������������
//	ADDR_ALL	ADDR_ALL	������������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETHEADERFILTER_DEFAULT_FIRTWIG2_H
#define PACKETHEADERFILTER_DEFAULT_FIRTWIG2_H

#include "Parser.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{
/// \brief ����� ������� ������� �� ������.
/// \details ����������, ��� ���������� �������� � �������: ����������, ������� ��� �������������. \n
/// ������� ����������� �� ������ ����� Dest � Source ��������� ������.\n
/// ������� ������ ������� ���������:\n
/// <table>
/// <tr><td>Dest</td><td>Source</td><td>���������</td></tr>\n
/// <tr><td>ADDR_PC</td><td>ADDR_PC</td><td>������������</td></tr>\n
/// <tr><td>ADDR_PC</td><td>ADDR_EXT</td><td>������������</td></tr>\n
/// <tr><td>ADDR_PC</td><td>ADDR_MODEM</td><td>������������</td></tr>\n
/// <tr><td>ADDR_PC</td><td>ADDR_ALL</td><td>������������</td></tr>\n
/// <tr><td>ADDR_EXT</td><td>ADDR_PC</td><td>����������</td></tr>\n
/// <tr><td>ADDR_EXT</td><td>ADDR_EXT</td><td>����������</td></tr>\n
/// <tr><td>ADDR_EXT</td><td>ADDR_MODEM</td><td>����������</td></tr>\n
/// <tr><td>ADDR_EXT</td><td>ADDR_ALL</td><td>������������</td></tr>\n
/// <tr><td>ADDR_MODEM</td><td>ADDR_PC</td><td>������������</td></tr>\n
/// <tr><td>ADDR_MODEM</td><td>ADDR_EXT</td><td>������������</td></tr>\n
/// <tr><td>ADDR_MODEM</td><td>ADDR_MODEM</td><td>������������</td></tr>\n
/// <tr><td>ADDR_MODEM</td><td>ADDR_ALL</td><td>������������</td></tr>\n
/// <tr><td>ADDR_ALL</td><td>ADDR_PC</td><td>������������</td></tr>\n
/// <tr><td>ADDR_ALL</td><td>ADDR_EXT</td><td>������������</td></tr>\n
/// <tr><td>ADDR_ALL</td><td>ADDR_MODEM</td><td>������������</td></tr>\n
/// <tr><td>ADDR_ALL</td><td>ADDR_ALL</td><td>������������</td></tr>\n
/// </table>
class PacketHeaderFilterDefault
{
public:
	/// �������� ���� �� ������������ � (���) ���������� �����.
	/// \param [in] packetBuffer ����� ������.
	/// \param [in] packetLength ����� ������.
	/// \param [out] process ������������ � \p true ���� ��������� ���������� �����, ����� \p false.
	/// \param [out] resend ������������ � \p true ���� ��������� ��������� �����, ����� \p false.
	static inline void Check(unsigned char* packetBuffer, unsigned int packetLength, bool *process, bool *resend)
	{
		Parser parse;
		parse.SelectPacket(packetBuffer, packetLength);

		*resend = false;
		if( (parse.GetDstAddress() != AddrExt) || (parse.GetSrcAddress() == AddrAll) )
		{
			*process = false;
			return;
		}		
		*process = true;				
	}
};
}
}
}

#endif