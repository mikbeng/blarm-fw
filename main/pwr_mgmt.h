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
#define bat_mon__on_pin 10
#define bat_mon_pin 4

typedef struct {
    uint32_t wake_count;
    uint64_t last_wakeup_pins;    // Store which pins triggered the wakeup
    uint64_t sleep_time_us;
    uint32_t last_state;          // Can store any state you want to preserve
} sleep_handle_t;

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
