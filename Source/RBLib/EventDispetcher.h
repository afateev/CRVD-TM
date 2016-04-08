///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������
//
// ��������, ��� ������ ������ "EventDispetcher", ����� ������ "PushEvent",
// ����������� �������� ������ �� �������� ���������. 
// ����� "PushEvent" ��������������, ��� ����� �������� �� ������ �����. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EVENTDISPETCHER_H
#define EVENTDISPETCHER_H

//#include <map>
//#include <vector>
//#include <queue>

#include <new>

#include "CriticalSection.h"
#include "Events.h"
#include "Utils\Functor.h"
#include "Utils\Singletone.h"

#ifndef __embedded_cplusplus
using namespace std;
#endif

namespace Rblib
{
	/// \brief ����� ���������� ���������.
	/// \details ����� �������� ���������� (Singletone).
	class EventDispetcher
	{
	public:
		/// ������� ��� ��������� ������ ����������� �������.
		typedef BasicFunctor<MakeTypeList<const Event &>::Result> EventFunctor;
	protected:
		/// ��������� ����������� �������.
		class HandlerInfo
		{
		public:
			/// ��� �������� � �����������.
			EventFunctor HandlerFunctor;
			/// ���������� �������������.
			long Id;
			/// ��������� ����������.
			HandlerInfo *Next;
		private:
			/// ����������� �� ���������.
			HandlerInfo()
			{}
		public:
			/// ����������� � �����������.
			/// \param handlerFunctor �����, ���������� � �����������.
			HandlerInfo(const EventFunctor& handlerFunctor) : HandlerFunctor(handlerFunctor), Next(0)
			{}
			/// ����������� �����������.
			HandlerInfo(const HandlerInfo& info) : HandlerFunctor(info.HandlerFunctor), Next(info.Next)
			{}
			/// �������� ������������.
			HandlerInfo& operator = (const HandlerInfo& info)
			{
				HandlerFunctor = info.HandlerFunctor;
				Next = info.Next;
				return *this;
			}
		};
		
		/// ������� ������ ������������ �������.
		struct EventHandlersListItem
		{
			/// ��� �������.
			EventCodeType EventCode;
			/// ������ ���������� ������������ ������� �������.
			HandlerInfo *Handlers;
			/// ������ ������ ������������ ������� �������.
			HandlerInfo *DirectHandlers;

			/// ��������� ������� ������.
			EventHandlersListItem *Next;

			EventHandlersListItem(EventCodeType eventCode) : EventCode(eventCode),
				Handlers(0), DirectHandlers(0), Next(0)
			{}
		};
		
		/// ������� �������.
		/// \details ������������ ��� ����������� ������� ���������� �������. � �������� ���� �������� ������ ������������ ��� Event.
		/// \tparam ��� ��������� ��������.
		template<class T>
		struct QueueItemStruct
		{
			/// �������� ������ (Event).
			T Item;
			/// ��������� �� ��������� ������� �������.
			QueueItemStruct *Next;
			/// �����������.
			QueueItemStruct(const T &item) : Item(item), Next(0)
			{}
		};
		
		/// ��������� �� ������� ������.
		typedef EventHandlersListItem *EventHandlersListItemPtr;
		/// ������ ������������.
		typedef EventHandlersListItemPtr EventHandlersListItemType;
		
		/// ������� ������� �������.
		typedef QueueItemStruct<Event> QueueItem;
		/// ��������� �� ������� ������� �������.
		typedef QueueItem *QueueItemPtr;
		
		/// ������ ������ ���������� �������.
		volatile QueueItemPtr _eventQueueHead;
		/// ����� ������ ���������� �������.
		volatile QueueItemPtr _eventQueueTail;
		/// ������ ������� ��������� �������, ��� ������� �������� ������, ������� ��� ����������
		/// � ����� ���� ������������ ��������, ��� ������������� ������.
		volatile QueueItemPtr _eventQueueNotUsedHead;
		/// ����� ������ ��������������� ���������.
		volatile QueueItemPtr _eventQueueNotUsedTail;
		
	protected:

		/// ����� ������������ �������.
		EventHandlersListItemType _handlersMap;
		/// ����� ����� ������������ �������.
		EventHandlersListItemType _handlersMapTail;
	
	private:

