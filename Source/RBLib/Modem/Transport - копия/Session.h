///////////////////////////////////////////////////////////////////////////////
//	Сессия обмена данными между одним или несколькими узлами
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TRANSPORT_SESSION_H
#define MODEM_TRANSPORT_SESSION_H

#include "../../Utils/queue_static.h"
#include "../../Utils/vector_static.h"
#include "SessionData.h"
#include "SessionBuffer.h"

namespace Rblib
{
namespace Modem
{
namespace Transport
{
	template
		<
			typename AddressType,						// тип идентификатора узла
			typename TicketType,						// тип уникального идентификатора сессии
			unsigned int MaxSessionPacketSize,			// максимальный размер пакета с данными сессии, который может быть передан
			unsigned int internalBufferSize,
			int debugDataCodeStart						// стартовый номер кодов отладочной информации
		>
	class Session
	{
	public:
		typedef unsigned char DataIdType;
		
		typedef SessionDataBase<TicketType> SessionDataBase;
		typedef SessionDataFastSingleDestination<AddressType, TicketType, MaxSessionPacketSize> SessionDataFastSingleDestination;
		typedef SessionDataFastSingleDestinationWithRouting<AddressType, TicketType, MaxSessionPacketSize> SessionDataFastSingleDestinationWithRouting;
		typedef SessionDataFastMultyDestination<AddressType, TicketType, MaxSessionPacketSize> SessionDataFastMultyDestination;
		typedef SessionDataStartPartialSingleDestination<AddressType, TicketType, MaxSessionPacketSize> SessionDataStartPartialSingleDestination;
		typedef SessionDataContinuePartial<AddressType, TicketType, MaxSessionPacketSize> SessionDataContinuePartial;
		typedef SessionDataStartPartialSingleDestinationWithRouting<AddressType, TicketType, MaxSessionPacketSize> SessionDataStartPartialSingleDestinationWithRouting;
		typedef SessionDataStartPartialMultyDestination<AddressType, TicketType, MaxSessionPacketSize> SessionDataStartPartialMultyDestination;
		typedef SessionDataStartPartialMultyDestinationWithRouting<AddressType, TicketType, MaxSessionPacketSize> SessionDataStartPartialMultyDestinationWithRouting;
		typedef SessionDataAck<AddressType, TicketType, DataIdType, MaxSessionPacketSize> SessionDataAck;
		typedef SessionDataNotifyIntermediateDelivery<AddressType, TicketType, DataIdType, MaxSessionPacketSize> SessionDataNotifyIntermediateDelivery;
		
		static const unsigned int InternalBufferSize = internalBufferSize;
		static const unsigned int MaxDestinationCount = 255;
		static const unsigned int MaxRouteSize = 255;
		// минимальный размер полезных данных в пакете TODO тут надо автоматом выбрать минимум из всех у которых есть поле данных
		static const unsigned int MinUsefulSessionPacketSize = SessionDataStartPartialSingleDestinationWithRouting::MaxDataSize;
		typedef SessionOutputBuffer<MaxSessionPacketSize, MinUsefulSessionPacketSize, DataIdType> OutputBufferType;
		typedef SessionInputBuffer<MaxSessionPacketSize, MinUsefulSessionPacketSize, DataIdType> InputBufferType;
		typedef vector_static<AddressType, MaxDestinationCount> UndeliveredNodesBufferType;

#pragma pack(push, 1)
		struct AckWaitBufferItem
		{
			DataIdType DataId;
			AddressType Node;
			float StartTime;
			unsigned char SkipCounter;
			
			AckWaitBufferItem(DataIdType dataId, AddressType node, float startTime)
			{
				DataId = dataId;
				Node = node;
				StartTime = startTime;
				SkipCounter = 0;
			}
		};
		
		struct AckOutputBufferItem
		{
			DataIdType DataId;
			bool FinalFlag;
			
			AckOutputBufferItem(DataIdType dataId, bool finalFlag)
			{
				DataId = dataId;
				FinalFlag = finalFlag;
			}
		};
#pragma pack (pop)
		
		typedef vector_static<AckWaitBufferItem, MaxDestinationCount> AckWaitBufferType;
		typedef vector_static<AckOutputBufferItem, MaxDestinationCount> AckOutputBufferType;
		
#pragma pack(push, 1)
		struct IntermediateListItem
		{
			AddressType Final;
			AddressType Intermediate;
			
			IntermediateListItem(AddressType final, AddressType intermediate)
			{
				Final = final;
				Intermediate = intermediate;
			}
		};
#pragma pack (pop)
		
		typedef vector_static<IntermediateListItem, SessionDataStartPartialMultyDestinationWithRouting::MaxDestinationCount> IntermediateListType;
		
		typedef CallbackWrapper<AddressType, AddressType, AddressType *, int, int *> GetRouteCallbackType;
		typedef CallbackWrapper<AddressType> RouteNotExistsCallbackType;
		typedef CallbackWrapper<bool, TicketType, AddressType> SessionDoneCallbackType;
		typedef CallbackWrapper<int, void *, int> DebugDataCallbackType;
		
		// числовые значиния кодов важны, они участвуют в отладочной информации, не изменяйте их чтобы не потерять совместимость с утилитами
		enum State
		{
			StateNotInited				= 0,
			StateInited					= 1,
			StateSendParts				= 2,
			StateWaitAck				= 3,
			StateWaitStart				= 4,
			StateWaitParts				= 5,
			StateDone					= 6,
			StateDeliveryImpossible		= 7,
			StateCancel					= 8,
			StateFastAck				= 9,
			StateError					= 10
		};
		
		enum SessionType
		{
			SessionTypeUnknown										= 0,
			SessionTypeFastSingleDestination						= 1,
			SessionTypeFastSingleDestinationWithRouting				= 2,
			SessionTypeFastMultyDestination							= 3,
			SessionTypePartialSingleDestination						= 4,
			SessionTypePartialSingleDestinationWithRouting			= 5,
			SessionTypePartialMultyDestination						= 6,
			SessionTypePartialMultyDestinationWithRouting			= 7,
		};
		
		// Коды отладочных сообщений
		// числовые значиния кодов важны, они участвуют в отладочной информации, не изменяйте их чтобы не потерять совместимость с утилитами
		enum DebugDataCode
		{
			DebugDataCodeSessionStateChanged		= debugDataCodeStart,
			DebugDataCodeSessionSendPart			= debugDataCodeStart + 1,
			DebugDataCodeSessionPartTimeout	 		= debugDataCodeStart + 2,
			DebugDataCodeSessionSendAck				= debugDataCodeStart + 3,
			DebugDataCodeSessionReceivePart			= debugDataCodeStart + 4,
			DebugDataCodeSessionReceiveAck			= debugDataCodeStart + 5,
		};
		
		// СТРУКТУРЫ ДАННЫХ ОТЛАДОЧНЫХ СООБЩЕНИЙ
#pragma pack(push, 1)
		struct DebugDataStructStateChanged
		{
			unsigned char Id;
			unsigned char Creator;
			float NetTime;
			State State;
			
			DebugDataStructStateChanged(unsigned char id, TicketType _ticket)
			{
				Id = id;
				Creator = _ticket.Creator;
				NetTime = _ticket.NetTime;
				State = StateNotInited;
			}
		};
		
		struct DebugDataStructPartTimeout
		{
			unsigned char Id;
			unsigned char Creator;
			float NetTime;
			DataIdType DataId;
			
			DebugDataStructPartTimeout(unsigned char id, TicketType _ticket)
			{
				Id = id;
				Creator = _ticket.Creator;
				NetTime = _ticket.NetTime;
				DataId = 0;
			}
		};
		
		struct DebugDataStructSendPart
		{
			unsigned char Id;
			unsigned char Creator;
			float NetTime;
			DataIdType DataId;
			
			DebugDataStructSendPart(unsigned char id, TicketType _ticket)
			{
				Id = id;
				Creator = _ticket.Creator;
				NetTime = _ticket.NetTime;
				DataId = 0;
			}
		};
		
		struct DebugDataStructSendAck
		{
			unsigned char Id;
			unsigned char Creator;
			float NetTime;
			unsigned char Count;
			
			DebugDataStructSendAck(unsigned char id, TicketType _ticket)
			{
				Id = id;
				Creator = _ticket.Creator;
				NetTime = _ticket.NetTime;
				Count = 0;
			}
		};
		
		struct DebugDataStructReceivePart
		{
			unsigned char Id;
			unsigned char Creator;
			float NetTime;
			unsigned char Source;
			DataIdType DataId;
			
			DebugDataStructReceivePart(unsigned char id, TicketType _ticket)
			{
				Id = id;
				Creator = _ticket.Creator;
				NetTime = _ticket.NetTime;
				Source = 0;
				DataId = 0;
			}
		};
		
