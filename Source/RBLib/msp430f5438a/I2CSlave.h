#ifndef _I2C_SLAVE_MSP430F5438A_H_
#define _I2C_SLAVE_MSP430F5438A_H_

#include "../msp430/f5xxx/I2CSlave.h"

#define REGISTER_8(x,y) static volatile unsigned char* const (x) = reinterpret_cast<volatile unsigned char*>((y));
#define REGISTER_16(x,y) static volatile unsigned short* const (x) = reinterpret_cast<volatile unsigned short*>((y));

namespace Rblib
{
namespace Msp430f5438a
{	
namespace I2CSlaveLib
{
	//1-ый i2c
	REGISTER_8 (_UCB0CTL0,  0x05E1u);
	REGISTER_8 (_UCB0CTL1,  0x05E0u);
	REGISTER_8 (_UCB0STAT,	0x05EAu);
	REGISTER_8 (_UCB0RXBUF, 0x05ECu);
	REGISTER_8 (_UCB0TXBUF, 0x05EEu);
	REGISTER_16(_UCB0I2COA, 0x05F0u);
	REGISTER_8 (_UCB0IE,	0x05FCu);
	REGISTER_8 (_UCB0IFG,   0x05FDu);
	REGISTER_8 (_UCB0IV,	0x05FEu);
	
	//2-ой i2c
	REGISTER_8 (_UCB1CTL0,  0x0621u);
	REGISTER_8 (_UCB1CTL1,  0x0620u);
	REGISTER_8 (_UCB1STAT,	0x062Au);
	REGISTER_8 (_UCB1RXBUF, 0x062Cu);
	REGISTER_8 (_UCB1TXBUF, 0x062Eu);
	REGISTER_16(_UCB1I2COA, 0x0630u);
	REGISTER_8 (_UCB1IE,	0x063Cu);
	REGISTER_8 (_UCB1IFG,   0x063Du);
	REGISTER_8 (_UCB1IV,	0x063Eu);
	
	//3-ий i2c
	REGISTER_8 (_UCB2CTL0,  0x0661u);
	REGISTER_8 (_UCB2CTL1,  0x0660u);
	REGISTER_8 (_UCB2STAT,	0x066Au);
	REGISTER_8 (_UCB2RXBUF, 0x066Cu);
	REGISTER_8 (_UCB2TXBUF, 0x066Eu);
	REGISTER_16(_UCB2I2COA, 0x0670u);
	REGISTER_8 (_UCB2IE,	0x067Cu);
	REGISTER_8 (_UCB2IFG,   0x067Du);
	REGISTER_8 (_UCB2IV,	0x067Eu);
	
	//4-ый i2c
	REGISTER_8 (_UCB3CTL0,  0x06A1u);
	REGISTER_8 (_UCB3CTL1,  0x06A0u);
	REGISTER_8 (_UCB3STAT,	0x06AAu);
	REGISTER_8 (_UCB3RXBUF, 0x06ACu);
	REGISTER_8 (_UCB3TXBUF, 0x06AEu);
	REGISTER_16(_UCB3I2COA, 0x06B0u);
	REGISTER_8 (_UCB3IE,	0x06BCu);
	REGISTER_8 (_UCB3IFG,   0x06BDu);
	REGISTER_8 (_UCB3IV,	0x06BEu);
	
	//все порты ножки которых используются под i2c  
	REGISTER_8 (_P3SEL,		0x022Au);
	REGISTER_8 (_P5SEL,		0x024Au);
	REGISTER_8 (_P9SEL,		0x028Au);
	REGISTER_8 (_P10SEL,	0x028Bu);
	}

// Шаблон служебных регистров
template<unsigned int IdObj,
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
	class RegisterI2CSlave:
		public IdObjectBase<IdObj>,
		public Msp430f5xxx::I2CSlave<RxBufSize, TxBufSize, _UCBxCTL0, _UCBxCTL1, _UCBxSTAT, _UCBxRXBUF, _UCBxTXBUF, _UCBxI2COA, _UCBxIE, _UCBxIFG, _UCBxIV, _PxSEL_SDA, _PxSEL_SCL, _SDA_Pin, _SCL_Pin>
	{
	
	};


	class I2CSlave
	{
		typedef void (*HandlerInterrupt)();
	
	public:
		static void HandlerI2CSlave1(HandlerInterrupt h = 0)
		{
			static HandlerInterrupt handler = 0;		
			if(h == 0)
			{
				if(handler != 0)
					handler();
			}
			else
				handler = h;
		}
		static void HandlerI2CSlave2(HandlerInterrupt h = 0)
		{
			static HandlerInterrupt handler = 0;		
			if(h == 0)
			{
				if(handler != 0)
					handler();
			}
			else
				handler = h;
		}
		static void HandlerI2CSlave3(HandlerInterrupt h = 0)
		{
			static HandlerInterrupt handler = 0;		
			if(h == 0)
			{
				if(handler != 0)
					handler();
			}
			else
				handler = h;
		}
		static void HandlerI2CSlave4(HandlerInterrupt h = 0)
		{
			static HandlerInterrupt handler = 0;		
			if(h == 0)
			{
				if(handler != 0)
					handler();
			}
			else
				handler = h;
		}
		//шаблон I2CSlave 1
		template <unsigned char RxBufSize, unsigned char TxBufSize>
		class _A : public RegisterI2CSlave<1,
				RxBufSize,
				TxBufSize,
				I2CSlaveLib::_UCB0CTL0,
				I2CSlaveLib::_UCB0CTL1,
				I2CSlaveLib::_UCB0STAT,
				I2CSlaveLib::_UCB0RXBUF,
				I2CSlaveLib::_UCB0TXBUF,
				I2CSlaveLib::_UCB0I2COA,
				I2CSlaveLib::_UCB0IE,
				I2CSlaveLib::_UCB0IFG,
				I2CSlaveLib::_UCB0IV,
				I2CSlaveLib::_P3SEL,  //SDA Port
				I2CSlaveLib::_P3SEL,  //SCL Port
				0x02,     //SDA Pin
				0x04      //SCL Pin
				>
		{
		public:
			//аппаратная инициализация I2C Slave и подключение обработчика прерывания
			static void Initialization(unsigned char address)
			{
				_A::Init(address);
				HandlerI2CSlave1(_A::Handler);
			}
		};
	
