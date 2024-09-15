#ifndef CMD_FIFO_H_
#define CMD_FIFO_H_

// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Defines
#define FIFO_MAX_NUM_CMDS 		8		    ///< Max number of commands to store in FIFO
#define FIFO_MAX_CMD_SIZE 		16		    ///< Max size of FIFO command in bytes
#define RESET_CHAR				0           ///< The value every byte in the buffer becomes on a reset

// Structure Declarations
typedef struct fifo_buf_t fifo_buf_t;       ///< Opaque FIFO buffer structure
typedef fifo_buf_t *fifo_handle_t;          ///< This is a handle for users to interact w/ API

// Function Declarations
fifo_handle_t fifo_init(void);
void fifo_free(fifo_handle_t fifo);
void fifo_reset_all(fifo_handle_t fifo);
void fifo_reset_item(fifo_handle_t fifo, uint8_t item_loc);
bool fifo_push(fifo_handle_t fifo, uint8_t* item, size_t size);
bool fifo_pop(fifo_handle_t fifo, uint8_t* item, size_t size);
uint8_t fifo_count(fifo_handle_t fifo);
bool fifo_full(fifo_handle_t fifo);
bool fifo_empty(fifo_handle_t fifo);


#endif /* CMD_FIFO_H_ */