		/// ����������� �� ���������
		EventDispetcher() : _handlersMap(0), _handlersMapTail(0),
		  					_eventQueueHead(0), _eventQueueTail(0),
							_eventQueueNotUsedHead(0), _eventQueueNotUsedTail(0)
		{}
		
		/// ����������� �����������.
		EventDispetcher(const EventDispetcher&) {}

		/// �������� ������������.
		void operator = (const EventDispetcher&) {}

		/// ����������.
		~EventDispetcher()
		{
			DestroyQueue();
			Clear();
		}

		// ���� ���������
		friend class Singletone<EventDispetcher>;

	protected:

		/// �������� ������ ������ ������������ � �����.
		/// \param eventCode ��� �������, ��� �������� �������� ������ ������������.
		/// \return ��������� ������ ������������.
		inline EventHandlersListItemPtr AddEventHandlersListItemByEventCode(EventCodeType eventCode)
		{
			EventHandlersListItemPtr newItem = new(nothrow) EventHandlersListItem(eventCode);
			if (0 == newItem)
				return 0;
			if (0 == _handlersMap)
				_handlersMap = newItem;
			else
				_handlersMapTail->Next = newItem;
			_handlersMapTail = newItem;
			return newItem;
		}

		/// �������� ������ ������������ �� ���� �������.
		/// \param eventCode ��� �������, ��� �������� ���� ������ ������������ � �����.
		/// \return ������ ������������.
		inline EventHandlersListItemPtr GetEventHandlersListItemByEventCode(EventCodeType eventCode)
		{
			EventHandlersListItemPtr current = _handlersMap;
			while (0 != current)
			{
				if (eventCode == current->EventCode)
					return current;
				current = current->Next;
			}
			return 0;	
		}

		/// �������� � ������ ������������ ������� ��� ���� �������.
		/// \param list ������ ������������ �������.
		/// \param functor ����� �������-����������.
		/// \param directCall ������ ����� (true) ��� ���������� (false).
		/// \return ��������� �� ��������� ������� ������ ������������.
		inline HandlerInfo *AddHandlersListItem(EventHandlersListItemPtr list, const EventFunctor& functor, bool directCall)
		{
			HandlerInfo *newItem = new(nothrow) HandlerInfo(functor);
			if (0 == newItem)
				return 0;
			HandlerInfo *handler = GetHandlersList(list, directCall);
			if (0 == handler)
			{
				if (directCall)
					list->DirectHandlers = newItem;
				else
					list->Handlers = newItem;
				return newItem;
			}
			
			while(0 != handler->Next)
				handler = handler->Next;
			
			handler->Next = newItem;
			return newItem;
		}
		
		/// �������� ������ ������������ �� ������� ������ (������/����������).
		/// \param list ��������� �� ������� ����� ������������ �������.
		/// \param directCall ����� ������ (true) ��� ���������� (false) �����������.
		/// \return ������ ������������.
		inline HandlerInfo *GetHandlersList(EventHandlersListItemPtr list, bool directCall)
		{
			if (directCall)
				return list->DirectHandlers;
			return list->Handlers;
		}
		
		/// �������� ������ ������������ ��� �������, �������� ������������� ������� ����� ������������ �������.
		/// \param list ������� ����� ������������ �������.
		inline void DestroyHandlersListSubItems(EventHandlersListItemPtr list)
		{
			if (0 == list)
				return;

			HandlerInfo *cur = list->DirectHandlers;
			HandlerInfo *del;
			while (0 != cur)
			{
				del = cur;
				cur = cur->Next;
				delete del;
			}
			list->DirectHandlers = 0;

			cur = list->Handlers;
			while (0 != cur)
			{
				del = cur;
				cur = cur->Next;
				delete del;
			}
			list->Handlers = 0;
		}

		/// ���������� ����� ������������.
		inline void DestroyHandlersList()
		{
			EventHandlersListItemPtr cur = _handlersMap;
			EventHandlersListItemPtr del;
			while (0 != cur)
			{
				del = cur;
				DestroyHandlersListSubItems(del);
				cur = cur->Next;
				delete del;
			}

			_handlersMap = 0;
			_handlersMapTail = 0;
		}
	public:

		/// ���������������� ���������� �������.
		/// \param eventCode ��� �������.
		/// \param functor ���������� �������.
		/// \param directCall ������ ����� (true) ��� ���������� (false).
		/// \return ������������� �����������. ���� ������� -1, �� ���������� �� ��� ���������������.
		long AddHandler(EventCode eventCode, const EventFunctor& functor, bool directCall)
		{
			CriticalSection criticalSection;
			criticalSection.Enter();
			
			// ���� ������ ������������ �� ���� �������
			EventHandlersListItemPtr handlersList = GetEventHandlersListItemByEventCode(eventCode);
			// ��� ������������ ������ �������
			if (0 == handlersList)
			{
				// ���������
				handlersList = AddEventHandlersListItemByEventCode(eventCode);
			}
			// �� ���������� ��������
			if (0 == handlersList)
			{
				criticalSection.Leave();
				return -1;
			}
			HandlerInfo *handler = AddHandlersListItem(handlersList, functor, directCall);
			if (0 == handler)
			{
				criticalSection.Leave();
				return -1;
			}

			static long handlerFreeId = 0;
			long id = handlerFreeId++;
			handler->Id = id;

			criticalSection.Leave();
			return id;//������� ���������� ������������� �����������
		}
		
		/// ���������������� ���������� �������.
		/// ������������� ������� ��� ���� ������� � ���� ������ �����,
		/// ��� ������� ��� ���������������� �������.
		/// \param eventCode ��� �������.
		/// \param functor ���������� �������.
		/// \param directCall ������ ����� (true) ��� ���������� (false).
		/// \return ������������� �����������. ���� ������� -1, �� ���������� �� ��� ���������������.
		long AddHandler(unsigned int eventCode, const EventFunctor& functor, bool directCall)
		{
			return AddHandler((EventCode) eventCode, functor, directCall);
		}

		/// ������� ������ ������������ ������������������ �� ��������� �������.
		/// \param eventCode ��� �������.
		void ClearEventHandlersList(EventCode eventCode)
		{
			CriticalSection criticalSection;
			criticalSection.Enter();
			
			// ���� ������ ������������ �� ���� �������
			EventHandlersListItemPtr handlersList = GetEventHandlersListItemByEventCode(eventCode);
			// ��� ������������ ������ �������
			if (0 == handlersList)
				return;

			DestroyHandlersListSubItems(handlersList);

			criticalSection.Leave();
		}
		
		/// ������� ���������� �� ������ �� ��� ��������������.
		void ClearEventHandler(long handlerId)
		{
			CriticalSection criticalSection;
			criticalSection.Enter();
			
			EventHandlersListItemPtr cur = _handlersMap;
			HandlerInfo *prev;
			HandlerInfo *next;
			
			while (0 != cur)
			{
				prev = 0;
				next = cur->DirectHandlers;
				while(0 != next)
				{
					if (handlerId == next->Id)
					{
						if (0 != prev)
						{
							prev->Next = next->Next;
						}
						else
						{
							cur->DirectHandlers = next->Next;
						}
						delete next;
						criticalSection.Leave();
						return;
					}
					prev = next;
					next = next->Next;
				}

				prev = 0;
				next = cur->Handlers;
				while(0 != next)
				{
					if (handlerId == next->Id)
					{
						if (0 != prev)
						{
							prev->Next = next->Next;
						}
						else
						{
							cur->Handlers = next->Next;
						}
						delete next;
						criticalSection.Leave();
						return;
					}
					prev = next;
					next = next->Next;
				}
				
				cur = cur->Next;
				
			}
			criticalSection.Leave();
		}

		/// �������� ������ �����������.
		void Clear()
		{
			CriticalSection criticalSection;
			criticalSection.Enter();
			
			DestroyHandlersList();
			
			criticalSection.Leave();
		}

