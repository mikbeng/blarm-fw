/**
 * @file pwr_mgmt.h
 * @brief 
 *
 * @author
 * @date 2025-01-22
 */

#ifndef PWR_MGMT_H
#define PWR_MGMT_H

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 * Public Definitions and Types
 ******************************************************************************/
#define pwr_off_pin 2

/******************************************************************************
 * Public Constants
 ******************************************************************************/


/******************************************************************************
 * Public Function Declarations
 ******************************************************************************/
void pwr_init(void);
void pwr_off(void);
void pwr_enter_deep_sleep(uint32_t state_to_save);

#endif /* PWR_MGMT_H */
