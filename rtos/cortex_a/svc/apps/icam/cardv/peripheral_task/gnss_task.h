#ifndef __GNSS_TASK_H__
#define __GNSS_TASK_H__

#include <app_base.h>

#define GNSS_DATA_READY           (1 << 0)

int gnss_task_start(void);
int gnss_task_stop(void);
int gnss_task_set_debug(int enable);
int gnss_task_uart_write(const unsigned char *data, const unsigned int len);
int gnss_debug_task_start(void);
int gnss_debug_task_stop(void);
int gnss_task_get_connected(void);
int gnss_task_uart_write2(const unsigned char *data, const unsigned int len);

#endif//__GNSS_TASK_H__

