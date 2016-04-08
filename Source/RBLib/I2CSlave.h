///////////////////////////////////////////////////////////////////////////////
//	����� I2C Slave ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include "Platform.h"

#ifdef __ICC430__
//#include "msp430f1611\uart.h"
//#include "msp430f2618\uart.h"
//#include "msp430f5435a\uart.h"
#include "msp430f5438a\I2CSlave.h"
#endif
 
namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef __ICC430__
//			PlatformMsp430f1611, Msp430f1611::Uart,
//			PlatformMsp430f2618, Msp430f2618::Uart,
//			PlatformMsp430f5435a, Msp430f5435a::Uart,
			PlatformMsp430f5438a, Msp430f5438a::I2CSlave
#endif
		>::Result I2CSlaveTypes;
	
	// �������� ������ ���������� UART �����������
	template <Platform p>
	class I2CSlaveSelector: public PlatformSelector<I2CSlaveTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		I2CSlaveSelector()
		{
		
		}
	};

	// ����� ���������� UART �����������
	typedef I2CSlaveSelector<RBLIB_PALTFORM_AUTODETECT> I2CSlave;
}

#endif //I2C_SLAVE_H
