#ifndef _24LC128_H
#define _24LC128_H

#include <intrinsics.h>

template<class I2C>
class _24LC128
{
public:
    static bool Write(unsigned int address, unsigned char* data, unsigned int length)
	{		
    	if(length == 0)
			return true;
		

	    unsigned int i = 0;
		 
		while(1)
		{
			unsigned char buf[66];
	
			buf[0] = (unsigned char)(address >> 8);//�������� 1-�� �������� ������
			buf[1] = (unsigned char)address;//�������� 2-�� �������� ������ 
			
			buf[2] = data[i++];
			address++;
			
			unsigned char j = 3;
			
            while( ((address&0x003f)!=0x0000) & (i<length) )
            {
				buf[j++] = data[i++];
                address++;
            }
			
			if(!I2C::Write(buf, j))
				return false;

			__delay_cycles(73746); // �� ����� 10�� (73746 ��� XTAL=7,3728���)���� ������ EEPROM  
			__delay_cycles(73746); // �� ����� 10�� (73746 ��� XTAL=7,3728���)���� ������ EEPROM  
			
			if(i >= length)
				break;
		}
		
		return true;
	}
	
    static bool Read(unsigned int address, unsigned char* data, int length)
	{
    	if(length == 0)
			return true;
 
		
		unsigned char buf[2];
		
		buf[0] = (unsigned char)(address>>8);
		buf[1] = (unsigned char)address; 
		
		if(!I2C::Write(buf, 2))
			return false;
		
		if(!I2C::Read(data, length))
			return false;
		
		return true;
	}
};


#endif //24LC128_H

