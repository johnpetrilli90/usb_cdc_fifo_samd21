/** 
 * @file commands.c
 * @author John Petrilli
 * @date 09.Aug.2024
 * @brief This file processes commands coming in from USB CDC
*/
#include "commands.h"

// System Libraries
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// User Includes
#include "atmel_start.h"
#include "usb_start.h"
#include "cmd_fifo.h"
#include "version.h"
#include "registers.h"

// Defines
#define HEX_BASE			16				///< pre-processor directive to define the hex number base for strtol function
#define DEC_BASE			10				///< pre-processor directive to define the decimal number base for strtol function
#define CMD_NUM_BASE		HEX_BASE		///< pre-processor directive to define the number base which is communicated over usb cdc
#define USB_TIMEOUT_MS		10				///< pre-processor directive to define the send/transmit timeout for a USB write
#define TX_ITEM_MAX_SIZE	64				///< pre-processor directive to define the max number of char's on a usb transmit

// Global Variables
bool g_tx_packet_complete;
volatile uint32_t g_board_millis;
volatile registers_t system_registers;
	
// Private Function Declarations
static void command_read_reg(const char* buf);
static void command_write_reg(const char* buf);
static void command_idn_request(void);
static void command_status_request(void);
static void usb_write(uint8_t* tx, uint8_t len);
static bool command_timeout(uint32_t start_time);

//Public Functions
/// @brief  function is called when the fifo buffer is not empty. It pops the command
///	off the fifo, determines if its a read, write, idn, or status command, then 
/// sends the command arguments off to be processed further
/// @param  fifo_handle_t - fifo buffer that holds the command to process
/// @return void
void process_command(fifo_handle_t fifo){
	static char command_buf[RX_BUFFER_SIZE];					//buffer to store command
				
	fifo_pop(fifo, (uint8_t*)command_buf, RX_BUFFER_SIZE);		//get the command from the fifo
	
	if(!strncmp((const char*)command_buf, (const char*)READ_REG_CMD, READ_REG_SIZE)){
		command_read_reg(&command_buf[READ_REG_SIZE]);
	}
	else if(!strncmp((const char*)command_buf, (const char*)WRITE_REG_CMD, WRITE_REG_SIZE)){
		command_write_reg(&command_buf[WRITE_REG_SIZE]);
	}
	else if(!strncmp((const char*)command_buf, (const char*)IDN_CMD, IDN_SIZE)){
		command_idn_request();
	}
	else if(!strncmp((const char*)command_buf, (const char*)STATUS_CMD, STATUS_SIZE)){
		command_status_request();
	}
	
}

//Private Functions
/// @brief  read register command called by 'process_command'. Takes the argument from the 
/// read register command [i.e. if cmd is rr1, the argument is 1], turns the arg into a integer,
/// formats the response based on the read reg arg, then prints the response in hex over USB CDC
/// @param  const char* - fifo buffer that holds the command argument to process
/// @return void 
static void command_read_reg(const char* buf){
	static char msg[TX_ITEM_MAX_SIZE];
	static uint8_t len;
	static uint8_t reg_num;
	
	reg_num = strtol(buf, NULL, CMD_NUM_BASE);
	
	switch (reg_num){
		case 0x1:
			len = sprintf(msg, "0x%x\r\n", (unsigned int)system_registers.register_01);
			break;
		case 0x2:
			len = sprintf(msg, "0x%x\r\n", (unsigned int)system_registers.register_02);
			break;;
		case 0x3:
			len = sprintf(msg, "0x%x\r\n", (unsigned int)system_registers.register_03);
			break;
		default:
			len = sprintf(msg, "0x%x\r\n", INVALID_RET);
			break;
	}

	usb_write((uint8_t *)msg, len);
}