		struct DebugDataStructReceiveAck
		{
			unsigned char Id;
			unsigned char Creator;
			float NetTime;
			unsigned char Source;
			unsigned char Count;
			
			DebugDataStructReceiveAck(unsigned char id, TicketType _ticket)
			{
				Id = id;
				Creator = _ticket.Creator;
				NetTime = _ticket.NetTime;
				Count = 0;
			}
		};
#pragma pack (pop)
		
	protected:
		State _state;
		AddressType _selfAddress;
		TicketType _ticket;
		bool _input;
		AddressType _dataSource;
		int _partsCount;
		int _partsDataCount;
		SessionType _sessionType;
		OutputBufferType _outputBuffer;
		InputBufferType _inputBuffer;
		UndeliveredNodesBufferType _undeliveredNodesBuffer;
		IntermediateListType _intermediateList;
		AddressType _prevIntermediate;
		static unsigned char tmpPartData[MaxSessionPacketSize];
		AckWaitBufferType _waitAckBuffer;
		AckOutputBufferType _outputAckBuffer;
		float _deliveryTimeout;	// таймаут доставки каждой части (в единицах сетевого времени)
		GetRouteCallbackType _getRouteCallback;
		RouteNotExistsCallbackType _routeNotExistsCallback;
		SessionDoneCallbackType _sessionDoneCallback;
		DebugDataCallbackType _debugDataCallback;
		int _cntRxData;
		int _cntRxAck;
		int _cntTxData;
		int _cntTxAck;
	public:
		unsigned char DebugId;
	public:
		Session()
		{
			Reset();
		}
		
