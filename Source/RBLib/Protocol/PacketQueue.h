#ifndef PACKETQUEUE_H
#define PACKETQUEUE_H

#include <string.h>

namespace Rblib
{
/// \brief ������� �������, ���������� �� ����� ������.
struct PacketQueueInfo
{
	/// ���������� ������������� ����������
	unsigned char InterfaceId;
	/// ��������� �� �����
	unsigned char* Data;
	/// ����� ������
	unsigned char Length;
};

/// ������� �������.
/// \tparam BufSize ������ ������ ������� � ������.
template<unsigned int BufSize>
class PacketQueue
{
	/// ������� ������ ������ �������.
	static const unsigned int _BufSize = BufSize;
	/// ������ ���������� ����� ����� ������.
	signed int _head; 
	/// ������ �����, ��������������� ������� ����� ����� ������ ������
	signed int _tail;
	// ������ ���������� �������� �����
	signed int _end;
	// ����� �������
	unsigned char _buf[_BufSize];
	// ������� ���������� �������
	unsigned int _lostPacketsCount;

  // ������ �����, ����� ������ ������� � ������� (������ _end, ������������ ��� ������� � ������ �������)
  signed int _begin;

public:

	/// \brief �����������.
	PacketQueue()
	{
		Clear();
	}
	
	/// \brief ������� ������� ���� �������.
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

    // ���� ������� �����, �� ������� �������
    if (Empty())
    {
      return Push(interfaceId, data, length);
    }

    //����� ����� ������ � �������������� ���������� �� ������ ��������� 255
    if(length > 254)
      return;

    unsigned int size = _BufSize - 1; // ��������� ������ ������
    unsigned int demandSize = length + 2; // data + interfaceId + len

    if(demandSize > _BufSize)//���� ����� ������ ������ 
      return;

    if (_head > _tail // ���� �� ������� ����� � ������ ������ (�� �������), � ��� ���� � ����� ������
        && _tail + 1 < demandSize
        && _BufSize - _head - 1 >= demandSize)
    {
      _begin = _tail; // ��������� ������� ����������� ������� ������
      _tail = _BufSize - 1; // ���� �����
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
          break; //��������� ����� ����
        }

        unsigned int rem;//���������� ���������� �����

        if(_head > _tail)
          rem = _tail + 1; // �� ������ ������ �� ������ �������
        else
          rem = _tail - _head - 1; // �� ���������� ������ �� ������ ������� ������ - 1 ����, ������ �� ������� _tail � _head

        if(demandSize > rem)
        {
          Pop();
          _lostPacketsCount++;
        }
        else
          break; // ���� �����

      }
    }

    //����� � ����� ����� �����
    _buf[_tail - length - 1] = length + 1; // ����� ������ + id
    _buf[_tail - length] = interfaceId;
    memcpy(&(_buf[_tail - length + 1]), data, length);
    _tail = _tail - demandSize;
  }

	/// \brief ������� ���������� ������ � ����� �������.
	void Push(unsigned char interfaceId, unsigned char* data, unsigned char length)
	{	
		//����� ����� ������ � �������������� ���������� �� ������ ��������� 255
		if(length > 254)
			return;

		unsigned int size = _BufSize - 1;
		unsigned int demandSize = length + 2;

		if(demandSize > _BufSize)//���� ����� ������ ������ 
			return;

		while(1)//�������� � ��������� ���������� ����� ��� ������ ������ 
		{
			if(_head == _tail)
			{
				_head = -1;
				_tail = -1;
				_end = size;
				break; //��������� ����� ����
			}

			unsigned int rem;//���������� ���������� �����
	
			if(_head > _tail)
				rem = size - _head;
			else
				rem = _tail - _head - 1;

			if(demandSize > rem)//���� ������ �� ������� ���������� �����
			{
				if((_head > _tail)&&(_tail != -1))
				{
					_end = _head;
					_head = -1;
					continue;//��������� �������� �������
				}	
				Pop();//������� ����� ������ �����
				_lostPacketsCount++;
			}
			else
				break;//��������� ����� ����
		}
	
		//����� � ����� ����� �����
		_buf[_head + 1] = length + 1; // ����� ������ + id
		_buf[_head + 2] = interfaceId;
		memcpy(&(_buf[_head + 3]), data, length);
		_head = _head + demandSize;
	}
	
	/// \brief ������ �������� ����� �� �������.
	bool Empty()
	{
		if(_tail == _head)//���� ������� �����
	   		return true;
		return false;
	}
	
	/// \brief ������� ��������� ��������� �� ��������� �����.
	void Front(PacketQueueInfo& info)
	{
		if(_tail == _head)//���� ������� �����
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
	
	/// \brief ������� �������� �� ������� ������ ������� ������.
	void Pop()
	{
		if(_tail == _head)//���� ������� �����
	   		return;
	
		//������� ����� ������ �����
		unsigned char len = _buf[_tail + 1] + 1;		
		_tail = _tail + len;//_tail �� ������ ����� �� ������� ������

		if(_tail == _head)
			return;
		
		if(_tail == _end)
    {
      // ���� ���� ������� � ������ ������� ��� ���������� ����� � ������ ������,
      // ���������� ����� ����� ������ �� � ������ ������, � ������� � _begin (��. PushFront)
			_tail = _begin;
      _begin = -1;
    }
	}
	
	/// \brief ���������� ������� ���������� �������.
	unsigned int GetLostPacketsCount()
	{
		return _lostPacketsCount;
	}
	
	/// \brief ���������� ������� ���������� �������.
	void ClearLostPacketsCount()
	{
		_lostPacketsCount = 0;
	}
};

}

#endif //PACKETQUEUE_H
