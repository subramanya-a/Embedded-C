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
#define RCC_BASE_ADDR	(0x40021000) /*  Reset and clock control RCC address from memory map table.*/
#define GPIO_BASE_ADDR	(0x40011000) /*  GPIO Port C address from memory map table.*/

void reset(void)
{
	
	volatile unsigned int *rcc_apb2enr = (unsigned int *)(RCC_BASE_ADDR + 0x18); /* RCC_APB2ENR offset:0x18 */
	volatile unsigned int *gpio_crh = (unsigned int *)(GPIO_BASE_ADDR + 0x04); /* GPIOx_CRH offset:0x04 */
	volatile unsigned int *gpio_odr = (unsigned int *)(GPIO_BASE_ADDR + 0x0C);/* GPIOx_ODR offset:0x0C */
	volatile unsigned int i;
	
	/*
	 When the peripheral clock is not active, the peripheral register values may not be readable\
	 by software and the returned value is always 0x0.
	*/
	/* Enabling peripheral clock for poart C IOPCEN (4), \
	   LED is connected to pc13 in case of STM32F103. 
	*/
	*rcc_apb2enr |= 1<<4; /* Enabling IOPCEN */
	
	/*
		GPIOx_CRH
	_________________________________
	|___23___|___22__|__21__|___20__|
	|   CNF13[1:0]   |  MODE13[1:0] |
	|	RW	 |	RW 	 |	RW	|	RW	|
	|-------------------------------|
	
		
		CNFy[1:0]: Port x configuration bits (y= 8 .. 15)
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
	*gpio_crh =(*gpio_crh &~(0xF << ((13-8)*4)))/* Clear the default configuration */\
			|(0x6 << ((13-8)*4));/* Configured pc13 as output with 2MHz */
	
	while(1)
	{
		/* Wait/delay for Blinking Led*/
		for (i=0;i<500000;i++);
		/* Toggle PC13 (i.e Port C 13) GPIO PIN*/
		/* For atomic bit set/reset, the ODR bits can be individually \
		   set and cleared by writing to the GPIOx_BSRR register (x = A .. G).*/
		*gpio_odr ^= 1<<13;
	}
	

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