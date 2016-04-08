///////////////////////////////////////////////////////////////////////////////
//	����� i2c ����������
//	
///////////////////////////////////////////////////////////////////////////////

/*---------�������� ������������ ���������� i2C (MASTER)------------------

��� ������ �����������, �������� � ��������� ��������� � ��� ���������:
I2C::_A, I2C::_B, I2C::_C, I2C4::_D � �.�.
���������� ������� ������� �� ��������� ���������.

����� ������ ��������� � ���������� ������� ����������� i2c ����������.

��������� �������:

//���������� ������������� i2c
//������������������ �������, �������� � ��������� � ������������ ����������
static void Initialization(...��������� ������� �� ���������...);

//��������� ������ �������� SLAVE-����������
static void SetupAddressSlaveDevices(unsigned char address);

//�������� ������ �� ������� ����������
//���� �������, �� ������� true
static bool Write(unsigned char* data, int length);

//����� ������ �� �������� ����������
//���� �������, �� ������� true
static bool Read(unsigned char* data, int length);


//��������� ������ ������������� ��� msp430f5438a:

typedef I2C::_A i2c;

main()
{
	i2c::Initialization(i2c::SMCLK, 0x0047);
	i2c::SetupAddressSlaveDevices(0x50);//����� �������� ����������

	char buf[7];

	bool successful = i2c::Read(buf, 7);//������� 7 ������ �� �������� ����������
	successful = i2c::Write(buf, 7);//��������� 7 ������ �� ������� ����������

}

----------����� ��������---------------------------------*/

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
	// �������� ��������� / ������������ ���
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
	
	// �������� ������ ���������� i2c �����������
	template <Platform p>
	class I2CSelector: public PlatformSelector<I2CTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		I2CSelector()
		{
		
		}
	};

	// ����� ���������� i2c �����������
	typedef I2CSelector<RBLIB_PALTFORM_AUTODETECT> I2C;
}

#endif //I2C_H
