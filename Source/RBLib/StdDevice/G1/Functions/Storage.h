///////////////////////////////////////////////////////////////////////////////
//	Класс реализации работы с функцие устройтсва "Хранилище" по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_FUNCTIONS_STORAGE_H
#define G1_FUNCTIONS_STORAGE_H

#include "../Function.h"
#include "../../../Protocol/G1/Function/Storage/FunctionData.h"

namespace Rblib
{
namespace StdDevice
{
namespace G1
{
namespace Functions
{
	template
		<
			unsigned int maxDataSize,
			int timerTickCountPerSecond,
			int DefaultTimeout
		>
	class Thread
	{
	public:
		static const int TimerTickCountPerSecond = timerTickCountPerSecond;
		static const unsigned int MaxDataSize = maxDataSize;
		
		enum State
		{
			StateDisabled = 0,
			StateEnabled,
			StateTxReadFile,
			StateTxReadFileWaitDelivery,
			StateRxReadFileResultWait,
			StateTxPushFile,
			StateTxPushFileWaitDelivery,
			StateRxPushFilePartsWait,
			StateIdle
		};
		
		typedef CallbackWrapper<Protocol::G1::Function::Storage::MessageCode, void *, unsigned int, int, int &> SendCommandToFunctionCallbackType;
	public:
		int _id;
		State _state;
		int _waitPacketId;
		int _waitTimeout;

		bool _readFile;
		unsigned int _readStartOffset;
		unsigned int _readSize;
		Protocol::G1::Function::Storage::ResultCode _readFileResultCode;
		
		bool _pushFile;
		unsigned int _pushFileAddr;
		unsigned int _pushFilePartSize;
		BitFlagsHolder _requestedParts;

		unsigned int _loadFileId; // id файла, который требуется загрузить
		
		unsigned char _dataBuffer[maxDataSize];
		
		SendCommandToFunctionCallbackType SendCommandToFunctionCallback;

	public:	

		Thread()
		{
			_id = -1;
			Reset();
		}
		
		void SetId(int id)
		{
			_id = id;
		}
		
		void Reset()
		{
			_state = StateDisabled;
			_waitPacketId = -1;
			_waitTimeout = 0;
			_readFile = false;
			_loadFileId = -1;
			_readStartOffset = -1;
			_readSize = 0;
			_readFileResultCode = Protocol::G1::Function::Storage::ResultUnknown;
			_pushFile = false;
			_pushFileAddr = -1;
			_pushFilePartSize = 0;
		}
		
		void SetEnabled()
		{
			_state = StateEnabled;
		}
		
		void OnTick()
		{
			switch(_state)
			{
			case StateDisabled:
				{
				}
				break;
			case StateEnabled:
				{
					if (_readFile)
					{
						_state = StateTxReadFile;
						break;
					}
					
					if (_pushFile)
					{
						_state = StateTxPushFile;
						break;
					}
				}
				break;
			case StateTxReadFile:
				{
					_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = DefaultTimeout;
					
					_readFileResultCode = Protocol::G1::Function::Storage::ResultUnknown;
					
					Protocol::G1::Function::Storage::MessageReadFile message;
					message.FileId = _loadFileId;
					message.StartOffset = _readStartOffset;
					message.Size = _readSize;
					
					unsigned char data[32];
					int dataLen = message.ToByteArray(data, sizeof(data));
					
					 SendCommandToFunctionCallback.Call(Protocol::G1::Function::Storage::MessageCodeReadFile, data, dataLen, deliveryTimeout, _waitPacketId);
					_state = StateTxReadFileWaitDelivery;
				}
				break;
			case StateTxReadFileWaitDelivery:
			case StateRxReadFileResultWait:
				{
					_waitTimeout--;
					if (_waitTimeout < 1)
					{
						_state = StateTxReadFile;
					}
				}
				break;
			case StateTxPushFile:
				{
					_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = DefaultTimeout;
					
					_readFileResultCode = Protocol::G1::Function::Storage::ResultUnknown;
					
					Protocol::G1::Function::Storage::MessagePushFile message;
					message.PushAddress = _pushFileAddr;
					message.FileId = _loadFileId;
					message.PartSize = _pushFilePartSize;
					message.RequestedParts = _requestedParts;
					
					unsigned char data[32];
					int dataLen = message.ToByteArray(data, sizeof(data));
					
					 SendCommandToFunctionCallback.Call(Protocol::G1::Function::Storage::MessageCodePushFile, data, dataLen, deliveryTimeout, _waitPacketId);
					_state = StateTxPushFileWaitDelivery;
				}
				break;
			case StateTxPushFileWaitDelivery:
				{
					_waitTimeout--;
					if (_waitTimeout < 1)
					{
						_state = StateTxPushFile;
					}
				}
				break;
			case StateRxPushFilePartsWait:
				{
					_waitTimeout--;
					if (_waitTimeout < 1)
					{
						_pushFile = false;
						_state = StateIdle;
					}
				}
				break;
			case StateIdle:
				{
					_state = StateEnabled;
				}
				break;
			}
		}
		
