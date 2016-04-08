///////////////////////////////////////////////////////////////////////////////
//	������������ ����
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TRANSPORT_TRANSPORT_H
#define MODEM_TRANSPORT_TRANSPORT_H

#include "../../Utils/CallbackWrapper.h"
#include "../../Utils/queue_static.h"
#include "../../Utils/vector_static.h"
#include "RingBuffer.h"
#include "Session.h"

namespace Rblib
{
namespace Modem
{
namespace Transport
{
	template
		<
			typename AddressType,						// ��� �������������� ����
			unsigned int MaxSessionPacketSize,			// ������������ ������ ������ � ������� ������, ������� ����� ���� �������
			int debugDataCodeStart,						// ��������� ����� ����� ���������� ����������
			int maxSessionsCount,
			int maxActiveOutputSessionsCount
		>
	class Transport
	{
	public:
		enum TxResult
		{
			TxResultDelivered,
			TxResultDeliveryImpossible,
			TxResultError
		};

#pragma pack(push, 1)		
		struct TicketStruct
		{
			AddressType Creator;
			float NetTime;
			
			TicketStruct()
			{
				Creator = 0;
				NetTime = 0;
			}
			
			TicketStruct(AddressType creator, float netTime)
			{
				Creator = creator;
				NetTime = netTime;
			}
			
			inline bool operator == (const TicketStruct& other)
			{
				return Creator == other.Creator && NetTime == other.NetTime;
			}
			
			inline bool operator != (const TicketStruct& other)
			{
				return Creator != other.Creator || NetTime != other.NetTime;
			}
		};
#pragma pack (pop)
		typedef TicketStruct TicketType;
#pragma pack(push, 1)		
		struct InputQueueHeader
		{
		public:
			enum StateCode
			{
				StateCodeNotInited,
				StateCodeFresh,
				StateCodeProgress,
				StateCodeRetry
			};
		public:
			unsigned char PacketId;
			StateCode State;
			float EnqueueTime;
			unsigned int DeliveryTimeout;
			TicketType Ticket;
		public:
			InputQueueHeader()
			{
				PacketId = 0;
				State = StateCodeNotInited;
				EnqueueTime = 0;
				DeliveryTimeout = 0;
			}
		};
#pragma pack (pop)
		static const unsigned int MaxDestinationCount = 255;
		static const unsigned int MaxDataCountPerSession = MaxSessionPacketSize;
		static const unsigned int InputQueueSize = 1024 * 5;
		static const unsigned int MaxSessionCount = maxSessionsCount;

		typedef InputQueueHeader InputQueueHeaderType;
		typedef RingBuffer<InputQueueHeaderType, InputQueueSize> InputQueueType;
		typedef Session<AddressType, TicketType, MaxSessionPacketSize, sizeof(AddressType) * MaxDestinationCount + MaxDataCountPerSession, debugDataCodeStart> SessionType;
		//typedef vector_static<SessionType, MaxSessionCount> SessionsListType;
		typedef unsigned char OutputBufferType[MaxDataCountPerSession];
		typedef TransportUsedTycketsBuffer<TicketType, 32> TransportUsedTycketsBufferType;
		typedef TransportUsedTycketsBuffer<TicketType, 32> DoneSessionsListType;
		typedef CallbackWrapper<int &, float &> GetTicketInfoCallbackType;
		typedef CallbackWrapper<AddressType, AddressType, AddressType *, int, int *> GetRouteCallbackType;
		typedef CallbackWrapper<AddressType, unsigned char *, int> OnDataReceivedCallbackType;
		typedef CallbackWrapper<int, AddressType, TxResult> OnTxCompleteCallbackType;
		typedef CallbackWrapper<int, void *, int> DebugDataCallbackType;
	protected:
		bool _running;													// ������ ��������, ������ ������������ / �����������
		int _maxActiveOutputSessionsCount;
		InputQueueType _inputQueue;
		OutputBufferType _outputBuffer;
		SessionType _sessions[MaxSessionCount];
		TransportUsedTycketsBufferType _transportUsedTycketsBuffer;
		DoneSessionsListType _doneInputSessionsList;						// ������ ������� �������� �������� ������
		GetTicketInfoCallbackType _getTicketInfoCallback;
		GetRouteCallbackType _getRouteCallback;
		OnDataReceivedCallbackType _onDataReceivedCallback;
		OnTxCompleteCallbackType _onTxCompleteCallback;
		DebugDataCallbackType _debugDataCallback;
		TicketType _prevCreatedTicket;
		int _firstDequeuedSession;
	public:
		// ����������� �� ���������
		Transport()
		{
			_maxActiveOutputSessionsCount = maxActiveOutputSessionsCount;
			for(int i = 0; i < MaxSessionCount; i++)
			{
				_sessions[i].DebugId = i;
			}
			Reset();
		}
		
