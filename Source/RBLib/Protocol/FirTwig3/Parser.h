///////////////////////////////////////////////////////////////////////////////
//	������ �������
//	�������� FirTwig3
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PARSER_FIRTWIG3_H
#define PARSER_FIRTWIG3_H

namespace Rblib
{
namespace Protocol
{
namespace FirTwig3
{

class Parser
{
public:
	// ������� �����
	// data - ��������� �� �����
	// length - ������ ������
	void SelectPacket(unsigned char *data, unsigned int length)
	{
	
	}

	// ������� ��������� "����������" ������

	// �������� ����� ���������
	unsigned char GetDstAddress(void)
	{
		return 0; //TODO
	}
	
	// �������� ����� ���������
	unsigned char GetSrcAddress(void)
	{
		return 0; //TODO
	}

	// �������� ����� �������
	unsigned int GetCommand(void)
	{
		return 0; //TODO
	}

	// �������� ������ ������
	unsigned int GetDataSize(void)
	{
		return 0; //TODO
	}

	// �������� ������
	unsigned char * GetData()
	{
		return 0; //TODO
	}

	// �������� ��� �������������
	unsigned char GetAckCode(void)
	{
		return 0; //TODO
	}

	// �������� ���� �������
	unsigned char GetRequestFlag(void)
	{
		return 0; //TODO
	}
};

}
}
}

#endif