		void OnTxComplete(int packetId, bool delivered)
		{
			bool waitComplete = packetId == _waitPacketId;
			// дождались завершения передачи
			if (waitComplete)
			{
				switch(_state)
				{
				case StateTxReadFileWaitDelivery:
					{
						if (delivered)
						{
							_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxReadFileResultWait;
						}
						else
						{
							_state = StateTxReadFile;
						}
					}
					break;
				case StateTxPushFileWaitDelivery:
					{
						if (delivered)
						{
							_pushFile = false;
							_state = StateIdle;
						}
						else
						{
							_state = StateTxPushFile;
						}
					}
					break;
				}
			}
		}
		
		void OnCommandFromFunction(unsigned int eventCode, int devId, Protocol::G1::Device::FunctionCode functionCode, unsigned char *data, int dataCount)
		{
			Protocol::G1::Function::Storage::InputMessageFormat command(data, dataCount);
			if (command.Valid())
			{
				if (command.Data.Valid())
				{
					switch(command.Data.Code)
					{
					case Protocol::G1::Function::Storage::MessageCodeReadFileResult:
						{
							bool readed = false;
							
							if (command.Data.DataLen >= 1 + 4 + 4 + 4)
							{
								unsigned char *data = (unsigned char *)command.Data.Data;
								if (data)
								{
									readed = true;
									
									int i = 0;
									_readFileResultCode = (Protocol::G1::Function::Storage::ResultCode) data[i];
									i++;
									
									unsigned int fileId = 0;
									for (int j = 0; j < sizeof(fileId); j++)
									{
										fileId |= (unsigned int)data[i] << (j * 8);
										i++;
									}
									
									unsigned int startOffset = 0;
									for (int j = 0; j < sizeof(startOffset); j++)
									{
										startOffset |= (unsigned int)data[i] << (j * 8);
										i++;
									}
									
									unsigned int size = 0;
									for (int j = 0; j < sizeof(size); j++)
									{
										size |= (unsigned int)data[i] << (j * 8);
										i++;
									}
									
									unsigned char *fileData = &data[i];
									
									if (size > MaxDataSize)
									{
										size = MaxDataSize;
									}
									
									for (int i = 0; i < size; i++)
									{
										_dataBuffer[i] = fileData[i];
									}
									
									readed &= fileId == _loadFileId;
									
									if (_readFile) // если получаем фото методом чтения, то проверяем смещение и размер куска
									{
										readed &= startOffset == _readStartOffset;
										readed &= size == _readSize;
									}
									else
									{
										_readStartOffset = startOffset;
										_readSize = size;
									}
								}
							}
							
							switch(_state)
							{
							case StateRxReadFileResultWait:
								{
									_readFile = false;
									_state = StateIdle;
								}
								break;
							}
							
							// уведомление о результате чтения
							readed &= _readFileResultCode == Protocol::G1::Function::Storage::ResultOk;
							
							Event e(eventCode, EventCodeStorageFileReadResult, devId, _id, readed);
							Singletone<EventDispetcher>::Instance().PushEvent(e);
						}
						break;

					case Protocol::G1::Function::Storage::MessageCodePushFileResult:
						{
							if (command.Data.DataLen < 5)
								break;

							unsigned char *data = (unsigned char *)command.Data.Data;
							if (*((unsigned int*)(data + 1)) != _loadFileId)
								break;

							Event e(eventCode, EventCodeStoragePushFileResult, data[0]);
							Singletone<EventDispetcher>::Instance().PushEvent(e);
						}
						break;

					case Protocol::G1::Function::Storage::MessageCodePushFileDone:


						if (command.Data.DataLen < sizeof(unsigned int))
							break;

						if (*((unsigned int*)command.Data.Data) != _loadFileId)
							break;

						Event e(eventCode, EventCodeStoragePushDone, devId, functionCode);
						Singletone<EventDispetcher>::Instance().PushEvent(e);

						break;
					}
				}
			}
		}
		
		bool Idle()
		{
			bool res = _state == StateIdle || _state == StateEnabled;
			res &= !_readFile;
			return res;
		}
		
		void ReadFile(unsigned int fileId, unsigned int startOffset, unsigned int size)
		{
			_readFile = true;
			_loadFileId = fileId;
			_readStartOffset = startOffset;
			_readSize = size;
		}
		
		void PushFile(unsigned int pushAddr, unsigned int fileId, unsigned int partSize)
		{
			_pushFileAddr = pushAddr;
			_loadFileId = fileId;
			_pushFilePartSize = partSize;
			_requestedParts = BitFlagsHolder();

			_pushFile = true;
		}
		
		void PushFile(unsigned int pushAddr, unsigned int fileId, unsigned int partSize, const BitFlagsHolder& requestedParts)
		{
			_pushFileAddr = pushAddr;
			_loadFileId = fileId;
			_pushFilePartSize = partSize;
			_requestedParts = requestedParts;

			_pushFile = true;
		}

