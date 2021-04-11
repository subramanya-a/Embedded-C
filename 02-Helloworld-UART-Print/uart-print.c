/*
 ************************************************************************************************************
 * License Information 	: The open source Software on https://github.com/subramanya-a						*
 *						: This source code is free software; you can redistribute it and/or modify it	    *
 *						: under the terms of the GNU General Public License version 2 as published by the	*
 *						:                                                                                   *
 *						: Free Software Foundation;                                                         *
 *						: See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.                       *
 *						:                                                                                   *
 * 						: This program is distributed in the hope that it will be useful, but               *
 * 						: WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY        *
 * 						: or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License          *
 * 						: for more details.           					  						  			*         
 ************************************************************************************************************
 * File                 : blink.c																  			*         
 *                      :                                                                         			*
 * Description          : This program shows, Blinking the LED of STM32F103 device. 				        *
 *                      : interacting directly with core registers.                                         *
 *                      :                                                                         			*
 * Compiler             :                                                                         			*
 *                      :                                                                         			*
 * Compiler Options     : [Build.bat]                                                    					*
 *                      :	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c -o blink.o blink.c                 *
 *                      :   arm-none-eabi-ld -T blink.ld -o blink.elf blink.o                               *
 *                      :   arm-none-eabi-objdump --section-headers --syms blink.elf 1>>elf.txt             *
 *                      :   arm-none-eabi-objcopy -O binary blink.elf blink.bin                             *
 *                      :                                                                         			*
 * Pragma               : None.                                                                   			*
 *                      :                                                                         			*
 * H/W Platform         : STM32F103 [ARM-CORTEX-M3]                                                         *
 *                      :                                                                         			*
 * Portability          :                                                                       			*
 *                      :                                                                         			*
 * Author(s)            : Subramanya A                                                                      *
 *                      :                                                                         			*
 * Email                :                                                                         			*
 *                      :                                                                         			*
 * Version History      : [7:5:2018]                                                                      	*
 *                      :                                                                         			*
 * Functions            :                                                                         			*
 *                      :                                                                         			*
 *                      :                                                                         			*
 * References           : [Documents]                                                    					*
 *                      :	1. Cortex™-M3 Technical Reference Manual                                        *
 *                      :   2. STM32F10xxx Cortex®-M3 programming manual                                    *
 *                      :                                                                         			*
 * Assumption(s)        : None.                                                                   			*
 *                      :                                                                         			*
 * Constraint(s)        : None.                                                                   			*
 *                      :                                                                         			*
 ************************************************************************************************************
*/
/*==========================================================================================================*/
/* Defines                                                                 						 			*/
/*==========================================================================================================*/
#include <stdint.h>

/* RCC Memory Map */
#define RCC_BASE_ADDR			(0x40021000) /*  Reset and clock control RCC address from memory map table.*/
#define RCC_APB2ENR_OFFSET		(0x18)
/* GPIO Memory Map */
#define GPIO_PORTA_BASE_ADDR	(0x40010800) /*  GPIO Port A address from memory map table.*/
#define GPIO_PORTC_BASE_ADDR	(0x40011000) /*  GPIO Port C address from memory map table.*/
#define GPIOX_CRL_OFFSET		(0x00)
#define GPIOX_CRH_OFFSET		(0x04)
#define GPIOX_ODR_OFFSET		(0x0C)
/* USART1 Memory Map */
#define USART1_BASE_ADDR		((volatile uint32_t *) 0x40013800) /*  USART1 Port address from memory map table.*/
#define USART1_SR_OFFSET		(0x00)
#define USART1_DR_OFFSET		(0x04)
#define USART1_CR1_OFFSET		(0x0C)
#define USART1_DR_BASE			((volatile uint32_t *) (USART1_BASE_ADDR + USART1_DR_OFFSET))
#define USART1_SR_BASE			((volatile uint32_t *) (USART1_BASE_ADDR + USART1_SR_OFFSET))
#define USART_FLAG_TXE			(1 << 3UL)

void main(void);
int syslog(const char *str);

void reset(void)
{
	
	/*Just call main*/
	main();
}