		// �����
		void Reset()
		{
			// ������� �� �������� ��� �������, ���� �������� ��������������� - �� ��������� ��������
			_running = false;
			for(int i = 0; i < MaxSessionCount; i++)
			{
				if (!_sessions[i].Empty())
				{
					if (_sessions[i].IsOutput())
					{
						// �������� �������
						MarkDataForRetry(_sessions[i].GetTicket());
					}
					// � ���� ��������
					_sessions[i].Reset();
				}
			}
			_firstDequeuedSession = -1;
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
		
		// ��������� ������ � ������� ��� ��������
		bool EnqueueData(AddressType *destination, int destinationCount, void *data, int dataCount, unsigned int deliveryTimeout, int &packetId)
		{
			static unsigned char nextPacketId = 0;
			
			if (destination == 0 || destinationCount < 1 || data == 0 || dataCount < 1)
			{
				return false;
			}
			
			if (destinationCount > MaxDestinationCount)
			{
				return false;
			}
			
			if (dataCount > MaxDataCountPerSession)
			{
				return false;
			}
			
			void *dataChunks[2] = {destination, data};
			typename InputQueueType::DataCountType dataChunkSizes[2] = {destinationCount, dataCount};
			
			if (!_inputQueue.IsFits(dataChunks, dataChunkSizes, 2))
			{
				return false;
			}
			
			packetId = nextPacketId++;
			
			InputQueueHeaderType header;
			header.PacketId = packetId;
			header.State = InputQueueHeaderType::StateCodeFresh;
			header.EnqueueTime = GetTime();
			header.DeliveryTimeout = deliveryTimeout;
			bool pushResult = _inputQueue.Push(&header, dataChunks, dataChunkSizes, 2);
			
			return pushResult;
		}
		
		// ������� ������ �� ������� ��� ��������
		void DequeueData(AddressType selfAddress)
		{
			if (!_running)
			{
				return;
			}
			
			if (_inputQueue.NowPush())
			{
				return;
			}
			
			// ������� �����
			float curTime = GetTime();
			for (int i = 0; i < _maxActiveOutputSessionsCount && i < _inputQueue.ItemsCount(); i++)
			{
			// ���������� �������� ���������� ������ ������ ������������ � ���� ��������� ������ � ���� ������ � �������
			if (ActiveOutputSessionsCount() < _maxActiveOutputSessionsCount && EmptySessionPresent() && _inputQueue.ItemsCount() > 0)
			{
				// ���������
				InputQueueHeaderType header;
				// ������ ���������
				AddressType destination[MaxDestinationCount];
				// ������
				unsigned char data[MaxDataCountPerSession];
				
				void *dataChunks[2] = {destination, data};
				typename InputQueueType::DataCountType destinationCount;
				typename InputQueueType::DataCountType dataCount;
				typename InputQueueType::DataCountType *dataChunkSizes[2] = {&destinationCount, &dataCount};
				// ��������� �� �������
				typename InputQueueType::DataCountType dataChunksCount = 0;
				_inputQueue.Pop(&header, dataChunks, dataChunkSizes, &dataChunksCount);
				
				// ���������� � �������
				bool returnToQueue = true;
				
				switch (header.State)
				{
				case InputQueueHeaderType::StateCodeFresh:
				case InputQueueHeaderType::StateCodeRetry:
					{
						bool createSession = true;
						
						// ����� ������ ����� �������� ����� �������� ����� ����� ����� �������, ��������, ����� ��� ���� ����� ���������������
						if (curTime < header.EnqueueTime)
						{
							// �������, ��� ������ ������
							header.EnqueueTime = curTime;
						}
						
						/* ��������� ������� ������, ���� ���� �� ���� �� ��������, �� ����� ���������� �������
						// ��� �� ������ ��� �������� ���������
						//if (header.State == InputQueueHeaderType::StateCodeRetry)
						*/
						{
							// ���� �� ��������� �������
							float elapsed = curTime - header.EnqueueTime;
							// �������� ������� �������� � �������
							if (elapsed >= header.DeliveryTimeout)
							{
								// � ������� �� ����������
								returnToQueue = false;
								// ������ �� �������
								createSession = false;
								
								// �������� � ���������� �������� �� ������� ���������
								for (int i = 0; i < destinationCount; i++)
								{
									_onTxCompleteCallback.Call(header.PacketId, destination[i], TxResultDeliveryImpossible);
								}
							}
						}
						
						if (createSession)
						{
							// ��������� ����� ������
							SessionType *session = GetEmptySession();
							if (session != 0)
							{
								TicketType ticket = CreateTicket();
								// ������ ��������� ������ ���������� ������ ����� ����������� ������� ��������� ������ ���������
								// ���������� ������ ����� ���������� ���� � ����� ���� ��������� ��������� ������
								if (_prevCreatedTicket != ticket)
								{
									// ������������ ���������
									header.Ticket = ticket;
									header.State = InputQueueHeaderType::StateCodeProgress;
									
									session->Init(selfAddress, ticket, destination, destinationCount, data, dataCount, header.DeliveryTimeout, GetRouteCallback, RxDoneCallback, DebugDataCallback, this);
									
									_prevCreatedTicket = ticket;
								}
							}
						}
					}
				}
				
				if (returnToQueue)
				{
					// ���������� � ����� �������
					typename InputQueueType::DataCountType dataChunkPushSizes[2] = {destinationCount, dataCount};
					_inputQueue.Push(&header, dataChunks, dataChunkPushSizes, dataChunksCount);
				}
			}
			}
		}
		
		int GetOutputData(AddressType selfAddress, unsigned char *buffer, int bufferSize)
		{
			// ������� �����
			float curTime = GetTime();
			
			int dequeuedDataCount = 0;
			
			int i = 0;
			// �������� ������ ��� �� � ����� � ��� �� ������, � �� ���������, ����� �� �������� ��������� ���������� ���� � ��� �� �����
			static int nextSession = 0;
			while (i < MaxSessionCount)
			{
				// �� ��� ������ DequeueData ��������� ��� ������, �� �� ������ ��������� ������ �� ����� � ��� �� ������ ��������� ���
				// _firstDequeuedSession ������������ � -1 ��� ������ UpdateSessions
				if (_firstDequeuedSession >= 0)
				{
					if (nextSession == _firstDequeuedSession)
					{
						break;
					}
				}
				bool needMoreSpace = false;
				dequeuedDataCount = _sessions[nextSession].PopNextOutputData(buffer, bufferSize, curTime, needMoreSpace);
				
				if (dequeuedDataCount > 0 && _firstDequeuedSession < 0)
				{
					_firstDequeuedSession = nextSession;
				}
				
				i++;
				nextSession++;
				if (nextSession >= MaxSessionCount)
				{
					nextSession = 0;
				}
				
				if (dequeuedDataCount > 0)
				{
					break;
				}
			}
			
			return dequeuedDataCount;
		}
		
		void RemoveDataFromQueueIfComplete(TicketType ticket, unsigned char &packetId, AddressType *destination, typename InputQueueType::DataCountType *destinationCount, AddressType *notDeliveredList, typename InputQueueType::DataCountType notDeliveredListSize)
		{
			if (destination == 0 || destinationCount == 0)
			{
				return;
			}
			
			bool found = false;
			for (int i = 0; i < _inputQueue.ItemsCount(); i++)
			{
				InputQueueHeaderType header;
				_inputQueue.Get(0, &header, 0, 0, 0);
				if (header.Ticket == ticket && !found)
				{
					packetId = header.PacketId;
					// ������
					unsigned char data[MaxDataCountPerSession];
					
					void *dataChunks[2] = {destination, data};
					typename InputQueueType::DataCountType dataCount;
					typename InputQueueType::DataCountType *dataChunkSizes[2] = {destinationCount, &dataCount};
					// ��������� �� �������
					typename InputQueueType::DataCountType dataChunksCount = 0;
					_inputQueue.Pop(&header, dataChunks, dataChunkSizes, &dataChunksCount);
					
					// ��������� ������ ���� ����������
					for (int j = 0; j < *destinationCount; j++)
					{
						bool delivered = true;
						if (notDeliveredList != 0)
						{
							for (int k = 0; k < notDeliveredListSize; k++)
							{
								if (notDeliveredList[k] == destination[j])
								{
									delivered = false;
									break;
								}
							}
						}
						// ������� ������ � ������ ���� �� �����������
						if (!delivered)
						{
							// ������� ������� ������� ���������
							for (int k = j + 1; k < *destinationCount; k++)
							{
								destination[k - 1] = destination[k];
							}
							(*destinationCount)--;
							j--;
						}
					}
					
					if (notDeliveredListSize > 0)
					{
						// ���������� � ����� ������� �� ������� ���� ���� ��������� ��������
						dataChunks[0] = notDeliveredList;
						typename InputQueueType::DataCountType dataChunkPushSizes[2] = {notDeliveredListSize, dataCount};
						header.State = InputQueueHeaderType::StateCodeRetry;
						_inputQueue.Push(&header, dataChunks, dataChunkPushSizes, dataChunksCount);
					}
					
					found = true;
					// �������� ��� ����� �������, ��������������� ������, �� ���� �� ���������, ����� �� �������� ������� �������
					// ��������� ��� �������, ����� ���, ��� ��� ������ �������, ������
					
					// �� ���� ����������� ������ ���������� �������, �� ����������� ��� ������� ������� ��� ������
					if (i == 0)
					{
						break;
					}
				}
				
				_inputQueue.RoolOverToNextChunk();
			}
		}
		
		void MarkDataForRetry(TicketType ticket)
		{
			bool marked = false;
			for (int i = 0; i < _inputQueue.ItemsCount(); i++)
			{
				InputQueueHeaderType header;
				_inputQueue.Get(0, &header, 0, 0, 0);
				if (header.Ticket == ticket && !marked)
				{
					header.State = InputQueueHeaderType::StateCodeRetry;
					_inputQueue.UpdateFrontHeader(&header);
					marked = true;
					
					if (i == 0)
					{
						break;
					}
				}
				
				_inputQueue.RoolOverToNextChunk();
			}
		}
		
		// ��������� ������� ������� ��������� ���������� ��� ��������� ������
		void SetGetTicketInfoCallback(typename GetTicketInfoCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_getTicketInfoCallback.Set(callback, callbackParam);
		}
		
		// ��������� ������� ������� ��������� �������� �� ����
		void SetGetRouteCallback(typename GetRouteCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_getRouteCallback.Set(callback, callbackParam);
		}
		
		// ��������� ���������� ������� ��������� ������
		void SetOnDataReceivedCallback(typename OnDataReceivedCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onDataReceivedCallback.Set(callback, callbackParam);
		}
		
		// ��������� ���������� ������� ��������� �������� ������
		void SetOnTxCompleteCallback(typename OnTxCompleteCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_onTxCompleteCallback.Set(callback, callbackParam);
		}
		
		// ��������� ���������� ���������� ������
		void SetDebugDataCallback(typename DebugDataCallbackType::CallbackPtrType callback, void *callbackParam)
		{
			_debugDataCallback.Set(callback, callbackParam);
		}
		
		
		// ���������� �������� ������
		void OnRxSessionData(AddressType selfAddress, AddressType sourceAddress, unsigned char *data, int dataCount)
		{
			
			if (!_running)
			{
				return;
			}
			
			if (!SessionType::BaseDataValid(data, dataCount))
			{
				return;
			}
			
			bool dataProcessed = false;
			
			for(int i = 0; i < MaxSessionCount; i++)
			{
				if (_sessions[i].OnDataReceived(sourceAddress, data, dataCount))
				{
					dataProcessed = true;
				}
			}
			
			if (!dataProcessed)
			{
				TicketType inputTicket = SessionType::GetTicket(data, dataCount);
				
				if (SessionType::ShouldOpenSession(data, dataCount))
				{
					if (!_transportUsedTycketsBuffer.Present(inputTicket))
					{
						SessionType *session = GetEmptySession();
						if (session != 0)
						{
							if (session->Init(selfAddress, sourceAddress, data, dataCount, GetRouteCallback, RxDoneCallback, DebugDataCallback, this))
							{
								_transportUsedTycketsBuffer.Push(inputTicket);
							}
						}
						else
						{
							//TODO
							__no_operation();
						}
					}
					else
					{
						// �� ���� ���� ������ � ����� ������� ��� ���� ������� ���������
						if (_doneInputSessionsList.Present(inputTicket))
						{
							// ��������� ����������� ������, ������� ������ ���������� �������������
							SessionType *session = GetEmptySession();
							if (session != 0)
							{
								session->Init(selfAddress, sourceAddress, data, dataCount, DebugDataCallback, this);
							}
						}
					}
				}
			}
		}
		
		void UpdateSessions()
		{
			if (_inputQueue.NowPush())
			{
				return;
			}
			
			for(int i = 0; i < MaxSessionCount; i++)
			{
				_sessions[i].UpdateState(GetTime(), _sessions[i].GetTicket().NetTime);
			}
			
			for(int i = 0; i < MaxSessionCount; i++)
			{
				if (_sessions[i].IsComplete())
				{
					if (_sessions[i].IsTxComplete())
					{
						unsigned char packetId = 0;
						// ������ ��������� ���� �� ������� ���������
						AddressType destinationNotDelivered[MaxDestinationCount];
						typename InputQueueType::DataCountType destinationNotDeliveredCount = 0;
						destinationNotDeliveredCount = _sessions[i].GetUndeliveredNodes(destinationNotDelivered, MaxDestinationCount);
						
						// ������ ���������
						AddressType destination[MaxDestinationCount];
						typename InputQueueType::DataCountType destinationCount = 0;
						// ������� �� �������
						RemoveDataFromQueueIfComplete(_sessions[i].GetTicket(), packetId, destination, &destinationCount, destinationNotDelivered, destinationNotDeliveredCount);
						// �������� � ���������� �������� �� ������� ���������
						for (int i = 0; i < destinationCount; i++)
						{
							_onTxCompleteCallback.Call(packetId, destination[i], TxResultDelivered);
						}
					}
					
					if (_sessions[i].IsTxImpossible())
					{
						// ��������
						MarkDataForRetry(_sessions[i].GetTicket());
						// �������� � ����������
						//_onTxCompleteCallback.Call(0, TxResultDeliveryImpossible);
					}
					
					if (_sessions[i].IsTxError())
					{
						unsigned char packetId = 0;
						// ������ ���������
						AddressType destination[MaxDestinationCount];
						typename InputQueueType::DataCountType destinationCount = 0;
						// ������� �� �������
						RemoveDataFromQueueIfComplete(_sessions[i].GetTicket(), packetId, destination, &destinationCount, 0, 0);
						// �������� � ���������� �������� �� ������� ���������
						for (int i = 0; i < destinationCount; i++)
						{
							_onTxCompleteCallback.Call(packetId, destination[i], TxResultError);
						}
					}
					
					_sessions[i].Reset();
				}
			}
			
			_firstDequeuedSession = -1;
		}
	protected:
		bool EmptySessionPresent()
		{
			for(int i = 0; i < MaxSessionCount; i++)
			{
				if (_sessions[i].Empty())
				{
					return true;
				}
			}
			
			return false;
		}
		
		int ActiveOutputSessionsCount()
		{
			int res = 0;
			
			for(int i = 0; i < MaxSessionCount; i++)
			{
				if (!_sessions[i].Empty() && _sessions[i].IsOutput())
				{
					res++;
				}
			}
			
			return res;
		}
		
		SessionType *GetEmptySession()
		{
			for(int i = 0; i < MaxSessionCount; i++)
			{
				if (_sessions[i].Empty())
				{
					return &_sessions[i];
				}
			}
			
			return 0;
		}
		
		TicketType CreateTicket()
		{
			int selfAddress = 0;
			float netTime = 0;
			_getTicketInfoCallback.Call(selfAddress, netTime);
			TicketType ticket(selfAddress, netTime);
			return ticket;
		}
		
		float GetTime()
		{
			int selfAddress = 0;
			float netTime = 0;
			_getTicketInfoCallback.Call(selfAddress, netTime);
			
			return netTime;
		}
		
		static void GetRouteCallback(void *callbackParam, AddressType src, AddressType dst, AddressType *buffer, int bufferSize, int *routeLen)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Transport *im = (Transport *)callbackParam;
			im->_getRouteCallback.Call(src, dst, buffer, bufferSize, routeLen);
		}
		
		static void RxDoneCallback(void *callbackParam, bool rxSession, TicketType ticket, AddressType source)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Transport *im = (Transport *)callbackParam;
			
			for(int i = 0; i < MaxSessionCount; i++)
			{
				if (im->_sessions[i].GetTicket() == ticket)
				{
					if (rxSession)
					{
						int dataCount = im->_sessions[i].GetData(im->_outputBuffer, sizeof(_outputBuffer));
						im->_onDataReceivedCallback.Call(source, im->_outputBuffer, dataCount);
					}
					
					im->_doneInputSessionsList.Push(ticket);
					
					return;
				}
			}
		}
		
		static void DebugDataCallback(void *callbackParam, int code, void *data, int dataCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			Transport *im = (Transport *)callbackParam;
			im->_debugDataCallback.Call(code, data, dataCount);
		}
	};
}
}
}

#endif