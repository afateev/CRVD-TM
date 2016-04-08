#ifndef I2CSLAVE_MSP430X5XX_H
#define I2CSLAVE_MSP430X5XX_H

//#include <intrinsics.h>
//#include <msp430f5438a.h>

namespace Rblib
{
namespace Msp430f5xxx
{
	typedef void (*HandlerReceiveFunc)(unsigned char* data, unsigned char length);

	template <
			unsigned char RxBufSize,
			unsigned char TxBufSize,
			volatile unsigned char* const _UCBxCTL0,  //Control Register 0
			volatile unsigned char*  const _UCBxCTL1, //Control Register 1
			volatile unsigned char* const _UCBxSTAT, //Status Register
			volatile unsigned char* const _UCBxRXBUF, //Receive Buffer Register
			volatile unsigned char* const _UCBxTXBUF, //Transmit Buffer Register
			volatile unsigned short* const _UCBxI2COA,//Own Address Register
			volatile unsigned char* const _UCBxIE, //Interrupt Enable Register
			volatile unsigned char* const _UCBxIFG, //Interrupt Flag Register
			volatile unsigned char* const _UCBxIV, //Interrupt Vector Register
			volatile unsigned char* const _PxSEL_SDA, //Function Select Registers SDA
			volatile unsigned char* const _PxSEL_SCL, //Function Select Registers SCL
			int _SDA_Pin,                          //SDA Pin
			int _SCL_Pin                          //SCL Pin
			>
	class I2CSlave
	{	
		//Control Register 0
		static const int _UCSYNC    = 0x01;
		static const int _UCMODE_3  = 0x06;
		static const int _UCMST     = 0x08;
	
		//Control Register 1
		static const int _UCSWRST   = 0x01;
		static const int _UCTXSTT   = 0x02;
		static const int _UCTXSTP   = 0x04;
		static const int _UCTR      = 0x10;
		static const int _UCSSEL_2  = 0x80;
		
		//Interrupt Enable Register
		static const int _UCRXIE	= 0x01;
		static const int _UCTXIE	= 0x02;
		static const int _UCSTTIE	= 0x04;
		static const int _UCSTPIE	= 0x08;
			
		//I2C Interrupt Flag Register
		static const int _UCRXIFG   = 0x01;
		static const int _UCTXIFG   = 0x02;
		static const int _UCSTTIFG	= 0x04;
		static const int _UCSTPIFG	= 0x08;
		static const int _UCNACKIFG = 0x20;		
		
		static unsigned char txBuf[TxBufSize];
		static unsigned char indexTxBuf;	
	public:
		static HandlerReceiveFunc HandlerReceive;
		
		static void Init(unsigned char address)
		{
			*_PxSEL_SDA |= _SDA_Pin;                            // Установка ножек на работу с переферией
			*_PxSEL_SCL |= _SCL_Pin;                            // Установка ножек на работу с переферией
	
			*_UCBxCTL1 |= _UCSWRST;                      // Enable SW reset
					
			*_UCBxCTL0 = _UCMODE_3 + _UCSYNC;     // I2C Slave, synchronous mode
			*_UCBxI2COA = address;					// Own Address
			
			*_UCBxCTL1 &= ~_UCSWRST;                     // Clear SW reset, resume operation
			*_UCBxIE |= _UCRXIE + _UCSTPIE + _UCSTTIE;     // Enable STT, STP & RX interrupt
		}
		
		static void Send(unsigned char* data, unsigned char length)
		{
			if(length > TxBufSize)
				return;
			memcpy(txBuf + TxBufSize - length, data, length);
			indexTxBuf = TxBufSize - length;
		}
	
		static void Handler()
		{
			static unsigned char rxBuf[RxBufSize];
			static unsigned char indexRxBuf = 0;
			static bool flagRx;
					
			switch(*_UCBxIV)
			{
			case  6:                                  // Vector  6: STTIFG
				if(*_UCBxCTL1&0x10)
				{
					flagRx = 0; // будет передавать байты
					*_UCBxIE |= _UCTXIE;
				}
				else
				{
					flagRx = 1; // будет принимать байты
					indexRxBuf = 0;
				}
				*_UCBxIFG &= ~_UCSTTIFG;
				break;
			case  8:                                  // Vector  8: STPIFG
				if(flagRx)
				{
					if(HandlerReceive)
						HandlerReceive(rxBuf, indexRxBuf);
				}		
				*_UCBxIE &= ~_UCTXIE;		
				*_UCBxIFG &= ~_UCSTPIFG;
				break;
			case 10: 
					if(indexRxBuf < RxBufSize)
						rxBuf[indexRxBuf++] = *_UCBxRXBUF;
					else
						int a = *_UCBxRXBUF;
				break;
			case 12:                           // Vector 12: TXIFG  
				{
					if(indexTxBuf < TxBufSize)
						*_UCBxTXBUF = txBuf[indexTxBuf++];
					else
						*_UCBxTXBUF = 0;
				}
				break;
			default: break;
			}  
		}
		
