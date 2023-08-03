#ifndef __GNSS_PPS_TASK_H__
#define __GNSS_PPS_TASK_H__

int gnss_pps_task_start(void);
int gnss_pps_task_set_debug(int enable);
int gnss_time_now(time_s *time);
int gnss_set_time(unsigned int sec, unsigned int usec);

#endif//__GNSS_PPS_TASK_H__

