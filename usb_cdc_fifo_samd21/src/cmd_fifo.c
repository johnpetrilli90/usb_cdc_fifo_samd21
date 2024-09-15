/** 
 * @file cmd_fifo.c
 * @brief First in first out (FIFO) buffer to store commands
          or items to be processed at a more convienent time
*/
#include "cmd_fifo.h"

// System Libraries
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

// Defines
struct fifo_buf_t{
    uint8_t buffer[FIFO_MAX_NUM_CMDS][FIFO_MAX_CMD_SIZE];
    size_t  head;
    size_t  tail;
    uint8_t count;
};

// Private Function Declarations
static bool fifo_assert_size(size_t size);
static inline void fifo_clear_location(fifo_handle_t fifo, uint8_t location);

// Private Functions
/// @brief  Resets the an item in the fifo and fill values with the RESET_CHAR
/// @param  fifo_handle_t	- the pointer handle to reset
/// @param  uint8_t			- location of the item to be cleared
/// @return void
static inline void fifo_clear_location(fifo_handle_t fifo, uint8_t location){
	for (uint8_t i=0; i<FIFO_MAX_CMD_SIZE; i++){
		fifo->buffer[location][i]= RESET_CHAR;
	}
}

/// @brief  checks that the size of the array is less than or equal to the max size
/// that the fifo buffer allows per command
/// @param  size_t    - number of characters in the buffer
/// @return bool      - returns true if size is ok & false is size is too large
static bool fifo_assert_size(size_t size){
  bool ret = false;

  if(size <= FIFO_MAX_CMD_SIZE){
    ret = true;
  }
  else{
    ret = false;
  }

  return ret;
}

// Public Functions
/// @brief  Reserves a block of memory for the fifo and returns the pointer handle
/// @param  void
/// @return fifo_handle_t -	fifo pointer handle
fifo_handle_t fifo_init(void)
{
	fifo_handle_t fifo = malloc(sizeof(fifo_buf_t));
	assert(fifo);
	
	fifo_reset_all(fifo);  
	  
	return fifo;
}

/// @brief  Releases the memory block for the pointer handle
/// @param  fifo_handle_t	- the pointer handle that should be deleted
/// @return void
void fifo_free(fifo_handle_t fifo){
	assert(fifo);
	
	free(fifo);
}

/// @brief  Resets the entire fifo and fills all values with the RESET_CHAR
/// @param  fifo_handle_t	- the pointer handle to reset
/// @return void
void fifo_reset_all(fifo_handle_t fifo){
	fifo->head = 0;
	fifo->tail = 0;
	fifo->count = 0;

	for (uint8_t i=0; i<FIFO_MAX_NUM_CMDS; i++){
		for (uint8_t j=0; j<FIFO_MAX_CMD_SIZE; j++){
			fifo->buffer[i][j]= RESET_CHAR;
		}
	}
}

/// @brief  Checks if the FIFO is empty and returns true if it is
/// @param  fifo_handle_t	- the FIFO handle pointer
/// @return bool - true if empty false if not
bool fifo_empty(fifo_handle_t fifo){
  bool ret = false;

  if(fifo->count == 0){
    ret = true;
  }
  else{
    ret = false;
  }

  return ret;  
}

/// @brief  Checks if the FIFO is full and returns true if it is
/// @param  fifo_handle_t	- the FIFO handle pointer
/// @return bool - true if full false if not
bool fifo_full(fifo_handle_t fifo){
  bool ret = false;
 
  if(fifo->count >= FIFO_MAX_NUM_CMDS){
    ret = true;
  }
  else{
    ret = false;
  }

  return ret;  
}

/// @brief  Resets the entire fifo and fills all values with the RESET_CHAR
/// @param  fifo_handle_t	- the pointer handle of the fifo you want to access
/// @param  uint8_t*		- pointer to the buffer that should be added to the fifo
/// @param	size_t			- size of the buffer that should be added to the fifo, in bytes
/// @return uint8_t			- returns 0 for failure & 1 for success
bool fifo_push(fifo_handle_t fifo, uint8_t* buf, size_t size)
{
  bool ret = false;

  if(fifo_full(fifo)){
    ret = false;
  }
  else if(!fifo_assert_size(size)){
    ret = false;
  }
  else{  
    fifo_clear_location(fifo, fifo->tail);
    memcpy(fifo->buffer[fifo->tail], buf, size);
    fifo->tail = (fifo->tail + 1) % FIFO_MAX_NUM_CMDS;
    fifo->count++;
    ret = true;
  }

    return ret;
}

/// @brief  Takes the tail item and puts it into the item pointer (buffer)
/// @param  fifo_handle_t	- the pointer handle of the fifo you want to access
/// @param  uint8_t*		- pointer to the buffer that should be removed from the fifo
/// @param	size_t			- size of the buffer that should be added to the fifo, in bytes
/// @return uint8_t			- returns 0 for failure & 1 for success
bool fifo_pop(fifo_handle_t fifo, uint8_t* item, size_t size)
{
  bool ret = false;

  if (fifo_empty(fifo)){
    ret = false;
  }
  else if (!fifo_assert_size(size)){
    ret = false;
  }
  else{
    memcpy(item, fifo->buffer[fifo->head], size);
    fifo->head = (fifo->head + 1) % FIFO_MAX_NUM_CMDS;
    fifo->count--;
    ret = true;
  }

  return ret;
}

/// @brief  Returns the number of items in the fifo
/// @param  fifo_handle_t	- the pointer handle of the fifo you want to access
/// @return uint8_t			- number of items in the fifo
uint8_t fifo_count(fifo_handle_t fifo){
	return fifo->count;
}