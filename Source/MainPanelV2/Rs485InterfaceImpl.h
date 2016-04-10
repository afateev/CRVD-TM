#ifndef RS485IMPL_H
#define RS485IMPL_H

template
	<
		class Usart,
		class WriteEnablePort,
		int WriteEnablePin
	>
class Rs485InterfaceImpl
{
public:
	typedef Rblib::CallbackWrapper<unsigned char> OnReadCallbackType;
	typedef Rblib::CallbackWrapper<> OnWriteCompleteCallbackType;
public:
	static OnReadCallbackType OnReadCallback;
	static OnWriteCompleteCallbackType OnWriteCompleteCallback;
public:
	static void Init(unsigned int clockSourceFrequency, unsigned int boudrate, bool parityEnable = false, bool parityEven = false)
	{
		Usart::Disable();
		Usart::Init(clockSourceFrequency, boudrate, Usart::StopBits1, parityEnable ? (parityEven ? Usart::ParityEven : Usart::ParityOdd) : Usart::ParityNone);
		Usart::OnRead = OnUsartRead;
		Usart::OnWriteComplete = OnWriteComplete;
		Usart::Enable();
	}
	
	static void Write(void *data, unsigned int len)
	{
		Usart::Write((unsigned char *)data, len);
	}
	
	static void OnUsartRead(unsigned int data)
	{
		OnReadCallback(data);
	}
	
	static void OnWriteComplete()
	{
		OnWriteCompleteCallback();
	}
	
	static void WriteEnable(bool enable)
	{
		if(enable)
		{
			WriteEnablePort::SetBit(WriteEnablePin);
		}
		else
		{
			WriteEnablePort::ClearBit(WriteEnablePin);
		}
	}
};

template <class Usart, class WriteEnablePort, int WriteEnablePin>
Rs485InterfaceImpl<Usart, WriteEnablePort, WriteEnablePin>::OnReadCallbackType Rs485InterfaceImpl<Usart, WriteEnablePort, WriteEnablePin>::OnReadCallback;

template <class Usart, class WriteEnablePort, int WriteEnablePin>
Rs485InterfaceImpl<Usart, WriteEnablePort, WriteEnablePin>::OnWriteCompleteCallbackType Rs485InterfaceImpl<Usart, WriteEnablePort, WriteEnablePin>::OnWriteCompleteCallback;

#endif