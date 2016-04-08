///////////////////////////////////////////////////////////////////////////////
//	���������� ������������ ������ ����������
//
// ������:
// ������� �����, ��� ������� ���� �������� ������� � _nodeQueue
// ������� ������ � _tableQueue
//
// ��� ������ Enqueue ������ ���������� � _nodeQueue � _tableQueue
// ��� ������ OnTxTable ������ ����������� �� _nodeQueue � _tableQueue �� ��������� ������ � ��������� _current
//	����������� ������ ��������� � ����� ���������� ��� �������� OnTxTable
//	���� ������ �� ���������� � ���� �����, �� ������� ����� ������ ��� ��������� ������� OnTxTable
//
// ����� ���� ����� �������� �� ������������ �������� �� ������ ����� ������
// �������� ������� ���������� � ������� _nodeQueue � _tableQueue ��� ������� ��� ��� �� ���� �������� �� ���� ���� �����
//	� ����� �������� �� �������� ��� ������ OnTxTable.
// ������� ���������� � ������� ������������ � ������ ������� (��� � �����) ������������ ��������������
// ��������� �������� �������������� ����������� � _ticketQueue � ������� ��������������� ���� ������������ ����������� � ���� �������
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_ACTIVITY_ACTIVITYTABLETRANSLATOR_H
#define MODEM_ACTIVITY_ACTIVITYTABLETRANSLATOR_H

#include "../../Utils/vector_static.h"
#include "../../Utils/queue_static.h"

namespace Rblib
{
namespace Modem
{
namespace Activity
{
	template
		<
			unsigned int TableLineSize,			// ����� ������ ������� ����������
			unsigned int QueueItemsMaxCount,	// ������������ ���-�� ��������� ������� ���������� � �������
			typename TicketType = float
		>
	class ActivityTableTranslator
	{
	public:
		static const unsigned int NodesQueueSize = TableLineSize;
		static const unsigned int TableQueueSize = QueueItemsMaxCount;
		static const unsigned int TicketQueueSize = TableLineSize;

#pragma pack(push, 1)		
		struct TableQueueItem
		{
			unsigned char Node;
			unsigned char Rate;
			
			TableQueueItem()
			{
				Node = 0;
				Rate = 0;
			}
			
			TableQueueItem(unsigned char node, unsigned char rate)
			{
				Node = node;
				Rate = rate;
			}
		};
		
		struct NodeQueueItem
		{
			unsigned char Node;
			unsigned char RelayMode;
			unsigned char Count;
			bool HaveTicket;
			TicketType Ticket;
			
			NodeQueueItem(unsigned char node, unsigned char relayMode, unsigned char count)
			{
				Node = node;
				RelayMode = relayMode;
				Count = count;
				HaveTicket = false;
				Ticket = 0;
			}
			
			NodeQueueItem(unsigned char node, unsigned char relayMode, unsigned char count, TicketType ticket)
			{
				Node = node;
				RelayMode = relayMode;
				Count = count;
				HaveTicket = true;
				Ticket = ticket;
			}
		};
#pragma pack (pop)
		
		typedef vector_static<NodeQueueItem, NodesQueueSize> NodeQueueType;
		typedef queue_static<TableQueueItem, TableQueueSize> TableQueueType;
		typedef vector_static<TicketType, TicketQueueSize> TicketQueueType;
	protected:
		NodeQueueType _nodeQueue;
		TableQueueType _tableQueue;
		TicketQueueType _ticketQueue;
		unsigned char _currentTxNode;
		unsigned char _currentRelayMode;
		bool _currentTxHaveTicket;
		TicketType _currentTxTicket;
		TableQueueItem _currentTxData[TableLineSize];
		unsigned int _currentTxDataTransmited;
		unsigned int _currentTxDataCount;
	public:
		// ����������� �� ���������
		ActivityTableTranslator()
		{
			_currentTxNode = 0;
			_currentRelayMode = 0;
			_currentTxHaveTicket = false;
			_currentTxDataTransmited = 0;
			_currentTxDataCount = 0;
		}
		
