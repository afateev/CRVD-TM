///////////////////////////////////////////////////////////////////////////////
//	������� ���������� ����� ����
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_ACTIVITY_ACTIVITYTABLE_H
#define MODEM_ACTIVITY_ACTIVITYTABLE_H

#include "../../Utils/CallbackWrapper.h"
#include "SelfActivityTableLine.h"
#include "RemoteActivityTable.h"

namespace Rblib
{
namespace Modem
{
namespace Activity
{
	template
		<
			typename AddressType,					// ��� �������������� ����
			unsigned int tableLineMaxLen,			// ������������ ������ ����� ������ ������� ���������� ����
			unsigned int RemoteTableSize,			// ������������ ���������� ����� ������� ����������� � ������� ��������� �����
			int debugDataCodeStart					// ��������� ����� ����� ���������� ����������
		>
	class ActivityTable
	{
	public:
		static const unsigned int TableLineMaxLen = tableLineMaxLen;
		typedef SelfActivityTableLine<AddressType, TableLineMaxLen> ActivityTableLineType;
		typedef RemoteActivityTable<AddressType, TableLineMaxLen, RemoteTableSize> RemoteActivityTableType;
		typedef CallbackWrapper<> CallbackType;
		typedef CallbackWrapper<int> RemoteTableCallbackType;
		typedef CallbackWrapper<int, void *, int> DebugDataCallbackType;
		
		enum DebugDataCode
		{
			DebugDataCodeActivityTableRxInfo	= debugDataCodeStart
		};
		
#pragma pack(push, 1)
		struct DebugDataStruct
		{
			AddressType Source;
			AddressType Owner;
			unsigned char RelayMode;
			float Ticket;
			
			DebugDataStruct()
			{
				Source = 0xFF;
				Owner = 0xFF;
				RelayMode = 0;
				Ticket = 0;
			}
		};
#pragma pack (pop)
		
		DebugDataCallbackType DebugDataCallback;
	protected:
		ActivityTableLineType _selfActivityTableLine;					// ������ ����� �������� ��������
		RemoteActivityTableType _remoteActivityTable;					// ������� ���������� �������� ����� (�������, ������� ������ �� �����)
		unsigned int _normalActivityInterval;							// ���������� �������� ���������� (� ����� �������� (� ������ ����) ������ ����������� ����������)
		unsigned int _checkPeriod;										// ������ �������� ���������� (������� ���������� ��������� ���������� ����� ��������� �� ��������� ��������)
		unsigned int _regularDistributionPeriod;						// ������ �������� ����������� ������� ��� ��������� ��������� (���������� � �������� �������� - _checkPeriod)
		bool _running;													// ������ ��������, ������� ��������
		unsigned int _regularDistributionCounter;						// ������� ���������� �������� ����������� �������
		CallbackType _selfActivityTableChangedCallback;					// ����������, ����� ���������� ����������� ������� ����������
		CallbackType _selfActivityTableRegularDistributionCallback;		// ����������, ����� ����������� ������� ���������� ����� �� ���������� � ���� � ��������
		RemoteTableCallbackType _remoteActivityTableChangedCallback;	// ����������, ����� �� ����� �� ������� ���� ������ ������� (��� � �����) � ��� ���� ��������� ����� ���� ���������
	public:
		// ����������� �� ���������
		ActivityTable()
		{
			_selfActivityTableLine.NodeOfflineCallback.Set(OnNodeOfflineCallback, this);
			_remoteActivityTable.RemoteActivityTableChangedCallback.Set(OnRemoteActivityTableChanged, this);
			Reset();
		}
		
		// �����
		void Reset(unsigned int normalActivityInterval = 1,	unsigned int checkPeriod = 1, unsigned int regularDistributionPeriod = 1)
		{
			_normalActivityInterval = normalActivityInterval;
			_checkPeriod = checkPeriod;
			_regularDistributionPeriod = regularDistributionPeriod;
			_running = false;
			_regularDistributionCounter = 0;
			
			_selfActivityTableLine.Reset();
			_remoteActivityTable.Reset();
		}
		
		// ������ ������
		bool Start(unsigned int normalActivityInterval,	unsigned int checkPeriod, unsigned int regularDistributionPeriod)
		{
			Reset(normalActivityInterval, checkPeriod, regularDistributionPeriod);
			_running = true;
			
			return true;
		}
		
		// ���������� ������
		void Stop()
		{
			// �������� ������� �������� ����� � ��������� �����������
			_remoteActivityTable.OnShutDown();
			// ���� ����������� ������� ���� �� ������
			bool notifySelfChanged = _selfActivityTableLine.OnShutDown();
			// �� ����� ������ ��� ������ ������
			Reset();
			// ���� �� ���� ���������
			if (notifySelfChanged)
			{
				_selfActivityTableChangedCallback.Call();
			}
		}
		
		// �������� ����������, ��� ���� ������� (����������)
		void OnActivityDetected(AddressType nodeId)
		{
			if (!_running)
			{
				return;
			}
			
			_selfActivityTableLine.OnActivityDetected(nodeId, _normalActivityInterval);
		}
		
		// ���������� ���������� �� ����� ������� ���������� (��� ����� �������)
		void OnRemoteNodeTableReceived(AddressType nodeId, unsigned char relayMode, AddressType *nodes, unsigned char *rates, unsigned int itemsCount)
		{
			_remoteActivityTable.UpdateLine(nodeId, relayMode, nodes, rates, itemsCount);
		}
		
