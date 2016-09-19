
#ifndef OSCCACHE_H
#define OSCCACHE_H

template
	<
		int oscRecordSize,
		int oscRequestMaxPortionSize
	>
class OscCacheImplementer
{
public:
	enum State
	{
		StateInit,
		StateCreateNewFile,
		StateFillBuffer,
		StateSendSequest,
		StateWaitPart,
		StateSaveBuffer,
		StateNextFile
	};
	
	static const unsigned int OscRecordSize = oscRecordSize;
	static const unsigned int OscRequestMaxPortionSize = oscRequestMaxPortionSize;
	static const unsigned int OscBufferMaxPointsCount = 65535;
	static const unsigned int OscFileSize = OscBufferMaxPointsCount * OscRecordSize;
	static const unsigned int MaxFileCount = 5;
	static const unsigned int PortionSize = OscRecordSize * OscRequestMaxPortionSize;
	static const unsigned int BufferSize = PortionSize * 2;
	
	typedef Rblib::CallbackWrapper<bool &> FileSystemReadyCallbackType;
	typedef Rblib::CallbackWrapper<unsigned int, unsigned int, bool &> CreateFileCallbackType;
	typedef Rblib::CallbackWrapper<unsigned int, bool &> DeleteFileCallbackType;
	typedef Rblib::CallbackWrapper<unsigned int, unsigned int, unsigned char *, unsigned int, bool &> WriteFileCallbackType;
	typedef Rblib::CallbackWrapper<unsigned int, bool &> SendRequestCallbackType;
	typedef Rblib::CallbackWrapper<bool &> AllowOscSkipCallbackType;
	typedef Rblib::CallbackWrapper<bool &> GetOscPendingCallbackType;
public:
	FileSystemReadyCallbackType FileSystemReadyCallback;
	CreateFileCallbackType CreateFileCallback;
	DeleteFileCallbackType DeleteFileCallback;
	WriteFileCallbackType WriteFileCallback;
	SendRequestCallbackType SendRequestCallback;
	AllowOscSkipCallbackType AllowOscSkipCallback;
	GetOscPendingCallbackType GetOscPendingCallback;
protected:
	State _state;
	unsigned int _fileNumber;
	unsigned int _currentOffset;
	unsigned int _bufferPos;
	unsigned char _buffer[BufferSize];
	unsigned int _loadedPos;
	unsigned int _oscPos;
	bool _oscRollower;
	unsigned int _requestPos;
public:
	OscCacheImplementer()
	{
		_state = StateInit;
		_fileNumber = 0;
		_bufferPos = 0;
		_loadedPos = 0;
		_oscPos = 0;
		_oscRollower = false;
		_requestPos = 0;
	}
	
	bool AllowRead()
	{
		return _state == StateFillBuffer;
	}
	
	static void AllowRead(void *callbackParam, bool &allow)
	{
		OscCacheImplementer *im = (OscCacheImplementer *)callbackParam;
		
		if (!im)
		{
			return;
		}
		
		allow = im->AllowRead();
	}
	
	void OscPosUpdated(unsigned int pos, bool rollower)
	{
		_oscPos = pos;
		// "или" чтобы флаг не снялся пока мы сами его не снимем
		_oscRollower |= rollower;
	}
	
	static void OscPosUpdated(void *p, unsigned int pos, bool rollower)
	{
		OscCacheImplementer *im = (OscCacheImplementer *)p;
		
		if (!im)
		{
			return;
		}
		
		im->OscPosUpdated(pos, rollower);
	}
	
	void StoreOscPart(unsigned int offset, unsigned char *data, int dataCount)
	{
		//return;
		if (_state != StateWaitPart)
		{
			return;
		}
		
		if (_bufferPos == 0)
		{
			_currentOffset = offset;
		}
		
		if (_bufferPos + dataCount <= BufferSize)
		{
			for (int i = 0; i < dataCount; i += 2)
			{
				_buffer[_bufferPos + i + 1] = data[i];
				_buffer[_bufferPos + i] = data[i + 1];
			}
		}
		
		_bufferPos += dataCount;
		
		_requestPos = (offset + dataCount) / OscRecordSize;
		
		if ((_bufferPos > BufferSize - PortionSize) || (_currentOffset + _bufferPos >= OscFileSize))
		{
			_state = StateSaveBuffer;
		}
		else
		{
			_state = StateFillBuffer;
		}
	}
	