		void Enqueue(unsigned char nodeId, unsigned char relayMode, unsigned char *nodes, unsigned char *rates, unsigned int itemsCount)
		{
			if (GetFreeItemsCountInNodeQueue() < 1 || GetFreeItemsCountInTableQueue() < itemsCount)
			{
				return;
			}
			
			if (NodeInQueue(nodeId))
			{
				RemoveNodeFromQueue(nodeId);
			}
			
			NodeQueueItem newItem(nodeId, relayMode, itemsCount);
			_nodeQueue.push_back(newItem);
			
			for (int i = 0; i < itemsCount; i++)
			{
				TableQueueItem item(nodes[i], rates[i]);
				_tableQueue.push(item);
			}
		}
		
		bool QueueEmpty()
		{
			return _nodeQueue.empty() || (InCurrentTxBuffer() > 0);
		}
		
		int OnTxTable(TicketType *ticket, unsigned char *owner, unsigned char *relayMode, unsigned char *nodes, unsigned char *rates,  unsigned int buffersSize)
		{
			if (QueueEmpty() || ticket == 0 || owner == 0 || relayMode == 0 || nodes == 0 || rates == 0 || buffersSize < 1)
			{
				return 0;
			}
			
			if (InCurrentTxBuffer() < 1)
			{
				DequeueFront();
			}
			
			unsigned int forTx = _currentTxDataCount - _currentTxDataTransmited;
			if (forTx > buffersSize)
			{
				forTx = buffersSize;
			}
			
			*owner = _currentTxNode;
			*relayMode = _currentRelayMode;
			// ���� �� ������� ��� ���� ���� � �������, �� ����������� ���, ������ ���� ��� ��� ������� � �������
			// ��� ��� ������� ������������ �������� ������
			// �������� ���� ������ ����������� ������ ��� ��� �����, ������� ��������� � ������� �� ����
			if (_currentTxHaveTicket)
			{
				*ticket = _currentTxTicket;
			}
			
			for (int i = 0; i < forTx; i++)
			{
				TableQueueItem item = _currentTxData[_currentTxDataTransmited + i];
				nodes[i] = item.Node;
				rates[i] = item.Rate;
			}
			
			TicketPush(*ticket);
			_currentTxDataTransmited += forTx;
			
			if (_currentTxDataTransmited >= _currentTxDataCount)
			{
				_currentTxHaveTicket = false;
				_currentTxDataCount = 0;
				_currentTxDataTransmited = 0;
			}
			
			return forTx;
		}
		
