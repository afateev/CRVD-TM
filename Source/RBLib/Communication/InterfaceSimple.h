///////////////////////////////////////////////////////////////////////////////
//	����� �������� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERFACESIMPLE_H
#define INTERFACESIMPLE_H

#include "..\Protocol\Protocol.h"
#include "..\Protocol\PacketExtractor.h"
#include "..\Common.h"

namespace Rblib
{
namespace Communication
{
/// \brief ���������� �������� ����������.
/// \details �� ������� �� ���������.
///	������������������ ������������ ���������� � �������� ���������� �������.
/// \tparam InterfaceId ���������� ������������� ����������.
/// \tparam HardwareInterface ���������� ��������� (�������� Uart).
/// \tparam InputQueueType ��� ������� �������� �������.
/// \tparam inputQueue ��������� �� ������� �������� �������.
/// \tparam OutputQueueType ��� ������� ��������� �������.
/// \tparam outputQueue ��������� �� ������� ��������� �������.
/// \tparam p �������� �� �������� �������� ���������.
/// \tparam PacketFilter ������ ���������� �������.
/// \tparam PacketBuilder �����������������.
/// \tparam PacketHandler ���������� ������.
template <
	unsigned char InterfaceId,
	class HardwareInterface,
	class InputQueueType,
	InputQueueType *inputQueue,
	class OutputQueueType,
	OutputQueueType *outputQueue,
	Protocol::ProtocolType p,
	class PacketFilter,
	class PacketBuilder,
	class PacketHandler
	>
class InterfaceSimple : public InterfaceBase
{
protected:

	/// ��� ����������� �������.
	typedef Protocol::PacketExtractor<p> ExtractorType;

public:	

protected:

	/// ��������� ����������� �������.
	ExtractorType _extractor;
	
	/// ��������� ��������� �������.
	RBLIB_NO_INIT static PacketBuilder _packetBuilder;
	/// ��������� ����������� �������.
	PacketHandler _packetHandler;

	/// ��������� ��������� �������.
	InterfaceBase *_resendInterface;

	/// ���������� ��������� �� ��������� ����������� ������.
	PacketHandler* GetPacketHandler()
	{
		return &_packetHandler;
	}

	/// ������� ��������� ������ � ������� (���������� �� ����������� �������).
	static void PushQueue(unsigned char *data, unsigned int length)
	{
		if (0 == inputQueue)
			return;
		if (0 == data)
			return;
		if (0 == length)
			return;
		// ��������� ����� � ������� ������ ���� �������������
		inputQueue->Push(InterfaceId, data, length);
	}
public:

	/// ����������� ��-���������.
	InterfaceSimple()
	{
		_resendInterface = 0;
	}
	
	/// �������� ���������� ������������� ����������.
	virtual unsigned char GetId(void)
	{
		return InterfaceId;
	}
	
	/// ���������� ��������� �� ��������� �����������������.
	PacketBuilder* GetPacketBuilder()
	{
		return &_packetBuilder;
	}
	
	/// ��������� ���������� ���������, �������� ������ � �������� �� � �������.
	virtual void ReadInterface(void)
	{
		unsigned char data[64]; //��������� �����
		
		while(1)
		{
			// ��������� ���������� ���������
			int length = HardwareInterface::Read(data, 64);
			if(length == 0)
				break;
			// ������� ����������
			_extractor.ExtractPackets(data, length, PushQueue);
		}
	}
	
	/// ���������� �����.
	virtual void ProcessPacket(PacketQueueInfo* packetInfo)
	{
		bool process = false;
		bool resend = false;
		PacketFilter::Check(packetInfo->Data, packetInfo->Length, &process, &resend);

		if (process)
		{
			// ������� ���������� ������
			_packetHandler.PreProcessCommand(packetInfo->Data, packetInfo->Length, this);
		}

		if (resend)
		{
			// ��������� �����
			_resendInterface->SendPacket(packetInfo->Data, packetInfo->Length);
		}
	}

	/// ���������� ��������� ����� (��������� � ���������� ���������).
	virtual void ProcessOutputPacket(PacketQueueInfo* packetInfo)
	{
		if (0 == packetInfo)
			return;
		if (0 == packetInfo->Data)
			return;
		if (0 == packetInfo->Length)
			return;
		// ����� � ���������� ���������
		HardwareInterface::Write(packetInfo->Data, packetInfo->Length);
	}

	/// ��������� ��������� ��� ��������� �������.
	///\param interfaceFroResend ��������� ��� ���������.
	void SetResendInterface(InterfaceBase *interfaceFroResend)
	{
		_resendInterface = interfaceFroResend;
	}

	/// ��������� �����.
	/// \details C����� ������� � �������� ����� � ������� (��������).
	/// \param data ��������� �� ����� ������.
	/// \param length ����� ������.
	virtual void SendPacket(unsigned char *data, unsigned int length)
	{
		if (0 == outputQueue)
			return;
		if (0 == data)
			return;
		if (0 == length)
			return;
		// ��������� ����� � ������� �� ��������
		outputQueue->Push(InterfaceId, data, length);
	}

	/// ��������� ����� �� �������.
	/// \param [in] cmd ��������� �� ��������� �������.
	/// \param [out] packetSize ������ ����������� ������.
	/// \return ��������� �� ���������� �����, � ������� ����� �������������� �����.
	unsigned char *BuildCommand(Protocol::CmdInfo *cmd, unsigned int *packetSize)
	{
		if (0 == cmd)
			return 0;
		// �������� ��������� �������
		_packetBuilder.Reset();
		// ��������� ����� �� �������
		bool builded = _packetBuilder.Build(cmd);
		if (!builded)
			return 0;
		// �������� ��, ��� ���������� ��������� �������
		return _packetBuilder.GetPacket(packetSize);
	}

	/// ��������� �������.
	/// \details ��������� ����� � ������� ���������, ������������� ������������������. ����� ����� �������� � ������� �� ��������.
	virtual void SendCommand(Protocol::CmdInfo *cmd)
	{
		// ����� �����
		unsigned char *packet = 0;
		// ������ ������ ������
		unsigned int packetSize = 0;
		// ������ ����� �� ������� � ������� ��������� �������.
		packet = BuildCommand(cmd, &packetSize);
		// ����������
		SendPacket(packet, packetSize);
	}

	virtual void SendCommandDirect(Protocol::CmdInfo *cmd)
	{
		unsigned char *packet = 0;
		// ������ ������ ������
		unsigned int packetSize = 0;
		// ������ ����� �� ������� � ������� ��������� �������.
		packet = BuildCommand(cmd, &packetSize);
		if (packetSize == 0)
			return;
		// ����������
		HardwareInterface::Write(packet, packetSize);
	}

};

// ��������������� ����������� ����������
template <unsigned char InterfaceId, class HardwareInterface, class InputQueueType, InputQueueType *inputQueue, class OutputQueueType, OutputQueueType *outputQueue,
	Protocol::ProtocolType p, class PacketFilter, class PacketBuilder, class PacketHandler>
PacketBuilder InterfaceSimple<InterfaceId, HardwareInterface, InputQueueType, inputQueue, OutputQueueType, outputQueue, p, PacketFilter, PacketBuilder, PacketHandler>::
	_packetBuilder;

}
}

#endif
