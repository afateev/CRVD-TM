#ifndef PACKETQUEUE_H
#define PACKETQUEUE_H

#include <string.h>

namespace Rblib
{
/// \brief Элемент очереди, информация об одном пакете.
struct PacketQueueInfo
{
	/// уникальный идентификатор интерфейса
	unsigned char InterfaceId;
	/// указатель на пакет
	unsigned char* Data;
	/// длина пакета
	unsigned char Length;
};

/// Очередь пакетов.
/// \tparam BufSize Размер буфера очереди в байтах.
template<unsigned int BufSize>
class PacketQueue
{
	/// Текущий размер буфера очереди.
	static const unsigned int _BufSize = BufSize;
	/// Индекс последнего самой новой записи.
	signed int _head; 
	/// Индекс байта, предшествующего первому байту самой старой записи
	signed int _tail;
	// Индекс последнего занятого байта
	signed int _end;
	// Буфер очереди
	unsigned char _buf[_BufSize];
	// Счётчик потерянных пакетов
	unsigned int _lostPacketsCount;

  // индекс байта, перед первым занятым в массиве (аналог _end, используется для вставок в начало очереди)
  signed int _begin;

public:

	/// \brief Конструктор.
	PacketQueue()
	{
		Clear();
	}
	
	/// \brief Функция очистки всей очереди.
	void Clear()
	{
		_head = -1; 
		_tail = -1;
		_end = _BufSize - 1;
		_lostPacketsCount = 0;
    _begin = -1;
	}
	
  void PushFront(unsigned char interfaceId, unsigned char* data, unsigned char length)
  {
    //return Push(interfaceId, data, length);

    // если очередь пуста, то обычная вставка
    if (Empty())
    {
      return Push(interfaceId, data, length);
    }

    //сумма длины пакета и идентификатора интерфейса не должна превышать 255
    if(length > 254)
      return;

    unsigned int size = _BufSize - 1; // последний индекс буфера
    unsigned int demandSize = length + 2; // data + interfaceId + len

    if(demandSize > _BufSize)//если пакет больше буфера 
      return;

    if (_head > _tail // если не хватает места в начале буфера (до пакетов), и оно есть в конце буфера
        && _tail + 1 < demandSize
        && _BufSize - _head - 1 >= demandSize)
    {
      _begin = _tail; // запомнили позицию предыдущего старого пакета
      _tail = _BufSize - 1; // есть место
    }
    else
    {
      while(1)
      {
        if(_head == _tail)
        {
          _head = -1;
          _tail = -1;
          _end = size;
          break; //свободное место есть
        }

        unsigned int rem;//количество свободного места

        if(_head > _tail)
          rem = _tail + 1; // от начала буфера до начала пакетов
        else
          rem = _tail - _head - 1; // от последнего пакета до самого старого пакета - 1 байт, чтоблы не совпали _tail и _head

        if(demandSize > rem)
        {
          Pop();
          _lostPacketsCount++;
        }
        else
          break; // есть место

      }
    }

    //кладём в буфер новый пакет
    _buf[_tail - length - 1] = length + 1; // длина данных + id
    _buf[_tail - length] = interfaceId;
    memcpy(&(_buf[_tail - length + 1]), data, length);
    _tail = _tail - demandSize;
  }

	/// \brief Функция добавления пакета в конец очереди.
	void Push(unsigned char interfaceId, unsigned char* data, unsigned char length)
	{	
		//сумма длины пакета и идентификатора интерфейса не должна превышать 255
		if(length > 254)
			return;

		unsigned int size = _BufSize - 1;
		unsigned int demandSize = length + 2;

		if(demandSize > _BufSize)//если пакет больше буфера 
			return;

		while(1)//проверка и выделение свободного места для нового пакета 
		{
			if(_head == _tail)
			{
				_head = -1;
				_tail = -1;
				_end = size;
				break; //свободное место есть
			}

			unsigned int rem;//количество свободного места
	
			if(_head > _tail)
				rem = size - _head;
			else
				rem = _tail - _head - 1;

			if(demandSize > rem)//если пакету не хватает свободного места
			{
				if((_head > _tail)&&(_tail != -1))
				{
					_end = _head;
					_head = -1;
					continue;//запускаем проверку сначала
				}	
				Pop();//удаляем самый старый пакет
				_lostPacketsCount++;
			}
			else
				break;//свободное место есть
		}
	
		//кладём в буфер новый пакет
		_buf[_head + 1] = length + 1; // длина данных + id
		_buf[_head + 2] = interfaceId;
		memcpy(&(_buf[_head + 3]), data, length);
		_head = _head + demandSize;
	}
	
	/// \brief Фукция проверки пуста ли очередь.
	bool Empty()
	{
		if(_tail == _head)//если очередь пуста
	   		return true;
		return false;
	}
	
	/// \brief Функция получения указателя на очередной пакет.
	void Front(PacketQueueInfo& info)
	{
		if(_tail == _head)//если очередь пуста
		{
			info.Length = 0;
			info.InterfaceId = 0;
			info.Data = 0;
			return;
		}
	
		info.Length = _buf[_tail + 1] - 1;
		info.InterfaceId = _buf[_tail + 2];
		info.Data = &(_buf[_tail + 3]);	
	}	
	
	/// \brief Функция удаления из очереди самого старого пакета.
	void Pop()
	{
		if(_tail == _head)//если очередь пуста
	   		return;
	
		//удаляем самый старый пакет
		unsigned char len = _buf[_tail + 1] + 1;		
		_tail = _tail + len;//_tail не должен выйти за границы буфера

		if(_tail == _head)
			return;
		
		if(_tail == _end)
    {
      // если была вставка в начало очереди при недостатке места в начале буфера,
      // очередлной пакет может лежать не с начала буфера, а начиная с _begin (см. PushFront)
			_tail = _begin;
      _begin = -1;
    }
	}
	
	/// \brief Возврящает счётчик потерянных пакетов.
	unsigned int GetLostPacketsCount()
	{
		return _lostPacketsCount;
	}
	
	/// \brief Сбрасывает счётчик потерянных пакетов.
	void ClearLostPacketsCount()
	{
		_lostPacketsCount = 0;
	}
};

}

#endif //PACKETQUEUE_H
