/** 
 * @file led.c
 * @author John Petrilli
 * @date 06.Sep.2024
 * @brief Blinks a status LED for a pre-defined amount of on & off time.
 */
#include "led.h"

// System Libraries
#include <stdint.h>

// User Includes
#include "atmel_start.h"

// Defines
#define	LED_ON_TIME_MILLIS		50				///< Amount of time in milli-seconds the status LED will remain on in blink cycle
#define LED_OFF_TIME_MILLIS		950				///< Amount of time in milli-seconds the status LED will remain off in blink cycle

// Global variables
volatile uint32_t g_board_millis;

// Public Functions
/// @brief  This is called from within the idle_daemon and blinks the status LED based on the 
/// pre-defined timing of LED_ON_TIME_MILLIS & LED_OFF_TIME_MILLIS
/// @param  n/a
/// @return n/a
void led_blink_status_led(void){
	static led_state_t led_state = OFF;
	static uint32_t led_state_timer = 0;
	volatile uint32_t current_state_time;
	
	current_state_time = g_board_millis - led_state_timer;

	switch(led_state){
		case OFF:
			if(current_state_time >= LED_OFF_TIME_MILLIS){
				gpio_set_pin_level(nSTATUS_LED, ON);
				led_state = ON;
				led_state_timer = g_board_millis;
				
			}
			break;
		case ON:
			if(current_state_time >= LED_ON_TIME_MILLIS){
				gpio_set_pin_level(nSTATUS_LED, OFF);
				led_state = OFF;
				led_state_timer = g_board_millis;
			}
			break;
		default:
			gpio_set_pin_level(nSTATUS_LED, OFF);
			led_state_timer = g_board_millis;
			break;
	}
}

