///////////////////////////////////////////////////////////////////////////////
//	Класс реализации работы с функцие устройтсва "камера" по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_FUNCTIONS_CAMERA_H
#define G1_FUNCTIONS_CAMERA_H

#include "../Function.h"
#include "../../../Utils/vector_static.h"

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
			class Base,
			int timerTickCountPerSecond
		>
	class Camera : public Base
	{
	public:
		static const int TimerTickCountPerSecond = timerTickCountPerSecond;
		
		enum State
		{
			StateDisabled = 0,
			StateEnabled,
			StateTxStartCapture,
			StateTxStartCaptureWaitDelivery,
			StateRxStartCaptureResultWait,
			StateIdle
		};
		
		struct CapureResultListItem
		{
			unsigned int FileId;
			unsigned int FileSize;
		public:
			CapureResultListItem()
			{
				FileId = -1;
				FileSize = 0;
			}
			
			CapureResultListItem(unsigned int fileId, unsigned int fileSize)
			{
				FileId = fileId;
				FileSize = fileSize;
			}
			
			bool operator == (CapureResultListItem &other)
			{
				// сравниваем только идентификатор файла
				return FileId == other.FileId;
			}
		};
		
		static const int CapureResultListMaxCount = 5;
		typedef vector_static<CapureResultListItem, CapureResultListMaxCount> CapureResultListType;
		
	protected:
		State _state;
		bool _startCapture;
		Protocol::G1::Function::Camera::CaptureStartResultCode _startCaptureResultCode;
		CapureResultListType _captureResultList;
		unsigned int _lastDownloadedFileId;
		unsigned int _lastDownloadedFileSize;
		bool _lastDownloadedFilePresent;
		bool _freshFoto;
	public:
		Camera()
		{
			_startCapture = false;
			_startCaptureResultCode = Protocol::G1::Function::Camera::CaptureStartResultUnknown;
			_freshFoto = false;
			_lastDownloadedFileId = 0xFFFFFFFF;
			_lastDownloadedFileSize = 0;
			_lastDownloadedFilePresent = false;
			
		}
		
		virtual Protocol::G1::Device::FunctionCode FunctionCode()
		{
			return Protocol::G1::Device::FunctionCodeCamera;
		}
		
		virtual void Reset()
		{
			Base::Reset();
			
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
					if (_startCapture)
					{
						_state = StateTxStartCapture;
						break;
					}
				}
				break;
			case StateTxStartCapture:
				{
					Base::_waitTimeout = 60 * TimerTickCountPerSecond;
					int deliveryTimeout = Base::DefaultTimeout;
					
					unsigned char paramsCount = 0;
					
					_startCaptureResultCode = Protocol::G1::Function::Camera::CaptureStartResultUnknown;
					
					Base::_waitPacketId = SendCommandToFunction(Protocol::G1::Function::Camera::MessageCodeStartCapture, &paramsCount, sizeof(paramsCount), deliveryTimeout);
					_state = StateTxStartCaptureWaitDelivery;
				}
				break;
			case StateTxStartCaptureWaitDelivery:
			case StateRxStartCaptureResultWait:
				{
					Base::_waitTimeout--;
					if (Base::_waitTimeout < 1)
					{
						_state = StateTxStartCapture;
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
		
		int SendCommandToFunction(Protocol::G1::Function::Camera::MessageCode code, void *data, int dataCount, int timeout)
		{
			Protocol::G1::Function::Camera::OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(Base::_functionData, sizeof(Base::_functionData));
			
			int packetId = 0;
			Base::SendCommandToFunctionCallback.Call(Base::_functionData, functionDataLen, timeout, packetId);
			return packetId;
		}
		
		virtual void OnTxComplete(int packetId, bool delivered)
		{
			Base::OnTxComplete(packetId, delivered);
			
			bool waitComplete = packetId == Base::_waitPacketId;
			// дождались завершения передачи
			if (waitComplete)
			{
				switch(_state)
				{
				case StateTxStartCaptureWaitDelivery:
					{
						if (delivered)
						{
							Base::_waitTimeout = 5 * TimerTickCountPerSecond;
							_state = StateRxStartCaptureResultWait;
						}
						else
						{
							_state = StateTxStartCapture;
						}
					}
					break;
				}
			}
		}
		
		virtual void OnCommandFromFunction(unsigned char *data, int dataCount)
		{
			Base::OnCommandFromFunction(data, dataCount);
			
			Protocol::G1::Function::Camera::InputMessageFormat command(data, dataCount);
			if (command.Valid())
			{
				if (command.Data.Valid())
				{
					switch(command.Data.Code)
					{
					case Protocol::G1::Function::Camera::MessageCodeStartCaptureResult:
						{
							if (command.Data.DataLen >= 1)
							{
								unsigned char *data = (unsigned char *)command.Data.Data;
								if (data)
								{
									_startCaptureResultCode = (Protocol::G1::Function::Camera::CaptureStartResultCode) data[0];
								}
							}
							
							switch(_state)
							{
							case StateRxStartCaptureResultWait:
								{
									_startCapture = false;
									_state = StateIdle;
								}
								break;
							}
							
							// уведомление о результате старта съемки
							bool started = _startCaptureResultCode == Protocol::G1::Function::Camera::CaptureStartResultOk;
							Event e(Base::EventCode, EventCodeCameraStartCaptureResultReceived, Base::_deviceId, FunctionCode(), started);
							Singletone<EventDispetcher>::Instance().PushEvent(e);
						}
						break;
					case Protocol::G1::Function::Camera::MessageCodeCaptureFinished:
						{
							if (command.Data.DataLen >= 8)
							{
								unsigned char *data = (unsigned char *)command.Data.Data;
								if (data)
								{
									int i = 0;
									
									unsigned int id = 0;
									for (int j = 0; j < sizeof(id); j++)
									{
										id |= (unsigned int)data[i] << (j * 8);
										i++;
									}
									
									unsigned int size = 0;
									for (int j = 0; j < sizeof(size); j++)
									{
										size |= (unsigned int)data[i] << (j * 8);
										i++;
									}
									
									CapureResultListAdd(id, size);
								}
							}
							
							// уведомление об окончании съемки
							Event e(Base::EventCode, EventCodeCameraCaptureFinished, Base::_deviceId, FunctionCode());
							Singletone<EventDispetcher>::Instance().PushEvent(e);
						}
						break;
					}
				}
			}
		}
		
		void StartCapture()
		{
			_startCapture = true;
		}
		
		bool GetCaptureResultInfo(unsigned int &fileId, unsigned int &fileSize)
		{
			if (_captureResultList.empty())
			{
				return false;
			}
			
			fileId = _captureResultList.front().FileId;
			fileSize = _captureResultList.front().FileSize;
			
			return true;
		}
		
		bool GetLastCaptureResultInfo(unsigned int &fileId, unsigned int &fileSize)
		{
			if (_captureResultList.empty())
			{
				return false;
			}
			
			fileId = _captureResultList.back().FileId;
			fileSize = _captureResultList.back().FileSize;
			
			return true;
		}
		
		
		bool GetLastDownloadedFileInfo(unsigned int &fileId, unsigned int &fileSize)
		{
			fileId = _lastDownloadedFileId;
			fileSize = _lastDownloadedFileSize;
			return _lastDownloadedFilePresent;
		}
		
		bool Alarm()
		{
			return _captureResultList.size() > 0;
		}
		
		virtual bool WaitResponseFlag()
		{
			bool res = Base::WaitResponseFlag();
			
			res |= _state > StateEnabled && _state < StateIdle;
			
			return res;
		}
		
		bool HaveFreshPhoto()
		{
			return _freshFoto;
		}
		
		void OnDownloadComplete(unsigned int fileId, unsigned int fileSize, bool downloaded)
		{
			// если успешно загружена, то запоминаем параметры того, что загрузили
			if (downloaded)
			{
				_lastDownloadedFileId = fileId;
				_lastDownloadedFileSize = fileSize;
				_lastDownloadedFilePresent = true;
				
				// помечаем наличие свежей фотографии
				_freshFoto = true;
				
				// уведомление об окончании загрузки
				Event e(Base::EventCode, EventCodeCameraDownloadComplete, Base::_deviceId, FunctionCode());
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
			
			// удаляем из списка даже если не была загружена
			CapureResultListRemove(fileId, fileSize);
		}
		
		void ResetFreshPhotoFlag()
		{
			_freshFoto = false;
		}
	protected:
		int SecondsToTimerTicks(int seconds)
		{
			return TimerTickCountPerSecond * seconds;
		}
		
		// пополнить в список ожидающих закачки
		void CapureResultListAdd(unsigned int fileId, unsigned int fileSize)
		{
			// если такой файл уже есть, то удалить
			// на случай если идентификатор тот же, а размер другой
			CapureResultListRemove(fileId, fileSize);
			
			// и добавить с актуальными параметрами
			CapureResultListItem captureResult(fileId, fileSize);
			
			// если уже нет места, то удаляем самый первый
			if (_captureResultList.size() >= CapureResultListMaxCount)
			{
				_captureResultList.erase(&_captureResultList[0]);
			}
			
			_captureResultList.push_back(captureResult);
		}
		
		// сократить список ожидающих закачки
		void CapureResultListRemove(unsigned int fileId, unsigned int fileSize)
		{
			CapureResultListItem captureResult(fileId, fileSize);
			
			for (int i = 0; i < _captureResultList.size(); i++)
			{
				// найден
				if (_captureResultList[i] == captureResult)
				{
					// удаляем
					_captureResultList.erase(&_captureResultList[i]);
					return;
				}
			}
		}
	};
}
}
}
}

#endif