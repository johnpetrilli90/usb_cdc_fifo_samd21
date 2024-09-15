/** 
 * @file led.h
 * @author John Petrilli
 * @date 06.Sep.2024
 * @brief Provides enum for LED states and public function defintions
 */
#ifndef LED_H_
#define LED_H_

// System Libraries
#include <stdbool.h>
#include <stdint.h>

/// @brief  enum containing the possible states of the status LED
enum _led_states {
	ON = false,
	OFF = true,	
};

typedef enum _led_states led_state_t;		///< typedef enum for user access to status led states

// Public Function Declarations
void led_blink_status_led(void);

#endif /* LED_H_ */