	static void StoreOscPart(void *callbackParam, unsigned int offset, unsigned char *data, int dataCount)
	{
		OscCacheImplementer *im = (OscCacheImplementer *)callbackParam;
		
		if (!im)
		{
			return;
		}
		
		im->StoreOscPart(offset, data, dataCount);
	}
	
	unsigned int GetCurrentFileNumber()
	{
		return _fileNumber;
	}
	
	static void GetCurrentFileNumber(void *callbackParam, unsigned int &fileNumber)
	{
		OscCacheImplementer *im = (OscCacheImplementer *)callbackParam;
		
		if (!im)
		{
			return;
		}
		
		fileNumber = im->GetCurrentFileNumber();
	}
	
	unsigned int GetRequestPos()
	{
		return _requestPos;
	}
	
	bool IsDataLoaded(unsigned int fileNumber, int pos)
	{
		if (pos < 0)
		{
			return true;
		}
		
		if (pos > OscBufferMaxPointsCount)
		{
			pos -= OscBufferMaxPointsCount;
			fileNumber++;
		}
		
		return GetCurrentFileNumber() >= fileNumber && (_loadedPos >= (pos * OscRecordSize));
	}
	
	static void IsDataLoaded(void *callbackParam, unsigned int fileNumber, int pos, bool &loaded)
	{
		OscCacheImplementer *im = (OscCacheImplementer *)callbackParam;
		
		if (!im)
		{
			return;
		}
		
		loaded = im->IsDataLoaded(fileNumber, pos);
	}
	
	void Run()
	{
		switch(_state)
		{
		case StateInit:
			{
				_fileNumber = 0;
				bool ready = false;
				FileSystemReadyCallback(ready);
				if (ready)
				{
					_state = StateCreateNewFile;
				}
			}
			break;
		case StateCreateNewFile:
			{
				bool result = false;
				CreateFileCallback(_fileNumber, OscFileSize, result);
				
				if (result)
				{
					_currentOffset = 0;
					_bufferPos = 0;
					_loadedPos = 0;
					_oscRollower = false;
					_requestPos = 0;
					_state = StateFillBuffer;
				}
			}
			break;
		case StateFillBuffer:
			{
				bool eventPending = false;
				AllowOscSkipCallback(eventPending);
				bool allowSkip = !eventPending;
				
				if (_oscRollower)
				{
					if (_requestPos < 65535)
					{
						if (allowSkip)
						{
							if (65535 - _requestPos > 5000)
							{
								_requestPos += 5000;
								break;
							}
							else
							{
								int delta = 65535 - _requestPos + _oscPos;
								if (delta > 5000)
								{
									_state = StateNextFile;
									break;
								}
							}
						}
						
						bool result = false;
						SendRequestCallback(_requestPos, result);
						if (result)
						{
							_state = StateWaitPart;
						}
					}
					else
					{
						_state = StateNextFile;
					}
				}
				else
				{
					if (_requestPos < _oscPos)
					{
						if (allowSkip)
						{
							int delta = _oscPos - _requestPos;
							if (delta > 5000)
							{
								_requestPos += 5000;						
								break;
							}
						}
						
						bool result = false;
						SendRequestCallback(_requestPos, result);
						if (result)
						{
							_state = StateWaitPart;
						}
					}
				}
			}
			break;
		case StateWaitPart:
			{
				bool pending = false;
				GetOscPendingCallback(pending);
				
				if (!pending)
				{
					_state = StateFillBuffer;
				}
			}
			break;
		case StateSaveBuffer:
			{
				bool result = false;
				WriteFileCallback(_fileNumber, _currentOffset, _buffer, _bufferPos, result);
				
				if (_currentOffset + _bufferPos >= OscFileSize)
				{
					_state = StateNextFile;
				}
				else
				{
					_state = StateFillBuffer;
				}
				
				if (_currentOffset + _bufferPos > _loadedPos)
				{
					_loadedPos = _currentOffset + _bufferPos;
				}
				
				_currentOffset = 0;
				_bufferPos = 0;
				
			}
			break;
		case StateNextFile:
			{
				_fileNumber++;
					
				if (_fileNumber >= MaxFileCount)
				{
					bool result = false;
					DeleteFileCallback(_fileNumber - MaxFileCount, result);
				}
				
				_state = StateCreateNewFile;
			}
			break;
		}
	}
};

#endif