		/// ������������ �������.
		/// \details �������� ������ ����������� ������� � ������ ������� � ������� ���������� �������.
		/// \param e �������.
		void PushEvent(const Event &e, bool noQueueEvent = false)
		{
			// �������� �� �����������, ������� ����� ���������� ������������ ��������.
			Execute(e, true);
			
			if (noQueueEvent)
			{
				return;
			}

			CriticalSection criticalSection;
			criticalSection.Enter();
			
			// ������ ������� � �������
			
			// ����� ������� �������
			QueueItemPtr newItem;
			// ���� ���� ����� ���������� �������������� ��������
			if (0 != _eventQueueNotUsedHead)
			{
			  	// ���� �������� ����� �������
				void *place = _eventQueueNotUsedHead;
			 	// �������� ������� �� ������ ��������������
				_eventQueueNotUsedHead = _eventQueueNotUsedHead->Next;
				if (0 == _eventQueueNotUsedHead)
					_eventQueueNotUsedTail = 0;
				// ������������ ������� � ����� ���������� ������
				newItem = new(place) QueueItem(e);
			}
			else
			{
				// ���������� �������������� ��������� ���, ������� ��������
				newItem = new(nothrow) QueueItem(e);
			}
			
		  	// ��������� ������ ��������� ������
			if (0 == newItem)
			{
				criticalSection.Leave();
			  	return;
			}
			
			// ������� �����
		  	if (0 ==_eventQueueHead)
			{
				// ������� ����� ����� �������
				_eventQueueHead = newItem;
				_eventQueueTail = _eventQueueHead;
			}
			else
			{
				// ������� �� ������ ��������� � �����
			  	_eventQueueTail->Next = newItem;
				_eventQueueTail = newItem;
			}
			
			criticalSection.Leave();
		}
	
	protected:

		/// ������� ���������� ��� �������.
		/// \param e �������.
		/// \param directCall ������ (true) ��� ���������� (false) �����������.
		void Execute(const Event &e, bool directCall)
		{
			// ���� ������ ������������ �� ���� �������
			EventHandlersListItemPtr handlersList = GetEventHandlersListItemByEventCode(e.Code);
			// ��� ����� ���� ������� ������ ��� ������������
			if (0 == handlersList)
				return;
			// �������� ������ ���������� ������� ���� ������
			HandlerInfo *handler = GetHandlersList(handlersList, directCall);
			// ���� �� ���� ������������ ������� ���� ������
			while (0 != handler)
			{
				handler->HandlerFunctor(e);
				handler = handler->Next;
			}
		}

		/// �������� ������� ������� � ���������� ������.
		void DestroyQueue()
		{
			CriticalSection criticalSection;
		  	criticalSection.Enter();

		  	QueueItemPtr cur = _eventQueueHead;
			QueueItemPtr del;
		  	while (cur)
			{
				del = cur;
				cur = cur->Next;
				delete del;
			}
			
			_eventQueueHead = 0;
			_eventQueueTail = 0;
			
		  	cur = _eventQueueNotUsedHead;
		  	while (cur)
			{
				del = cur;
				cur = cur->Next;
				delete del;
			}
			
			_eventQueueNotUsedHead = 0;
			_eventQueueNotUsedTail = 0;
			criticalSection.Leave();
		}
	public:

		/// ���������� ������� �� ������� ���������� ������� (������� ���������� �����������).
		void DispatchMessages()
		{		
			CriticalSection criticalSection;
			
			criticalSection.Enter();
			QueueItemPtr old = 0;
			// ������ ������� � �������
			QueueItemPtr current = _eventQueueHead;
			criticalSection.Leave();
			
			while(0 != current)
			{
				// ��������, ������ �� ����������� ��� ������� �������� ���������� �����
				Execute(current->Item, false);
				
				criticalSection.Enter();
				
				// ������� ������� �� �������
				
				// ���� ������� ����� �������
				old = current;
				// ���������� �������� �������
				current = current->Next;
				// �� �������� ��������� �� ������ ���� ������,
				// �.�. ��� �� ����� ������� �������, � �������� � ����� ������ �������������� ���������
				// � ����� ����� ������������ ��� ������ ��������
				old->Next = 0;
				// ���� ������ �������������� ��������� ����
				if (0 == _eventQueueNotUsedHead)
				{
				  	// ���� ������� ����� "�������"
					_eventQueueNotUsedHead = old;
					_eventQueueNotUsedTail = old;
				}
				else
				{
				  	// ��������� ��������� ������ � ����� ������ ��������������
					_eventQueueNotUsedTail->Next = old;
					_eventQueueNotUsedTail = old;
				}
				
				// ��������� ��������� �� ������
				_eventQueueHead = current;
				
				// ������ ��� ���������, ������ � ��� "������"
				if (0 == _eventQueueHead)
					_eventQueueTail = 0;
				
				criticalSection.Leave();
			}
		}
	};
}

#endif
