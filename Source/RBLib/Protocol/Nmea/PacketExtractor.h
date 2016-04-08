///////////////////////////////////////////////////////////////////////////////
//	Экстрактор пакетов
//	Протокол NMEA
//
//	Метод ExtractPackets выделяет из переданного буфера пакет
//	и вызывает для него функцию помещения пакета в очередь,
//	которая определяется пользователем класса.
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETEXTRACTOR_NMEA_H
#define PACKETEXTRACTOR_NMEA_H

#include "../PacketExtractorCallbackTypeProvider.h"
#include "Packet.h"

namespace Rblib
{
namespace Protocol
{
namespace Nmea
{
	template<bool functorCallback = false>
	class PacketExtractor : public PacketExtractorCallbackTypeProvider<functorCallback>
	{
	public:
    	/// Тип функции обратного вызова для помещения пакета в очередь.
		typedef typename PacketExtractorCallbackTypeProvider<functorCallback>::CallbackType PacketReadyCallback;
		// Размер внутреннего буфера
		static const unsigned int BufferSize = 128;
		// Тип пакета
		typedef Packet PacketType;
	protected:
		char _currentPacketSourceData[BufferSize];		// буфер под входящие данные
		unsigned int _currentPacketSourceDataCount;		// сколько днных во входящем буфере
		PacketType packet;								// текущий разбираемый пакет
	public:
		// Конструктор по умолчанию
		PacketExtractor() : packet(0, 0)
		{
			BreakPacket();
		}
		
		// функция выделения пакетов и автоматического их укладывания в очередь
		// data - данные из приемного буфера UART'а
		// length - количество данных в буфере
		// pushFunctor - функция, которая должна быть вызвана для помещения выделенного пакета в очередь
		void ExtractPackets(unsigned char* data, unsigned int length, PacketReadyCallback pushFunctor)
		{
			if (data == 0 || length < 1)
			{
				return;
			}
		
			for (unsigned int i = 0; i < length; i++)
			{
				unsigned char b = data[i];
				if (b == PacketType::PacketStart)
				{
					// если уже есть часть пакета
					if (_currentPacketSourceDataCount > 1)
					{
						// то на этом текущий прерывается
						BreakPacket();
					}

					// первый символ пакета
					CurrentPacketSourceDataAdd(b);
				}
				else
                {
                    // если нет ни одного символа, значит PacketStart ещё не было и пока накапливать пакет нет смысла
                    if (_currentPacketSourceDataCount < 1)
                    {
                        continue;
                    }
                    
                    // следующий символ
                    CurrentPacketSourceDataAdd(b);
					
					// пытаемся на основе накопленных данных построить пакет
					packet = PacketType(_currentPacketSourceData, _currentPacketSourceDataCount);
					// пакет полностью пришел
					bool complete = packet.Complete();
					
					if (complete)
					{
						if (packet.CrcValid())
						{
							if (pushFunctor)
							{
								pushFunctor((unsigned char *)_currentPacketSourceData, _currentPacketSourceDataCount);
							}
						}
						else
						{
							// TODO кривой пакет
							BreakPacket();
						}
						
						// начинаем новый
						BreakPacket();
					}
                }
			}
		}
	protected:
		// Начать заново
		void BreakPacket()
		{
			_currentPacketSourceDataCount = 0;
		}
		// Положить данные во внутренний буфер
		void CurrentPacketSourceDataAdd(char b)
		{
			if (_currentPacketSourceDataCount < BufferSize)
			{
				_currentPacketSourceData[_currentPacketSourceDataCount] = b;
				_currentPacketSourceDataCount++;
			}
		}
	};
}
}
}

#endif