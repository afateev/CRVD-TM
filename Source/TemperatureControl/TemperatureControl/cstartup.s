////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		04/12/2011
////////////////////////////////////////////////////////////////////////////////

 MODULE  ?cstartup
 
		SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  ISR_BusFault
		;EXTERN  ISR_Usart2
		;EXTERN  ISR_Usart3
		;EXTERN	ISR_Timer6
		;EXTERN	ISR_Timer7
		;EXTERN	ISR_USB
		
        PUBLIC  __vector_table
		
        DATA
__vector_table
        DCD     sfe(CSTACK)						; 0x0000
        DCD     __iar_program_start             ; 0x0004	Reset Handler
		DCD     0;ISR_BusFault					; 0x0008	NMI
		DCD     0;ISR_BusFault					; 0x000C	HardFault
		DCD     0;ISR_BusFault					; 0x0010	MemManage
		DCD     0;ISR_BusFault					; 0x0014	BusFault
		DCD     0;ISR_BusFault					; 0x0018	UsageFault
		DCD     0								; 0x001C
		DCD     0								; 0x0020
		DCD     0								; 0x0024
		DCD     0								; 0x0028
		DCD     0;ISR_BusFault					; 0x002C	SVCall
		DCD     0;ISR_BusFault					; 0x0030	Debug Monitor
		DCD     0								; 0x0034
		DCD     0;ISR_BusFault					; 0x0038	PendSV
		DCD     0;ISR_BusFault					; 0x003C	SysTick
		DCD     0								; 0x0040	WWDG
		DCD     0								; 0x0044	PVD
		DCD     0								; 0x0048	TAMPER
		DCD     0								; 0x004C	RTC
		DCD     0								; 0x0050	FLASH
		DCD     0								; 0x0054	RCC
		DCD     0								; 0x0058	EXTI0
		DCD     0								; 0x005C	EXTI1
		DCD     0								; 0x0060	EXTI2
		DCD     0								; 0x0064	EXTI3
		DCD     0								; 0x0068	EXTI4
		DCD     0								; 0x006C	DMA1_Channel1
		DCD     0								; 0x0070	DMA1_Channel2
		DCD     0								; 0x0074	DMA1_Channel3
		DCD     0								; 0x0078	DMA1_Channel4
		DCD     0								; 0x007C	DMA1_Channel5
		DCD     0								; 0x0080	DMA1_Channel6
		DCD     0								; 0x0084	DMA1_Channel7
		DCD     0								; 0x0088	ADC1_2
		DCD     0								; 0x008C	CAN1_TX
		DCD     0								; 0x0090	CAN1_RX0
		DCD     0								; 0x0094	CAN1_RX1
		DCD     0								; 0x0098	CAN1_SCE
		DCD     0								; 0x009C	EXTI9_5
		DCD     0								; 0x00A0	TIM1_BRK
		DCD     0								; 0x00A4	TIM1_UP
		DCD     0								; 0x00A8	TIM1_TRG_COM
		DCD     0								; 0x00AC	TIM1_CC
		DCD     0								; 0x00B0	TIM2
		DCD     0								; 0x00B4	TIM3
		DCD     0								; 0x00B8	TIM4
		DCD     0								; 0x00BC	2C1_EV
		DCD     0								; 0x00C0	I2C1_ER
		DCD     0								; 0x00C4	I2C2_EV
		DCD     0								; 0x00C8	I2C2_ER
		DCD     0								; 0x00CC	SPI1
		DCD     0								; 0x00D0	SPI2
		DCD     0								; 0x00D4	USART1
		DCD     0;ISR_Usart2						; 0x00D8	USART2
		DCD     0;ISR_Usart3						; 0x00DC	USART3
		DCD     0								; 0x00E0	EXTI15_10
		DCD     0								; 0x00E4	RTCAlarm
		DCD     0								; 0x00E8	OTG_FS_WKUP
		DCD     0								; 0x00EC	
		DCD     0								; 0x00F0	
		DCD     0								; 0x00F4	
		DCD     0								; 0x00F8	
		DCD     0								; 0x00FC	
		DCD     0								; 0x0100	
		DCD     0								; 0x0104	
		DCD     0								; 0x0108	TIM5
		DCD     0								; 0x010C	SPI3
		DCD     0								; 0x0110	UART4
		DCD     0								; 0x0114	UART5
		DCD     0;ISR_Timer6						; 0x0118	TIM6
		DCD     0;ISR_Timer7						; 0x011C	TIM7
		DCD     0								; 0x0120	DMA2_Channel1
		DCD     0								; 0x0124	DMA2_Channel2
		DCD     0								; 0x0128	DMA2_Channel3
		DCD     0								; 0x012C	DMA2_Channel4
		DCD     0								; 0x0130	DMA2_Channel5
		DCD     0								; 0x0134	ETH
		DCD     0								; 0x0138	ETH_WHUP
		DCD     0								; 0x013C	CAN2_TX
		DCD     0								; 0x0140	CAN2_RX0
		DCD     0								; 0x0144	CAN2_RX1
		DCD     0								; 0x0148	CAN2_SCE
		DCD     0;ISR_USB							; 0x014C	OTG_FS
 END