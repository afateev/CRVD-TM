///////////////////////////////////////////////////////////////////////////////
//	Экстрактор пакетов
//	Протокол BigWAKE
//
//	Метод ExtractPackets выделяет из переданного буфера пакет
//	и вызывает для него функцию помещения пакета в очередь,
//	которая определяется пользователем класса.
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETEXTRACTOR_BIGWAKE_H
#define PACKETEXTRACTOR_BIGWAKE_H

#include "../PacketExtractorCallbackTypeProvider.h"
#include "Packet.h"

namespace Rblib
{
namespace Protocol
{
namespace BigWake
{
	template<bool functorCallback = false, unsigned int bufferSize = 1050> // bufferSize немного больше чем 1024 на обвеску
	class PacketExtractor : public PacketExtractorCallbackTypeProvider<functorCallback>
	{
	public:
    	/// Тип функции обратного вызова для помещения пакета в очередь.
		typedef typename PacketExtractorCallbackTypeProvider<functorCallback>::CallbackType PacketReadyCallback;
		// Размер внутреннего буфера
		static const unsigned int BufferSize = bufferSize;
		// Максимальный размер пакета
		static const unsigned int MaxPacketSize = BufferSize > 0xFFFF ? 0xFFFF : BufferSize;
		// Тип пакета
		typedef Packet<MaxPacketSize> PacketType;
	protected:
		unsigned char _currentPacketSourceData[BufferSize];		// буфер под входящие данные со стаффингом
		unsigned int _currentPacketSourceDataCount;				// сколько днных во входящем буфере
		PacketType packet;										// текущий разбираемый пакет
	public:
		// Конструктор по умолчанию
		PacketExtractor()
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
				if (b == PacketType::FrameStart)
				{
					// если уже есть часть пакета
					if (_currentPacketSourceDataCount > 1)
					{
						// то на этом текущий прерывается
						BreakPacket();
					}

					// первый байт пакета
					CurrentPacketSourceDataAdd(b);
				}
				else
                {
                    // если нет ни одного байта, значит FrameStart ещё не было и пока накапливать пакет нет смысла
                    if (_currentPacketSourceDataCount < 1)
                    {
                        continue;
                    }
                    
                    // следующий байт
                    CurrentPacketSourceDataAdd(b);
					
					// пытаемся на основе накопленных данных построить пакет
					packet.FromByteArray(_currentPacketSourceData, _currentPacketSourceDataCount);
					// пакет полностью пришел
					bool complete = packet.HaveFrameStart();
					
					if (complete)
					{
						complete &= packet.HaveCommand();
					}

					if (complete)
					{
						complete &= packet.HaveDataLen();
					}

					if (complete && packet.DataLen() > 0)
					{
						complete &= packet.HaveData();
					}

					if (complete)
					{
						complete &= packet.HaveCrc();
					}
					
					if (complete)
					{
						if (packet.CrcValid())
						{
							if (pushFunctor)
							{
								pushFunctor(_currentPacketSourceData, _currentPacketSourceDataCount);
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
		void CurrentPacketSourceDataAdd(unsigned char b)
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