/**
 * @file switch_input.h
 * @brief 
 *
 * @author
 * @date 2025-01-22
 */

#ifndef SWITCH_INPUT_H
#define SWITCH_INPUT_H

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 * Public Definitions and Types
 ******************************************************************************/
#define SWITCH_PIN 5

/******************************************************************************
 * Public Constants
 ******************************************************************************/


/******************************************************************************
 * Public Function Declarations
 ******************************************************************************/
/**
 * @brief Initialize the switch GPIO pin
 */
void switch_input_init(void);

/**
 * @brief Get the debounced state of the switch
 * @return true if switch is pressed, false otherwise
 */
bool switch_input_is_pressed(void);

#endif /* SWITCH_INPUT_H */
