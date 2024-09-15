/** 
 * @file version.h
 * @author John Petrilli
 * @date 11.Sep.2024
 * @brief This file holds the HW version and FW version. 
 *
 * The build version will get auto incremented using a pre-build script.
 * The major, minor, and bugfix versions need to be updated manually. 
 */
#ifndef FW_VERSION_H
#define FW_VERSION_H

// System Libraries
#include <stdint.h>

// Defines
#define FW_VERSION_MAJOR 1
#define FW_VERSION_MINOR 3
#define FW_VERSION_BUGFIX 8
#define FW_VERSION_BUILD 9

#define FW_VERSION "1.3.8.9"

//The identification string in format: <manufacturer>, <model>, <serial number>, <software version>/<hardware version>.
#define MFG				"MFG"
#define MODEL			"MODEL"
#define SERIAL_NUM		"SN"
#define HW_VERSION		"1.0"

#endif /* VERSION_H_ */