		void Init(AddressType selfAddress, TicketType ticket, AddressType *destination, unsigned char destinationCount, unsigned char *data, int dataCount, float deliveryTimeout, typename GetRouteCallbackType::CallbackPtrType getRouteCallback, typename SessionDoneCallbackType::CallbackPtrType sessionDoneCallback, typename DebugDataCallbackType::CallbackPtrType debugDataCallback, void *callbackParam)
		{
			Reset();
			_ticket = ticket;
			_input = false;
			_selfAddress = selfAddress;
			_deliveryTimeout = deliveryTimeout;
			if (_deliveryTimeout < 1)
			{
				_deliveryTimeout = 1;
			}
			
			if (destination == 0 || destinationCount < 1 || data == 0 || dataCount < 1 || getRouteCallback == 0)
			{
				return;
			}
			
			_getRouteCallback.Set(getRouteCallback, callbackParam);
			_sessionDoneCallback.Set(sessionDoneCallback, callbackParam);
			_debugDataCallback.Set(debugDataCallback, callbackParam);
			
			AddressType dst[MaxDestinationCount];
			int initialDstCount = destinationCount;
			int realDstCount = 0;
			
			bool singleDirection = initialDstCount == 1;
			
			bool routingNecessary = false;
			for (int i = 0; i < initialDstCount; i++)
			{
				AddressType node = destination[i];
				bool routeExists;
				routingNecessary |= CheckDestinationRoutingNecessary(node, routeExists);
				
				if (routeExists)
				{
					dst[realDstCount] = node;
					realDstCount++;
				}
				else
				{
					UndeliveredNodePush(node);
				}
			}
			
			// Начинать доставку
			bool startDelivery = false;
			
			// доступных адресатов меньше, чем планировалось изначально
			if (realDstCount < initialDstCount)
			{
				if (realDstCount < 1)
				{
					// значит не можем доставить
					ChangeState(StateDeliveryImpossible);
				}
				else
				{
					// начинаем доставку, хотя бы тем кому можем
					startDelivery = true;
				}
			}
			else
			{
				// адресатов вообще нет
				if (realDstCount < 1)
				{
					// отменяем
					ChangeState(StateCancel);
				}
				else
				{
					// начинаем доставку
					startDelivery = true;
				}
			}
			
			if (startDelivery)
			{
				int maxDataCount = 0;
				
				// одно направление
				if (singleDirection)
				{
					// нужна маршрутизация
					if (routingNecessary)
					{
						maxDataCount = SessionDataFastSingleDestinationWithRouting::MaxDataSize;
						_sessionType = SessionTypeFastSingleDestinationWithRouting;
					}
					// не нужна маршрутизация
					else
					{
						maxDataCount = SessionDataFastSingleDestination::MaxDataSize;
						_sessionType = SessionTypeFastSingleDestination;
					}
				}
				// множество направлений
				else
				{
					// нужна маршрутизация
					if (routingNecessary)
					{
						// множество направлений с маршрутизацией не поддерживается быстрым способом в один пакет
					}
					// не нужна маршрутизация
					else
					{
						// можно ли адресовать необходимое кол-во узлов
						if (SessionDataFastMultyDestination::MaxDestinationCount >= realDstCount)
						{
							maxDataCount = SessionDataFastMultyDestination::GetMaxDataSize(realDstCount);
							_sessionType = SessionTypeFastMultyDestination;
						}
						
						// иначе maxDataCount останется 0 и будет попытка выбрать другой тип сессии
					}
				}
				
				// можно ли уместить требуемое кол-во данных в один пакет
				bool partialNecessary = maxDataCount < dataCount;
				
				// нельзя уместить требуемое кол-во данных в один пакет
				// будем передавать по частям
				// вычисляем тип стартового пакета и сколько в него можно поместить полезных данных
				if (partialNecessary)
				{
					// одно направление
					if (singleDirection)
					{
						// нужна маршрутизация
						if (routingNecessary)
						{
							//maxDataCount = SessionDataStartPartialSingleDestinationWithRouting::GetMaxDataSize(bufferSize);
							_sessionType = SessionTypePartialMultyDestinationWithRouting;//SessionTypePartialSingleDestinationWithRouting;
						}
						// не нужна маршрутизация
						else
						{
							//maxDataCount = SessionDataStartPartialSingleDestination::GetMaxDataSize(bufferSize);
							_sessionType = SessionTypePartialSingleDestination;
						}
					}
					// множество направлений
					else
					{
						// нужна маршрутизация
						if (routingNecessary)
						{
							//maxDataCount = SessionDataStartPartialMultyDestinationWithRouting::GetMaxDataSize(bufferSize);
							// можно ли адресовать необходимое кол-во узлов
							if (SessionDataStartPartialMultyDestinationWithRouting::MaxDestinationCount >= realDstCount)
							{
								_sessionType = SessionTypePartialMultyDestinationWithRouting;
							}
							else
							{
								// иначе _sessionType делаем неизвествным и сессия завершится с ошибкой
								_sessionType = SessionTypeUnknown;
							}
						}
						// не нужна маршрутизация
						else
						{
							// можно ли адресовать необходимое кол-во узлов
							if (SessionDataStartPartialMultyDestination::MaxDestinationCount >= realDstCount)
							{
								_sessionType = SessionTypePartialMultyDestination;
							}
							else
							{
								// иначе _sessionType делаем неизвествным и сессия завершится с ошибкой
								_sessionType = SessionTypeUnknown;
							}
						}
					}
				}
				
				switch(_sessionType)
				{
				case SessionTypeFastSingleDestination:
					{
						DataIdType id = CreateDataId();
						AddressType node = dst[0];
						PushWaitAck(id, node);
						SessionDataFastSingleDestination sessionData(id, _ticket, node, data, dataCount);
						bool outOfBounds = false;
						int partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
						if (!outOfBounds && _outputBuffer.Push(id, tmpPartData, partSize))
						{
							ChangeState(StateSendParts);
						}
						else
						{
							ChangeState(StateError);
						}
					}
					break;
				case SessionTypeFastSingleDestinationWithRouting:
					{
						DataIdType id = CreateDataId();
						AddressType final = dst[0];
						AddressType intermediate = 0;
						if (GetFirstIntermediate(final, intermediate))
						{
							SessionDataFastSingleDestinationWithRouting sessionData(id, _ticket, deliveryTimeout, _selfAddress, intermediate, final, data, dataCount);
							bool outOfBounds = false;
							int partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
							IntermediatePush(final, intermediate);
							PushWaitAck(id, intermediate);
							if (!outOfBounds && _outputBuffer.Push(id, tmpPartData, partSize))
							{
								ChangeState(StateSendParts);
							}
							else
							{
								ChangeState(StateError);
							}
						}
						else
						{
							ChangeState(StateError);
						}
					}
					break;
				case SessionTypeFastMultyDestination:
					{
						DataIdType id = CreateDataId();
						for (int i = 0; i < realDstCount; i++)
						{
							PushWaitAck(id, dst[i]);
						}
						SessionDataFastMultyDestination sessionData(id, _ticket, dst, realDstCount,data, dataCount);
						bool outOfBounds = false;
						int partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
						if (!outOfBounds && _outputBuffer.Push(id, tmpPartData, partSize))
						{
							ChangeState(StateSendParts);
						}
						else
						{
							ChangeState(StateError);
						}
					}
					break;
				case SessionTypePartialSingleDestination:
					{
						int firstPartSize = SessionDataStartPartialSingleDestination::MaxDataSize;
						int nextPartSize = SessionDataContinuePartial::MaxDataSize;
						int dataTailSize = dataCount - firstPartSize;
						if (dataTailSize < 0)
						{
							dataTailSize = 0;
						}
						// сколько промежуточных частей
						int nParts = dataTailSize / nextPartSize;
						// последний кусок
						int lastPartSize = dataTailSize % nextPartSize;
						// всего частей = первая + промежуточные + последняя (если есть) не кратная по размеру промежуточным
						int nPartsTotal = 1 + nParts + (lastPartSize > 0 ? 1 : 0);
						
						unsigned char *dataSrc = data;
						if (firstPartSize > 0)
						{
							// первая часть
							DataIdType id = CreateDataId();
							AddressType node = dst[0];
							PushWaitAck(id, node);
							SessionDataStartPartialSingleDestination sessionData(id, _ticket, node, nPartsTotal, _deliveryTimeout, dataSrc, firstPartSize);
							bool outOfBounds = false;
							int partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
							// двигаем указатель исходных данных
							dataSrc += firstPartSize;
							// помещаем в выходной буфер сформированный пакет
							if (!outOfBounds && _outputBuffer.Push(id, tmpPartData, partSize))
							{
								bool ok = true;
								
								if (nextPartSize > 0)
								{
									// промежуточные части
									// счёт с 1, т.к. первую часть уже сформировали
									for (int i = 0; i < nParts && ok; i++)
									{
										DataIdType id = CreateDataId();
										PushWaitAck(id, node);
										SessionDataContinuePartial sessionData(id, _ticket, (unsigned char)nPartsTotal, (unsigned char)(i + 2), _deliveryTimeout, dataSrc, nextPartSize);
										bool outOfBounds = false;
										partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
										// двигаем указатель исходных данных
										dataSrc += nextPartSize;
										// помещаем в выходной буфер сформированный пакет
										if (outOfBounds || !_outputBuffer.Push(id, tmpPartData, partSize))
										{
											ChangeState(StateError);
											ok = false;
										}
									}
								}
								else
								{
									ChangeState(StateError);
									ok = false;
								}
								
								// последняя часть не кратная по размеру промежуточным
								if (ok)
								{
									if (lastPartSize > 0)
									{
										DataIdType id = CreateDataId();
										PushWaitAck(id, node);
										SessionDataContinuePartial sessionData(id, _ticket, (unsigned char)nPartsTotal, (unsigned char)(nPartsTotal), _deliveryTimeout, dataSrc, lastPartSize);
										bool outOfBounds = false;
										partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
										if (!outOfBounds && _outputBuffer.Push(id, tmpPartData, partSize))
										{
											ChangeState(StateSendParts);
										}
										else
										{
											ChangeState(StateError);
										}
									}
									else
									{
										ChangeState(StateSendParts);
									}
								}
							}
							else
							{
								ChangeState(StateError);
							}
						}
						else
						{
							ChangeState(StateError);
						}
					}
					break;
				case SessionTypePartialSingleDestinationWithRouting:
					{
						ChangeState(StateError);
					}
					break;
				case SessionTypePartialMultyDestination:
					{
						int firstPartSize = SessionDataStartPartialMultyDestination::MaxDataSize;
						int nextPartSize = SessionDataContinuePartial::MaxDataSize;
						int dataTailSize = dataCount - firstPartSize;
						if (dataTailSize < 0)
						{
							dataTailSize = 0;
						}
						// сколько промежуточных частей
						int nParts = dataTailSize / nextPartSize;
						// последний кусок
						int lastPartSize = dataTailSize % nextPartSize;
						// всего частей = первая + промежуточные + последняя (если есть) не кратная по размеру промежуточным
						int nPartsTotal = 1 + nParts + (lastPartSize > 0 ? 1 : 0);
						
						unsigned char *dataSrc = data;
						
						// первая часть (данных не содержит)
						DataIdType id = CreateDataId();
						for (int i = 0; i < realDstCount; i++)
						{
							PushWaitAck(id, dst[i]);
						}
						SessionDataStartPartialMultyDestination sessionData(id, _ticket, dst, realDstCount, _deliveryTimeout);
						bool outOfBounds = false;
						int partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
						// помещаем в выходной буфер сформированный пакет
						if (!outOfBounds && _outputBuffer.Push(id, tmpPartData, partSize))
						{
							bool ok = true;
								
							if (nextPartSize > 0)
							{
								// промежуточные части
								// счёт с 1, т.к. первую часть уже сформировали
								for (int i = 0; i < nParts && ok; i++)
								{
									DataIdType id = CreateDataId();
									for (int j = 0; j < realDstCount; j++)
									{
										PushWaitAck(id, dst[j]);
									}
									SessionDataContinuePartial sessionData(id, _ticket, (unsigned char)nPartsTotal, (unsigned char)(i + 2), _deliveryTimeout, dataSrc, nextPartSize);
									bool outOfBounds = false;
									partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
									// двигаем указатель исходных данных
									dataSrc += nextPartSize;
									// помещаем в выходной буфер сформированный пакет
									if (outOfBounds || !_outputBuffer.Push(id, tmpPartData, partSize))
									{
										ChangeState(StateError);
										ok = false;
									}
								}
							}
							else
							{
								ChangeState(StateError);
								ok = false;
							}
							// последняя часть не кратная по размеру промежуточным
							if (ok)
							{
								if (lastPartSize > 0)
								{
									DataIdType id = CreateDataId();
									for (int i = 0; i < realDstCount; i++)
									{
										PushWaitAck(id, dst[i]);
									}
									SessionDataContinuePartial sessionData(id, _ticket, (unsigned char)nPartsTotal, (unsigned char)(nPartsTotal), _deliveryTimeout, dataSrc, lastPartSize);
									bool outOfBounds = false;
									partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
									if (!outOfBounds && _outputBuffer.Push(id, tmpPartData, partSize))
									{
										ChangeState(StateSendParts);
									}
									else
									{
										ChangeState(StateError);
									}
								}
								else
								{
									ChangeState(StateSendParts);
								}
							}
						}
						else
						{
							ChangeState(StateError);
						}
					}
					break;
				case SessionTypePartialMultyDestinationWithRouting:
					{
						int firstPartSize = SessionDataStartPartialMultyDestinationWithRouting::MaxDataSize;
						int nextPartSize = SessionDataContinuePartial::MaxDataSize;
						int dataTailSize = dataCount - firstPartSize;
						if (dataTailSize < 0)
						{
							dataTailSize = 0;
						}
						// сколько промежуточных частей
						int nParts = dataTailSize / nextPartSize;
						// последний кусок
						int lastPartSize = dataTailSize % nextPartSize;
						// всего частей = первая + промежуточные + последняя (если есть) не кратная по размеру промежуточным
						int nPartsTotal = 1 + nParts + (lastPartSize > 0 ? 1 : 0);
						
						unsigned char *dataSrc = data;
						
						bool ok = true;
						
						// первая часть (данных не содержит)
						DataIdType id = CreateDataId();
						for (int i = 0; i < realDstCount; i++)
						{
							// подтверждения будем ждать от промежуточных узлов
							AddressType intermediate = 0;
							if (GetFirstIntermediate(dst[i], intermediate))
							{
								IntermediatePush(dst[i], intermediate);
								PushWaitAck(id, intermediate);
							}
							else
							{
								ok = false;
							}
						}
						SessionDataStartPartialMultyDestinationWithRouting sessionData(id, _ticket, _selfAddress, dst, realDstCount, _deliveryTimeout);
						bool outOfBounds = false;
						int partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
						// помещаем в выходной буфер сформированный пакет
						if (ok && !outOfBounds && _outputBuffer.Push(id, tmpPartData, partSize))
						{
							if (nextPartSize > 0)
							{
								// промежуточные части
								// счёт с 1, т.к. первую часть уже сформировали
								for (int i = 0; i < nParts && ok; i++)
								{
									DataIdType id = CreateDataId();
									for (int j = 0; j < realDstCount; j++)
									{
										// подтверждения будем ждать от промежуточных узлов
										AddressType intermediate = 0;
										if (GetFirstIntermediate(dst[i], intermediate))
										{
											IntermediatePush(dst[i], intermediate);
											PushWaitAck(id, intermediate);
										}
										else
										{
											ok = false;
										}
									}
									SessionDataContinuePartial sessionData(id, _ticket, (unsigned char)nPartsTotal, (unsigned char)(i + 2), _deliveryTimeout, dataSrc, nextPartSize);
									bool outOfBounds = false;
									partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
									// двигаем указатель исходных данных
									dataSrc += nextPartSize;
									// помещаем в выходной буфер сформированный пакет
									if (outOfBounds || !_outputBuffer.Push(id, tmpPartData, partSize))
									{
										ChangeState(StateError);
										ok = false;
									}
								}
							}
							else
							{
								ChangeState(StateError);
								ok = false;
							}
							// последняя часть не кратная по размеру промежуточным
							if (ok)
							{
								if (lastPartSize > 0)
								{
									DataIdType id = CreateDataId();
									for (int i = 0; i < realDstCount; i++)
									{
										// подтверждения будем ждать от промежуточных узлов
										AddressType intermediate = 0;
										if (GetFirstIntermediate(dst[i], intermediate))
										{
											IntermediatePush(dst[i], intermediate);
											PushWaitAck(id, intermediate);
										}
										else
										{
											ok = false;
										}
									}
									SessionDataContinuePartial sessionData(id, _ticket, (unsigned char)nPartsTotal, (unsigned char)(nPartsTotal), _deliveryTimeout, dataSrc, lastPartSize);
									bool outOfBounds = false;
									partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
									if (!outOfBounds && _outputBuffer.Push(id, tmpPartData, partSize))
									{
										ChangeState(StateSendParts);
									}
									else
									{
										ChangeState(StateError);
									}
								}
								else
								{
									ChangeState(StateSendParts);
								}
							}
						}
						else
						{
							ChangeState(StateError);
						}
					}
					break;
				default:
					{
						ChangeState(StateError);
					}
					break;
				}
			}
		}
		
