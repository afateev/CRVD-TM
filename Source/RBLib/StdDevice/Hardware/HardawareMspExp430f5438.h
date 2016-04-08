///////////////////////////////////////////////////////////////////////////////
//	Класс стандартного набора перифирии
//	Набор для отладочной платы MSP-EXP430F5438:
//	- порт КСК
//	- порт модема
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
	typedef Uart::_B<> UartPc; 		// USCI_A1 (на плате это USB)
	typedef Uart::_A<> UartModem; 	// USCI_A2 (на плате это RF3: 5 - TX, 7 - RX)

	static const unsigned char InterfaceIdPc = 1;
	static const unsigned char InterfaceIdModem = 2;
	
	static typename Params::PacketQueueType InputQueue;			// Очередь входящих пакетов
	static typename Params::PacketQueueType OutputQueue;		// Очередь исходящих пакетов
	
	typedef Communication::CommunicationModule<typename Params::PacketQueueType, &InputQueue, typename Params::PacketQueueType, &OutputQueue> CommunicationModule;

protected:
	// Тип интерфейса обмена с ПК
	typedef Communication::InterfaceSimple<InterfaceIdPc,	// Идентификатор интерфейса КСК
		UartPc,
		typename Params::PacketQueueType, &InputQueue,
		typename Params::PacketQueueType, &OutputQueue,
		Params::ProtocolType,
		typename Params::PcPacketFilter,
		typename Params::PcPacketBuilder,
		typename Params::PcCommandHandler
		> InterfacePc;

	// Тип интерфейса обмена с модемом
	typedef Communication::InterfaceSimple<InterfaceIdModem,	// Идентификатор интерфейса модема
		UartModem,
		typename Params::PacketQueueType, &InputQueue,
		typename Params::PacketQueueType, &OutputQueue,
		Params::ProtocolType,
		typename Params::ModemPacketFilter,
		typename Params::ModemPacketBuilder,
		typename Params::ModemCommandHandler
		> InterfaceModem;
	
	InterfacePc _interfacePc;				// Экземпляр интерфейса обмена по порту ПК
	InterfaceModem _interfaceModem;			// Экземпляр интерфейса обмена по порту модема

public:
	// Инициализация перифирии
	void HardwareInit()
	{
		// Регистрируем интерфейсы
		Singletone<CommunicationModule>::Instance().RegisterInterface(&_interfacePc);
		Singletone<CommunicationModule>::Instance().RegisterInterface(&_interfaceModem);

		// Пересылка данных между двумя интерфейсами
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