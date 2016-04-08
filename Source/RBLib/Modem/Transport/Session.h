///////////////////////////////////////////////////////////////////////////////
//	������ ������ ������� ����� ����� ��� ����������� ������
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
			typename AddressType,						// ��� �������������� ����
			typename TicketType,						// ��� ����������� �������������� ������
			unsigned int MaxSessionPacketSize,			// ������������ ������ ������ � ������� ������, ������� ����� ���� �������
			unsigned int internalBufferSize,
			int debugDataCodeStart						// ��������� ����� ����� ���������� ����������
		>
	class Session
	{
	public:
		typedef unsigned char DataIdType;
		
		typedef SessionDataBase<TicketType> SessionDataBase;
		typedef SessionDataFastSingleDestination<AddressType, TicketType, MaxSessionPacketSize> SessionDataFastSingleDestination;
		typedef SessionDataFastSingleDestinationWithRouting<AddressType, TicketType, MaxSessionPacketSize> SessionDataFastSingleDestinationWithRouting;
		typedef SessionDataFastMultyDestination<AddressType, TicketType, MaxSessionPacketSize> SessionDataFastMultyDestination;
		typedef SessionDataAck<AddressType, TicketType, DataIdType, MaxSessionPacketSize> SessionDataAck;
		typedef SessionDataNotifyIntermediateDelivery<AddressType, TicketType, DataIdType, MaxSessionPacketSize> SessionDataNotifyIntermediateDelivery;
		
		static const unsigned int InternalBufferSize = internalBufferSize;
		static const unsigned int MaxDestinationCount = 20;
		static const unsigned int MaxRouteSize = 255;
		typedef SessionBuffer<MaxSessionPacketSize> OutputBufferType;
		typedef SessionBuffer<MaxSessionPacketSize> InputBufferType;
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
		
		typedef vector_static<IntermediateListItem, SessionDataFastMultyDestination::MaxDestinationCount> IntermediateListType;
		
		typedef CallbackWrapper<AddressType, AddressType, AddressType *, int, int *> GetRouteCallbackType;
		typedef CallbackWrapper<AddressType> RouteNotExistsCallbackType;
		typedef CallbackWrapper<bool, TicketType, AddressType> SessionDoneCallbackType;
		typedef CallbackWrapper<int, void *, int> DebugDataCallbackType;
		
		// �������� �������� ����� �����, ��� ��������� � ���������� ����������, �� ��������� �� ����� �� �������� ������������� � ���������
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
			SessionTypeFastSingleDestinationWithRoutingInitiator	= 2,
			SessionTypeFastSingleDestinationWithRoutingFinal		= 3,
			SessionTypeFastSingleDestinationWithRoutingIntermediate	= 4,
			SessionTypeFastMultyDestination							= 5,
		};
		
		// ���� ���������� ���������
		// �������� �������� ����� �����, ��� ��������� � ���������� ����������, �� ��������� �� ����� �� �������� ������������� � ���������
		enum DebugDataCode
		{
			DebugDataCodeSessionStateChanged		= debugDataCodeStart,
			DebugDataCodeSessionSendPart			= debugDataCodeStart + 1,
			DebugDataCodeSessionPartTimeout	 		= debugDataCodeStart + 2,
			DebugDataCodeSessionSendAck				= debugDataCodeStart + 3,
			DebugDataCodeSessionReceivePart			= debugDataCodeStart + 4,
			DebugDataCodeSessionReceiveAck			= debugDataCodeStart + 5,
		};
		
		// ��������� ������ ���������� ���������
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
		SessionType _sessionType;
		OutputBufferType _outputBuffer;
		InputBufferType _inputBuffer;
		UndeliveredNodesBufferType _undeliveredNodesBuffer;
		IntermediateListType _intermediateList;
		int _prevIntermediate;
		int _nextIntermediate;
		static unsigned char tmpPartData[MaxSessionPacketSize];
		AckWaitBufferType _waitAckBuffer;
		AckOutputBufferType _outputAckBuffer;
		float _deliveryTimeout;	// ������� �������� ������ ����� (� �������� �������� �������)
		GetRouteCallbackType _getRouteCallback;
		RouteNotExistsCallbackType _routeNotExistsCallback;
		SessionDoneCallbackType _sessionDoneCallback;
		DebugDataCallbackType _debugDataCallback;
		int _cntRxData;
		int _cntRxAck;
		int _cntRxIntermediateAck;
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
			
			// �������� ��������
			bool startDelivery = false;
			
			// ��������� ��������� ������, ��� ������������� ����������
			if (realDstCount < initialDstCount)
			{
				if (realDstCount < 1)
				{
					// ������ �� ����� ���������
					ChangeState(StateDeliveryImpossible);
				}
				else
				{
					// �������� ��������, ���� �� ��� ���� �����
					startDelivery = true;
				}
			}
			else
			{
				// ��������� ������ ���
				if (realDstCount < 1)
				{
					// ��������
					ChangeState(StateCancel);
				}
				else
				{
					// �������� ��������
					startDelivery = true;
				}
			}
			
			if (startDelivery)
			{
				int maxDataCount = 0;
				
				// ���� �����������
				if (singleDirection)
				{
					// ����� �������������
					if (routingNecessary)
					{
						maxDataCount = SessionDataFastSingleDestinationWithRouting::MaxDataSize;
						_sessionType = SessionTypeFastSingleDestinationWithRoutingInitiator;
					}
					// �� ����� �������������
					else
					{
						maxDataCount = SessionDataFastSingleDestination::MaxDataSize;
						_sessionType = SessionTypeFastSingleDestination;
					}
				}
				// ��������� �����������
				else
				{
					// ����� �������������
					if (routingNecessary)
					{
						// ��������� ����������� � �������������� �� �������������� ������� �������� � ���� �����
					}
					// �� ����� �������������
					else
					{
						// ����� �� ���������� ����������� ���-�� �����
						if (SessionDataFastMultyDestination::MaxDestinationCount >= realDstCount)
						{
							maxDataCount = SessionDataFastMultyDestination::GetMaxDataSize(realDstCount);
							_sessionType = SessionTypeFastMultyDestination;
						}
						
						// ����� maxDataCount ��������� 0 � ����� ������� ������� ������ ��� ������
					}
				}
				
				// ����� �� �������� ��������� ���-�� ������ � ���� �����
				bool partialNecessary = maxDataCount < dataCount;
				
				// ������ �������� ��������� ���-�� ������ � ���� �����
				// �� ������ �� �����
				// ��������� ��� ���������� ������ � ������� � ���� ����� ��������� �������� ������
				if (partialNecessary)
				{
					_sessionType = SessionTypeUnknown;
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
						if (!outOfBounds && _outputBuffer.Set(id, tmpPartData, partSize))
						{
							ChangeState(StateSendParts);
						}
						else
						{
							ChangeState(StateError);
						}
					}
					break;
				case SessionTypeFastSingleDestinationWithRoutingInitiator:
					{
						DataIdType id = CreateDataId();
						AddressType final = dst[0];
						AddressType intermediate = 0;
						if (GetFirstIntermediate(final, intermediate))
						{
							_nextIntermediate = intermediate;
							SessionDataFastSingleDestinationWithRouting sessionData(id, _ticket, deliveryTimeout, _selfAddress, intermediate, final, data, dataCount);
							bool outOfBounds = false;
							int partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
							IntermediatePush(final, intermediate);
							PushWaitAck(id, intermediate);
							if (!outOfBounds && _outputBuffer.Set(id, tmpPartData, partSize))
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
						if (!outOfBounds && _outputBuffer.Set(id, tmpPartData, partSize))
						{
							ChangeState(StateSendParts);
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
		
		bool Init(AddressType selfAddress, AddressType dataSource, unsigned char *data, int dataCount, typename GetRouteCallbackType::CallbackPtrType getRouteCallback, typename SessionDoneCallbackType::CallbackPtrType sessionDoneCallback, typename DebugDataCallbackType::CallbackPtrType debugDataCallback, void *callbackParam)
		{
			Reset();
			_ticket = TicketType();
			_input = true;
			_deliveryTimeout = 8; // ��� ����� ������ ��� ������� ������� �� ���������
			_selfAddress = selfAddress;
			_getRouteCallback.Set(getRouteCallback, callbackParam);
			_sessionDoneCallback.Set(sessionDoneCallback, callbackParam);
			_debugDataCallback.Set(debugDataCallback, callbackParam);
			
			bool res = false;
			
			SessionDataBase sessionBaseData(data, dataCount);
			
			if (sessionBaseData.Valid())
			{
				_ticket = sessionBaseData.Ticket;
				
				switch(sessionBaseData.DataType)
				{
				case SessionDataTypeFastSingleDestination:
					{
						SessionDataFastSingleDestination sessionData(data, dataCount);
						if (sessionData.Valid())
						{
							if (sessionData.DestinationAddress == _selfAddress)
							{
								_cntRxData++;
								_dataSource = dataSource;
								_sessionType = SessionTypeFastSingleDestination;
								
								if (!PushInputData(sessionData.PacketId, sessionData.Data, sessionData.DataCount))
								{
									ChangeState(StateError);
								}
								else
								{
									ChangeState(StateWaitParts);
								}
								res = true;
							}
						}
					}
					break;
				case SessionDataTypeFastSingleDestinationWithRouting:
					{
						SessionDataFastSingleDestinationWithRouting sessionData(data, dataCount);
						if (sessionData.Valid())
						{
							if (sessionData.IntermediateAddress == _selfAddress)
							{
								_deliveryTimeout = sessionData.DeliveryTimeout;
								
								// �������� ���� ����������
								if (sessionData.IntermediateAddress == sessionData.FinalAddress)
								{
									_sessionType = SessionTypeFastSingleDestinationWithRoutingFinal;
									
									_dataSource = sessionData.InitialAddress;
									if (!PushInputData(sessionData.PacketId, sessionData.Data, sessionData.DataCount))
									{
										ChangeState(StateError);
									}
									else
									{
										ChangeState(StateWaitParts);
									}
									res = true;
								}
								
								// �� ������������
								if (sessionData.IntermediateAddress != sessionData.FinalAddress)
								{
									_sessionType = SessionTypeFastSingleDestinationWithRoutingIntermediate;
									
									PushOutputAck(sessionData.PacketId, false);
									_prevIntermediate = dataSource;
									AddressType nextNode = 0;
									if (GetFirstIntermediate(sessionData.FinalAddress, nextNode))
									{
										// ���� ��� ����� ��� ����������
										if (!WaitAckContains(sessionData.PacketId, nextNode))
										{
											_nextIntermediate = nextNode;
											SessionDataFastSingleDestinationWithRouting retranslateSessionData
												(sessionData.PacketId, _ticket, _deliveryTimeout,
												 sessionData.InitialAddress, nextNode, sessionData.FinalAddress,
												 sessionData.Data, sessionData.DataCount);
											bool outOfBounds = false;
											int partSize = retranslateSessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
											PushWaitAck(sessionData.PacketId, nextNode);
											if (!outOfBounds && _outputBuffer.Set(sessionData.PacketId, tmpPartData, partSize))
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
										// ���� ����, ������� ������ ���, ������ �������� ��� ������
										// ���� �������� ��� ���, �� ������ ������������� �� ��������� ������
										// � ������ ����� �������, ����� ���������� ������
										ChangeState(StateError);
									}
									res = true;
								}
							}
						}
					}
					break;
				}
			}
			
			// ������ �� ��������
			if (!res)
			{
				Reset();
			}
			
			return res;
		}
		
		// ������� ������ ���������� �������� �������������
		void Init(AddressType selfAddress, AddressType dataSource, unsigned char *data, int dataCount, typename DebugDataCallbackType::CallbackPtrType debugDataCallback, void *callbackParam)
		{
			Reset();
			_ticket = TicketType();
			_input = true;
			_selfAddress = selfAddress;
			_deliveryTimeout = 8; // � �������� �� �����
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
					Reset();
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
			_sessionType = SessionTypeUnknown;
			_outputBuffer.Reset();
			_inputBuffer.Reset();
			_undeliveredNodesBuffer.clear();
			_intermediateList.clear();
			_prevIntermediate = -1;
			_nextIntermediate = -1;
			_waitAckBuffer.clear();
			_outputAckBuffer.clear();
			_deliveryTimeout = 0;
			_getRouteCallback.Reset();
			_sessionDoneCallback.Reset();
			_cntRxData = 0;
			_cntRxAck = 0;
			_cntRxIntermediateAck = 0;
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
		
		int PopNextOutputData(unsigned char *buffer, int bufferSize, float curTime, bool &needMoreSpace)
		{
			if (Empty())
			{
				return 0;
			}
			
			int bufferUsed = 0;
			needMoreSpace = false;
			
			int outputAckCount = OutputAckCount();
			
			if (outputAckCount > 0)
			{
				if (bufferSize < SessionDataAck::CalckPacketSize(outputAckCount))
				{
					needMoreSpace = true;
					return 0;
				}
				
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
				if (StateSendParts == _state)
				{
					int partSize = 0;
					int dataId = 0;
					if (!_outputBuffer.Empty())
					{
						if (bufferSize < _outputBuffer.DataCount())
						{
							needMoreSpace = true;
							return 0;
						}
						
						// �������� ��� �������� ��������� ����� �� ������ �������
						if (partSize = _outputBuffer.Get(buffer, bufferSize, &dataId))
						{
							_cntTxData++;
							// TODO � ������ ��������� ��������, ������ ����� ������������ ������ ������� �� ���������������
							// ������� ��� ������, ��������� ��� ������������� ����
							// ������ ��� ������
							// ��� �� ���������� � ������ ��������� �������� ���������� ����� ������
							
							bool timeout = false;
							CheckWaitAckTimeout(dataId, curTime, timeout);
							if (timeout)
							{
								DebugDataStructPartTimeout debugData(DebugId, _ticket);
								debugData.DataId = dataId;
								_debugDataCallback.Call(DebugDataCodeSessionPartTimeout, &debugData, sizeof(debugData));
								
								// ����� �������� �����, ������ �� �������
								_outputBuffer.Reset();
							}
							else
							{
								unsigned char skipCounter = GetSkipCounter(dataId);
								if (skipCounter < 1)
								{
									// ����������
									bufferUsed = partSize;
									SetSkipCounter(dataId, 1);
									
									DebugDataStructSendPart debugData(DebugId, _ticket);
									debugData.DataId = dataId;
									_debugDataCallback.Call(DebugDataCodeSessionSendPart, &debugData, sizeof(debugData));
								}
								else
								{
									SetSkipCounter(dataId, skipCounter - 1);
									// ���������� ��� �����, ������ ���������
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
				return false;
			}
			
			switch(receivedDataType)
			{
			case SessionDataTypeAck:
				{
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
					
				}
				break;
			case SessionDataTypeFastSingleDestinationWithRouting:
				{
					switch (_sessionType)
					{
					case SessionTypeFastSingleDestinationWithRoutingInitiator:
						{
						}
						break;
					case SessionTypeFastSingleDestinationWithRoutingIntermediate:
						{
							SessionDataFastSingleDestinationWithRouting sessionData(data, dataCount);
							if (sessionData.Valid())
							{
								if (sessionData.IntermediateAddress == _selfAddress)
								{
									// �� ������������
									if (sessionData.IntermediateAddress != sessionData.FinalAddress)
									{
										// ��� �������� ������ �� ��������� ��� ������
										// ������ ������ ������������
										PushOutputAck(sessionData.PacketId, false);
									}
								}
							}
						}
						break;
					case SessionTypeFastSingleDestinationWithRoutingFinal:
						{
						}
						break;
					default:
						{
							ChangeState(StateError);
						}
						break;
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
								if (!PushInputData(sessionData.PacketId, sessionData.Data, sessionData.DataCount))
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
			case SessionDataTypeAck:
				{
					SessionDataAck sessionData(data, dataCount);
					if (sessionData.Valid())
					{
						for (int i = 0; i < sessionData.IdsCount; i++)
						{
							DataIdType dataId = sessionData.DataIds[i];
							// ������������� ������������� ��� ���
							bool finalFlag = sessionData.FinalFlags[i];
					
							if (_input)
							{
								switch (_sessionType)
								{
								case SessionTypeFastSingleDestinationWithRoutingInitiator:
									{
									}
									break;
								case SessionTypeFastSingleDestinationWithRoutingIntermediate:
									{
										if (sourceAddress == _nextIntermediate && WaitAckContains(dataId))
										{
											_cntRxAck++;
											if (finalFlag)
											{
												PopWaitAck(dataId, sourceAddress);
												_outputBuffer.ResetIfContains(dataId);
												
												DataIdType id = dataId + 100;
												AddressType node = _prevIntermediate;
												PushWaitAck(id, node);
												SessionDataNotifyIntermediateDelivery sessionData(id, _ticket, node, dataId);
												bool outOfBounds = false;
												int partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
												if (outOfBounds || !_outputBuffer.Set(id, tmpPartData, partSize))
												{
													ChangeState(StateError);
												}
											}
											else
											{
												_outputBuffer.ResetIfContains(dataId);
											}
										}
										
										if (sourceAddress == _prevIntermediate && WaitAckContains(dataId))
										{
											_cntRxAck++;
											PopWaitAck(dataId, sourceAddress);
											_outputBuffer.ResetIfContains(dataId);
										}
									}
									break;
								case SessionTypeFastSingleDestinationWithRoutingFinal:
									{
									}
									break;
								default:
									{
										ChangeState(StateError);
									}
									break;
								}
							}
							else
							{
								switch (_sessionType)
								{
								case SessionTypeFastSingleDestination:
									{
										_cntRxAck++;
										PopWaitAck(dataId, sourceAddress);
										_outputBuffer.ResetIfContains(dataId);
									}
									break;
								case SessionTypeFastSingleDestinationWithRoutingInitiator:
									{
										if (sourceAddress == _nextIntermediate && WaitAckContains(dataId))
										{
											_cntRxAck++;
											// ������ ����������, �� ���� ����� ������������� � �������� ������ �� ������� ������������
											// ������ ���������� �� �����, ����� ����� �������������� �������������
											_outputBuffer.ResetIfContains(dataId);
										}
									}
									break;
								case SessionTypeFastSingleDestinationWithRoutingIntermediate:
									{
									}
									break;
								case SessionTypeFastSingleDestinationWithRoutingFinal:
									{
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
					}
				}
				break;
			case SessionDataTypeNotifyIntermediateDelivery:
				{
					_cntRxIntermediateAck++;
					SessionDataNotifyIntermediateDelivery sessionData(data, dataCount);
					if (sessionData.Valid())
					{
						if (sessionData.DestinationAddress == _selfAddress)
						{
							int dataId = sessionData.DataId;
							
							PopWaitAck(dataId, sourceAddress);
							PushOutputAck(sessionBaseData.PacketId, true);
							
							if (_prevIntermediate != -1)
							{
								DataIdType id = dataId + 100;
								AddressType node = _prevIntermediate;
								PushWaitAck(id, node);
								SessionDataNotifyIntermediateDelivery sessionData(id, _ticket, node, dataId);
								bool outOfBounds = false;
								int partSize = sessionData.ToByteArray(tmpPartData, sizeof(tmpPartData), outOfBounds);
								if (outOfBounds || !_outputBuffer.Set(id, tmpPartData, partSize))
								{
									ChangeState(StateError);
								}
							}
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
				switch (_state)
				{
				case StateWaitParts:
					{
						// ������ �������, ������������� ����������
						if (!_inputBuffer.Empty() && _outputAckBuffer.empty() && _waitAckBuffer.empty())
						{
							ChangeState(StateDone);
							_sessionDoneCallback.Call(true, _ticket, _dataSource);
						}
					}
					break;
				// ���� �������� ������ ���������� ������, ������ ��� ������ ������������
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
					if (!_inputBuffer.Empty()&& _outputBuffer.Empty() && _waitAckBuffer.empty() && _outputAckBuffer.empty())
					{
						ChangeState(StateDone);
						_sessionDoneCallback.Call(false, _ticket, _dataSource);
					}
					else
					{
						// ������� ������� ������ � ������� �������� ������
						if (curTime >= startTime)
						{
							float elapsed = curTime - startTime;
							// ����� ������
							if (elapsed >= _deliveryTimeout && _outputAckBuffer.empty())
							{
								ChangeState(StateCancel);
							}
						}
						else
						{
							// ������� ����� ������ ������, ������ ��� ���������� ����
							ChangeState(StateCancel);
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
			switch(_sessionType)
			{
			case SessionTypeFastSingleDestination:
			case SessionTypeFastSingleDestinationWithRoutingFinal:
			case SessionTypeFastMultyDestination:
				{
					int totalDataCount = _inputBuffer.Get(buffer, bufferSize);
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
			// � ������� � ������������� ���������� �� ������������� �����, ����� � ���������� �� �������� �����
			case SessionTypeFastSingleDestinationWithRoutingInitiator:
				{
					for (int i = 0; i < _undeliveredNodesBuffer.size(); i++)
					{
						// �� ���������� �� ����� ��������������
						AddressType intermediate = _undeliveredNodesBuffer[i];
						
						// ���� ����� �������� ���� ��� ����� ���� �������������
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
			
			// ������ ������, ����� �������� ������ ������� ������, ������������ ����� ���� �� ������
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
			
			// ������ ������, ����� �������� ������ ������� ������, ������������ ����� ���� �� ������
			if (routeLen > bufferSize)
			{
				return false;
			}
			
			if (routeLen < 1)
			{
				return false;
			}
			
			// WARNING ������� � ������ ��� ������� initial ����!!!
			
			// ���� node ��������� � ���������, �� � ������� route ��� ������ �� �����,
			// � ����� for �� �� ��������,
			// ������� ���������� ��� �������� ��� ������
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
			// ������� �� ����� dataId
			timeout = true;
			
			typename AckWaitBufferType::iterator i;
			bool restart = false;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end();)
			{
				if (i->DataId == dataId)
				{
					if (i->StartTime > 0)
					{
						// ��� ����� ��� ����������
						// ������� ������� ������ � ������ �������
						float elapsed = curTime - i->StartTime;
						// ����� ������
						if (elapsed >= _deliveryTimeout)
						{
							// ��� ���� ��� �� ����� ���������� �� ����
							// ������ ��� c����� �� ��������� ��� ���� node
							AddressType node = i->Node;
							// ������� ��� �������� ��� ����� ����
							PopWaitAck(node);
							// �������� ��� ����� ���� ������ �� ����������
							UndeliveredNodePush(node);
							// �������� ��������, ����� �������
							restart = true;
						}
						else
						{
							// �������� �� ���� ����� ��� ���, �.�. ���� �� � ����� ���� ������� ��� �� ��������
							timeout = false;
						}
					}
					else
					{
						// ��� ����� ��� �� ���� �� ����������
						// ������� ����� ��������� ������ ���
						i->StartTime = curTime;
						// �������� �� ���� ����� ����� ���
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
					// ��� ����� ��� ����������
					// ������� ������� ������ � ������ �������
					float elapsed = curTime - i->StartTime;
					// ����� ������
					if (elapsed >= _deliveryTimeout)
					{
						// ��� ���� ��� �� ����� ���������� �� ����
						// ������ ��� c����� �� ��������� ��� ���� node
						AddressType node = i->Node;
						// ������� ��� �������� ��� ����� ����
						PopWaitAck(node);
						// �������� ��� ����� ���� ������ �� ����������
						UndeliveredNodePush(node);
						// �������� ��������, ����� �������
						restart = true;
					}
				}
				else
				{
					// ��� ����� ��� �� ���� �� ����������
					// ������ ������ ������
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
			return 0;
			/*
			unsigned char res = 0;
			
			typename AckWaitBufferType::iterator i;
			for (i = _waitAckBuffer.begin(); i != _waitAckBuffer.end(); i++)
			{
				if (i->DataId == dataId)
				{
					if (i->StartTime > 0)
					{
						// ��� ����� ��� ����������
						res = i->SkipCounter;
					}
					else
					{
						// ��� ����� ��� �� ���� �� ����������
						res = 0;
					}
				}
			}
			
			return res;
			*/
		}
		
		bool PushInputData(DataIdType dataId, unsigned char *data, int dataCount)
		{
			bool res = false;
			
			if (_inputBuffer.Set(dataId, data, dataCount))
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
			// ������� �� ���������
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
			// ������� �� ���������
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