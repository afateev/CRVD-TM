///////////////////////////////////////////////////////////////////////////////
//	Экстрактор пакетов
//	Протокол FirTwig3
//
//	Метод ExtractPackets выделяет из переданного буфера пакет
//	и вызывает для него функцию помещения пакета в очередь,
//	которая определяется пользователем класса.
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETEXTRACTOR_FIRTWIG3_H
#define PACKETEXTRACTOR_FIRTWIG3_H

#include "../PacketExtractorCallbackTypeProvider.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig3
{
//--------- экстрактор пакетов FirTwig3 --------------------------------------
template<bool functorCallback = false>
class PacketExtractor : public PacketExtractorCallbackTypeProvider<functorCallback>
{
public:

	// Тип функции обратного вызова для помещения пакета в очередь.
    typedef typename PacketExtractorCallbackTypeProvider<functorCallback>::CallbackType PacketReadyCallback;
    
	// функция выделения пакетов и автоматического их укладывания в очередь
	// data - данные из приемного буфера UART'а
	// length - количество данных в буфере
	// pushFunctor - функция, которая должна быть вызвана для помещения выделенного пакета в очередь
	void ExtractPackets(unsigned char* data, unsigned int length, PacketReadyCallback pushFunctor)
	{
	}
};

}
}
}

#endif // PACKETEXTRACTOR_FIRTWIG3_H