		void Init(AddressType selfAddress, AddressType dataSource, unsigned char *data, int dataCount, typename GetRouteCallbackType::CallbackPtrType getRouteCallback, typename SessionDoneCallbackType::CallbackPtrType sessionDoneCallback, typename DebugDataCallbackType::CallbackPtrType debugDataCallback, void *callbackParam)
		{
			Reset();
			_ticket = TicketType();
			_input = true;
			_deliveryTimeout = 8; // для типов сессий для которых таймаут не определен
			_selfAddress = selfAddress;
			_getRouteCallback.Set(getRouteCallback, callbackParam);
			_sessionDoneCallback.Set(sessionDoneCallback, callbackParam);
			_debugDataCallback.Set(debugDataCallback, callbackParam);
			
			SessionDataBase sessionBaseData(data, dataCount);
			
			if (sessionBaseData.Valid())
			{
				_ticket = sessionBaseData.Ticket;
				
				switch(sessionBaseData.DataType)
				{
				case SessionDataTypeFastSingleDestination:
					{
						_sessionType = SessionTypeFastSingleDestination;
						_partsCount = 1;
						ChangeState(StateWaitParts);
					}
					break;
				case SessionDataTypeFastSingleDestinationWithRouting:
					{
						_sessionType = SessionTypeFastSingleDestinationWithRouting;
						_partsCount = 1;
						ChangeState(StateWaitParts);
					}
					break;
				case SessionDataTypeFastMultyDestination:
					{
						_sessionType = SessionTypeFastMultyDestination;
						_partsCount = 1;
						ChangeState(StateWaitParts);
					}
					break;
				case SessionDataTypeStartPartialSingleDestination:
					{
						// прием начался с начала
						_sessionType = SessionTypePartialSingleDestination;
						ChangeState(StateWaitParts);
					}
					break;
				case SessionDataTypeStartPartialMultyDestination:
					{
						// прием начался с начала
						_sessionType = SessionTypePartialMultyDestination;
						ChangeState(StateWaitParts);
					}
					break;
				case SessionDataTypeStartPartialMultyDestinationWithRouting:
					{
						_sessionType = SessionTypePartialMultyDestinationWithRouting;
						ChangeState(StateWaitParts);
					}
					break;
				case SessionDataTypeContinuePartial:
					{
						// если прием начинается с середины
						_sessionType = SessionTypeUnknown;
						ChangeState(StateWaitStart);
					}
					break;
				default:
					{
						ChangeState(StateCancel);
					}
					break;
				}
				
				OnDataReceived(dataSource, data, dataCount);
			}
		}
		
		// открыть сессию беузловной отправки подтверждений
		void Init(AddressType selfAddress, AddressType dataSource, unsigned char *data, int dataCount, typename DebugDataCallbackType::CallbackPtrType debugDataCallback, void *callbackParam)
		{
			Reset();
			_ticket = TicketType();
			_input = true;
			_selfAddress = selfAddress;
			_deliveryTimeout = 8; // в принципе не важно
			_debugDataCallback.Set(debugDataCallback, callbackParam);
			
			SessionDataBase sessionBaseData(data, dataCount);
			
			if (sessionBaseData.Valid())
			{
				_ticket = sessionBaseData.Ticket;
				
				bool sendAck = true;
				
				switch(sessionBaseData.DataType)
				{
				case SessionDataTypeUnknown:
				case SessionDataTypeAck:
					{
						sendAck = false;
					}
					break;
				case SessionDataTypeNotifyIntermediateDelivery:
					{
						SessionDataNotifyIntermediateDelivery sessionData(data, dataCount);
						if (sessionData.Valid())
						{
							sendAck = sessionData.DestinationAddress == _selfAddress;
						}
					}
					break;
				}
				
				if (sendAck)
				{
					ChangeState(StateFastAck);
					PushOutputAck(sessionBaseData.PacketId, true);
				}
				else
				{
					ChangeState(StateCancel);
				}
			}
			else
			{
				ChangeState(StateError);
			}
		}
		
		void Reset()
		{
			_ticket = TicketType();
			_input = true;
			_dataSource = 0;
			ChangeState(StateNotInited);
			_selfAddress = 0;
			_partsCount = 0;
			_partsDataCount = 0;
			_sessionType = SessionTypeUnknown;
			_outputBuffer.Reset();
			_inputBuffer.Reset();
			_undeliveredNodesBuffer.clear();
			_intermediateList.clear();
			_prevIntermediate = -1;
			_waitAckBuffer.clear();
			_outputAckBuffer.clear();
			_deliveryTimeout = 0;
			_getRouteCallback.Reset();
			_sessionDoneCallback.Reset();
			_cntRxData = 0;
			_cntRxAck = 0;
			_cntTxData = 0;
			_cntTxAck = 0;
		}
		
		bool Empty()
		{
			return _state == StateNotInited;
		}
		
		bool IsOutput()
		{
			return !_input;
		}
		
		int PopNextOutputData(unsigned char *buffer, int bufferSize, float curTime)
		{
			if (Empty())
			{
				return 0;
			}
			
			int bufferUsed = 0;
			
			if (bufferSize < MaxSessionPacketSize)
			{
				ChangeState(StateError);
				return 0;
			}
			
			int outputAckCount = OutputAckCount();
			
			if (outputAckCount > 0)
			{
				DataIdType dataIds[SessionDataAck::MaxCount];
				bool finalFlags[SessionDataAck::MaxCount];
				
				int sendCount = outputAckCount;
				if (sendCount > SessionDataAck::MaxCount)
				{
					sendCount = SessionDataAck::MaxCount;
				}
				
				sendCount = PopOutputAck(dataIds, finalFlags, sendCount);
				
				DebugDataStructSendAck debugData(DebugId, _ticket);
				debugData.Count = sendCount;
				_debugDataCallback.Call(DebugDataCodeSessionSendAck, &debugData, sizeof(debugData));
				
				_cntTxAck++;
				
				SessionDataAck sessionData(_ticket, dataIds, finalFlags, sendCount);
				bool outOfBounds = false;
				bufferUsed = sessionData.ToByteArray(buffer, bufferSize, outOfBounds);
				if(outOfBounds || bufferUsed < 1)
				{
					ChangeState(StateError);
				}
			}
			else
			{
				if (StateSendParts == _state || (StateWaitParts == _state && SessionTypePartialMultyDestinationWithRouting == _sessionType))
				{
					int partSize = 0;
					unsigned char dataId = 0;
					for (int i = 0; i < _outputBuffer.Count(); i++)
					{
						// извлечем для отправки ближайший кусок из головы очереди
						if (partSize = _outputBuffer.GetNext(buffer, bufferSize, &dataId))
						{
							_cntTxData++;
							
							if (_input && _cntTxData > 5)
							{
								__no_operation();
							}
							
							// TODO в случае изменения маршрута, первое звено отправляющей сессии никогда не перестраивается
							// маршрут уже выбран, запомнены все промежуточные узлы
							// выбран тип сессии
							// что то переделать в случае изменения маршрута технически очень сложно
							
							bool timeout = false;
							CheckWaitAckTimeout(dataId, curTime, timeout);
							if (timeout)
							{
								DebugDataStructPartTimeout debugData(DebugId, _ticket);
								debugData.DataId = dataId;
								_debugDataCallback.Call(DebugDataCodeSessionPartTimeout, &debugData, sizeof(debugData));
								
								// время ожидания вышло, удалим из очереди
								if (!_outputBuffer.Remove(dataId))
								{
									ChangeState(StateError);
								}
							}
							else
							{
								unsigned char skipCounter = GetSkipCounter(dataId);
								if (skipCounter < 1)
								{
									// отправляем
									bufferUsed = partSize;
									SetSkipCounter(dataId, 1);
									
									DebugDataStructSendPart debugData(DebugId, _ticket);
									debugData.DataId = dataId;
									_debugDataCallback.Call(DebugDataCodeSessionSendPart, &debugData, sizeof(debugData));
									
									break; // выбрали часть для отправки
								}
								else
								{
									SetSkipCounter(dataId, skipCounter - 1);
									// пропустили эту часть, поищем следующую
								}
							}
						}
					}
					
					if (_outputBuffer.Empty())
					{
						CheckWaitAckTimeout(curTime);
					}
				}
			}
			
			return bufferUsed;
		}
		
