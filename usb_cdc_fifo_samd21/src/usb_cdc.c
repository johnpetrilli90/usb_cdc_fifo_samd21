/** 
 * @file usb_cdc.c
 * @author John Petrilli
 * @date 23.Aug.2024
 * @brief Initially auto-generated from ASF4/Atmel Start. Setups up the USB type, descriptors, 
 * interrupts, callbacks, tx/rx buffers
 */
#include "usb_cdc.h"

// System Libraries
#include <stdbool.h>

// User Includes
#include "atmel_start.h"
#include "usb_start.h"
#include "cmd_fifo.h"
#include "commands.h"

bool g_tx_packet_complete;

#if CONF_USBD_HS_SP
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_LS_FS};
static uint8_t single_desc_bytes_hs[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_HS};
#define CDCD_ECHO_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ_HS
#else
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_DESCES_LS_FS};
#define CDCD_ECHO_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ
#endif

static struct usbd_descriptors single_desc[]
    = {{single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes)}
#if CONF_USBD_HS_SP
       ,
       {single_desc_bytes_hs, single_desc_bytes_hs + sizeof(single_desc_bytes_hs)}
#endif
};

/** Buffers to receive and echo the communication bytes. */
static uint32_t usbd_cdc_buffer[CDCD_ECHO_BUF_SIZ / 4];

/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64];

static usb_buffer_t usb_buffer;
fifo_handle_t g_command_fifo;

/// @brief  callback when USB tx is complete. Sets the tx_packet_complete flag to true
/// @param  n/a
/// @return n/a
static bool usb_device_cb_bulk_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	g_tx_packet_complete = true;

	/* No error. */
	return false;
}

/// @brief  callback on usb packet reception. Parses the characters and adds them to a buffer. 
/// Once the terminating character is identified it pushes the command the the FIFO
/// @param  n/a
/// @return n/a
static bool usb_device_cb_bulk_in(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	uint8_t temp_buf[RX_BUFFER_SIZE];
	
	memcpy((void*)temp_buf, (void*)usbd_cdc_buffer, RX_BUFFER_SIZE);
	
	for(uint8_t i=0; i < count; i++){
		if(usb_buffer.rx_idx >= RX_BUFFER_SIZE)
			usb_buffer.rx_idx = 0;		//reset the buffer to prevent overflow
	
		usb_buffer.rx[usb_buffer.rx_idx] = (uint8_t)temp_buf[i];
		if(usb_buffer.rx[usb_buffer.rx_idx] == '\r'){}				//do nothing if carriage return
		else if(usb_buffer.rx[usb_buffer.rx_idx] == '\n'){			//line feed is terminating char
			fifo_push(g_command_fifo, usb_buffer.rx, usb_buffer.rx_idx);	//send command to the buffer, not the newline char
			usb_buffer.rx_idx = 0;		//reset the rx buffer
		}
		else{
			usb_buffer.rx_idx++;		//increment the rx buffer index. 
		}
	}
	/* Re-arm the cdc read callback */
	cdcdf_acm_read((uint8_t *)usbd_cdc_buffer, sizeof(usbd_cdc_buffer));

	/* No error. */
	return false;
}

/**
 * \brief Callback invoked when Line State Change
 */
static bool usb_device_cb_state_c(usb_cdc_control_signal_t state)
{
	if (state.rs232.DTR) {
		/* Callbacks must be registered after endpoint allocation */
				
		//lets brew something up
		cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)usb_device_cb_bulk_in);		//go to the rx callback on a read
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_device_cb_bulk_out);		//write callback*/
		
		/* Start Rx */
		cdcdf_acm_read((uint8_t *)usbd_cdc_buffer, sizeof(usbd_cdc_buffer));		//initializes rx
	}

	/* No error. */
	return false;
}

/// @brief  initializes, sets up irqs, and starts the USB
/// @param  void
/// @return void
void cdc_device_acm_init(void)
{
	/* usb stack init */
	usbdc_init(ctrl_buffer);

	/* usbdc_register_funcion inside */
	cdcdf_acm_init();

	usbdc_start(single_desc);
	usbdc_attach();
	
	usb_buffer.rx_idx = 0;		//init to 0
}

/**
 * Example of using CDC ACM Function.
 * \note
 * In this example, we will use a PC as a USB host:
 * - Connect the DEBUG USB on XPLAINED board to PC for program download.
 * - Connect the TARGET USB on XPLAINED board to PC for running program.
 * The application will behave as a virtual COM.
 * - Open a HyperTerminal or other COM tools in PC side.
 * - Send out a character or string and it will echo the content received.
 */
void cdcd_acm_init(void)
{
	while (!cdcdf_acm_is_enabled()) {
		// wait cdc acm to be installed
	};
	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)usb_device_cb_state_c);	//line state change. goes to callback usb_device_cb_state_c

}

void usb_init(void)
{
	cdc_device_acm_init();
}
