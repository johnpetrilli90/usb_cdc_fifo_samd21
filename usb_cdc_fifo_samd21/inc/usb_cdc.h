/** 
 * @file usb_start.h
 * @author John Petrilli
 * @date 09.Aug.2024
 * @brief Provides usb rx buffer and public function declarations
 */
#ifndef USB_CDC_H_
#define USB_CDC_H_

// System Libraries
#include <stdint.h>

// User Includes
#include "cmd_fifo.h"

// Defines
#define RX_BUFFER_SIZE		FIFO_MAX_CMD_SIZE			///< pre-processor directive for max size of usb rx buffer in bytes

/// @brief struct containing buffer and buffer index
struct _config_usb_buffer{
 uint8_t rx[RX_BUFFER_SIZE];
 uint8_t rx_idx;
};

typedef struct _config_usb_buffer usb_buffer_t;			///< typedef struct for user access to possible usb command buffer

// Public Function Declarations
void cdcd_acm_init(void);

#endif /* USB_CDC_H_ */