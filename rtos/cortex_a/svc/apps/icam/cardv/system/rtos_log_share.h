#ifndef __RTOS_LOG_SHARE_H__
#define __RTOS_LOG_SHARE_H__

int rtos_log_share_get_info(unsigned long long *share_addr, unsigned long long *phy_addr, unsigned int *share_size);
int rtos_log_share_set_enable(int enable);
int rtos_log_share_send_data(const unsigned char *data, unsigned int len);
//void rtos_log_share_send_power_on_log(void);

#endif//__RTOS_LOG_SHARE_H__

