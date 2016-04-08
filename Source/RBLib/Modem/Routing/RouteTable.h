///////////////////////////////////////////////////////////////////////////////
//	������� ��������� ����
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
			typename AddressType						// ��� �������������� ����
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
			typename AddressType,						// ��� �������������� ����
			unsigned int maxTableSize					// ������������ ������ ������� ���������
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
		GetLinksCallbackType GetLinksCallback;							// ����� ��������� ������ ������ � �� "�����" ��� ����������� ����
		GetRelayModeCallbackType GetRelayModeCallback;					// ����� ��������� ������ ������������ ����
		OnRouteTableChangedCallbackType OnRouteTableChangedCallback;	// ���������� ������� ��������� ���������� �������
	protected:
		bool _running;													// ������ ��������, ������� ��������
		bool _needUpdate;												// ���� �������� �������
		int _n;															// ������� ����� � �����
		unsigned int _d[MaxTableSize];									// ������ ���������� �� �����
		static const unsigned int InfiniteDistance = 0xFFFFFFFF;		// ��������� ��� "������������" ����������
		static const unsigned int NoLinkDistanceEquivalent = 100;		// ���� ���������� ������ ��� ����� �����, �� ������ ����� ������ ��� �����
		bool _u[MaxTableSize];											// ������ ���������� ������
		AddressType _p[MaxTableSize][MaxTableSize];						// ������ ������� (������ _p[node][i] ����� ������� � ���� i), ������ ������, ��� ��� ������ ���� ������� ����������
		bool _pExists[MaxTableSize][MaxTableSize];						// ������ ������ ��� ������� ������ �������
		int _self;														// "����" �������
		
		// ��������� ������ ��� ������ ������ � �� ����
		AddressType _toNode[MaxTableSize];
		unsigned char _len[MaxTableSize];
		// ��������� ������ ��� ��������� ����� � ���������� ���������� �������
		AddressType _pPrev[MaxTableSize];
		bool _pExistsPrev[MaxTableSize];
	public:
		// ����������� �� ���������
		RouteTable()
		{
			_n = MaxTableSize;
			_self = -1;
			_running = false;
			_needUpdate = false;
			Reset();
		}
		
		// �����
		void Reset()
		{
			bool changed = false;
			
			if (_running)
			{
				for (int i = 0; i < _n; i++)
				{
					// ���� ���� �� ���� ������ ��� �������, ������� ��� ������� ����������
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
		
		// ������ ������
		bool Start()
		{
			Reset();
			_running = true;
			
			return true;
		}
		
		// ���������� ������
		void Stop()
		{
			Reset();
		}
		
		// ���������� ������� ����������
		void OnActivityTablesChanged()
		{
			if (_running)
			{
				_needUpdate = true;
			}
		}
		
		// ����� ����� ����
		void OnEndTimeSlotChain(AddressType startVertex)
		{
			bool changed = false;
			if (_running && _needUpdate)
			{
				_needUpdate = false;
				// ���������� ����� ������� �������� "�����"
				_self = startVertex;
				for (int i = 0; i < MaxTableSize; i++)
				{
					// ��������� ��� ������
					// �� ��������� ��������� ������ ��� ����� ���������� �������
					changed |= UpdateRoutes(i) && i == _self;
				}
				
				if (changed)
				{
					OnRouteTableChangedCallback.Call();
				}
			}
		}
		
		// ���������� ��������� ����� ��� ������ ���������� �������
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
		
		// �������� ���������� �������
		//	to - ������ ����� �� ������� ���� �������
		// from - �������������� ������ to ���� ����� ������� ����� ���� �� ����� to, �.�. ����������� ������� �� ���� to[i] ����� ����� ���� from[i]
		// buffersSize - ������� �������� ������� to � from
		// ������������ ���������� ���������� ���������� ��������� ������� to � from
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
			
			// ��������� ��� ������� �� ������� to ����������
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
				
				// ����� �� ����������� ��� ����� �� ����������� ������
				// ������� �� ����� ���� ������� ������� ���������� �������
				if (routeLen > MaxTableSize)
				{
					return 0;
				}
			}
			
			// ������������ �� ��������, �� ������ �� ������ ����� (������ ����� ��� ����)
			
			// ���� ���� ������ ����� �� ���������� � �����, �� ���������� ��� ������
			if (firstPartSize >= bufferLen)
			{
				return 0;
			}
			
			// ���������� ����� �� ������� ������ �����
			buffer += firstPartSize;
			bufferLen -= firstPartSize;
			
			// ������� � ������ � ������ ����������� ����� ������ � ������� ������ �����
			int bufferCount = routeLen - firstPartSize;
			if (bufferCount > bufferLen)
			{
				bufferCount = bufferLen;
			}
			
			// ������� ���� ������������
			int reversCount = bufferCount;
			// ���� �������� ����� ���������, �� ����������� ������� �� ���� ������������ � ����� �����
			if (reversCount % 2 > 0)
			{
				reversCount--;
			}
			// ����� ������������ �������� �� �����
			reversCount /= 2;
			// ������
			for (int i = 0; i < reversCount; i++)
			{
				AddressType tmp = buffer[i];
				buffer[i] = buffer[bufferCount - 1 - i];
				buffer[bufferCount - 1 - i] = tmp;
			}
			// ���������� �������� ����� ��������
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
		// ���������� ������ ��������� � ��������
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
		
		// ����������� ���������� �������
		bool UpdateRoutes(AddressType startVertex)
		{
			// ��������� ����� ���������, ����� ����� ���� ������� ���������
			for (int i = 0; i < _n; i++)
			{
				_pPrev[i] = _p[startVertex][i];
				_pExistsPrev[i] = _pExists[startVertex][i];
				// ������ � ���� �� ����� ������ ������� ������
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
				
				// ���� ��� �� ������ ����� �������� ������
				if (i > 0)
				{
					// �� ��������� ����� �� ���� ���� v ��������������
					unsigned char relayMode = 0;
					GetRelayModeCallback.Call(v, relayMode);
					
					// ���� �� ������������
					if (relayMode == 0)
					{
						// ������� ��� ������ �� ������
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
			
			// ���������� ����� ��������� � ����������
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