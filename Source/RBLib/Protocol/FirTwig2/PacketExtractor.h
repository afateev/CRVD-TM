///////////////////////////////////////////////////////////////////////////////
//	Экстрактор пакетов
//	Протокол FirTwig2
//
//	Метод ExtractPackets выделяет из переданного буфера пакет
//	и вызывает для него функцию помещения пакета в очередь,
//	которая определяется пользователем класса.
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETEXTRACTOR_FIRTWIG2_H
#define PACKETEXTRACTOR_FIRTWIG2_H

#include "../PacketExtractorCallbackTypeProvider.h"
#include "PacketFormat.h"

#include <string.h>

// Усиленный поиск ищет пакеты внутри битых пакетов.
// Тестировалось всё с помощью утилиты Караваева П.В.
// Находит больше пакетов чем нужно, по всей видимости в шумовых байтах и искажённых пакетах находятся целые пакеты
//#define INTENSIVE_SEARCH 

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{
/// Класс извлекает пакеты в формате протокола FirTwig2 из переданного буфера
template<bool functorCallback = false>
class PacketExtractor : public PacketExtractorCallbackTypeProvider<functorCallback>
{
	/// Внутренний класс для поиска заголовка пакета
	class HeadFld
	{
		unsigned char buf[3];
		unsigned char index;
		unsigned char count;
		
	public:	
		HeadFld()
		{
			index = 0;
			count = 0;
		}

		/// Поиск заголовка.
		/// \param byte очередной приянтый байт.
		/// \param data внешний буфер для выделенного пакета, в него записываются байты заголовка, если параметр modify = true.
		/// \param modify разрешает копирывание заголовка в буфер, на который указывает data.
		/// \return @c true - найден заголовок пакета, @c false - заголовок не найден.
		bool Check(unsigned char byte, unsigned char* data, bool modify = true)	
		{
			if(count < 3)
			{
				count++;
				buf[index] = byte;
				if(++index == 3)
					index = 0;
				return false;
			}
			unsigned char crc = 0x89 + buf[0] + buf[1] + buf[2];
			if(byte != crc)
			{
				buf[index] = byte;
				if(++index == 3)
					index = 0;
				return false;
			}
			if(modify)
			{
				for(char i = 0; i < 3; i++)
				{			
					data[i] = buf[index];
					if(++index == 3)
						index = 0;
				}			
				data[3] = crc;
			}			
			count = 0;
			return true;//заголовок найден
		}	
	} _headFld;

public:
    
	/// Тип функции обратного вызова для помещения пакета в очередь.
    typedef typename PacketExtractorCallbackTypeProvider<functorCallback>::CallbackType PacketReadyCallback;
    
protected:	

	/// Внутренний буфер парсера (
	unsigned char _buf[5 + 256]; //внутренний буфер парсера
	unsigned int _index; //шаг парсера
	
	enum StateParser
	{
		FindHeaderState = 0,
		FindDataState = 1,
		FindDataCrcState = 2,
	} _state; //состояние парсера
	
	static const unsigned char _lengthData = 2;
	static const unsigned char _crcHead = 3;


public:
	//конструктор с привязкой к очереди
	PacketExtractor()
	{
		_state = FindHeaderState;
	}

	/// Функция выделения пакетов и автоматического их укладывания в очередь.
	/// \param data Lанные из приемного буфера UART'а.
	/// \param length Количество данных в буфере.
	/// \param pushFunctor Функция, которая должна быть вызвана для помещения выделенного пакета в очередь.
	void ExtractPackets(unsigned char* data, unsigned int length, PacketReadyCallback pushFunctor)
	{
		unsigned int ptr = 0;
		if(length == 0)
			return;
		
		while(1)
		{
			switch(_state)
			{
			case FindHeaderState://ожидаем заголовок
				if(_headFld.Check(data[ptr++], _buf))//если заголовок найден
				{
					if(_buf[_lengthData] != 0)//если длина поля данных не равна нулю
					{
						_state = FindDataState;
						_index = 4;
					}
					else
						pushFunctor(_buf, 4);
				}
				break;
				
			case FindDataState://ожидаем данные
				{
				unsigned int len = _buf[_lengthData] - (_index - 4);//количество ожидаемых байт
				unsigned int rem = length - ptr;//количество оставшихся байт в приёмном буфере
				if(len > rem)
					len = rem;
				else
					_state = FindDataCrcState;
				memcpy(&(_buf[_index]), &(data[ptr]), len);
				_index += len;
				ptr += len;
				break;
				}

			case FindDataCrcState://ожидаем контрольную сумму пакета
				{
					unsigned char crc = _buf[_crcHead];
					unsigned char* pt = &(_buf[4]);//указатель на первый байт данных
					for(unsigned char i = 0; i < _buf[_lengthData]; i++)
						crc += *pt++;
					
					if(crc == data[ptr])//если контрольная сумма правильная
					{
						_buf[_index++] = crc;
						pushFunctor(_buf, _index);

#ifdef INTENSIVE_SEARCH
						ptr++;						
						_state = FindHeaderState;
#endif
					}					
#ifndef INTENSIVE_SEARCH
					ptr++;						
					_state = FindHeaderState;
#else					
					else
					{
						unsigned int p = 0;
						unsigned int repeatIndex = _index;
						unsigned int offset = 1;
						unsigned char* buf = &(_buf[1]);						

						_state = FindHeaderState;
						StateParser repeatState = FindHeaderState;
						
						while(1)
						{
							switch(repeatState)
							{
								case FindHeaderState://ожидаем заголовок
									if(_headFld.Check(buf[p++], buf, false))//если заголовок найден
									{
										buf += p - 4;
										if(buf[_lengthData] != 0)//если длина поля данных не равна нулю
										{
											repeatState = FindDataState;
											repeatIndex = 4;
										}
										else
										{
											pushFunctor(buf, 4);
											buf += 4;
											offset += p;
											p = 0;
											repeatIndex = 0;
										}
									}
									break;
									
								case FindDataState://ожидаем данные
								{
									unsigned int len = buf[_lengthData] - (repeatIndex - 4);//количество ожидаемых байт
									unsigned int rem = _index - (p + offset);//количество оставшихся байт в буфере
									if(len > rem)
										len = rem;
									else
										repeatState = FindDataCrcState;
									repeatIndex += len;
									p += len;
									break;
								}
								
								case FindDataCrcState://ожидаем контрольную сумму пакета
								{
									unsigned char crc = buf[_crcHead];
									unsigned char* pt = &(buf[4]);//указатель на первый байт данных
									for(unsigned char i = 0; i < buf[_lengthData]; i++)
										crc += *pt++;
					
									if(crc == buf[repeatIndex++])//если контрольная сумма правильная
									{
										pushFunctor(buf, repeatIndex);										
										buf += repeatIndex;
										offset += repeatIndex;
									}
									else
									{
										buf++;
										offset++;
									}
									p = 0;
									repeatIndex = 0;
									repeatState = FindHeaderState;
									break;
								}			
							}
							if((p + offset) >= _index)
							{
								if(repeatIndex != _index)
								{
									memmove(_buf, buf, repeatIndex);
									_index = repeatIndex;
									_state = repeatState;
								}
								break;
							}
						}
					}
#endif					
				}
				break;					
			}	
			
			if(ptr >= length)
				break;
		}
	}		
};

}
}
}

#endif // PACKETEXTRACTOR_FIRTWIG2_H