		static void drug()
		{
			static int count = 0;
			if((*_UCBxSTAT & 0x10) && (*_UCBxIFG == 0))
			{
				if(count == 10000)
				{
					*_UCBxCTL1 |= _UCSWRST;                      // Enable SW reset				
					*_UCBxCTL0 = _UCMODE_3 + _UCSYNC;     // I2C Slave, synchronous mode
					*_UCBxCTL1 &= ~_UCSWRST;                     // Clear SW reset, resume operation
					*_UCBxIE |= _UCRXIE + _UCSTPIE + _UCSTTIE;     // Enable STT, STP & RX interrupt
					
					count = 0;
				}
				count++;
			}
			else
				count = 0;
		}
		
	};
	
	template<unsigned char RxBufSize, unsigned char TxBufSize,	volatile unsigned char* const _UCBxCTL0, volatile unsigned char*  const _UCBxCTL1, volatile unsigned char* const _UCBxSTAT,
			volatile unsigned char* const _UCBxRXBUF, volatile unsigned char* const _UCBxTXBUF, volatile unsigned short* const _UCBxI2COA, volatile unsigned char* const _UCBxIE,
			volatile unsigned char* const _UCBxIFG,	volatile unsigned char* const _UCBxIV, volatile unsigned char* const _PxSEL_SDA, volatile unsigned char* const _PxSEL_SCL, int _SDA_Pin, int _SCL_Pin>
	unsigned char I2CSlave<RxBufSize, TxBufSize, _UCBxCTL0, _UCBxCTL1, _UCBxSTAT, _UCBxRXBUF, _UCBxTXBUF, _UCBxI2COA, _UCBxIE, _UCBxIFG, _UCBxIV, _PxSEL_SDA, _PxSEL_SCL, _SDA_Pin, _SCL_Pin>
	::txBuf[TxBufSize];
	
	template<unsigned char RxBufSize, unsigned char TxBufSize,	volatile unsigned char* const _UCBxCTL0, volatile unsigned char*  const _UCBxCTL1, volatile unsigned char* const _UCBxSTAT,
			volatile unsigned char* const _UCBxRXBUF, volatile unsigned char* const _UCBxTXBUF, volatile unsigned short* const _UCBxI2COA, volatile unsigned char* const _UCBxIE,
			volatile unsigned char* const _UCBxIFG,	volatile unsigned char* const _UCBxIV, volatile unsigned char* const _PxSEL_SDA, volatile unsigned char* const _PxSEL_SCL, int _SDA_Pin, int _SCL_Pin>
	unsigned char I2CSlave<RxBufSize, TxBufSize, _UCBxCTL0, _UCBxCTL1, _UCBxSTAT, _UCBxRXBUF, _UCBxTXBUF, _UCBxI2COA, _UCBxIE, _UCBxIFG, _UCBxIV, _PxSEL_SDA, _PxSEL_SCL, _SDA_Pin, _SCL_Pin>
	::indexTxBuf = 0;
	
	template<unsigned char RxBufSize, unsigned char TxBufSize,	volatile unsigned char* const _UCBxCTL0, volatile unsigned char*  const _UCBxCTL1, volatile unsigned char* const _UCBxSTAT,
			volatile unsigned char* const _UCBxRXBUF, volatile unsigned char* const _UCBxTXBUF, volatile unsigned short* const _UCBxI2COA, volatile unsigned char* const _UCBxIE,
			volatile unsigned char* const _UCBxIFG,	volatile unsigned char* const _UCBxIV, volatile unsigned char* const _PxSEL_SDA, volatile unsigned char* const _PxSEL_SCL, int _SDA_Pin, int _SCL_Pin>
	HandlerReceiveFunc I2CSlave<RxBufSize, TxBufSize, _UCBxCTL0, _UCBxCTL1, _UCBxSTAT, _UCBxRXBUF, _UCBxTXBUF, _UCBxI2COA, _UCBxIE, _UCBxIFG, _UCBxIV, _PxSEL_SDA, _PxSEL_SCL, _SDA_Pin, _SCL_Pin>
	::HandlerReceive;
}
}
#endif //I2CSLAVE_MSP430X5XX_H