		TicketType GetTicket()
		{
			return _ticket;
		}
		
		static TicketType GetTicket(unsigned char *data, int dataCount)
		{
			TicketType res;
			
			if (data != 0 && dataCount >= SessionDataBase::RequiredDataCount)
			{
				SessionDataBase * sessionBaseData = (SessionDataBase *)data;
				res = sessionBaseData->Ticket;
			}
			
			return res;
		}
		
		static bool ShouldOpenSession(unsigned char *data, int dataCount)
		{
			SessionDataBase baseData(data, dataCount);
			if (baseData.Valid())
			{
				switch(baseData.DataType)
				{
				case SessionDataTypeUnknown:
				case SessionDataTypeAck:
					{
						return false;
					}
					break;
				default:
					{
						return true;
					}
					break;
				}
			}
			
			return false;
		}
		
		static bool BaseDataValid(unsigned char *data, int dataCount)
		{
			SessionDataBase sessionBaseData(data, dataCount);
			return sessionBaseData.Valid();
		}
		
		bool OnDataReceived(AddressType sourceAddress, unsigned char *data, int dataCount)
		{
			if (Empty())
			{
				return false;
			}
			
			TicketType receivedTicket;
			SessionDataType receivedDataType = SessionDataTypeUnknown;
			
			SessionDataBase sessionBaseData(data, dataCount);

			if (!sessionBaseData.Valid())
			{
				return false;
			}
			
			receivedDataType = sessionBaseData.DataType;
			receivedTicket = sessionBaseData.Ticket;
			
			if (receivedTicket != _ticket)
			{
				if (receivedDataType == SessionDataTypeAck)
				{
					__no_operation();
				}
				return false;
			}
			
			switch(receivedDataType)
			{
			case SessionDataTypeAck:
				{
					_cntRxAck++;
					SessionDataAck sessionData(data, dataCount);
					if (sessionData.Valid())
					{
						DebugDataStructReceiveAck debugData(DebugId, _ticket);
						debugData.Source = sourceAddress;
						debugData.Count = sessionData.IdsCount;
						_debugDataCallback.Call(DebugDataCodeSessionReceiveAck, &debugData, sizeof(debugData));
					}
				}
				break;
			default:
				{
					DebugDataStructReceivePart debugData(DebugId, _ticket);
					debugData.Source = sourceAddress;
					debugData.DataId = sessionBaseData.PacketId;
					_debugDataCallback.Call(DebugDataCodeSessionReceivePart, &debugData, sizeof(debugData));
				}
				break;
			}
			
			switch(receivedDataType)
			{
			case SessionDataTypeFastSingleDestination:
				{
					_cntRxData++;
					if (_sessionType != SessionTypeFastSingleDestination)
					{
						ChangeState(StateError);
						break;
					}
					
					if (StateWaitStart == _state || StateWaitParts == _state)
					{
						SessionDataFastSingleDestination sessionData(data, dataCount);
						if (sessionData.Valid())
						{
							if (sessionData.DestinationAddress == _selfAddress)
							{
								_dataSource = sourceAddress;
								if (!PushInputData(sessionData.PacketId, 1, sessionData.Data, sessionData.DataCount))
								{
									ChangeState(StateError);
								}
							}
							else
							{
								ChangeState(StateCancel);
							}
						}
					}
				}
				break;
			case SessionDataTypeFastSingleDestinationWithRouting:
				{
					if (_sessionType != SessionTypeFastSingleDestinationWithRouting)
					{
						ChangeState(StateError);
						break;
					}
					
					SessionDataFastSingleDestinationWithRouting sessionData(data, dataCount);
					if (sessionData.Valid())
					{
						if (sourceAddress == 1)
						{
							__no_operation();
						}
						
						if (sessionData.IntermediateAddress != 2 && sourceAddress == 1)
						{
							__no_operation();
						}
						
						if (StateWaitStart == _state || StateWaitParts == _state || StateSendParts == _state)
						{
							if (sessionData.IntermediateAddress == _selfAddress)
							{
								_deliveryTimeout = sessionData.DeliveryTimeout;
								
								// достигли узла назначения
								if (sessionData.IntermediateAddress == sessionData.FinalAddress)
								{
									_dataSource = sessionData.InitialAddress;
									if (!PushInputData(sessionData.PacketId, 1, sessionData.Data, sessionData.DataCount))
									{
										ChangeState(StateError);
									}
								}
								else
								{
									PushOutputAck(sessionData.PacketId, false);
									_prevIntermediate = sourceAddress;
									AddressType nextNode = 0;
									if (GetFirstIntermediate(sessionData.FinalAddress, nextNode))
									{
										if (_input && _outputBuffer.Count() > 1)
										{
											__no_operation();
										}
										
										// если эту часть ещё
										if (!WaitAckContains(sessionData.PacketId, nextNode))
										{
											SessionDataFastSingleDestinationWithRouting retranslateSessionData
												(sessionData.PacketId, _ticket, _deliveryTimeout,
												 sessionData.InitialAddress, nextNode, sessionData.FinalAddress,
												 sessionData.Data, sessionData.DataCount);
											bool outOfBounds = false;
											int partSize = retranslateSessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
											PushWaitAck(sessionData.PacketId, nextNode);
											if (!outOfBounds && _outputBuffer.Push(sessionData.PacketId, tmpPartData, partSize))
											{
												ChangeState(StateSendParts);
											}
											else
											{
												ChangeState(StateError);
											}
										}
									}
									else
									{
										// цель одна, маршута дальше нет, дальше пытаться нет смысла
										// если таймаута ещё нет, то сессия переоткроется от входящего пакета
										// а сейчас можно закрыть, чтобы освободить сессию
										ChangeState(StateError);
									}
								}
							}
						}
					}
				}
				break;
			case SessionDataTypeFastMultyDestination:
				{
					if (_sessionType != SessionTypeFastMultyDestination)
					{
						ChangeState(StateError);
						break;
					}
					
					if (StateWaitStart == _state || StateWaitParts == _state)
					{
						SessionDataFastMultyDestination sessionData(data, dataCount);
						if (sessionData.Valid())
						{
							if (sessionData.DestinationsContains(_selfAddress))
							{
								_dataSource = sourceAddress;
								if (!PushInputData(sessionData.PacketId, 1, sessionData.Data, sessionData.DataCount))
								{
									ChangeState(StateError);
								}
							}
							else
							{
								ChangeState(StateCancel);
							}
						}
					}
				}
				break;
			case SessionDataTypeStartPartialSingleDestination:
				{
					if (StateWaitParts == _state)
					{
						if (_sessionType != SessionTypePartialSingleDestination)
						{
							ChangeState(StateError);
							break;
						}
					}
					
					SessionDataStartPartialSingleDestination sessionData(data, dataCount);
					if (sessionData.Valid())
					{
						if (StateWaitStart == _state)
						{
							_sessionType = SessionTypePartialSingleDestination;
							ChangeState(StateWaitParts);
						}
						
						_partsCount = sessionData.PartialData.Count;
						_deliveryTimeout = sessionData.PartialData.DeliveryTimeout;
						
						if (sessionData.DestinationAddress == _selfAddress)
						{
							_dataSource = sourceAddress;
							// в первый элемент буфера запихнём стартовую часть целиком для дальнейшего использования
							if (!PushInputData(sessionData.PacketId, 0, data, dataCount))
							{
								ChangeState(StateError);
							}
							// в следующий элемент полезные данные первой части
							if (!PushInputData(sessionData.PacketId, sessionData.PartialData.Number, sessionData.PartialData.Data, sessionData.PartialData.DataCount))
							{
								ChangeState(StateError);
							}
						}
						else
						{
							ChangeState(StateCancel);
						}
					}
				}
				break;
			case SessionDataTypeStartPartialMultyDestination:
				{
					if (StateWaitParts == _state)
					{
						if (_sessionType != SessionTypePartialMultyDestination)
						{
							ChangeState(StateError);
							break;
						}
					}
					
					SessionDataStartPartialMultyDestination sessionData(data, dataCount);
					if (sessionData.Valid())
					{
						if (StateWaitStart == _state)
						{
							_sessionType = SessionTypePartialMultyDestination;
							ChangeState(StateWaitParts);
						}
						
						// если пока количество частей неизвестно
						if (_partsCount < 1)
						{
							_partsCount = 2; // на старте предполагаем столько, в очередной SessionDataTypeContinuePartial поправится на реальный
						}
						
						_deliveryTimeout = sessionData.DeliveryTimeout;
						
						if (sessionData.DestinationsContains(_selfAddress))
						{
							_dataSource = sourceAddress;
							// в первый элемент буфера запихнём стартовую часть целиком для дальнейшего использования
							if (!PushInputData(sessionData.PacketId, 0, data, dataCount))
							{
								ChangeState(StateError);
							}
						}
						else
						{
							ChangeState(StateCancel);
						}
					}
				}
				break;
			case SessionDataTypeStartPartialMultyDestinationWithRouting:
				{
					bool cont = false;
					switch (_state)
					{
					case StateWaitStart:
					case StateWaitParts:
						cont = true;
						break;
					}
					
					if (!cont)
					{
						break;
					}
					
					if (StateWaitParts == _state)
					{
						if (_sessionType != SessionTypePartialMultyDestinationWithRouting)
						{
							ChangeState(StateError);
							break;
						}
					}
					
					SessionDataStartPartialMultyDestinationWithRouting sessionData(data, dataCount);
					if (sessionData.Valid())
					{
						if (StateWaitStart == _state)
						{
							_sessionType = SessionTypePartialMultyDestinationWithRouting;
							ChangeState(StateWaitParts);
							
							// TODO надо ретранслировать всё что накопилось в приемном буфере (кроме нулевой части), т.к. пока не знали тип - не ретранслировали
						}
						
						// если пока количество частей неизвестно
						if (_partsCount < 1)
						{
							_partsCount = 2; // на старте предполагаем столько, в очередной SessionDataTypeContinuePartial поправится на реальный
						}
						
						_deliveryTimeout = sessionData.DeliveryTimeout;
						
						bool retranslate = false;
						
						for (int i = 0; i < sessionData.DestinationsCount; i++)
						{
							AddressType initial = sessionData.InitialAddress;
							AddressType final = sessionData.Destinations[i];
							AddressType prev = 0;
							AddressType next = 0;
							if (RouteIncludeNode(initial, final, _selfAddress, prev, next))
							{
								// если не достигли хотя бы одного адресата
								// ретрансляция до которого идет через этот узел строго вперед по цепочке
								// и при условии что пакет пришел строго от предыдущего по цепочке узла
								if (_selfAddress != final && sourceAddress != next && sourceAddress == prev)
								{
									retranslate = true;
									PushWaitAck(sessionData.PacketId, next);
								}
							}
						}
						
						if (retranslate)
						{
							// помещаем в выходной буфер то, что приняли
							// подверждение отошлём только после того, как адресат пришлёт своё подтверждение
							bool outOfBounds = false;
							if (outOfBounds || !_outputBuffer.Push(sessionData.PacketId, data, dataCount))
							{
								ChangeState(StateError);
							}
						}
						
						if (sessionData.DestinationsContains(_selfAddress) || retranslate)
						{
							_dataSource = sessionData.InitialAddress;
							// в первый элемент буфера запихнём стартовую часть целиком для дальнейшего использования
							if (!PushInputData(sessionData.PacketId, 0, data, dataCount))
							{
								ChangeState(StateError);
							}
						}
						else
						{
							ChangeState(StateCancel);
						}
					}
				}
				break;
			case SessionDataTypeAck:
				{
					SessionDataAck sessionData(data, dataCount);
					if (sessionData.Valid())
					{
						for (int i = 0; i < sessionData.IdsCount; i++)
						{
							DataIdType dataId = sessionData.DataIds[i];
							// окончательное подтверждение или нет
							bool finalFlag = sessionData.FinalFlags[i];
							bool notifyIntermediate = false;
							
							if (finalFlag)
							{
								if (sourceAddress == 0)
								{
									__no_operation();
								}
								
								PopWaitAck(dataId, sourceAddress);
							}
							else
							{
								// данные доставлены, но надо ждать подтверждение о доставке дальше по цепочке ретрансляцци
								// больше отправлять не будем, будем ждать окончательного подтверждения
								_outputBuffer.Remove(dataId);
							}
							
							bool waitAck = WaitAckContains(dataId);
							
							if (!waitAck)
							{
								_outputBuffer.Remove(dataId);
								notifyIntermediate = sourceAddress != _prevIntermediate;
							}
							
							if (finalFlag && _input)
							{
								// приняли сами, теперь передадим тому кто от нас ждёт
								switch(_sessionType)
								{
								case SessionTypeFastSingleDestinationWithRouting:
									{
										//PushOutputAck(dataId, true);
										if (notifyIntermediate)
										{
											DataIdType id = dataId + 100;
											AddressType node = _prevIntermediate;
											PushWaitAck(id, node);
											SessionDataNotifyIntermediateDelivery sessionData(id, _ticket, node, dataId);
											bool outOfBounds = false;
											int partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
											if (outOfBounds || !_outputBuffer.Push(id, tmpPartData, partSize))
											{
												ChangeState(StateError);
											}
										}
									}
									break;
								case SessionTypePartialMultyDestinationWithRouting:
									{
										// подтверждаем сами, только если все идущие после нас узлы тоже приняли эти данные
										if (!waitAck)
										{
											PushOutputAck(dataId, true);
										}
									}
									break;
								}
							}
						}
					}
				}
				break;
			case SessionDataTypeContinuePartial:
				{
					bool cont = false;
					switch (_state)
					{
					case StateWaitStart:
					case StateWaitParts:
						cont = true;
						break;
					}
					
					if (!cont)
					{
						break;
					}
					
					// пока не дождались старта, считаем что все части для нас
					bool thisPartForMe = StateWaitStart == _state;
					bool retranslate = false;
					
					if (StateWaitParts == _state)
					{
						// был старт но не знаем тип - какой то косяк
						if (_sessionType == SessionTypeUnknown)
						{
							ChangeState(StateError);
							break;
						}
						// был старт, можем понять наша часть или нет
						switch(_sessionType)
						{
						case SessionTypePartialSingleDestination:
							{
								unsigned char *partData = 0;
								// стартовый кусок лежит в первом элементе приемного буфера
								unsigned char partDataCount = _inputBuffer.GetPartDataPtr(0, partData);
								SessionDataStartPartialSingleDestination sessionData(partData, partDataCount);
								if (sessionData.Valid())
								{
									if (sessionData.DestinationAddress == _selfAddress)
									{
										thisPartForMe = true;
									}
								}
							}
							break;
						case SessionTypePartialMultyDestination:
							{
								unsigned char *partData = 0;
								// стартовый кусок лежит в первом элементе приемного буфера
								unsigned char partDataCount = _inputBuffer.GetPartDataPtr(0, partData);
								SessionDataStartPartialMultyDestination sessionData(partData, partDataCount);
								if (sessionData.Valid())
								{
									if (sessionData.DestinationsContains(_selfAddress))
									{
										thisPartForMe = true;
									}
								}
							}
							break;
						case SessionTypePartialMultyDestinationWithRouting:
							{
								unsigned char *partData = 0;
								// стартовый кусок лежит в первом элементе приемного буфера
								unsigned char partDataCount = _inputBuffer.GetPartDataPtr(0, partData);
								SessionDataStartPartialMultyDestinationWithRouting sessionData(partData, partDataCount);
								if (sessionData.Valid())
								{
									for (int i = 0; i < sessionData.DestinationsCount; i++)
									{
										AddressType initial = sessionData.InitialAddress;
										AddressType final = sessionData.Destinations[i];
										AddressType prev = 0;
										AddressType next = 0;
										if (RouteIncludeNode(initial, final, _selfAddress, prev, next))
										{
											// если не достигли хотя бы одного адресата
											// ретрансляция до которого идет через этот узел строго вперед по цепочке
											// и при условии что пакет пришел строго от предыдущего по цепочке узла
											if (_selfAddress != final && sourceAddress != next && sourceAddress == prev)
											{
												retranslate = true;
												PushWaitAck(sessionData.PacketId, next);
											}
										}
									}
									
									if (retranslate)
									{
										// помещаем в выходной буфер то, что приняли
										// подверждение отошлём только после того, как адресат пришлёт своё подтверждение
										bool outOfBounds = false;
										if (outOfBounds || !_outputBuffer.Push(sessionData.PacketId, data, dataCount))
										{
											ChangeState(StateError);
										}
									}
									
									if (sessionData.DestinationsContains(_selfAddress))
									{
										thisPartForMe = true;
									}
								}
							}
							break;
						}
					}
					
					SessionDataContinuePartial sessionData(data, dataCount);
					if (sessionData.Valid())
					{
						_partsCount = sessionData.PartialData.Count;
						_deliveryTimeout = sessionData.PartialData.DeliveryTimeout;
						
						if (thisPartForMe)
						{
							if (!PushInputData(sessionData.PacketId, sessionData.PartialData.Number, sessionData.PartialData.Data, sessionData.PartialData.DataCount))
							{
								ChangeState(StateError);
							}
						}
						else
						{
							if (!retranslate)
							{
								ChangeState(StateCancel);
							}
						}
					}
				}
				break;
			case SessionDataTypeNotifyIntermediateDelivery:
				{
					SessionDataNotifyIntermediateDelivery sessionData(data, dataCount);
					if (sessionData.Valid())
					{
						if (sessionData.DestinationAddress == _selfAddress)
						{
							PopWaitAck(sessionData.DataId, sourceAddress);
							PushOutputAck(sessionBaseData.PacketId, true);
						}
					}
				}
				break;
			default:
				{
					ChangeState(StateWaitStart);
				}
				break;
			}
			
			return true;
		}
		
