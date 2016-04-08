#ifndef MODEM_SIGNALTIMEOUT_H
#define MODEM_SIGNALTIMEOUT_H

namespace Rblib
{
namespace Modem
{
	template
		<
			class DebugSignal,
			int signalId
		>
	class SignalTimeout
	{
	protected:
		int _countDown;
	public:
		SignalTimeout()
		{
			_countDown = 0;
		}
		
		void Set(int timeout = 5)
		{
			_countDown = timeout;
			DebugSignal::Set(signalId);
		}
		
		void Clear()
		{
			_countDown = 0;
			DebugSignal::Clear(signalId);
		}
		
		void OnTimer()
		{
			if (_countDown > 0)
			{
				_countDown--;
				
				if (_countDown < 1)
				{
					Clear();
				}
			}
		}
	};
}
}

#endif