		bool OnRxActivityTable(unsigned char owner, unsigned char relayMode, TicketType ticket, unsigned char itemsCount, unsigned char *nodes, unsigned char *rates)
		{
			if (GetFreeItemsCountInNodeQueue() < 1 || GetFreeItemsCountInTableQueue() < itemsCount)
			{
				return false;
			}
			
			// ���� ������� ��� ���� � ������ ���������������, �� ���������� ��������� ������
			if (TicketInQueue(ticket))
			{
				return false;
			}
			
			bool HaveTicket = false;
			TicketType ticketInQueue;
			if (NodeInQueue(owner, &HaveTicket, &ticketInQueue))
			{
				if (HaveTicket)
				{
					// ���� ����� � ���� �� ������� ������ ��� ��������� �����, �� �������� ������� ��� ����� ���� �� ����� �����
					if (ticketInQueue < ticket)
					{
						RemoveNodeFromQueue(owner);
					}
					else
					{
						// ����� ������� ������, � ��������� �������������
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			
			NodeQueueItem newItem(owner, relayMode, itemsCount, ticket);
			_nodeQueue.push_back(newItem);
						
			for (int i = 0; i < itemsCount; i++)
			{
				TableQueueItem item(nodes[i], rates[i]);
				_tableQueue.push(item);
			}
			
			return true;
		}
	protected:
		int GetFreeItemsCountInNodeQueue()
		{
			return NodesQueueSize - _nodeQueue.size();
		}
		
		int GetFreeItemsCountInTableQueue()
		{
			return TableQueueSize - _tableQueue.size();
		}
		
		bool NodeInQueue(unsigned char nodeId, bool *haveTicket = 0, TicketType *ticket = 0)
		{
			typename NodeQueueType::iterator i;
			
			for (i = _nodeQueue.begin(); i != _nodeQueue.end(); i++)
			{
				if (i->Node == nodeId)
				{
					if (haveTicket != 0)
					{
						*haveTicket = i->HaveTicket;
					}
					
					if (ticket != 0)
					{
						*ticket = i->Ticket;
					}
					
					return true;
				}
			}
			
			return false;
		}
		
		void FrontRollOver()
		{
			if (_nodeQueue.empty())
			{
				return;
			}
			
			NodeQueueItem front = _nodeQueue.front();
			_nodeQueue.erase(_nodeQueue.begin());
			_nodeQueue.push_back(front);
			
			for (int i = 0; i < front.Count; i++)
			{
				TableQueueItem item = _tableQueue.front();
				_tableQueue.pop();
				_tableQueue.push(item);
			}
		}
		
		void FrontErase()
		{
			if (_nodeQueue.empty())
			{
				return;
			}
			
			NodeQueueItem front = _nodeQueue.front();
			_nodeQueue.erase(_nodeQueue.begin());
			
			for (int i = 0; i < front.Count; i++)
			{
				_tableQueue.pop();
			}
		}
		
		void RemoveNodeFromQueue(unsigned char nodeId)
		{
			if (_nodeQueue.empty())
			{
				return;
			}
			
			typename NodeQueueType::iterator i = _nodeQueue.begin();
			// �������� �� ����� ���� ���, ����� �� ��������� � ������ ���� nodeId ��� � �������
			int iterationCoundown = _nodeQueue.size();
			
			while (iterationCoundown > 0)
			{
				if (i->Node == nodeId)
				{
					FrontErase();
					break;
				}
				else
				{
					FrontRollOver();
					i = _nodeQueue.begin();
				}
				
				iterationCoundown--;
			}
		}
		
		void DequeueFront()
		{
			if (_nodeQueue.empty())
			{
				return;
			}
			
			NodeQueueItem front = _nodeQueue.front();
			_nodeQueue.erase(_nodeQueue.begin());
			
			_currentTxNode = front.Node;
			_currentRelayMode = front.RelayMode;
			_currentTxHaveTicket = front.HaveTicket;
			_currentTxTicket = front.Ticket;
			_currentTxDataTransmited = 0;
			_currentTxDataCount = front.Count;
			
			for (int i = 0; i < front.Count; i++)
			{
				TableQueueItem item = _tableQueue.front();
				_tableQueue.pop();
				if (i < 111)
				{
					_currentTxData[i] = item;
				}
			}
		}
		
		int InCurrentTxBuffer()
		{
			return _currentTxDataCount - _currentTxDataTransmited;
		}
		
		void TicketPush(TicketType ticket)
		{
			if (_ticketQueue.size() >= TicketQueueSize - 1)
			{
				TicketPop();
			}
			
			_ticketQueue.push_back(ticket);
		}
		
		bool TicketInQueue(TicketType ticket)
		{
			if (_ticketQueue.empty())
			{
				return false;
			}
			
			typename TicketQueueType::iterator i;
			
			for (i = _ticketQueue.begin(); i != _ticketQueue.end(); i++)
			{
				if (*i == ticket)
				{
					return true;
				}
			}
			
			return false;
		}
		
		void TicketPop()
		{
			if (_ticketQueue.empty())
			{
				return;
			}
			
			// ����������, ��� ������� ��� ������� � ������� ������ ����
			_ticketQueue.erase(_ticketQueue.begin());
			
			/* ������� ���� ������� ����� ����������� ����� ������, �� ��� �������� ����� � ����� �� �� �������� ��������� ����� � ��� ����
			typename TicketQueueType::iterator i;
			typename TicketQueueType::iterator oldest = _ticketQueue.begin();
			
			for (i = _ticketQueue.begin(); i != _ticketQueue.end(); i++)
			{
				if (*i < *oldest)
				{
					oldest = i;
				}
			}
			
			_ticketQueue.erase(oldest);
			*/
		}
	};
}
}
}

#endif