		// ���������� ���������� ���������� �� ����� ������� ����������
		void OnRemoteNodeTableReceivedDebug(AddressType source, AddressType owner, unsigned char relayMode, float ticket, AddressType *nodes, unsigned char *rates, unsigned int itemsCount)
		{
			DebugDataStruct debugData;
			debugData.Source = source;
			debugData.Owner = owner;
			debugData.RelayMode = relayMode;
			debugData.Ticket = ticket;
			DebugDataCallback(DebugDataCodeActivityTableRxInfo, &debugData, sizeof(debugData));
		}
		
		// ���������� ��������� ����� ����
		void OnEndTimeSlotChain()
		{
			if (!_running)
			{
				return;
			}
			
			_selfActivityTableLine.IncrementCheckIntervalCounters();
			bool changed = _selfActivityTableLine.CheckActivity(_normalActivityInterval, _checkPeriod);
			if (changed)
			{
				_selfActivityTableChangedCallback.Call();
				_regularDistributionCounter = 0;
			}
			
			if (_regularDistributionPeriod > 0 && _checkPeriod > 0 && _normalActivityInterval > 0)
			{
				unsigned int maxCount = _regularDistributionPeriod * _checkPeriod * _normalActivityInterval;
				
				if (_regularDistributionCounter >= maxCount)
				{
					_regularDistributionCounter = 0;
					
					_selfActivityTableRegularDistributionCallback.Call();
				}
				else
				{
					_regularDistributionCounter++;
				}
			}
			
			_remoteActivityTable.CheckUpdateTimeout(_checkPeriod * _checkPeriod * _normalActivityInterval + _normalActivityInterval);
		}
		
		// ��������� ���������� ������� ������� ����������� ������� ����������
		void SetSelfActivityTableChangedCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_selfActivityTableChangedCallback.Set(callback, callbackParam);
		}
		
		// ��������� ���������� ������� ���������� �������� ����������� ������� ����������
		void SetSelfActivityTableRegularDistributionCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_selfActivityTableRegularDistributionCallback.Set(callback, callbackParam);
		}
		
		// ��������� ���������� ������� ������� ������� ���������� ���������� ����
		void SetRemoteNodeActivityTableChangedCallback(RemoteTableCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_remoteActivityTableChangedCallback.Set(callback, callbackParam);
		}
		
		// �������� ������ ����������� ������� ����������
		int GetSelfActivityTableSize()
		{
			return _selfActivityTableLine.GetSize();
		}
		
		// �������� ������ ����������� ������� ����������
		// nodes - �������� ����� ��� ������� �����
		// rates - �������� ����� ��� ���������
		// buffersSize - ������� ������� nodes � rates, �� ������� ������ ���������
		// � ������ nodes � rates ����� ����������� �� ����� buffersSize ���������
		// ������������ ���������� ������������� ����� ���������
		int GetSelfActivityTable(AddressType *nodes, unsigned char *rates, unsigned int buffersSize)
		{
			return _selfActivityTableLine.GetTable(nodes, rates, buffersSize);
		}
		
		// �������� ������� ���������� ��� ���� �� ����������� ������� ����������
		unsigned char GetSelfActivityTableValue(AddressType node)
		{
			return _selfActivityTableLine.GetTableValue(node);
		}
		
		// ���� �� ������� ��� ��������� ���� � ������� remoteNodeId
		bool IsRemoteNodeActivityTablePresent(AddressType remoteNodeId)
		{
			return _remoteActivityTable.TablePresent(remoteNodeId);
		}
		
		// ���� �� ������� ���������� ���������� ����
		bool RemoteNodeActivityTablePresent(AddressType remoteNodeId)
		{
			return _remoteActivityTable.TablePresent(remoteNodeId);
		}
		
		// �������� ������ ������� ���������� ��������� ���� � ������� remoteNodeId
		int GetRemoteNodeActivityTableSize(AddressType remoteNodeId)
		{
			return _remoteActivityTable.GetSize(remoteNodeId);
		}
		
		// �������� ������ ������� ���������� ���������� ����
		// remoteNodeId - ������������� ��������� ����
		// nodes - �������� ����� ��� ������� �����
		// rates - �������� ����� ��� ���������
		// buffersSize - ������� ������� nodes � rates, �� ������� ������ ���������
		// � ������ nodes � rates ����� ����������� �� ����� buffersSize ���������
		// ������������ ���������� ������������� ����� ���������
		int GetRemoteNodeActivityTable(AddressType remoteNodeId, AddressType *nodes, unsigned char *rates, unsigned int buffersSize)
		{
			return _remoteActivityTable.GetTable(remoteNodeId, nodes, rates, buffersSize);
		}
		
		// �������� ������� ���������� ��� ���� �� ������� ���������� ���������� ����
		unsigned char GetRemoteActivityTableValue(AddressType remoteNodeId, AddressType node)
		{
			return _remoteActivityTable.GetTableValue(remoteNodeId, node);
		}
		
		// �������� ����� ������������ ���������� ����
		unsigned char GetRemoteNodeRelayMode(AddressType remoteNodeId)
		{
			return _remoteActivityTable.GetRelayMode(remoteNodeId);
		}
	protected:
		// ���������� ������ ����� � �����
		static void OnNodeOfflineCallback(void *callbackParam, int node)
		{
			if (0 == callbackParam)
			{
				return;
			}
		}
		
		static void OnRemoteActivityTableChanged(void *callbackParam, int node)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			ActivityTable *im = (ActivityTable *)callbackParam;
			
			im->_remoteActivityTableChangedCallback.Call(node);
		}
	};
}
}
}

#endif