///////////////////////////////////////////////////////////////////////////////
//	Таблица маршрутов узла
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_ROUTING_ROUTETABLE_H
#define MODEM_ROUTING_ROUTETABLE_H

#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Modem
{
namespace Routing
{
	template
		<
			typename AddressType						// тип идентификатора узла
		>
	struct RouteTableItem
	{
		AddressType To;
		AddressType From;
		
		RouteTableItem(AddressType to, AddressType from)
		{
			To = to;
			From = from;
		}
	};
	
	template
		<
			typename AddressType,						// тип идентификатора узла
			unsigned int maxTableSize					// максимальный размер таблицы маршрутов
		>
	class RouteTable
	{
	public:
		static const unsigned int MaxTableSize = maxTableSize;
		typedef CallbackWrapper<AddressType, AddressType *, unsigned char *, int, int *> GetLinksCallbackType;
		typedef CallbackWrapper<AddressType, unsigned char &> GetRelayModeCallbackType;
		typedef CallbackWrapper<> OnRouteTableChangedCallbackType;
		typedef RouteTableItem<AddressType> RouteTableItemType;
	public:
		GetLinksCallbackType GetLinksCallback;							// метод получения списка связей и их "длины" для конкретного узла
		GetRelayModeCallbackType GetRelayModeCallback;					// метод получения режима ретрансялции узла
		OnRouteTableChangedCallbackType OnRouteTableChangedCallback;	// обработчик события изменения маршрутной таблицы
	protected:
		bool _running;													// модуль работает, таблица строится
		bool _needUpdate;												// надо обновить таблицы
		int _n;															// сколько узлов в графе
		unsigned int _d[MaxTableSize];									// список расстояний до узлов
		static const unsigned int InfiniteDistance = 0xFFFFFFFF;		// константа для "бесконечного" расстояния
		static const unsigned int NoLinkDistanceEquivalent = 100;		// если расстояние больше или равно этому, то значит между узлами нет связи
		bool _u[MaxTableSize];											// список посещенных вершин
		AddressType _p[MaxTableSize][MaxTableSize];						// список предков (откуда _p[node][i] можно попасть в узел i), первый индекс, это для какого узла таблица слышимости
		bool _pExists[MaxTableSize][MaxTableSize];						// список вершин для которых найден маршрут
		int _self;														// "своя" вершина
		
		// временные буферы для списка связей и их длин
		AddressType _toNode[MaxTableSize];
		unsigned char _len[MaxTableSize];
		// временные буферы для сравнения новой и пердыдущей маршрутной таблицы
		AddressType _pPrev[MaxTableSize];
		bool _pExistsPrev[MaxTableSize];
	public:
		// Конструктор по умолчанию
		RouteTable()
		{
			_n = MaxTableSize;
			_self = -1;
			_running = false;
			_needUpdate = false;
			Reset();
		}
		
		// Сброс
		void Reset()
		{
			bool changed = false;
			
			if (_running)
			{
				for (int i = 0; i < _n; i++)
				{
					// если хоть до куда нибудь был маршрут, считаем что таблица изменилась
					changed |= _pExists[_self][i];
				}
			}
			
			_running = false;
			_needUpdate = false;
			
			for (int i = 0; i < _n; i++)
			{
				_pPrev[i] = 0;
				_pExistsPrev[i] = false;
			}
			ResetTables();
			_self = -1;
			
			if (changed)
			{
				OnRouteTableChangedCallback.Call();
			}
		}
		
		// Начать работу
		bool Start()
		{
			Reset();
			_running = true;
			
			return true;
		}
		
		// Остановить работу
		void Stop()
		{
			Reset();
		}
		
		// Изменились таблицы активности
		void OnActivityTablesChanged()
		{
			if (_running)
			{
				_needUpdate = true;
			}
		}
		
		// Конец цикла сети
		void OnEndTimeSlotChain(AddressType startVertex)
		{
			bool changed = false;
			if (_running && _needUpdate)
			{
				_needUpdate = false;
				// запоминаем какая вершина является "своей"
				_self = startVertex;
				for (int i = 0; i < MaxTableSize; i++)
				{
					// обновляем все строки
					// но изменения интересны только для своей маршрутной таблицы
					changed |= UpdateRoutes(i) && i == _self;
				}
				
				if (changed)
				{
					OnRouteTableChangedCallback.Call();
				}
			}
		}
		
		// Количество доступных узлов или размер маршрутной таблицы
		int GetRoutesCount()
		{
			int res = 0;
			
			if (_self < 0 || _self >= MaxTableSize)
			{
				return res;
			}
			
			for (int i = 0; i < _n; i++)
			{
				if (_pExists[_self][i])
				{
					res++;
				}
			}
			
			return res;
		}
		
		// Получить маршрутную таблицу
		//	to - список узлов до которых есть маршрут
		// from - соответсвующие списку to узлы через которые лежат пути до узлов to, т.е. оптимальный маршрут до узла to[i] лежит через узел from[i]
		// buffersSize - размеры приемных буферов to и from
		// возвращается количество фактически заполенных элементов буферов to и from
		int GetRouteTable(AddressType *to, AddressType *from, unsigned int buffersSize)
		{
			if (to == 0 || buffersSize < 1 || _self < 0 || _self >= MaxTableSize)
			{
				return 0;
			}
			
			int dst = 0;
			for (int i = 0; i < _n && dst < buffersSize; i++)
			{
				if (_pExists[_self][i])
				{
					to[dst] = i;
					if (from != 0)
					{
						from[dst] = _p[_self][i];
					}
					dst++;
				}
			}
			
			return dst;
		}
		
		int GetRouteToNode(AddressType from, AddressType to, AddressType *buffer, int bufferLen)
		{
			if (from >= MaxTableSize || to >= MaxTableSize || buffer == 0 || bufferLen < 1)
			{
				return 0;
			}
			
			int routeLen = 0;
			AddressType cur = from;
			int firstPartSize = 0;
			
			// проверяем что маршрут до вершины to существует
			if (!_pExists[from][to])
			{
				return 0;
			}
			
			cur = to;
			
			while(cur != from)
			{
				if (routeLen < bufferLen)
				{
					buffer[routeLen] = cur;
				}
				routeLen++;
				cur = _p[from][cur];
				
				// чтобы не зациклиться при каком то невероятном косяке
				// маршрут не может быть длиннее размера маршрутной таблицы
				if (routeLen > MaxTableSize)
				{
					return 0;
				}
			}
			
			// переставляем всё наоборот, но только во второй части (первая нужна как есть)
			
			// если даже первая часть не помещается в буфер, то продолжать нет смысла
			if (firstPartSize >= bufferLen)
			{
				return 0;
			}
			
			// пододвигем буфер за пределы первой части
			buffer += firstPartSize;
			bufferLen -= firstPartSize;
			
			// сколько в буфере с учётом ограничения длины буфера и размера первой части
			int bufferCount = routeLen - firstPartSize;
			if (bufferCount > bufferLen)
			{
				bufferCount = bufferLen;
			}
			
			// сколько надо перестановок
			int reversCount = bufferCount;
			// если нечетное число элементов, то центральный элемент не надо переставлять с самим собой
			if (reversCount % 2 > 0)
			{
				reversCount--;
			}
			// итого перестановок половина от длины
			reversCount /= 2;
			// реверс
			for (int i = 0; i < reversCount; i++)
			{
				AddressType tmp = buffer[i];
				buffer[i] = buffer[bufferCount - 1 - i];
				buffer[bufferCount - 1 - i] = tmp;
			}
			// возвращяем реальную длину маршрута
			return routeLen;
		}
		
		bool RouteToNodePresent(AddressType id)
		{
			bool res = false;
			
			if (_running)
			{
				if (id >= 0 || id < _n)
				{
					res = _pExists[_self][id];
				}
			}
			
			return res;
		}
	protected:
		// Подготовка таблиц алгоритма к расчётам
		void ResetTables()
		{
			for (int j = 0; j < _n; j++)
			for (int i = 0; i < _n; i++)
			{
				ResetItem(i, j);
			}
		}
		
		void ResetItem(int i, int j)
		{
			_d[i] = InfiniteDistance;
			_u[i] = false;
			_p[j][i] = 0;
			_pExists[j][i] = false;
		}
		
		// Перестроить маршрутную таблицу
		bool UpdateRoutes(AddressType startVertex)
		{
			// сохраняем копию маршрутов, чтобы можно было выявить изменения
			for (int i = 0; i < _n; i++)
			{
				_pPrev[i] = _p[startVertex][i];
				_pExistsPrev[i] = _pExists[startVertex][i];
				// заодно в этом же цикле делаем очистку таблиц
				ResetItem(i, startVertex);
			}
			
			if (startVertex >= _n)
			{
				return false;
			}
			
			_d[startVertex] = 0;
			
			for (int i = 0; i < _n; i++)
			{
				bool anyOne = false;
				AddressType v = 0;
				for (int j = 0; j < _n; j++)
				{
					if (!_u[j])
					{
						if (!anyOne)
						{
							v = j;
							anyOne = true;
						}
						else
						{
							if (_d[j] < _d[v])
							{
								v = j;
							}
						}
					}
				}
				
				if (!anyOne)
				{
					break;
				}
				
				if (_d[v] == InfiniteDistance)
				{
					break;
				}
				
				_u[v] = true;
				
				int itemsCount = 0;
				GetLinksCallback.Call(v, _toNode, _len, MaxTableSize, &itemsCount);
				
				// если уже не первое звено передачи данных
				if (i > 0)
				{
					// то проверяем может ли быть узел v ретранслятором
					unsigned char relayMode = 0;
					GetRelayModeCallback.Call(v, relayMode);
					
					// если не ретранслятор
					if (relayMode == 0)
					{
						// считаем что никого не слышит
						itemsCount = 0;
					}
				}
				
				
				for (int j = 0; j < itemsCount; j++)
				{
					AddressType to = _toNode[j];
					if (to < MaxTableSize)
					{
						
						int len = _len[j];
						if (len < NoLinkDistanceEquivalent)
						{
							if (_d[v] + len < _d[to])
							{
								_d[to] = _d[v] + len;
								_p[startVertex][to] = v;
								_pExists[startVertex][to] = true;
							}
						}
					}
				}
			}
			
			// сравниваем новое состояние с предыдущим
			bool changed = false;
			for (int i = 0; i < _n; i++)
			{
				if (_pPrev[i] != _p[startVertex][i] || _pExistsPrev[i] != _pExists[startVertex][i])
				{
					changed = true;
					break;
				}
			}
			
			return changed;
		}
	};
}
}
}

#endif