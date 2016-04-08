///////////////////////////////////////////////////////////////////////////////
//	���������� ����� ���������� � ��������� �� ������������ �����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMON_H
#define COMMON_H

#include "Platform.h"

// ��������� ���������� ������������� �������� ������� �� ����� ������
#define RBLIB_STR(x)        #x
#define RBLIB_XSTR(x)       RBLIB_STR(x)

// ��������� �� ������������ �����������

// IAR
#ifdef __IAR_SYSTEMS_ICC__

#ifdef __ICC430__
    #include <intrinsics.h>

	// ������ ���������� ��� ���������� ���������
	#define RBLIB_ISR_RAW(vec) _Pragma (RBLIB_STR(vector = (vec))) __raw __interrupt
	// ������ ����������
	#define RBLIB_ISR(vec) _Pragma (RBLIB_STR(vector = (vec))) __interrupt
	// ������������ �������
	#define RBLIB_ASM(code) asm(code)
	// ������ ������������� ����������
	#define RBLIB_NO_INIT __no_init
    #if (RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F1611)
        // ������ ����� �� "��������" ���������
    	#define RBLIB_WRITE_FULL_ADDR_CHAR(addr, value) (*((unsigned char*)addr) = value)
        #define RBLIB_READ_FULL_ADDR_CHAR(addr) (*((unsigned char*)addr))
    #else
        // ������ ����� �� "��������" ���������
        #define RBLIB_WRITE_FULL_ADDR_CHAR(addr, value) __data20_write_char(addr, value)
        #define RBLIB_READ_FULL_ADDR_CHAR(addr) __data20_read_char(addr)
    #endif
	// ������ ����� �� "��������" ���������
	#define RBLIB_WRITE_FULL_ADDR_SHORT(addr, value) __data20_write_short(addr, value)
	// ������ ����� �� "��������" ���������
	#define RBLIB_READ_FULL_ADDR_SHORT(addr) __data20_read_short(addr)
  // �������
  #define RBLIB_DELAY_CYCLES(cycles) __delay_cycles(cycles)

    // ��������� watch dog ��� ������ ��������� (�� �������������)
	#define WTD_STOP_ON_START() \
	int __low_level_init(void) \
	{ \
		Wdt::Stop(); \
		return 1; \
	}

#endif // __ICC430__

#ifdef __ICCARM__
	// ������ ������������� ����������
	#define RBLIB_NO_INIT __no_init

	#define RBLIB_GLOBAL_INTERRUPT_ENABLE __enable_interrupt()
	#define RBLIB_GLOBAL_INTERRUPT_DISABLE __disable_interrupt()
	#define RBLIB_WFI __WFI()
#endif // _ICCARM__

	// �������������� ����� �����������
	#define RBLIB_COMPILE_TIME_WARNING(str) _Pragma(RBLIB_STR(message(str)))

// ������
#else
	// ������ ���������� ��� ���������� ���������
	#define RBLIB_ISR_RAW(vec)
	// ������ ����������
	#define RBLIB_ISR(vec)
	// ������������ �������
	#define RBLIB_ASM(code)
	// ������ ������������� ����������
	#define RBLIB_NO_INIT
	// ������ �� "��������" ���������
	#define RBLIB_WRITE_FULL_ADDR_CHAR(addr, value)
	// ������ �� "��������" ���������
	#define RBLIB_READ_FULL_ADDR_CHAR(addr)
	// ������ ����� �� "��������" ���������
	#define RBLIB_WRITE_FULL_ADDR_SHORT(addr, value)
	// ������ ����� �� "��������" ���������
	#define RBLIB_READ_FULL_ADDR_SHORT(addr)
	// �������
	#define RBLIB_DELAY_CYCLES(cycles)
	// �������������� ����� �����������
	#define RBLIB_COMPILE_TIME_WARNING(str) __pragma(message(str))
#endif

//Keil
#ifdef __arm__
  #define nothrow std::nothrow
#endif

#endif

/*--------�������� �������� � ������---------------
//��������� �������� � ������ �� 6 �� 4294967295 (���� ����� ����� �������������� � ����).
//������ ��� ����������� ������� ����� ������������� ��� ������������ ��� � ������������,
//������� ������������ ����������� +2 �����.
//���������� ��������, ����� �� ���������� ���������, ��� R14 � R15 ��������,
//���� �� ����� �� ���������, �� ����� �������� ������ ������������ ����������, ��� PUSH � ��� POP.

#define loop(x,y,z,m) asm("mov.w #"#x", R15"); \
	asm("mov.w #"#y", R14"); \
		asm("m"#m":"); \
	asm("add.w #0xFFFF, R15"); \
	asm("addc.w #0xFFFF, R14"); \
		asm("jc m"#m""); \
	if(z == 1) \
		asm("nop"); \
	if(z == 2){ \
		asm("nop"); \
		asm("nop");} \
	if(z == 3){ \
		asm("nop"); \
		asm("nop"); \
		asm("nop");} \
		

#define __delay(x) \
	if(x >= 6){ loop(((x-6)/4)-((((x-6)/4)/0x10000ul) * 0x10000ul), (((x-6)/4)/0x10000ul), (x-6)-(((x-6)/4) * 4), x)}
		
*/