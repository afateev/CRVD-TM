///////////////////////////////////////////////////////////////////////////////
//	����� ������ �����������
//	������������ ����������
//	�������� ����������� ������ �����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMUNICATIONMODULE_H
#define COMMUNICATIONMODULE_H

#include "InterfaceBase.h"
#include "..\Utils\Singletone.h"

namespace Rblib
{
namespace Communication
{
/// ����� ��� ������ � ������� ����������.
/// \tparam InputQueueType ��� ������� �������� �������.
/// \tparam inputQueue ��������� �� ������� �������� �������.
/// \tparam OutputQueueType ��� ������� ��������� �������.
/// \tparam outputQueue ��������� �� ������� ��������� �������.
template <
	class InputQueueType,		// ��� ������� �������� �������
	InputQueueType *inputQueue,	// ��������� �� ������� �������� �������
	class OutputQueueType,		// ��� ������� ��������� �������
	OutputQueueType *outputQueue// ��������� �� ������� ��������� �������
	>
class CommunicationModule
{
protected:
	//������������ ���������� �������������� ����������� 
	static const unsigned char _size = 5;	
	//���������� ������������������ �����������
	unsigned char _count;	
	// ��������� ������ �����������
	InterfaceBase* _interfaceArray[_size];
	
public:
	/// �����������.
	CommunicationModule()
	{
		_count = 0;
	}
	
	/// ���������������� ���������.
	/// \param transport ��������� �� ���������.
	void RegisterInterface(InterfaceBase* transport)
	{
		//���� �������� ����� �� ���������� �����������
		if((_count == _size) || (transport == 0))
			return;
		
		// �������� �� ������������ ��������������
		for(int i = 0; i < _count; i++)
		{
			if(_interfaceArray[i]->GetId() == transport->GetId())
				return;
		}
		
		// �������� �� ������������ ���������
		for(int i = 0; i < _count; i++)
		{
			if(_interfaceArray[i] == transport)
				return;
		}
		
		//�������� ���������
		_interfaceArray[_count++] = transport;
	}

	/// ��������� ������ �� ���� ���������� �����������.
	/// ������ ������� � ������� �������� �������.
	void ReadInterfaces(void)
	{
		for(unsigned char i = 0; i < _count; i++)
			_interfaceArray[i]->ReadInterface();
	}

	/// ������� ����������� ������� ���� ����������� �� ������� �������� �������.
	void ProcessPackets(void)
	{	
		if (0 == inputQueue)
			return;
		// ���� ���� ������
		while(!inputQueue->Empty())
		{
			// ����� ����� �� �������
			PacketQueueInfo info;
			inputQueue->Front(info);
		
			// �������� ����� � ������ ��������� � ����������� �� �������������� ����������,
			// ������� ������ � ���������� � ������ ������ �� �������
			for(unsigned char i = 0; i < _count; i++)
			{
				if(_interfaceArray[i]->GetId() == info.InterfaceId)
				{
					_interfaceArray[i]->ProcessPacket(&info);
					break;
				}
			}

			// ������� ������������ ����� �� �������
			inputQueue->Pop();
		}
	}

	/// ������� �������� ���� ������� �� ������� ��������� �������.
	void SendPackets(void)
	{
		if (0 == outputQueue)
			return;
		
		// ���� ���� ������
		while(!outputQueue->Empty())
		{
			// ����� ����� �� �������
			PacketQueueInfo info;
			outputQueue->Front(info);
		
			// �������� ����� � ������ ��������� � ����������� �� �������������� ����������,
			// ������� ������ � ���������� � ������ ������ �� �������
			for(unsigned char i = 0; i < _count; i++)
			{
				if(_interfaceArray[i]->GetId() == info.InterfaceId)
				{
					_interfaceArray[i]->ProcessOutputPacket(&info);
					break;
				}
			}

			// ������� ������������ ����� �� �������
			outputQueue->Pop();
		}
	}

	/// ��������� �������.
	/// \details ��������� ����� � �������� ��� � ������� ��������� �������.
	/// \param interfaceId ������������� ����������, ����� ������� ������� ���������� �������.
	/// \param cmd ���������� � �������.
	void SendCommand(unsigned char interfaceId, Protocol::CmdInfo *cmd)
	{
		if (0 == cmd)
			return;
		for(unsigned char i = 0; i < _count; i++)
		{
			if(_interfaceArray[i]->GetId() == interfaceId)
			{
				_interfaceArray[i]->SendCommand(cmd);
				break;
			}
		}
	}
};

}
}

#endif
