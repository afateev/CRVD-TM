///////////////////////////////////////////////////////////////////////////////
//	Класс управления событиями
//
// Внимание, все методы класса "EventDispetcher", кроме метода "PushEvent",
// разрешается вызывать только из основной программы. 
// Метод "PushEvent" покобезопасный, его можно вызывать из любого места. 
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
	/// \brief Класс управления событиями.
	/// \details Класс является синглтоном (Singletone).
	class EventDispetcher
	{
	public:
		/// Функтор для обратного вызова обработчика события.
		typedef BasicFunctor<MakeTypeList<const Event &>::Result> EventFunctor;
	protected:
		/// Описатель обработчика событий.
		class HandlerInfo
		{
		public:
			/// Что вызывать у обработчика.
			EventFunctor HandlerFunctor;
			/// уникальный идентификатор.
			long Id;
			/// следующий обработчик.
			HandlerInfo *Next;
		private:
			/// Конструктор по умолчанию.
			HandlerInfo()
			{}
		public:
			/// Конструктор с параметрами.
			/// \param handlerFunctor Метод, вызываемый у обработчика.
			HandlerInfo(const EventFunctor& handlerFunctor) : HandlerFunctor(handlerFunctor), Next(0)
			{}
			/// Конструктор копирования.
			HandlerInfo(const HandlerInfo& info) : HandlerFunctor(info.HandlerFunctor), Next(info.Next)
			{}
			/// Оператор присваивания.
			HandlerInfo& operator = (const HandlerInfo& info)
			{
				HandlerFunctor = info.HandlerFunctor;
				Next = info.Next;
				return *this;
			}
		};
		
		/// Элемент списка обработчиков событий.
		struct EventHandlersListItem
		{
			/// Код события.
			EventCodeType EventCode;
			/// Список отложенных обработчиков данного события.
			HandlerInfo *Handlers;
			/// Список прямых обработчиков данного события.
			HandlerInfo *DirectHandlers;

			/// Следующий элемент списка.
			EventHandlersListItem *Next;

			EventHandlersListItem(EventCodeType eventCode) : EventCode(eventCode),
				Handlers(0), DirectHandlers(0), Next(0)
			{}
		};
		
		/// Элемент очереди.
		/// \details Используется для организации очереди отложенных событий. В качестве типа хранимых дынных используется тип Event.
		/// \tparam Тип хранимого элемента.
		template<class T>
		struct QueueItemStruct
		{
			/// Хранимые данные (Event).
			T Item;
			/// Указатель на следующий элемент очереди.
			QueueItemStruct *Next;
			/// Конструктор.
			QueueItemStruct(const T &item) : Item(item), Next(0)
			{}
		};
		
		/// Указатель на элемент списка.
		typedef EventHandlersListItem *EventHandlersListItemPtr;
		/// Список обработчиков.
		typedef EventHandlersListItemPtr EventHandlersListItemType;
		
		/// Элемент очереди событий.
		typedef QueueItemStruct<Event> QueueItem;
		/// Указатель на элемент очереди событий.
		typedef QueueItem *QueueItemPtr;
		
		/// Голова списка отложенных событий.
		volatile QueueItemPtr _eventQueueHead;
		/// Хвост списка отложенных событий.
		volatile QueueItemPtr _eventQueueTail;
		/// Голова списока элементов очереди, для которых выделена память, которые уже обработаны
		/// и могут быть использованы повторно, без перевыделения памяти.
		volatile QueueItemPtr _eventQueueNotUsedHead;
		/// Хвост списка переиспльзуемых элементов.
		volatile QueueItemPtr _eventQueueNotUsedTail;
		
	protected:

		/// Карта обработчиков событий.
		EventHandlersListItemType _handlersMap;
		/// Хвост карты обработчиков событий.
		EventHandlersListItemType _handlersMapTail;
	
	private:

		/// Конструткор по умолчанию
		EventDispetcher() : _handlersMap(0), _handlersMapTail(0),
		  					_eventQueueHead(0), _eventQueueTail(0),
							_eventQueueNotUsedHead(0), _eventQueueNotUsedTail(0)
		{}
		
		/// Конструктор копирования.
		EventDispetcher(const EventDispetcher&) {}

		/// Оператор присваивания.
		void operator = (const EventDispetcher&) {}

		/// Деструктор.
		~EventDispetcher()
		{
			DestroyQueue();
			Clear();
		}

		// Друг синглетон
		friend class Singletone<EventDispetcher>;

	protected:

		/// Добавить пустой список обработчиков в карту.
		/// \param eventCode Код события, для которого создаётся список обработчиков.
		/// \return Созданный список обработчиков.
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

		/// Получить список обработчиков по коду события.
		/// \param eventCode Код события, для которого ищем список обработчиков в карте.
		/// \return Список обработчиков.
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

		/// Добавить в список обработчиков события ещё один элемент.
		/// \param list Список обработчиков события.
		/// \param functor Новый функтор-обработчик.
		/// \param directCall прямой вызов (true) или отложенный (false).
		/// \return Указатель на созданный элемент списка обработчиков.
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
		
		/// Получить список обработчиков по способу вызова (прямой/отложенный).
		/// \param list Указатель на элемент карты обработчиков событий.
		/// \param directCall Нужны прямые (true) или отложенные (false) обработчики.
		/// \return Список обработчиков.
		inline HandlerInfo *GetHandlersList(EventHandlersListItemPtr list, bool directCall)
		{
			if (directCall)
				return list->DirectHandlers;
			return list->Handlers;
		}
		
		/// Очистить списки обработчиков для события, которому соответствует элемент карты обработчиков событий.
		/// \param list Элемент карты обработчиков событий.
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

		/// Уничтожить карту обработчиков.
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

		/// Зарегистрировать обработчик события.
		/// \param eventCode Код события.
		/// \param functor Обработчик события.
		/// \param directCall Прямой вызов (true) или отложенный (false).
		/// \return Идентификатор обработчика. Если вернётся -1, то обработчик не был зарегистрирован.
		long AddHandler(EventCode eventCode, const EventFunctor& functor, bool directCall)
		{
			CriticalSection criticalSection;
			criticalSection.Enter();
			
			// ищем список обработчиков по коду события
			EventHandlersListItemPtr handlersList = GetEventHandlersListItemByEventCode(eventCode);
			// нет обработчиков такого события
			if (0 == handlersList)
			{
				// добавляем
				handlersList = AddEventHandlersListItemByEventCode(eventCode);
			}
			// не получилось добавить
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
			return id;//вернуть уникальный индетификатор обработчика
		}
		
		/// Зарегистрировать обработчик события.
		/// Перегруженная функция для кода события в виде целого числа,
		/// так удобнее для пользовательских событий.
		/// \param eventCode Код события.
		/// \param functor Обработчик события.
		/// \param directCall Прямой вызов (true) или отложенный (false).
		/// \return Идентификатор обработчика. Если вернётся -1, то обработчик не был зарегистрирован.
		long AddHandler(unsigned int eventCode, const EventFunctor& functor, bool directCall)
		{
			return AddHandler((EventCode) eventCode, functor, directCall);
		}

		/// Удалить список обработчиков зарегистрированных на указанное событие.
		/// \param eventCode Код события.
		void ClearEventHandlersList(EventCode eventCode)
		{
			CriticalSection criticalSection;
			criticalSection.Enter();
			
			// ищем список обработчиков по коду события
			EventHandlersListItemPtr handlersList = GetEventHandlersListItemByEventCode(eventCode);
			// нет обработчиков такого события
			if (0 == handlersList)
				return;

			DestroyHandlersListSubItems(handlersList);

			criticalSection.Leave();
		}
		
		/// Удалить обработчик из списка по его идентификатору.
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

		/// Очистить список обработчков.
		void Clear()
		{
			CriticalSection criticalSection;
			criticalSection.Enter();
			
			DestroyHandlersList();
			
			criticalSection.Leave();
		}

		/// Сгенерирвать событие.
		/// \details Вызывает прямые обработчики события и ставит событие в очередь отложенных событий.
		/// \param e Событие.
		void PushEvent(const Event &e, bool noQueueEvent = false)
		{
			// Вызываем те обработчики, которые хотят немедленно обрабатывать сообытие.
			Execute(e, true);
			
			if (noQueueEvent)
			{
				return;
			}

			CriticalSection criticalSection;
			criticalSection.Enter();
			
			// Ставим событие в очередь
			
			// новый элемент очереди
			QueueItemPtr newItem;
			// Если есть ранее выделенные неиспользуемые элементы
			if (0 != _eventQueueNotUsedHead)
			{
			  	// сюда поместим новый элемент
				void *place = _eventQueueNotUsedHead;
			 	// забираем элемент из списка неиспользуемых
				_eventQueueNotUsedHead = _eventQueueNotUsedHead->Next;
				if (0 == _eventQueueNotUsedHead)
					_eventQueueNotUsedTail = 0;
				// конструируем элемент в ранее выделенной памяти
				newItem = new(place) QueueItem(e);
			}
			else
			{
				// выделенных неиспользуемых элементов нет, придётся выделять
				newItem = new(nothrow) QueueItem(e);
			}
			
		  	// Произошла ошибка выделения памяти
			if (0 == newItem)
			{
				criticalSection.Leave();
			  	return;
			}
			
			// Очередь пуста
		  	if (0 ==_eventQueueHead)
			{
				// Головой будет новый элемент
				_eventQueueHead = newItem;
				_eventQueueTail = _eventQueueHead;
			}
			else
			{
				// Очередь не пустая добавляем в хвост
			  	_eventQueueTail->Next = newItem;
				_eventQueueTail = newItem;
			}
			
			criticalSection.Leave();
		}
	
	protected:

		/// Вызвать обработчки для события.
		/// \param e Событие.
		/// \param directCall Прямые (true) или отложенные (false) обработчики.
		void Execute(const Event &e, bool directCall)
		{
			// ищем список обработчиков по коду события
			EventHandlersListItemPtr handlersList = GetEventHandlersListItemByEventCode(e.Code);
			// для этого кода события вообще нет обработчиков
			if (0 == handlersList)
				return;
			// получаем первый обработчик нужного типа вызова
			HandlerInfo *handler = GetHandlersList(handlersList, directCall);
			// цикл по всем обработчикам нужного типа вызова
			while (0 != handler)
			{
				handler->HandlerFunctor(e);
				handler = handler->Next;
			}
		}

		/// Очистить очередь событий и освободить память.
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

		/// Обработать события из очереди отложенных событий (Вызвать отложенные обработчики).
		void DispatchMessages()
		{		
			CriticalSection criticalSection;
			
			criticalSection.Enter();
			QueueItemPtr old = 0;
			// первое событие в очереди
			QueueItemPtr current = _eventQueueHead;
			criticalSection.Leave();
			
			while(0 != current)
			{
				// вызываем, только те обработчики для которых назначен отложенный вызов
				Execute(current->Item, false);
				
				criticalSection.Enter();
				
				// удаляем событие из очереди
				
				// этот элемент будем удалять
				old = current;
				// запоминаем следующиё элемент
				current = current->Next;
				// за удалённым элементом не должно быть ничего,
				// т.к. его не будем реально удалять, а поместим в конец списка неиспользуемых элементов
				// и потом будем использовать его память повторно
				old->Next = 0;
				// если список неиспользуемых элементов пуст
				if (0 == _eventQueueNotUsedHead)
				{
				  	// этот элемент будет "головой"
					_eventQueueNotUsedHead = old;
					_eventQueueNotUsedTail = old;
				}
				else
				{
				  	// добавляем удаляемый эемент в хвост списка неиспользуемых
					_eventQueueNotUsedTail->Next = old;
					_eventQueueNotUsedTail = old;
				}
				
				// обновляем указатель на голову
				_eventQueueHead = current;
				
				// больше нет элементов, значит и нет "хвоста"
				if (0 == _eventQueueHead)
					_eventQueueTail = 0;
				
				criticalSection.Leave();
			}
		}
	};
}

#endif
