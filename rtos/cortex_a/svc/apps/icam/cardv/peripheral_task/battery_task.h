#ifndef __BATTERY_TASK_H__
#define __BATTERY_TASK_H__

#include <app_base.h>

int battery_task_start(void);
int battery_task_stop(void);
int battery_task_set_debug(int enable);
int battery_task_set_low_power_check(int enable);
int battery_power_on_check(void);
unsigned int battery_task_get_adc(void);
void battery_task_get_adc_impl(void);
int battery_task_check_charger_ic_stat(unsigned char *is_charge, unsigned char *power_good, unsigned char *thermal_protect, unsigned char *charge_full);
int battery_task_get_percentage(void);
int battery_task_get_voltage_mv(void);
unsigned int battery_task_get_raw_adc(void);
unsigned int battery_task_get_cable_adc(void);
unsigned int battery_task_get_mcu_adc(void);
int battery_task_reset_param(void);

#endif//__BATTERY_TASK_H__