		void UpdateState(float curTime, float startTime)
		{
			if (Empty())
			{
				return;
			}
			
			if (_input)
			{
				int notUsedPartsCount = 0;
				// для некоторых типов первый элемент буфера используется под служебые данные
				switch(_sessionType)
				{
				case SessionTypePartialSingleDestination:
				case SessionTypePartialMultyDestination:
				case SessionTypePartialMultyDestinationWithRouting:
					{
						notUsedPartsCount = 1;
					}
					break;
				}
				
				switch (_state)
				{
				case StateWaitParts:
					{
						if (_partsCount > 0)
						{
							// Данные приняты, подтверждения отправлены
							if ((_inputBuffer.Count() - notUsedPartsCount) >= _partsCount && _outputAckBuffer.empty())
							{
								// для такого вида сессии надо не только всё принять, но и дождаться пока все примут или таймаута
								bool waitMore = SessionTypePartialMultyDestinationWithRouting == _sessionType && !_waitAckBuffer.empty();
								
								if (!waitMore)
								{
									ChangeState(StateDone);
									_sessionDoneCallback.Call(true, _ticket, _dataSource);
								}
							}
						}
					}
					break;
				// если входящая сессия отправляет данные, значит это сессия ретрансляции
				case StateSendParts:
					{
						if (_outputBuffer.Empty() && _waitAckBuffer.empty() && _outputAckBuffer.empty())
						{
							ChangeState(StateDone);
						}
					}
					break;
				case StateFastAck:
					{
						if (_outputAckBuffer.empty())
						{
							ChangeState(StateCancel);
						}
					}
					break;
				}
				
				if (_state != StateDone)
				{
					if (_partsCount > 0)
					{
						if (_inputBuffer.Count() >= _partsCount && _outputBuffer.Empty() && _waitAckBuffer.empty() && _outputAckBuffer.empty())
						{
							ChangeState(StateDone);
							_sessionDoneCallback.Call(false, _ticket, _dataSource);
						}
						else
						{
							// сколько времени прошло с момента открытия сессии
							if (curTime >= startTime)
							{
								float elapsed = curTime - startTime;
								// много прошло
								if (elapsed >= _deliveryTimeout && _outputAckBuffer.empty())
								{
									ChangeState(StateCancel);
								}
							}
							else
							{
								// текущее время меньше начала, значит был перезапуск сети
								ChangeState(StateCancel);
							}
						}
					}
				}
			}
			else
			{
				switch (_state)
				{
				case StateSendParts:
					{
						if (_outputBuffer.Empty() && _waitAckBuffer.empty() && _outputAckBuffer.empty())
						{
							ChangeState(StateDone);
							_sessionDoneCallback.Call(false, _ticket, _dataSource);
						}
					}
					break;
				}
			}
		}
		
