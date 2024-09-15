/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#ifndef USB_DEVICE_MAIN_H
#define USB_DEVICE_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "cdcdf_acm.h"
#include "cdcdf_acm_desc.h"
#include "cmd_fifo.h"

// Defines
#define RX_BUFFER_SIZE		FIFO_MAX_CMD_SIZE			///< pre-processor directive for max size of usb rx buffer in bytes

/// @brief struct containing buffer and buffer index
struct _config_usb_buffer{
	uint8_t rx[RX_BUFFER_SIZE];
	uint8_t rx_idx;
};

typedef struct _config_usb_buffer usb_buffer_t;			///< typedef struct for user access to possible usb command buffer

void cdcd_acm_example(void);
void cdc_device_acm_init(void);
void cdcd_acm_register_callback(void);

/**
 * \berif Initialize USB
 */
void usb_init(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // USB_DEVICE_MAIN_H