/// @brief  write register command called by 'process_command'. Takes the args from the
/// write register command [i.e. if cmd is wr 3 50, the register arg is 3 and write/set arg is 0x50],
/// Turns the args into a integers, them attempts to write the set value to the register granted
/// its valid and in-bounds. It will print the set-value arg on success and INVALID_RET on failure
/// @param  const char* - fifo buffer that holds the write reg arguments to process
/// @return void
static void command_write_reg(const char* buf){
	static char ascii_reg[2];				//array to hold the register value
	static char ascii_val[10];				//array to hold the set value
	static char msg[TX_ITEM_MAX_SIZE];		//array to hold the return message
	const char *p_arg = &buf[2];			//pointer to the passed arg
	static uint8_t len;
	bool command_valid = false;				//bool for valid/invalid command
	
	memcpy((void*)ascii_reg, (void*)buf, 2);						//load ascii reg with reg value
	uint8_t  reg = strtol((const char*)ascii_reg, NULL, CMD_NUM_BASE);	//convert ascii reg into integer
	
	memcpy((void*)ascii_val, (void*)p_arg, 10);						//load the ascii val with set value
	unsigned int arg = strtol(ascii_val, NULL, CMD_NUM_BASE);			//convert ascii value into integer
	
	switch(reg){
		case 0x1:
			command_valid = true;
			system_registers.register_01 = arg;
			len = sprintf(msg, "0x%x\r\n", arg);		//return message is the set value
			break;
		case 0x2:
			command_valid = true;						
			system_registers.register_02 = arg;	
			len = sprintf(msg, "0x%x\r\n", arg);		//return message is the set value
			break;
		case 0x3:
			command_valid = true;						
			system_registers.register_03 = arg;	
			len = sprintf(msg, "0x%x\r\n", arg);		//return message is the set value
			break;
		default:
			command_valid = false;
			break;
	}
	
	//create the return message
	if(!command_valid){
		len = sprintf(msg, "0x%x\r\n", INVALID_RET);	//invalid register write
	}
	
	usb_write((uint8_t *)msg, len);						//write to usb cdc
}

static void command_idn_request(void){
	static char	msg[TX_ITEM_MAX_SIZE];
	
	//The identification string in format: <manufacturer>, <model>, <serial number>, <software version>/<hardware version>.	
	sprintf(msg, "%s, %s, %s, %s/%s\r\n", MFG, MODEL, SERIAL_NUM, FW_VERSION, HW_VERSION);

	usb_write((uint8_t *)msg, sizeof(msg));
}

/// @brief  prints out a verbose human readable multi-line status message that displays
/// all states, registers, and values for debugging purposes.
/// @param  void
/// @return void
static void command_status_request(void){
	static uint8_t	tx[TX_ITEM_MAX_SIZE];
	static uint8_t	len;
	
	//Registers
	len = sprintf((char*)tx, "\r\n** Registers **\r\n");
	usb_write((uint8_t*)tx, len);
	len = sprintf((char*)tx, "Reg 0x01:\t0x%x\r\n", (unsigned int)system_registers.register_01);
	usb_write((uint8_t*)tx, len);
	len = sprintf((char*)tx, "Reg 0x02:\t0x%x\r\n", (unsigned int)system_registers.register_02);
	usb_write((uint8_t*)tx, len);
	len = sprintf((char*)tx, "Reg 0x03:\t0x%x\r\n", (unsigned int)system_registers.register_03);
	usb_write((uint8_t*)tx, len);
	len = sprintf((char*)tx, "Board Millis:\t%lu\r\n", g_board_millis);
	usb_write((uint8_t*)tx, len);
}

/// @brief  function prints out up to 64 bytes per transfer over USB CDC. Gives the cdc async
/// write 10ms to transmit befor the function times out. Clears the tx_packet_complete flag
/// before the write. The CDC write callback will set tx_packet_complete flag back to true. 
/// The watchdog is reset after the USB write because it could be many 10's of milli-seconds
/// before the stack pointer is back in the idle_daemon for a wdt reset for a sts command. 
/// @param  uint8_t*	- Pointer to the message to print. Message is already const from the
/// formatting from the command parsing functions. 'cdcdf_acm_write' discards the const qualifier. 
/// @param uint8_t		- length of the message to print
/// @return void
static void usb_write(uint8_t* tx, uint8_t len){
	static volatile uint32_t start_time;			//grab board-millis for start time of usb_write
	
	start_time = g_board_millis;
	g_tx_packet_complete = false;
	cdcdf_acm_write((uint8_t*)tx, len);
	while(!g_tx_packet_complete && !command_timeout(start_time));
}

/// @brief  reads the current board_millis value and subtracts it from the start time. If this 
/// value is greater than the timeout value, then the timeout is true. Otherwise timeout is false. 
/// @param uint32_t	- board_millis value when the usb write function was called
/// @return bool	- true if specified timeout is exceeded. False if the timeout is not exceeded.
static bool command_timeout(uint32_t start_time){
	static volatile bool ret = true;
	
	if((g_board_millis - start_time) > USB_TIMEOUT_MS){
		ret = true;
	}
	else{
		ret = false;
	}
	
	return ret;
}