		int GetData(unsigned char *buffer, int bufferSize)
		{
			// для некоторых типов первый элемент буфера используется под служебые данные
			// чтобы он не попал в полезные данные, надо его удалить перед вычитыванием
			switch(_sessionType)
			{
			case SessionTypePartialSingleDestination:
			case SessionTypePartialMultyDestination:
			case SessionTypePartialMultyDestinationWithRouting:
				{
					_inputBuffer.Remove(0);
				}
				break;
			}
			
			switch(_sessionType)
			{
			case SessionTypeFastSingleDestination:
			case SessionTypeFastSingleDestinationWithRouting:
			case SessionTypeFastMultyDestination:
			case SessionTypePartialSingleDestination:
			case SessionTypePartialMultyDestination:
			case SessionTypePartialMultyDestinationWithRouting:
				{
					int totalDataCount = _inputBuffer.GetData(buffer, bufferSize);
					return totalDataCount;
				}
				break;
			}
			return 0;
		}
		
		bool IsComplete()
		{
			if (Empty())
			{
				return false;
			}
			
			return StateDone == _state || StateDeliveryImpossible == _state || StateCancel == _state || StateError == _state;
		}
		
		bool IsTxComplete()
		{
			return !_input && StateDone == _state;
		}
		
		bool IsTxImpossible()
		{
			return !_input && StateDeliveryImpossible == _state;
		}
		
		bool IsTxError()
		{
			return !_input && StateError == _state;
		}
		
		int GetUndeliveredNodes(AddressType *buffer, int maxItemsCount)
		{
			int result = 0;
			
			switch(_sessionType)
			{
			// в сессиях с ретрансляцией недоставка до промежуточных узлов, ведет к недоставке до конечных узлов
			case SessionTypeFastSingleDestinationWithRouting:
			case SessionTypePartialMultyDestinationWithRouting:
				{
					for (int i = 0; i < _undeliveredNodesBuffer.size(); i++)
					{
						// не доставлено до этого промежуточного
						AddressType intermediate = _undeliveredNodesBuffer[i];
						
						// ищем какие конечные узлы шли через этот промежуточный
						for (int j = 0; j < _intermediateList.size(); j ++)
						{
							if (_intermediateList[j].Intermediate == intermediate)
							{
								if (buffer != 0 && i < maxItemsCount - 1)
								{
									buffer[i] = _intermediateList[j].Final;
									result++;
								}
							}
						}
					}
				}
				break;
			default:
				{
					for (int i = 0; i < _undeliveredNodesBuffer.size(); i++)
					{
						AddressType node = _undeliveredNodesBuffer[i];
						
						if (buffer != 0 && i < maxItemsCount - 1)
						{
							buffer[i] = node;
							result++;
						}
					}
				}
				break;
			}
			
			return result;
		}
	protected:
		void ChangeState(State state)
		{
			bool notify = _state != state;
			_state = state;
			if (notify)
			{
				DebugDataStructStateChanged debugData(DebugId, _ticket);
				debugData.State = _state;
				_debugDataCallback.Call(DebugDataCodeSessionStateChanged, &debugData, sizeof(debugData));
			}
		}
		
		bool GetFirstIntermediate(AddressType node, AddressType &intremediate)
		{
			AddressType route[MaxRouteSize];
			int bufferSize = sizeof(route);
			int routeLen = 0;
			_getRouteCallback.Call(_selfAddress, node, route, bufferSize, &routeLen);
			
			// особый случай, длина маршрута больше размера буфера, теоретически этого быть не должно
			if (routeLen > bufferSize)
			{
				return false;
			}
			
			if (routeLen > 0)
			{
				intremediate = route[0];
			}
			
			return routeLen > 0;
		}
		
		bool CheckDestinationRoutingNecessary(AddressType node, bool &routeExists)
		{
			AddressType intremediate = 0;
			routeExists = GetFirstIntermediate(node, intremediate);
			bool routingNecessary = routeExists ? node != intremediate : false;
						
			return routingNecessary;
		}
		
