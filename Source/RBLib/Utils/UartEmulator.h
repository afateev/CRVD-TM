namespace Rblib
{
namespace Utils
{
	//������
	template <int NumberUart, unsigned char RxBufSize>
	class UARTEmulator
	{
		// ��� ������� ��������� ������
		typedef void (*Handler)(const unsigned char *data, int length);
		
		static const int _numberUart = NumberUart;
		static volatile unsigned char _RxHead; 
		static unsigned char _RxTail; 
		static unsigned char _RxBuf[RxBufSize];
		static Handler _callback;
 		
	public:	
		// ������� �������� 
		static void Initialization(Handler callback)
		{
			_callback = callback;
		}
		
		// ������� �������� ����� ������ � ���������� �����
		static void RxHandler(const unsigned char* data, int length)
		{
			for(int i = 0; i < length; i++)
			{
				_RxHead++;
				if(_RxHead == RxBufSize)
					_RxHead = 0;

	   			if (_RxHead == _RxTail) //���� ������������ �������
     			{
					//���� ����� ����������� ������ ������������ � �����
					_RxHead = _RxTail + 1;
					if(_RxHead == RxBufSize)
						_RxHead = 0;			
    			}
				_RxBuf[_RxHead] = data[i]; //�������� ����� ����
			}

		}
		
		static void Write(const unsigned char* data, int length)
		{
			if(_callback != 0)
				_callback(data, length);
		}		
	
		static int Read(unsigned char* data, int length)
		{			
			for(int i = 0; i < length; i++)
			{
	  		  	if(_RxHead == _RxTail)
					return i;//������ ������ ���
			
				_RxTail++;
				if(_RxTail == RxBufSize)
					_RxTail = 0;			
		
				*(data++) = _RxBuf[_RxTail];
			}
			return length;	
		}
	};
	
	
	template <int NumberUart, unsigned char RxBufSize>
	volatile unsigned char UARTEmulator<NumberUart, RxBufSize>
	::_RxHead = 0;
	
	template <int NumberUart, unsigned char RxBufSize>
	unsigned char UARTEmulator<NumberUart, RxBufSize>
	::_RxTail = 0;
	
	template <int NumberUart, unsigned char RxBufSize>
	unsigned char UARTEmulator<NumberUart, RxBufSize>
	::_RxBuf[RxBufSize];
	
	template <int NumberUart, unsigned char RxBufSize>
	typename UARTEmulator<NumberUart, RxBufSize>::Handler UARTEmulator<NumberUart, RxBufSize>
	::_callback = 0;
	
	
	
	class UartEmulator
	{
	public:
		//������ UART 1
		template <unsigned char RxBufSize, unsigned char TxBufSize>
		class _A : public UARTEmulator<1, RxBufSize>						
		{
		};
		//������ UART 2
		template <unsigned char RxBufSize, unsigned char TxBufSize>
		class _B : public UARTEmulator<2, RxBufSize>						
		{
		};
		//������ UART 3
		template <unsigned char RxBufSize, unsigned char TxBufSize>
		class _C : public UARTEmulator<3, RxBufSize>						
		{
		};
		//������ UART 4
		template <unsigned char RxBufSize, unsigned char TxBufSize>
		class _D : public UARTEmulator<4, RxBufSize>						
		{
		};
	};
	
}	
}