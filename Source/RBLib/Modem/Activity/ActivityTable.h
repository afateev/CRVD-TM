///////////////////////////////////////////////////////////////////////////////
//	Таблица активности узлов сети
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
			typename AddressType,					// тип идентификатора узла
			unsigned int tableLineMaxLen,			// максимальный размер одной строки таблицы активности узла
			unsigned int RemoteTableSize,			// максимальное количество строк которое сохраняется в таблице удаленных узлов
			int debugDataCodeStart					// стартовый номер кодов отладочной информации
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
		ActivityTableLineType _selfActivityTableLine;					// список узлов слышимых напрямую
		RemoteActivityTableType _remoteActivityTable;					// тыблицы активности удалённых узлов (таблицы, которые пришли по радио)
		unsigned int _normalActivityInterval;							// нормальный интервал активности (с каким периодом (в циклах сети) должна наблюдаться активность)
		unsigned int _checkPeriod;										// период проверки активности (сколько нормальных итервалов активности нужно подождать до очередной проверки)
		unsigned int _regularDistributionPeriod;						// период рассылки собственной таблицы при отсутсвии изменений (измеряется в периодах проверки - _checkPeriod)
		bool _running;													// модуль работает, таблицы строятся
		unsigned int _regularDistributionCounter;						// счётчик регулярной рассылки собственной таблицы
		CallbackType _selfActivityTableChangedCallback;					// вызывается, когда изменилась собственная таблица активности
		CallbackType _selfActivityTableRegularDistributionCallback;		// вызывается, когда собственная таблица активности давно не изменялась и пора её отослать
		RemoteTableCallbackType _remoteActivityTableChangedCallback;	// вызывается, когда по радио от другого узла пришла таблица (или её часть) и при этом произошли какие либо изменения
	public:
		// Конструктор по умолчанию
		ActivityTable()
		{
			_selfActivityTableLine.NodeOfflineCallback.Set(OnNodeOfflineCallback, this);
			_remoteActivityTable.RemoteActivityTableChangedCallback.Set(OnRemoteActivityTableChanged, this);
			Reset();
		}
		
		// Сброс
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
		
		// Начать работу
		bool Start(unsigned int normalActivityInterval,	unsigned int checkPeriod, unsigned int regularDistributionPeriod)
		{
			Reset(normalActivityInterval, checkPeriod, regularDistributionPeriod);
			_running = true;
			
			return true;
		}
		
		// Остановить работу
		void Stop()
		{
			// почистим таблицу удаленны узлов с рассылкой уведомлений
			_remoteActivityTable.OnShutDown();
			// если собственная таблица была не пустой
			bool notifySelfChanged = _selfActivityTableLine.OnShutDown();
			// то после сброса она станет пустой
			Reset();
			// надо об этом уведомить
			if (notifySelfChanged)
			{
				_selfActivityTableChangedCallback.Call();
			}
		}
		
		// Получена информация, что узел активен (синхопакет)
		void OnActivityDetected(AddressType nodeId)
		{
			if (!_running)
			{
				return;
			}
			
			_selfActivityTableLine.OnActivityDetected(nodeId, _normalActivityInterval);
		}
		
		// Обработчик полученной по радио таблицы активности (или части таблицы)
		void OnRemoteNodeTableReceived(AddressType nodeId, unsigned char relayMode, AddressType *nodes, unsigned char *rates, unsigned int itemsCount)
		{
			_remoteActivityTable.UpdateLine(nodeId, relayMode, nodes, rates, itemsCount);
		}
		
		// Отладочный обработчик полученной по радио таблицы активности
		void OnRemoteNodeTableReceivedDebug(AddressType source, AddressType owner, unsigned char relayMode, float ticket, AddressType *nodes, unsigned char *rates, unsigned int itemsCount)
		{
			DebugDataStruct debugData;
			debugData.Source = source;
			debugData.Owner = owner;
			debugData.RelayMode = relayMode;
			debugData.Ticket = ticket;
			DebugDataCallback(DebugDataCodeActivityTableRxInfo, &debugData, sizeof(debugData));
		}
		
		// Обработчик окончания цикла сети
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
		
		// Назначить обработчик события измения собственной таблицы активности
		void SetSelfActivityTableChangedCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_selfActivityTableChangedCallback.Set(callback, callbackParam);
		}
		
		// Назначить обработчик события регулярной отправки собственной таблицы активности
		void SetSelfActivityTableRegularDistributionCallback(CallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_selfActivityTableRegularDistributionCallback.Set(callback, callbackParam);
		}
		
		// Назначить обработчик события измения таблицы активности удаленного узла
		void SetRemoteNodeActivityTableChangedCallback(RemoteTableCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_remoteActivityTableChangedCallback.Set(callback, callbackParam);
		}
		
		// Получить размер собственной таблицы активности
		int GetSelfActivityTableSize()
		{
			return _selfActivityTableLine.GetSize();
		}
		
		// Получить данные собственной таблицы активности
		// nodes - приемный буфер для номеров узлов
		// rates - приемный буфер для рейтингов
		// buffersSize - размеры буферов nodes и rates, из размеры должны совпадать
		// в буферы nodes и rates будет скопировано не более buffersSize элементов
		// возвращается фактически скопированное число элементов
		int GetSelfActivityTable(AddressType *nodes, unsigned char *rates, unsigned int buffersSize)
		{
			return _selfActivityTableLine.GetTable(nodes, rates, buffersSize);
		}
		
		// Получить рейтинг слышимости для узла из собственной таблицы активности
		unsigned char GetSelfActivityTableValue(AddressType node)
		{
			return _selfActivityTableLine.GetTableValue(node);
		}
		
		// Есть ли таблица для удаленнго узла с номером remoteNodeId
		bool IsRemoteNodeActivityTablePresent(AddressType remoteNodeId)
		{
			return _remoteActivityTable.TablePresent(remoteNodeId);
		}
		
		// Есть ли таблица активности удаленного узла
		bool RemoteNodeActivityTablePresent(AddressType remoteNodeId)
		{
			return _remoteActivityTable.TablePresent(remoteNodeId);
		}
		
		// Получить размер таблицы активности удаленнго узла с номером remoteNodeId
		int GetRemoteNodeActivityTableSize(AddressType remoteNodeId)
		{
			return _remoteActivityTable.GetSize(remoteNodeId);
		}
		
		// Получить данные таблицы активности удаленного узла
		// remoteNodeId - идентификатор удаленнго узла
		// nodes - приемный буфер для номеров узлов
		// rates - приемный буфер для рейтингов
		// buffersSize - размеры буферов nodes и rates, из размеры должны совпадать
		// в буферы nodes и rates будет скопировано не более buffersSize элементов
		// возвращается фактически скопированное число элементов
		int GetRemoteNodeActivityTable(AddressType remoteNodeId, AddressType *nodes, unsigned char *rates, unsigned int buffersSize)
		{
			return _remoteActivityTable.GetTable(remoteNodeId, nodes, rates, buffersSize);
		}
		
		// Получить рейтинг слышимости для узла из таблицы активности удаленного узла
		unsigned char GetRemoteActivityTableValue(AddressType remoteNodeId, AddressType node)
		{
			return _remoteActivityTable.GetTableValue(remoteNodeId, node);
		}
		
		// Получить режим ретрансляции удаленного узла
		unsigned char GetRemoteNodeRelayMode(AddressType remoteNodeId)
		{
			return _remoteActivityTable.GetRelayMode(remoteNodeId);
		}
	protected:
		// Обработчик потери связи с узлом
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