		bool GetReadedData(unsigned int &fileId, unsigned int &startOffset, unsigned int &dataLen, unsigned char * &data)
		{
			fileId = _loadFileId;
			startOffset = _readStartOffset;
			dataLen = _readSize;
			data = _dataBuffer;
			
			return true;
		}
	};
	
	
	template
		<
			class Base,
			unsigned int maxDataSize,
			int timerTickCountPerSecond
		>
	class Storage : public Base
	{
	public:
		static const unsigned int MaxDataSize = maxDataSize;
		static const int TimerTickCountPerSecond = timerTickCountPerSecond;
		static const int ThreadCount = 1;
		static const int MaxThreadDataSize = 512;
		
		enum State
		{
			StateDisabled = 0,
			StateEnabled,
			StateIdle
		};
		
	protected:
		State _state;
		Thread<MaxThreadDataSize, TimerTickCountPerSecond, Base::DefaultTimeout> _thread[ThreadCount];
	public:
		Storage()
		{
			for (int i = 0; i < ThreadCount; i++)
			{
				_thread[i].SetId(i);
				_thread[i].Reset();
				_thread[i].SendCommandToFunctionCallback.Set(SendCommandToFunction, this);
			}
		}
		
		virtual Protocol::G1::Device::FunctionCode FunctionCode()
		{
			return Protocol::G1::Device::FunctionCodeStorage;
		}
		
		virtual void Reset()
		{
			Base::Reset();
			ResetThreads();
			_state = StateDisabled;
		}
		
		virtual void SetEnabled()
		{
			Base::SetEnabled();
			
			_state = StateEnabled;
		}
		
		virtual void OnTick()
		{
			Base::OnTick();
			
			switch(_state)
			{
			case StateDisabled:
				{
				}
				break;
			case StateEnabled:
				{
					for (int i = 0; i < ThreadCount; i++)
					{
						_thread[i].SetEnabled();
					}
					
					_state = StateIdle;
				}
				break;
			case StateIdle:
				{
				}
				break;
			}
			
			for (int i = 0; i < ThreadCount; i++)
			{
				_thread[i].OnTick();
			}
		}
		
		int SendCommandToFunction(Protocol::G1::Function::Storage::MessageCode code, void *data, int dataCount, int timeout)
		{
			Protocol::G1::Function::Storage::OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(Base::_functionData, sizeof(Base::_functionData));
			
			int packetId = 0;
			Base::SendCommandToFunctionCallback.Call(Base::_functionData, functionDataLen, timeout, packetId);
			return packetId;
		}
		
		static void SendCommandToFunction(void *callbackParam, Protocol::G1::Function::Storage::MessageCode code, void *data, unsigned int dataCount, int timeout, int &packetId)
		{
			Storage *im = (Storage *)callbackParam;
			
			if (!im)
			{
				return;
			}
			
			packetId = im->SendCommandToFunction(code, data, dataCount, timeout);
		}
		
		virtual void OnTxComplete(int packetId, bool delivered)
		{
			Base::OnTxComplete(packetId, delivered);
			
			for (int i = 0; i < ThreadCount; i++)
			{
				_thread[i].OnTxComplete(packetId, delivered);
			}
		}
		
		virtual void OnCommandFromFunction(unsigned char *data, int dataCount)
		{
			Base::OnCommandFromFunction(data, dataCount);
			
			for (int i = 0; i < ThreadCount; i++)
			{
				_thread[i].OnCommandFromFunction(Base::EventCode, Base::_deviceId, FunctionCode(), data, dataCount);
			}
		}
		
		bool ReadFile(unsigned int fileId, unsigned int startOffset, unsigned int Size, int &threadId)
		{
			bool res = false;
			
			for (int i = 0; i < ThreadCount; i++)
			{
				if (_thread[i].Idle())
				{
					_thread[i].ReadFile(fileId, startOffset, Size);
					res = true;
					threadId = i;
					break;
				}
			}
			
			return res;
		}
		
		bool PushFile(unsigned int pushAddr, unsigned int fileId, unsigned int partSize)
		{
			bool res = false;
			
			for (int i = 0; i < ThreadCount; i++)
			{
				if (_thread[i].Idle())
				{
					_thread[i].PushFile(pushAddr, fileId, partSize);
					res = true;
					break;
				}
			}
			
			return res;
		}

		bool PushFile(unsigned int pushAddr, unsigned int fileId, unsigned int partSize, const BitFlagsHolder& requestedParts)
		{
			for (int i = 0; i < ThreadCount; i++)
			{
				if (_thread[i].Idle())
				{
					_thread[i].PushFile(pushAddr, fileId, partSize, requestedParts);
					return true;
				}
			}
			
			return false;
		}
		
		bool GetData(int threadId, unsigned int &fileId, unsigned int &startOffset, unsigned int &dataLen, unsigned char * &data)
		{
			bool res = false;
			
			if (threadId < 0 || threadId >= ThreadCount)
			{
				return res;
			}
			
			res = _thread[threadId].GetReadedData(fileId, startOffset, dataLen, data);
			
			return res;
		}
	protected:
		int SecondsToTimerTicks(int seconds)
		{
			return TimerTickCountPerSecond * seconds;
		}

		void ResetThreads()
		{
			for (int i = 0; i < ThreadCount; i++)
			{
				_thread[i].Reset();
			}
		}
	};
}
}
}
}

#endif