		bool RouteIncludeNode(AddressType initial, AddressType final, AddressType node, AddressType &prev, AddressType &next)
		{
			AddressType route[MaxRouteSize];
			int bufferSize = sizeof(route);
			int routeLen = 0;
			_getRouteCallback.Call(initial, final, route, bufferSize, &routeLen);
			
			// особый случай, длина маршрута больше размера буфера, теоретически этого быть не должно
			if (routeLen > bufferSize)
			{
				return false;
			}
			
			if (routeLen < 1)
			{
				return false;
			}
			
			// WARNING маршрут в буфере без первого initial узла!!!
			
			// если node совпадает с начальным, то в массиве route его номера не будет,
			// в цикле for он не найдется,
			// поэтому обработаем эту ситуацию как особую
			if (node == initial)
			{
				prev = initial;
				next = route[0];
				return true;
			}
			
			for (int i = 0; i < routeLen; i++)
			{
				if (route[i] == node)
				{
					if (i > 0)
					{
						prev = route[i - 1];
					}
					else
					{
						prev = initial;
					}
					
					if (i < routeLen - 1)
					{
						next = route[i + 1];
					}
					else
					{
						next = node;
					}
					
					break;
				}
			}
			return true;
		}
		
		void OnRouteNotExistxs(AddressType node)
		{
			_routeNotExistsCallback.Call(node);
		}
		
		DataIdType CreateDataId()
		{
			static DataIdType lastId = 0;
			return lastId++;
		}
		
		void PushWaitAck(DataIdType dataId, AddressType node)
		{
			typename AckWaitBufferType::iterator i;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end(); i++)
			{
				if (i->DataId == dataId && i->Node == node)
				{
					return;
				}
			}
			
			_waitAckBuffer.push_back(AckWaitBufferItem(dataId, node, 0));
		}
		
		void PopWaitAck(DataIdType dataId, AddressType node)
		{
			typename AckWaitBufferType::iterator i;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end(); i++)
			{
				if (i->DataId == dataId && i->Node == node)
				{
					_waitAckBuffer.erase(i);
					return;
				}
			}
		}
		
		void PopWaitAck(AddressType node)
		{
			typename AckWaitBufferType::iterator i;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end(); i++)
			{
				if (i->Node == node)
				{
					_waitAckBuffer.erase(i);
					return;
				}
			}
		}
		
		bool WaitAckContains(DataIdType dataId)
		{
			bool res = false;
			
			typename AckWaitBufferType::iterator i;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end(); i++)
			{
				if (i->DataId == dataId)
				{
					res = true;
					break;
				}
			}
			/*
			if (!res)
			{
				for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end(); i++)
				{
					if (i->DataId == dataId)
					{
						res = true;
						break;
					}
				}
			}*/
			
			return res;
		}
		
		bool WaitAckContains(DataIdType dataId, AddressType node)
		{
			bool res = false;
			
			typename AckWaitBufferType::iterator i;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end(); i++)
			{
				if (i->DataId == dataId && i->Node == node)
				{
					res = true;
					break;
				}
			}
			
			return res;
		}
		
		void CheckWaitAckTimeout(DataIdType dataId, float curTime, bool &timeout)
		{
			// таймаут по части dataId
			timeout = true;
			
			typename AckWaitBufferType::iterator i;
			bool restart = false;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end();)
			{
				if (i->DataId == dataId)
				{
					if (i->StartTime > 0)
					{
						// эту часть уже отправляли
						// сколько времени прошло с первой попытки
						float elapsed = curTime - i->StartTime;
						// много прошло
						if (elapsed >= _deliveryTimeout)
						{
							// эта чать уже не будет доставлена до узла
							// значит вся cессия не выполнима для узла node
							AddressType node = i->Node;
							// удаляем все ожидания для этого узла
							PopWaitAck(node);
							// помечаем что этому узлу данные не доставлены
							UndeliveredNodePush(node);
							// итератор сломался, начнём сначала
							restart = true;
						}
						else
						{
							// таймаута по этой части ещё нет, т.к. хотя бы у этого узла таймаут ещё не наступил
							timeout = false;
						}
					}
					else
					{
						// эту часть ещё ни разу не отправляли
						// пометим когда отправили первый раз
						i->StartTime = curTime;
						// таймаута по этой части точно нет
						timeout = false;
					}
				}
				
				if (restart)
				{
					i = _waitAckBuffer.begin();
					restart = false;
				}
				else
				{
					i++;
				}
			}
		}
		
		void CheckWaitAckTimeout(float curTime)
		{
			typename AckWaitBufferType::iterator i;
			bool restart = false;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end();)
			{
				if (i->StartTime > 0)
				{
					// эту часть уже отправляли
					// сколько времени прошло с первой попытки
					float elapsed = curTime - i->StartTime;
					// много прошло
					if (elapsed >= _deliveryTimeout)
					{
						// эта чать уже не будет доставлена до узла
						// значит вся cессия не выполнима для узла node
						AddressType node = i->Node;
						// удаляем все ожидания для этого узла
						PopWaitAck(node);
						// помечаем что этому узлу данные не доставлены
						UndeliveredNodePush(node);
						// итератор сломался, начнём сначала
						restart = true;
					}
				}
				else
				{
					// эту часть ещё ни разу не отправляли
					// начнем отсчет сейчас
					i->StartTime = curTime;
				}
			
				if (restart)
				{
					i = _waitAckBuffer.begin();
					restart = false;
				}
				else
				{
					i++;
				}
			}
		}
		
		void SetSkipCounter(DataIdType dataId, unsigned char skipCounter)
		{
			typename AckWaitBufferType::iterator i;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end(); i++)
			{
				if (i->DataId == dataId)
				{
					i->SkipCounter = skipCounter;
				}
			}
		}
		
		unsigned char GetSkipCounter(DataIdType dataId)
		{
			unsigned char res = 0;
			
			typename AckWaitBufferType::iterator i;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end(); i++)
			{
				if (i->DataId == dataId)
				{
					if (i->StartTime > 0)
					{
						// эту часть уже отправляли
						res = i->SkipCounter;
					}
					else
					{
						// эту часть ещё ни разу не отправляли
						res = 0;
					}
				}
			}
			
			return res;
		}
		
		bool PushInputData(DataIdType dataId, unsigned char partNumber, unsigned char *data, int dataCount)
		{
			bool res = false;
			
			if (_inputBuffer.Push(dataId, partNumber, data, dataCount))
			{
				PushOutputAck(dataId, true);
				res = true;
			}
			
			return res;
		}
		
		void PushOutputAck(DataIdType dataId, bool finalFlag)
		{
			typename AckOutputBufferType::iterator i;
			for (i = _outputAckBuffer.begin(); i != _outputAckBuffer.end(); i++)
			{
				if (i->DataId == dataId && i->FinalFlag == finalFlag)
				{
					return;
				}
			}
			
			_outputAckBuffer.push_back(AckOutputBufferItem(dataId, finalFlag));
		}
		
		int OutputAckCount()
		{
			return _outputAckBuffer.size();
		}
		
		int PopOutputAck(DataIdType *buffer, bool *finalFlags, int maxCount)
		{
			int copyCount = OutputAckCount();
			if (copyCount > maxCount)
			{
				copyCount = maxCount;
			}
			
			for (int i = 0; i < copyCount; i++)
			{
				buffer[i] = _outputAckBuffer.front().DataId;
				finalFlags[i] = _outputAckBuffer.front().FinalFlag;
				_outputAckBuffer.erase(_outputAckBuffer.begin());
			}
			
			return copyCount;
		}
		
		void IntermediatePush(AddressType final, AddressType intermediate)
		{
			// повторы не добавляем
			for (int i = 0; i < _intermediateList.size(); i++)
			{
				if (_intermediateList[i].Final == final)
				{
					return;
				}
			}
			
			_intermediateList.push_back(IntermediateListItem(final, intermediate));
		}
		
		void UndeliveredNodePush(AddressType node)
		{
			// повторы не добавляем
			for (int i = 0; i < _undeliveredNodesBuffer.size(); i++)
			{
				if (_undeliveredNodesBuffer[i] == node)
				{
					return;
				}
			}
			_undeliveredNodesBuffer.push_back(node);
		}
	};
	
	template <typename AddressType,	typename TicketType, unsigned int MaxSessionPacketSize, unsigned int internalBufferSize, int debugDataCodeStart>
	unsigned char Session<AddressType, TicketType, MaxSessionPacketSize, internalBufferSize, debugDataCodeStart>::tmpPartData[MaxSessionPacketSize];
}
}
}

#endif