		//шаблон I2CSlave 2
		template <unsigned char RxBufSize, unsigned char TxBufSize>
		class _B : public RegisterI2CSlave<2,
				RxBufSize,
				TxBufSize,
				I2CSlaveLib::_UCB1CTL0,
				I2CSlaveLib::_UCB1CTL1,
				I2CSlaveLib::_UCB1STAT,
				I2CSlaveLib::_UCB1RXBUF,
				I2CSlaveLib::_UCB1TXBUF,
				I2CSlaveLib::_UCB1I2COA,
				I2CSlaveLib::_UCB1IE,
				I2CSlaveLib::_UCB1IFG,
				I2CSlaveLib::_UCB1IV,
				I2CSlaveLib::_P3SEL,  //SDA Port
				I2CSlaveLib::_P5SEL,  //SCL Port
				0xF0,     //SDA Pin
				0x10      //SCL Pin
				>
		{
		public:
			//аппаратная инициализация I2C Slave и подключение обработчика прерывания
			static void Initialization(unsigned char address)
			{
				_B::Init(address);
				HandlerI2CSlave1(_B::Handler);
			}
		};
	
		//шаблон I2CSlave 3
		template <unsigned char RxBufSize, unsigned char TxBufSize>
		class _C : public RegisterI2CSlave<3,
				RxBufSize,
				TxBufSize,
				I2CSlaveLib::_UCB2CTL0,
				I2CSlaveLib::_UCB2CTL1,
				I2CSlaveLib::_UCB2STAT,
				I2CSlaveLib::_UCB2RXBUF,
				I2CSlaveLib::_UCB2TXBUF,
				I2CSlaveLib::_UCB2I2COA,
				I2CSlaveLib::_UCB2IE,
				I2CSlaveLib::_UCB2IFG,
				I2CSlaveLib::_UCB2IV,
				I2CSlaveLib::_P9SEL,  //SDA Port
				I2CSlaveLib::_P9SEL,  //SCL Port
				0x02,     //SDA Pin
				0x04      //SCL Pin
				>
		{
		public:
			//аппаратная инициализация I2C Slave и подключение обработчика прерывания
			static void Initialization(unsigned char address)
			{
				_C::Init(address);
				HandlerI2CSlave1(_C::Handler);
			}
		};
	
		//шаблон I2CSlave 4
		template <unsigned char RxBufSize, unsigned char TxBufSize>
		class _D : public RegisterI2CSlave<4,
				RxBufSize,
				TxBufSize,
				I2CSlaveLib::_UCB3CTL0,
				I2CSlaveLib::_UCB3CTL1,
				I2CSlaveLib::_UCB3STAT,
				I2CSlaveLib::_UCB3RXBUF,
				I2CSlaveLib::_UCB3TXBUF,
				I2CSlaveLib::_UCB3I2COA,
				I2CSlaveLib::_UCB3IE,
				I2CSlaveLib::_UCB3IFG,
				I2CSlaveLib::_UCB3IV,
				I2CSlaveLib::_P10SEL,  //SDA Port
				I2CSlaveLib::_P10SEL,  //SCL Port
				0x02,     //SDA Pin
				0x04      //SCL Pin
				>
		{
		public:
			//аппаратная инициализация I2C Slave и подключение обработчика прерывания
			static void Initialization(unsigned char address)
			{
				_D::Init(address);
				HandlerI2CSlave1(_D::Handler);
			}
		};
	};

}
}
/*---------описание программного интерфейса i2C (MASTER)------------------

//первым делом нужно проинициализироваться
static void Initialization(ClockSource source, unsigned int prescaler);

//не забываем выставить адрес внешнего SLAVE-устройства
static void SetupAddressSlaveDevices(unsigned char address);

//отправить данные во внешнее устройство
//если успешно, то вернётся true
static bool Send(unsigned char* data, int length);

//принять данные из внешнего устройства
//если успешно, то вернётся true
static bool Recv(unsigned char* data, int length);


//маленький пример использования:

typedef I2C::_A i2c;

main()
{
	i2c::Initialization(i2c::SMCLK, 0x0047);
	i2c::SetupAddressSlaveDevices(0x50);//адрес внешнего устройства

	char buf[7];

	bool successful = i2c::Recv(buf, 7);//принять 7 байтов из внешнего устройства
	successful = i2c::Send(buf, 7);//отправить 7 байтов во внешнее устройство

}
----------конец описания---------------------------------*/

#endif //_I2C_SLAVE_MSP430F5438A_H_