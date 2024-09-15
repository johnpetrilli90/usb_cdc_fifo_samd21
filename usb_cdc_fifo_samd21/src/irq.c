/** 
 * @file irq.c
 * @author John Petrilli
 * @date 06.Sep.2024
 * @brief Sets up the Systick interrupt to count board-millis 
 */
#include "irq.h"

// System Libraries
#include <stdint.h>

// User Includes
#include "atmel_start.h"

// Global Variables
volatile uint32_t g_board_millis;

/// @brief  setups up the systick timer interrupt to fire every 1ms
/// @param  n/a
/// @return n/a
void irq_systick_init(void){
	SysTick_Config(SystemCoreClock/125);		//8MHz/1000 = 8000. There are 8000 clks every 1ms.
	NVIC_EnableIRQ(SysTick_IRQn);				//enable systick
}

/// @brief	increment the board_millis variable once per milli-second of on time
/// @param  n/a
/// @return n/a
void SysTick_Handler(void){
	g_board_millis++;
}