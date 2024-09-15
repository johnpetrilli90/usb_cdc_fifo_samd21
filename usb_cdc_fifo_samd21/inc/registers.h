/** 
 * @file registers.h
 * @author John Petrilli
 * @date 25.July.2024
 * @brief Provides the position and mask for each variables stored in the status register,
 * struct to hold the individual values, struct to hold the registers
 */
#ifndef REGISTERS_H_
#define REGISTERS_H_

// System Libraries
#include <stdint.h>
#include <stdbool.h>

/// @brief struct that contains all the registers in the system
struct _config_registers{
	uint32_t register_01;
	uint32_t register_02;
	uint32_t register_03;
};


typedef struct _config_registers registers_t;				///< typedef struct for user access to the system registers

#endif /* REGISTERS_H_ */