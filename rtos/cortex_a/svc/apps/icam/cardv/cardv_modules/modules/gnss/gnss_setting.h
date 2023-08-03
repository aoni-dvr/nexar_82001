#ifndef __GNSS_SETTING_H__
#define __GNSS_SETTING_H__

#include "cardv_include.h"

void gnss_set_satellite_id(UINT8 id);
void gnss_set_boot_mode(UINT8 mode);
void gnss_set_data_rate(UINT8 rate);
void gnss_set_dsp_uart_baudrate(UINT8 rate);
void gnss_set_gst_enable(UINT8 enable);

#endif//__GNSS_SETTING_H__

