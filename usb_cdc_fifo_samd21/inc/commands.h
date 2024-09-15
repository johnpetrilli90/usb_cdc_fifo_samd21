/** 
 * @file commands.h
 * @author John Petrilli
 * @date 09.Aug.2024
 * @brief Provides command definitions and public function declarations
 */
#ifndef COMMANDS_H_
#define COMMANDS_H_

// User Includes
#include "cmd_fifo.h"

// Defines
#define READ_REG_CMD		"rr"		///< string that represents the read register command 
#define READ_REG_SIZE		2			///< size of the read register command in bytes
#define WRITE_REG_CMD		"wr"		///< string that represents the write register command 
#define WRITE_REG_SIZE		2			///< size of the write register command in bytes
#define IDN_CMD				"*IDN?"		///< string that represents the idn command 
#define IDN_SIZE			5			///< size of the idn command in bytes
#define STATUS_CMD			"sts?"		///< string that represents the status command 
#define STATUS_SIZE			4			///< size of the status command in bytes
#define INVALID_RET			0xFFFF		///< invalid response 
#define INVALID_RET_SIZE	4			///< size of the invalid response in bytes

// Public Function Declarations
void process_command(fifo_handle_t fifo);


#endif /* COMMANDS_H_ */