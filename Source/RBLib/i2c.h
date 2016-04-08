///////////////////////////////////////////////////////////////////////////////
//	Класс i2c интерфейса
//	
///////////////////////////////////////////////////////////////////////////////

/*---------описание программного интерфейса i2C (MASTER)------------------

Все классы статические, названия и буквенная нумерация у них попорядку:
I2C::_A, I2C::_B, I2C::_C, I2C4::_D и т.д.
Количество классов зависит от выбранной платформы.

Номер класса совпадает с порядковым номером аппаратного i2c интерфейса.

Интерфейс классов:

//аппаратная инициализация i2c
//платформозависимая функция, смотрите её интерфейс в подключаемой библиотеке
static void Initialization(...аргументы зависят от платформы...);

//установка адреса внешнего SLAVE-устройства
static void SetupAddressSlaveDevices(unsigned char address);

//отправка данных во внешнее устройство
//если успешно, то вернётся true
static bool Write(unsigned char* data, int length);

//принём данных из внешнего устройства
//если успешно, то вернётся true
static bool Read(unsigned char* data, int length);


//маленький пример использования для msp430f5438a:

typedef I2C::_A i2c;

main()
{
	i2c::Initialization(i2c::SMCLK, 0x0047);
	i2c::SetupAddressSlaveDevices(0x50);//адрес внешнего устройства

	char buf[7];

	bool successful = i2c::Read(buf, 7);//принять 7 байтов из внешнего устройства
	successful = i2c::Write(buf, 7);//отправить 7 байтов во внешнее устройство

}

----------конец описания---------------------------------*/

#ifndef I2C_H
#define I2C_H

#include "Platform.h"

#ifdef __ICC430__
#include "msp430f1611\i2c.h"
#include "msp430f2618\i2c.h"
#include "msp430f5340\i2c.h"
//#include "msp430f5342\i2c.h"
#include "msp430f5435a\i2c.h"
#include "msp430f5438a\i2c.h"
#include "msp430f5659\i2c.h"
#endif

#ifdef __ICCARM__
#include "efm32gg990f1024\i2c.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef __ICC430__
			PlatformMsp430f1611, Msp430f1611::I2C,
			PlatformMsp430f2618, Msp430f2618::I2C,
			PlatformMsp430f5340, Msp430f5340::I2C,
			//PlatformMsp430f5342, Msp430f5342::I2C,
			PlatformMsp430f5435a, Msp430f5435a::I2C,
			PlatformMsp430f5438a, Msp430f5438a::I2C,
			PlatformMsp430f5659, Msp430f5659::I2C
#endif
#ifdef __ICCARM__
			PlatformEfm32gg990f1024, Efm32gg990f1024::I2C
#endif
		>::Result I2CTypes;
	
	// Селектор класса управления i2c интерфейсом
	template <Platform p>
	class I2CSelector: public PlatformSelector<I2CTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		I2CSelector()
		{
		
		}
	};

	// Класс управления i2c интерфейсом
	typedef I2CSelector<RBLIB_PALTFORM_AUTODETECT> I2C;
}

#endif //I2C_H
