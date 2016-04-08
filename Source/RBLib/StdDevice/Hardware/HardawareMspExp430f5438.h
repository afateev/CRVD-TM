///////////////////////////////////////////////////////////////////////////////
//	����� ������������ ������ ���������
//	����� ��� ���������� ����� MSP-EXP430F5438:
//	- ���� ���
//	- ���� ������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_MSPEXP430F5438_H
#define HARDWARE_MSPEXP430F5438_H

namespace Rblib
{
namespace StdDevice
{
namespace Hardware
{
template<class Params>
class HardawareMspExp430f5438
{
public:	
	typedef Uart::_B<> UartPc; 		// USCI_A1 (�� ����� ��� USB)
	typedef Uart::_A<> UartModem; 	// USCI_A2 (�� ����� ��� RF3: 5 - TX, 7 - RX)

	static const unsigned char InterfaceIdPc = 1;
	static const unsigned char InterfaceIdModem = 2;
	
	static typename Params::PacketQueueType InputQueue;			// ������� �������� �������
	static typename Params::PacketQueueType OutputQueue;		// ������� ��������� �������
	
	typedef Communication::CommunicationModule<typename Params::PacketQueueType, &InputQueue, typename Params::PacketQueueType, &OutputQueue> CommunicationModule;

protected:
	// ��� ���������� ������ � ��
	typedef Communication::InterfaceSimple<InterfaceIdPc,	// ������������� ���������� ���
		UartPc,
		typename Params::PacketQueueType, &InputQueue,
		typename Params::PacketQueueType, &OutputQueue,
		Params::ProtocolType,
		typename Params::PcPacketFilter,
		typename Params::PcPacketBuilder,
		typename Params::PcCommandHandler
		> InterfacePc;

	// ��� ���������� ������ � �������
	typedef Communication::InterfaceSimple<InterfaceIdModem,	// ������������� ���������� ������
		UartModem,
		typename Params::PacketQueueType, &InputQueue,
		typename Params::PacketQueueType, &OutputQueue,
		Params::ProtocolType,
		typename Params::ModemPacketFilter,
		typename Params::ModemPacketBuilder,
		typename Params::ModemCommandHandler
		> InterfaceModem;
	
	InterfacePc _interfacePc;				// ��������� ���������� ������ �� ����� ��
	InterfaceModem _interfaceModem;			// ��������� ���������� ������ �� ����� ������

public:
	// ������������� ���������
	void HardwareInit()
	{
		// ������������ ����������
		Singletone<CommunicationModule>::Instance().RegisterInterface(&_interfacePc);
		Singletone<CommunicationModule>::Instance().RegisterInterface(&_interfaceModem);

		// ��������� ������ ����� ����� ������������
		_interfacePc.SetResendInterface(&_interfaceModem);
		_interfaceModem.SetResendInterface(&_interfacePc);
	}
};
template<class Params>
typename Params::PacketQueueType HardawareMspExp430f5438<Params>::InputQueue;
template<class Params>
typename Params::PacketQueueType HardawareMspExp430f5438<Params>::OutputQueue;

}
}
}

#endif