void main(void)
{
	volatile unsigned int *rcc_apb2enr = (unsigned int *)(RCC_BASE_ADDR + RCC_APB2ENR_OFFSET); /* RCC_APB2ENR offset:0x18 */
	volatile unsigned int *gpioc_crh = (unsigned int *)(GPIO_PORTC_BASE_ADDR + GPIOX_CRH_OFFSET); /* GPIOx_CRH offset:0x04 */
	volatile unsigned int *gpioc_odr = (unsigned int *)(GPIO_PORTC_BASE_ADDR + GPIOX_ODR_OFFSET);/* GPIOx_ODR offset:0x0C */
	
	volatile unsigned int *gpioa_crl = (unsigned int *)(GPIO_PORTA_BASE_ADDR + GPIOX_CRL_OFFSET); /* GPIOx_CRL offset:0x00 */
	volatile unsigned int *gpioa_crh = (unsigned int *)(GPIO_PORTA_BASE_ADDR + GPIOX_CRH_OFFSET);/* GPIOx_CRH offset:0x04 */
	
	volatile unsigned int *uart1_reg = (unsigned int *)(USART1_BASE_ADDR + USART1_CR1_OFFSET);/* USART1_CR1_OFFSET offset:0x0C */
	
	volatile unsigned int i;
	
	/*
	 When the peripheral clock is not active, the peripheral register values may not be readable\
	 by software and the returned value is always 0x0.
	*/
	/* Enabling peripheral clock for poart C IOPCEN (4), \
	   LED is connected to pc13 in case of STM32F103. 
	*/
	*rcc_apb2enr |= 1<<4; /* Enabling IOPCEN */
	*rcc_apb2enr |= 1<<2; /* Enabling IOPAEN */
	*rcc_apb2enr |= 1<<0; /* Enabling AFIOEN */
	*rcc_apb2enr |= 1<<14; /* Enabling USART 1EN */
	
	/*
		GPIOx_CRH
	_________________________________
	|___23___|___22__|__21__|___20__|
	|   CNF13[1:0]   |  MODE13[1:0] |
	|	RW	 |	RW 	 |	RW	|	RW	|
	|-------------------------------|
	
		
		CNFy[1:0]: Port x configuration bits (y= 8 .. 15)
		In input mode (MODE[1:0]=00):
			00: Analog mode
			01: Floating input (reset state)
			10: Input with pull-up / pull-down
			11: Reserved
		In output mode (MODE[1:0] > 00):
			00: General purpose output push-pull
			01: General purpose output Open-drain
			10: Alternate function output Push-pull
			11: Alternate function output Open-drain
		MODEy[1:0]: Port x mode bits (y= 8 .. 15)
			00: Input mode (reset state)
			01: Output mode, max speed 10 MHz.
			10: Output mode, max speed 2 MHz.
			11: Output mode, max speed 50 MHz.
	*/
	
	/* Configure the pc13 pin as output as General purpose output Open-drain @ max speed 2 MHz.*/
	*gpioc_crh =(*gpioc_crh &~(0xF << ((13-8)*4)))/* Clear the default configuration */\
			|(0x6 << ((13-8)*4));/* Configured pc13 as output with 2MHz */
	
	
	/*
	* UART1 GPIO PIN A9 and A10
	*	GPIO A9  (RX) : OUT mode Alternate function output Push-pull with speed as 50Mhz
	*	GPIO A10 (TX) : IN mode, Floating input (reset state)
	*/
	
	*gpioa_crh = (*gpioa_crh &~(0xF << ((9-8)*4)))/* Clear the default configuration */\
			|(0xB << ((9-8)*4));/* Configured A9 as output with 50MHz AF_PP */
	*gpioa_crh |= (*gpioa_crh &~(0xF << ((10-8)*4)))/* Clear the default configuration */\
			|(0x4 << ((10-8)*4));/* Configured A10 as Floating input */
	
	/* congiruring uart port 
	* USART enable | Transmitter enable |Receiver enable
	*/
	*uart1_reg |= ((1<<13U) |(1<<3U) |(1<<2U)); 
	
	while(1)
	{
		/* Wait/delay for Blinking Led*/
		for (i=0;i<500000;i++);
		/* Toggle PC13 (i.e Port C 13) GPIO PIN*/
		/* For atomic bit set/reset, the ODR bits can be individually \
		   set and cleared by writing to the GPIOx_BSRR register (x = A .. G).*/
		*gpioc_odr ^= 1<<13;
		
		syslog("Hello World!\r\n");
		
	}
	
}

int syslog(const char *str)
{
	
	while (*str) 
	{
		while (!(*(USART1_SR_BASE) & USART_FLAG_TXE));
		*(USART1_DR_BASE) = *str++ & 0xFF;
	}
	return 0;
}

/* Stack */
int STACK[256];

/* Place the vectors at .vectors section (present in linker file)*/
/*
	The vector table at location 0 is only required to have four values:
	• stack top address
	• reset routine location
	• NMI ISR location
	• Hard Fault ISR location
	for more details refere arm cortex-m3 refere manual [ARM DDI 0337E] \
	[section: Vector Table and Reset]
*/
const void *vectors[] __attribute__((section(".vectors")))={
		STACK + sizeof(STACK)/sizeof(*STACK),
		reset,/* placing the reset function after stack */
		/*
		  Below handlers to be filled for the respective vector index.
		*/
};