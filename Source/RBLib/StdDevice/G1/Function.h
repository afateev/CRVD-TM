///////////////////////////////////////////////////////////////////////////////
//	 ласс реализации работы с функци€ми устройтсва по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_FUNCTION_H
#define G1_FUNCTION_H

#include "../../Utils/CallbackWrapper.h"
#include "EventCode.h"
#include "../../Protocol/G1/Device/FunctionCode.h"

namespace Rblib
{
namespace StdDevice
{
namespace G1
{
	template
		<
			int eventCode,
			int maxDataSize
		>
	class Function
	{
	public:
		static const int EventCode = eventCode;
		static const unsigned int MaxDataSize = maxDataSize;
		static const int DefaultTimeout = 10;
		
		typedef CallbackWrapper<unsigned char *, unsigned int, int, int &> SendCommandToFunctionCallbackType;
	protected:
		int _deviceId;
		bool _enabled;
		int _waitPacketId;
		int _waitTimeout;
		// один на всех экземпл€р буфера дл€ отправки данных
		static unsigned char _functionData[MaxDataSize];
	public:
		SendCommandToFunctionCallbackType SendCommandToFunctionCallback;
	public:
		Function()
		{
			_deviceId = -1;
			_waitPacketId = -1;
			_enabled = false;
			Reset();
		}
		
		virtual Protocol::G1::Device::FunctionCode FunctionCode()
		{
			return Protocol::G1::Device::FunctionCodeContainer;
		}
		
		void SetDeviceId(int id)
		{
			_deviceId = id;
		}
		
		virtual void Reset()
		{
			bool enabledChanged = _enabled;
			_enabled = false;
			
			if (enabledChanged)
			{
				Event e(EventCode, EventCodeFunctionDisabled, _deviceId, FunctionCode());
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
		}
		
		virtual void SetEnabled()
		{
			bool enabledChanged = !_enabled;
			_enabled = true;
			
			if (enabledChanged)
			{
				Event e(EventCode, EventCodeFunctionEnabled, _deviceId, FunctionCode());
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
		}
		
		virtual bool Enabled()
		{
			return _enabled;
		}
		
		virtual void OnTick()
		{
		}
		
		virtual void OnTxComplete(int packetId, bool delivered)
		{
		}
		
		virtual void OnCommandFromFunction(unsigned char *data, int dataLen)
		{
		}
		
		virtual bool WaitResponseFlag()
		{
			return false;
		}
	};
	
	template <int eventCode, int maxDataSize>
		unsigned char Function<eventCode, maxDataSize>::_functionData[Function<eventCode, maxDataSize>::MaxDataSize];
}
}
}

#endif