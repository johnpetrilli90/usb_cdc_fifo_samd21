/** 
 * @file main.c
 * @author John Petrilli
 * @date 13.Sep.2024
 * @brief This file contains main and is where all the global variables are defined.
*/

// User Includes
#include "atmel_start.h"
#include "atmel_start_pins.h"
#include "cmd_fifo.h"
#include "commands.h"
#include "irq.h"
#include "usb_start.h"
#include "registers.h"
#include "led.h"

// Global Variables
/** @defgroup Global_Variables Global Variables
 *  @{
 */
extern fifo_handle_t g_command_fifo;			///< global variable of type fifo_handle_t for the usb command buffer
extern volatile uint32_t g_board_millis;		///< global variable of type uint32_t to track the board's on time in milli-seconds
extern volatile registers_t system_registers;	///< global variable of type registers_t to hold the values in various registers
extern bool g_tx_packet_complete;				///< global variable of type bool to hold the usb tx complete flag
/** @} */ // end of Global_Variables

// Function Declarations
void usb_cdc_fifo_init(void);

/// @brief  The main function initializes the board and peripheral drivers then in the while loop it
/// checks for a usb command, runs the idle daemon, and runs the status blink function. 
/// @param  void
/// @return n/a
int main(void)
{
	usb_cdc_fifo_init();
	
	while(1){
		if (fifo_count(g_command_fifo)){
			process_command(g_command_fifo);
		}
		led_blink_status_led();
	}
}

void usb_cdc_fifo_init(void){
	atmel_start_init();
	g_command_fifo = fifo_init();
	g_tx_packet_complete = true;
	g_board_millis = 0;
	irq_systick_init();
	
}

